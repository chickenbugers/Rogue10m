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
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
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
	bRotatedClockwise = InEntry.bRotatedClockwise;
	CellSize = InCellSize;

	if (UI_InventoryItemIcon)
	{
		UI_InventoryItemIcon->SetBrushFromTexture(ItemData ? ItemData->InventoryIcon.LoadSynchronous() : nullptr);
	}
	if (UI_InventoryItemQuantityText)
	{
		UI_InventoryItemQuantityText->SetText(Quantity > 1 ? FText::AsNumber(Quantity) : FText::GetEmpty());
	}
	SetPlacementPreview(false, true);
}

void URogue10mInventoryItemWidget::SetPlacementPreview(bool bPreviewing, bool bCanPlace)
{
	if (UI_InventoryItemPreviewBorder)
	{
		UI_InventoryItemPreviewBorder->SetBrushColor(bPreviewing
			? (bCanPlace ? FLinearColor(0.1f, 0.8f, 0.2f, 0.45f) : FLinearColor(0.9f, 0.08f, 0.05f, 0.55f))
			: (ItemData ? ItemData->InventoryTint : FLinearColor(0.05f, 0.05f, 0.05f, 0.35f)));
	}
}

void URogue10mInventoryItemWidget::SetPreviewSize(FIntPoint GridSize, float InCellSize)
{
	SetRenderScale(FVector2D(FMath::Max(1, GridSize.X), FMath::Max(1, GridSize.Y)));
	CellSize = InCellSize;
}

FReply URogue10mInventoryItemWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemData && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const FVector2D Local = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		const FIntPoint BaseSize = ItemData->GetClampedInventorySize();
		const FIntPoint Size = bRotatedClockwise ? FIntPoint(BaseSize.Y, BaseSize.X) : BaseSize;
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
		ItemData, bRotatedClockwise, CellSize);
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
		PreviewEntry.bRotatedClockwise = bRotatedClockwise;
		Preview->InitializeGridItem(Inventory, InventoryWindow, ContainerIndex, PreviewEntry, CellSize);
		Preview->SetPlacementPreview(true, true);
		Preview->SetPreviewSize(Operation->GetFootprint(), CellSize);
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
	if (InventoryWindow)
	{
		InventoryWindow->SelectInventoryContainer(ContainerIndex);
	}
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

void URogue10mInventoryWindowWidget::SelectInventoryContainer(int32 ContainerIndex)
{
	const URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (Inventory && Inventory->GetInventoryContainers().IsValidIndex(ContainerIndex))
	{
		DisplayedContainerIndex = ContainerIndex;
		CachedGridSize = FIntPoint::ZeroValue;
		RefreshInventoryDisplay();
	}
}

FReply URogue10mInventoryWindowWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	URogue10mItemDragDropOperation* Operation = ActiveGridDragOperation.Get();
	if (Operation && InKeyEvent.GetKey() == EKeys::R)
	{
		Operation->RotateClockwise();
		if (URogue10mInventoryComponent* Inventory = GetInventoryComponent())
		{
			const FGuid IgnoredId = Operation->SourceContainerIndex == DisplayedContainerIndex ? Operation->InstanceId : FGuid();
			Operation->bPreviewCanPlace = Inventory->CanPlaceGridItem(DisplayedContainerIndex, Operation->ItemData,
				Operation->PreviewGridPosition, IgnoredId, Operation->bRotatedClockwise);
			if (Operation->PreviewWidget)
			{
				Operation->PreviewWidget->SetPlacementPreview(true, Operation->bPreviewCanPlace);
			}
		}
		return FReply::Handled();
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
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
		Operation->InstanceId, DisplayedContainerIndex, Operation->PreviewGridPosition, Operation->bRotatedClockwise);
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
	const FGuid IgnoredId = Operation->SourceContainerIndex == DisplayedContainerIndex ? Operation->InstanceId : FGuid();
	Operation->bPreviewCanPlace = Inventory->CanPlaceGridItem(DisplayedContainerIndex, Operation->ItemData,
		Operation->PreviewGridPosition, IgnoredId, Operation->bRotatedClockwise);
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
	if (!Containers.IsValidIndex(DisplayedContainerIndex))
	{
		DisplayedContainerIndex = 0;
	}
	if (Containers.IsValidIndex(DisplayedContainerIndex))
	{
		RebuildInventoryCells(Containers[DisplayedContainerIndex].GridSize);
		RebuildBagTabs();
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
	if (!Inventory || !InventoryItemWidgetClass || !Inventory->GetInventoryContainers().IsValidIndex(DisplayedContainerIndex))
	{
		return;
	}
	const FRogue10mInventoryContainer& Container = Inventory->GetInventoryContainers()[DisplayedContainerIndex];
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
		ItemWidget->InitializeGridItem(Inventory, this, DisplayedContainerIndex, Entry, InventoryCellSize);
		UCanvasPanelSlot* CanvasSlot = UI_InventoryItemCanvas->AddChildToCanvas(ItemWidget);
		const FIntPoint BaseSize = Entry.ItemData->GetClampedInventorySize();
		const FIntPoint Size = Entry.bRotatedClockwise ? FIntPoint(BaseSize.Y, BaseSize.X) : BaseSize;
		CanvasSlot->SetPosition(FVector2D(Entry.Position.X * InventoryCellSize, Entry.Position.Y * InventoryCellSize));
		CanvasSlot->SetSize(FVector2D(Size.X * InventoryCellSize, Size.Y * InventoryCellSize));
	}
}

void URogue10mInventoryWindowWidget::RebuildBagTabs()
{
	if (!UI_BagTabContainer)
	{
		return;
	}
	UI_BagTabContainer->ClearChildren();
	const URogue10mInventoryComponent* Inventory = GetInventoryComponent();
	if (!Inventory || !BagTabWidgetClass)
	{
		return;
	}
	const TArray<FRogue10mInventoryContainer>& Containers = Inventory->GetInventoryContainers();
	for (int32 Index = 0; Index < Containers.Num(); ++Index)
	{
		URogue10mBagTabWidget* Tab = CreateWidget<URogue10mBagTabWidget>(GetOwningPlayer(), BagTabWidgetClass);
		if (Tab)
		{
			Tab->InitializeBagTab(this, Index, Containers[Index].DisplayName, Index == DisplayedContainerIndex);
			UI_BagTabContainer->AddChild(Tab);
		}
	}
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
