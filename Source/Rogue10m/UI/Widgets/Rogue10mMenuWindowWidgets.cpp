// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mMenuWindowWidgets.h"

#include "Rogue10mInventoryComponent.h"
#include "Rogue10m.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCombatComponent.h"
#include "Rogue10mItemDataAsset.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "InputCoreTypes.h"
#include "Widgets/Rogue10mItemDragDropOperation.h"
#include "Widgets/Rogue10mSkillDragDropOperation.h"

void URogue10mMenuWindowWidget::InitializeMenuWindow(URogue10mInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	BP_OnMenuWindowInitialized();
}

URogue10mInventoryComponent* URogue10mMenuWindowWidget::GetInventoryComponent() const
{
	if (InventoryComponent)
	{
		return InventoryComponent;
	}

	const ARogue10mCharacter* RogueCharacter = Cast<ARogue10mCharacter>(GetOwningPlayerPawn());
	return RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
}

void URogue10mMenuWindowWidget::SetWindowOpen(bool bOpen)
{
	const bool bChanged = bWindowOpen != bOpen;
	bWindowOpen = bOpen;
	SetVisibility(bWindowOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bChanged)
	{
		BP_OnWindowOpenChanged(bWindowOpen);
	}
}

void URogue10mInventoryItemWidget::InitializeGridItem(
	URogue10mInventoryComponent* InInventory,
	URogue10mInventoryWindowWidget* InInventoryWindow,
	int32 InContainerIndex,
	const FRogue10mInventoryGridEntry& InEntry,
	float InCellSize)
{
	Inventory = InInventory;
	InventoryWindow = InInventoryWindow;
	ContainerIndex = InContainerIndex;
	InstanceId = InEntry.InstanceId;
	ItemData = InEntry.ItemData;
	Quantity = InEntry.Quantity;
	CellSize = InCellSize;

	const FIntPoint Footprint = ItemData ? ItemData->GetClampedInventorySize() : FIntPoint(1, 1);
	if (UI_InventoryItemSize)
	{
		UI_InventoryItemSize->SetWidthOverride(Footprint.X * CellSize);
		UI_InventoryItemSize->SetHeightOverride(Footprint.Y * CellSize);
		UI_InventoryItemSize->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}
	UTexture2D* Icon = ItemData ? ItemData->InventoryIcon.LoadSynchronous() : nullptr;

	if (UI_InventoryItemIcon)
	{
		// Compute the fitted size explicitly so the icon always receives non-zero paint geometry.
		UI_InventoryItemIcon->SetBrushFromTexture(Icon, true);
		UI_InventoryItemIcon->SetBrushTintColor(FSlateColor(FLinearColor::White));
		UI_InventoryItemIcon->SetColorAndOpacity(FLinearColor::White);
		UI_InventoryItemIcon->SetRenderOpacity(1.0f);
		if (Icon)
		{
			const float AvailableWidth = FMath::Max(1.0f, Footprint.X * CellSize - 8.0f);
			const float AvailableHeight = FMath::Max(1.0f, Footprint.Y * CellSize - 8.0f);
			const float TextureWidth = FMath::Max(1.0f, static_cast<float>(Icon->GetSizeX()));
			const float TextureHeight = FMath::Max(1.0f, static_cast<float>(Icon->GetSizeY()));
			const float FitScale = FMath::Min(AvailableWidth / TextureWidth, AvailableHeight / TextureHeight);
			const float IconScale = ItemData ? FMath::Clamp(ItemData->InventoryIconScale, 0.1f, 2.0f) : 1.0f;
			FSlateBrush IconBrush = UI_InventoryItemIcon->GetBrush();
			IconBrush.SetImageSize(FVector2D(TextureWidth, TextureHeight) * FitScale * IconScale);
			UI_InventoryItemIcon->SetBrush(IconBrush);
		}
		UI_InventoryItemIcon->SetRenderScale(FVector2D::UnitVector);
		UI_InventoryItemIcon->SetVisibility(Icon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (UI_InventoryItemQuantityText)
	{
		const bool bShowQuantity = ItemData
			&& ItemData->Category != ERogue10mItemCategory::Equipment
			&& Quantity > 1;
		FText Label = FText::GetEmpty();
		if (Icon)
		{
			Label = bShowQuantity ? FText::AsNumber(Quantity) : FText::GetEmpty();
		}
		else
		{
			const FString FallbackLabel = bShowQuantity
				? FString::Printf(TEXT("%d×%d · %d"), Footprint.X, Footprint.Y, Quantity)
				: FString::Printf(TEXT("%d×%d"), Footprint.X, Footprint.Y);
			Label = FText::FromString(FallbackLabel);
		}
		UI_InventoryItemQuantityText->SetText(Label);
	}
	SetPlacementPreview(false, true);
}

void URogue10mInventoryItemWidget::SetPlacementPreview(bool bPreviewing, bool bCanPlace)
{
	if (UI_InventoryItemPreviewBorder)
	{
		FLinearColor ItemTint = ItemData ? ItemData->InventoryTint : FLinearColor(0.05f, 0.05f, 0.05f, 0.35f);
		if (!bPreviewing && ItemData && ItemData->InventoryIcon.IsValid())
		{
			ItemTint.A = FMath::Min(ItemTint.A, 0.18f);
		}
		UI_InventoryItemPreviewBorder->SetBrushColor(bPreviewing
			? (bCanPlace ? FLinearColor(0.1f, 0.8f, 0.2f, 0.45f) : FLinearColor(0.9f, 0.08f, 0.05f, 0.55f))
			: ItemTint);
	}
}


FReply URogue10mInventoryItemWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemData && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const FVector2D Local = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		const FIntPoint Size = ItemData->GetClampedInventorySize();
		DragGrabOffset = FIntPoint(
			FMath::Clamp(FMath::FloorToInt(Local.X / CellSize), 0, Size.X - 1),
			FMath::Clamp(FMath::FloorToInt(Local.Y / CellSize), 0, Size.Y - 1));
		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URogue10mInventoryItemWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if (!Inventory || !InventoryWindow || !ItemData || !InstanceId.IsValid())
	{
		return;
	}

	URogue10mItemDragDropOperation* Operation = NewObject<URogue10mItemDragDropOperation>(this);
	Operation->InitializeGridItemDrag(Inventory, InventoryWindow, ContainerIndex, InstanceId,
		ItemData, CellSize);
	Operation->Pivot = EDragPivot::MouseDown;
	Operation->GrabCellOffset = DragGrabOffset;

	URogue10mInventoryItemWidget* Preview = CreateWidget<URogue10mInventoryItemWidget>(
		GetOwningPlayer(), GetClass());
	if (Preview)
	{
		FRogue10mInventoryGridEntry PreviewEntry;
		PreviewEntry.InstanceId = InstanceId;
		PreviewEntry.ItemData = ItemData;
		PreviewEntry.Quantity = Quantity;
		Preview->InitializeGridItem(Inventory, InventoryWindow, ContainerIndex, PreviewEntry, CellSize);
		Preview->SetPlacementPreview(true, true);
		Preview->SetVisibility(ESlateVisibility::HitTestInvisible);
		Operation->PreviewWidget = Preview;
		Operation->DefaultDragVisual = Preview;
	}
	InventoryWindow->BeginGridItemDrag(Operation);
	OutOperation = Operation;
}

void URogue10mBagTabWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UI_BagTabButton)
	{
		UI_BagTabButton->OnClicked.AddUniqueDynamic(this, &URogue10mBagTabWidget::HandleBagTabClicked);
	}
}

void URogue10mBagTabWidget::InitializeBagTab(
	URogue10mInventoryWindowWidget* InInventoryWindow,
	int32 InContainerIndex,
	const FText& DisplayName,
	bool bSelected)
{
	InventoryWindow = InInventoryWindow;
	ContainerIndex = InContainerIndex;
	if (UI_BagNameText)
	{
		UI_BagNameText->SetText(DisplayName);
	}
	SetRenderOpacity(bSelected ? 1.0f : 0.55f);
}

void URogue10mBagTabWidget::HandleBagTabClicked()
{
	// 현재 인벤토리 화면은 기본 컨테이너 하나로 고정되어 있습니다.
}
void URogue10mInventoryWindowWidget::InitializeMenuWindow(
	URogue10mInventoryComponent* InInventoryComponent)
{
	if (URogue10mInventoryComponent* PreviousInventory = GetInventoryComponent())
	{
		PreviousInventory->OnInventoryGridChanged.RemoveDynamic(
			this, &URogue10mInventoryWindowWidget::HandleInventoryGridChanged);
	}

	Super::InitializeMenuWindow(InInventoryComponent);
	if (InInventoryComponent)
	{
		InInventoryComponent->OnInventoryGridChanged.AddUniqueDynamic(
			this, &URogue10mInventoryWindowWidget::HandleInventoryGridChanged);
	}
	RefreshInventoryDisplay();
}

void URogue10mInventoryWindowWidget::SetWindowOpen(bool bOpen)
{
	Super::SetWindowOpen(bOpen);
	if (bOpen)
	{
		RefreshInventoryDisplay();
	}
}

void URogue10mInventoryWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (URogue10mInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->OnInventoryGridChanged.AddUniqueDynamic(
			this, &URogue10mInventoryWindowWidget::HandleInventoryGridChanged);
	}
	RefreshInventoryDisplay();
}

void URogue10mInventoryWindowWidget::NativeDestruct()
{
	if (URogue10mInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->OnInventoryGridChanged.RemoveDynamic(
			this, &URogue10mInventoryWindowWidget::HandleInventoryGridChanged);
	}
	Super::NativeDestruct();
}

void URogue10mInventoryWindowWidget::BeginGridItemDrag(URogue10mItemDragDropOperation* Operation)
{
	ActiveGridDragOperation = Operation;
	SetIsFocusable(true);
	SetFocus();
}

void URogue10mInventoryWindowWidget::EndGridItemDrag(URogue10mItemDragDropOperation* Operation)
{
	if (!Operation || ActiveGridDragOperation.Get() == Operation)
	{
		ClearGridDropPreview();
		ActiveGridDragOperation.Reset();
	}
}


bool URogue10mInventoryWindowWidget::NativeOnDragOver(const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	URogue10mItemDragDropOperation* Operation = Cast<URogue10mItemDragDropOperation>(InOperation);
	return Operation && Operation->Source == ERogue10mItemDragSource::GridInventory
		? UpdateGridDropPreview(InDragDropEvent, Operation)
		: Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool URogue10mInventoryWindowWidget::NativeOnDrop(const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	URogue10mItemDragDropOperation* Operation = Cast<URogue10mItemDragDropOperation>(InOperation);
	URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Operation || Operation->Source != ERogue10mItemDragSource::GridInventory || !Inventory)
	{
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}
	UpdateGridDropPreview(InDragDropEvent, Operation);
	const bool bMoved = Operation->bPreviewCanPlace && Inventory->TryMoveGridItem(Operation->SourceContainerIndex,
		Operation->InstanceId, PrimaryContainerIndex, Operation->PreviewGridPosition);
	if (bMoved)
	{
		Operation->MarkDropHandled();
	}
	EndGridItemDrag(Operation);
	return bMoved;
}

void URogue10mInventoryWindowWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	ClearGridDropPreview();
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool URogue10mInventoryWindowWidget::UpdateGridDropPreview(const FDragDropEvent& DragDropEvent,
	URogue10mItemDragDropOperation* Operation)
{
	URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory || !UI_InventoryItemCanvas || !Operation || !Operation->ItemData)
	{
		return false;
	}
	const FVector2D Local = UI_InventoryItemCanvas->GetCachedGeometry().AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
	Operation->PreviewGridPosition = FIntPoint(
		FMath::FloorToInt(Local.X / InventoryCellSize) - Operation->GrabCellOffset.X,
		FMath::FloorToInt(Local.Y / InventoryCellSize) - Operation->GrabCellOffset.Y);
	const FGuid IgnoredId = Operation->SourceContainerIndex == PrimaryContainerIndex ? Operation->InstanceId : FGuid();
	Operation->bPreviewCanPlace = Inventory->CanPlaceGridItem(PrimaryContainerIndex, Operation->ItemData,
		Operation->PreviewGridPosition, IgnoredId);
	if (Operation->PreviewWidget)
	{
		Operation->PreviewWidget->SetPlacementPreview(true, Operation->bPreviewCanPlace);
	}
	return true;
}

void URogue10mInventoryWindowWidget::ClearGridDropPreview()
{
	if (URogue10mItemDragDropOperation* Operation = ActiveGridDragOperation.Get())
	{
		Operation->bPreviewCanPlace = false;
		if (Operation->PreviewWidget)
		{
			Operation->PreviewWidget->SetPlacementPreview(false, false);
		}
	}
}
void URogue10mInventoryWindowWidget::HandleInventoryGridChanged()
{
	RefreshInventoryDisplay();
}

void URogue10mInventoryWindowWidget::RefreshInventoryDisplay()
{
	URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory)
	{
		return;
	}
	const TArray<FRogue10mInventoryContainer>& Containers = Inventory->GetInventoryContainers();
	if (Containers.IsValidIndex(PrimaryContainerIndex))
	{
		RebuildInventoryCells(Containers[PrimaryContainerIndex].GridSize);
		RebuildInventoryItems();
	}
	if (UI_InventoryMoneyText)
	{
		UI_InventoryMoneyText->SetText(FText::Format(NSLOCTEXT("Rogue10mInventory", "InventoryMoney", "Gold: {0}"), FText::AsNumber(Inventory->GetGold())));
	}
	if (UI_InventoryWeightText)
	{
		FNumberFormattingOptions Format;
		Format.MinimumFractionalDigits = 1;
		Format.MaximumFractionalDigits = 1;
		FFormatNamedArguments Args;
		Args.Add(TEXT("Current"), FText::AsNumber(Inventory->GetTotalInventoryWeight(), &Format));
		Args.Add(TEXT("Maximum"), FText::AsNumber(Inventory->GetMaxCarryWeight(), &Format));
		UI_InventoryWeightText->SetText(FText::Format(NSLOCTEXT("Rogue10mInventory", "InventoryWeight", "Weight: {Current} / {Maximum} kg"), Args));
	}
}
void URogue10mInventoryWindowWidget::RebuildInventoryCells(FIntPoint GridSize)
{
	if (!UI_InventoryGrid)
	{
		return;
	}

	GridSize.X = FMath::Clamp(GridSize.X, 1, 20);
	GridSize.Y = FMath::Clamp(GridSize.Y, 1, 20);
	if (GridSize == CachedGridSize && UI_InventoryGrid->GetChildrenCount() == GridSize.X * GridSize.Y)
	{
		return;
	}

	UI_InventoryGrid->ClearChildren();
	CachedGridSize = GridSize;
	if (!InventoryCellWidgetClass)
	{
		UE_LOG(LogRogue10m, Warning,
			TEXT("InventoryCellWidgetClass가 설정되지 않아 인벤토리 셀을 생성하지 못했습니다."));
		return;
	}

	for (int32 Y = 0; Y < GridSize.Y; ++Y)
	{
		for (int32 X = 0; X < GridSize.X; ++X)
		{
			URogue10mInventoryCellWidget* Cell = CreateWidget<URogue10mInventoryCellWidget>(
				GetOwningPlayer(), InventoryCellWidgetClass);
			if (!Cell)
			{
				continue;
			}

			Cell->SetGridPosition(FIntPoint(X, Y));
			UI_InventoryGrid->AddChildToUniformGrid(Cell, Y, X);
		}
	}
}

void URogue10mInventoryWindowWidget::RebuildInventoryItems()
{
	if (!UI_InventoryItemCanvas)
	{
		return;
	}
	UI_InventoryItemCanvas->ClearChildren();
	URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory || !InventoryItemWidgetClass || !Inventory->GetInventoryContainers().IsValidIndex(PrimaryContainerIndex))
	{
		return;
	}
	const FRogue10mInventoryContainer& Container = Inventory->GetInventoryContainers()[PrimaryContainerIndex];
	for (const FRogue10mInventoryGridEntry& Entry : Container.Entries)
	{
		if (!Entry.ItemData)
		{
			continue;
		}
		URogue10mInventoryItemWidget* ItemWidget = CreateWidget<URogue10mInventoryItemWidget>(GetOwningPlayer(), InventoryItemWidgetClass);
		if (!ItemWidget)
		{
			continue;
		}
		ItemWidget->InitializeGridItem(Inventory, this, PrimaryContainerIndex, Entry, InventoryCellSize);
		UCanvasPanelSlot* CanvasSlot = UI_InventoryItemCanvas->AddChildToCanvas(ItemWidget);
		const FIntPoint Size = Entry.ItemData->GetClampedInventorySize();
		CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
		CanvasSlot->SetAlignment(FVector2D::ZeroVector);
		CanvasSlot->SetAutoSize(false);
		CanvasSlot->SetPosition(FVector2D(Entry.Position.X * InventoryCellSize, Entry.Position.Y * InventoryCellSize));
		CanvasSlot->SetSize(FVector2D(Size.X * InventoryCellSize, Size.Y * InventoryCellSize));
	}
}


void URogue10mEquipmentWindowWidget::InitializeMenuWindow(URogue10mInventoryComponent* InInventoryComponent)
{
	if (URogue10mInventoryComponent* PreviousInventory = GetInventoryComponent())
	{
		PreviousInventory->OnEquipmentChanged.RemoveDynamic(
			this, &URogue10mEquipmentWindowWidget::HandleEquipmentChanged);
	}
	Super::InitializeMenuWindow(InInventoryComponent);
	if (URogue10mInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->OnEquipmentChanged.AddUniqueDynamic(
			this, &URogue10mEquipmentWindowWidget::HandleEquipmentChanged);
	}
	RefreshEquipmentDisplay();
}

void URogue10mEquipmentWindowWidget::SetWindowOpen(bool bOpen)
{
	Super::SetWindowOpen(bOpen);
	if (bOpen)
	{
		RefreshEquipmentDisplay();
	}
}

void URogue10mEquipmentWindowWidget::NativeDestruct()
{
	if (URogue10mInventoryComponent* Inventory = GetInventoryComponent())
	{
		Inventory->OnEquipmentChanged.RemoveDynamic(
			this, &URogue10mEquipmentWindowWidget::HandleEquipmentChanged);
	}
	Super::NativeDestruct();
}

void URogue10mEquipmentWindowWidget::HandleEquipmentChanged()
{
	RefreshEquipmentDisplay();
}

void URogue10mEquipmentWindowWidget::RefreshEquipmentDisplay()
{
	URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory)
	{
		return;
	}

	const auto FindSlot = [Inventory](ERogue10mInventorySlotType SlotType) -> const FRogue10mInventorySlot*
	{
		for (const FRogue10mInventorySlot& Slot : Inventory->GetLeftEquipmentSlots())
		{
			if (Slot.SlotType == SlotType)
			{
				return &Slot;
			}
		}
		for (const FRogue10mInventorySlot& Slot : Inventory->GetRightEquipmentSlots())
		{
			if (Slot.SlotType == SlotType)
			{
				return &Slot;
			}
		}
		return nullptr;
	};

	const auto SetSlotIcon = [](UImage* Image, const FRogue10mInventorySlot* EquipmentSlot)
	{
		if (!Image)
		{
			return;
		}
		const URogue10mItemDataAsset* ItemData = EquipmentSlot && EquipmentSlot->bHasEquippedItem
			? EquipmentSlot->EquippedItem.ItemData.Get()
			: nullptr;
		UTexture2D* Icon = ItemData ? ItemData->InventoryIcon.LoadSynchronous() : nullptr;
		Image->SetBrushFromTexture(Icon, true);
		if (!Icon)
		{
			Image->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		const float IconScale = FMath::Clamp(ItemData->InventoryIconScale, 0.1f, 2.0f);
		Image->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		Image->SetRenderScale(FVector2D(IconScale, IconScale));
		Image->SetVisibility(ESlateVisibility::HitTestInvisible);
	};

	const FRogue10mInventorySlot* AccessorySlot = FindSlot(ERogue10mInventorySlotType::Ring);
	if (!AccessorySlot || !AccessorySlot->bHasEquippedItem)
	{
		AccessorySlot = FindSlot(ERogue10mInventorySlotType::Earring);
	}

	SetSlotIcon(UI_WeaponSlotIcon, FindSlot(ERogue10mInventorySlotType::MainWeapon));
	SetSlotIcon(UI_HeadSlotIcon, FindSlot(ERogue10mInventorySlotType::Head));
	SetSlotIcon(UI_ChestSlotIcon, FindSlot(ERogue10mInventorySlotType::Armor));
	SetSlotIcon(UI_HandsSlotIcon, FindSlot(ERogue10mInventorySlotType::Hands));
	SetSlotIcon(UI_LegsSlotIcon, FindSlot(ERogue10mInventorySlotType::Legs));
	SetSlotIcon(UI_FeetSlotIcon, FindSlot(ERogue10mInventorySlotType::Shoes));
	SetSlotIcon(UI_AccessorySlotIcon, AccessorySlot);
}
void URogue10mSkillTreeEntryWidget::SetSkillData(URogue10mAttackSkillData* InSkillData, bool bInUnlocked)
{
	SkillData = InSkillData;
	bUnlocked = bInUnlocked;
	SetIsEnabled(bUnlocked);
	SetRenderOpacity(bUnlocked ? 1.0f : 0.35f);
	SetToolTipText(SkillData ? SkillData->SkillDescription : FText::GetEmpty());

	if (UI_SkillNameText)
	{
		UI_SkillNameText->SetText(SkillData ? SkillData->SkillName : FText::GetEmpty());
	}
	if (UI_SkillDescriptionText)
	{
		UI_SkillDescriptionText->SetText(SkillData ? SkillData->SkillDescription : FText::GetEmpty());
	}
	if (UI_SkillLockText)
	{
		UI_SkillLockText->SetVisibility(bUnlocked ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
	}
	if (UI_SkillIconImage)
	{
		UI_SkillIconImage->SetBrushFromTexture(SkillData ? SkillData->SkillIcon : nullptr);
		UI_SkillIconImage->SetColorAndOpacity(SkillData ? SkillData->IconTint : FLinearColor::White);
	}
}

FReply URogue10mSkillTreeEntryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bUnlocked && SkillData && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URogue10mSkillTreeEntryWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if (!bUnlocked || !SkillData)
	{
		return;
	}

	URogue10mSkillDragDropOperation* Operation = NewObject<URogue10mSkillDragDropOperation>(this);
	Operation->SkillData = SkillData;
	Operation->bUnlocked = true;
	Operation->Pivot = EDragPivot::MouseDown;
	OutOperation = Operation;
}

void URogue10mSkillTreeWindowWidget::InitializeMenuWindow(
	URogue10mInventoryComponent* InInventoryComponent)
{
	Super::InitializeMenuWindow(InInventoryComponent);
	RefreshSkillTree();
}

void URogue10mSkillTreeWindowWidget::SetWindowOpen(bool bOpen)
{
	Super::SetWindowOpen(bOpen);
	if (bOpen)
	{
		RefreshSkillTree();
	}
}

void URogue10mSkillTreeWindowWidget::RefreshSkillTree()
{
	URogue10mCombatComponent* Combat = GetCombatComponent();
	if (!UI_SkillListContainer || !SkillTreeEntryWidgetClass || !Combat)
	{
		return;
	}

	UI_SkillListContainer->ClearChildren();
	for (URogue10mAttackSkillData* Skill : Combat->GetActiveSkillTreeSkills())
	{
		URogue10mSkillTreeEntryWidget* Entry = CreateWidget<URogue10mSkillTreeEntryWidget>(
			GetOwningPlayer(), SkillTreeEntryWidgetClass);
		if (!Entry)
		{
			continue;
		}
		Entry->SetSkillData(Skill, Combat->IsAttackSkillUnlocked(Skill));
		UI_SkillListContainer->AddChild(Entry);
	}
}

URogue10mCombatComponent* URogue10mSkillTreeWindowWidget::GetCombatComponent() const
{
	const ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwningPlayerPawn());
	return Character ? Character->GetCombatComponent() : nullptr;
}
