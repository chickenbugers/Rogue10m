// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mMenuWindowWidgets.h"

#include "Rogue10mInventoryComponent.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCombatComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WrapBox.h"
#include "InputCoreTypes.h"
#include "Widgets/Rogue10mSkillDragDropOperation.h"

namespace
{
	const FLinearColor MenuFrameColor(0.02f, 0.025f, 0.03f, 0.96f);
	const FLinearColor MenuSectionColor(0.055f, 0.06f, 0.07f, 0.96f);
	const FLinearColor MenuCellColor(0.075f, 0.08f, 0.09f, 0.92f);
	const FLinearColor MenuAccentColor(0.72f, 0.58f, 0.28f, 1.0f);
	const FLinearColor MenuTextColor(0.92f, 0.90f, 0.84f, 1.0f);
	const FLinearColor MenuMutedTextColor(0.60f, 0.61f, 0.63f, 1.0f);

	UTextBlock* CreateMenuText(
		UWidgetTree* WidgetTree, const FName Name, const FText& Text,
		const int32 FontSize, const FLinearColor& Color = MenuTextColor)
	{
		UTextBlock* TextBlock = WidgetTree
			? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name)
			: nullptr;
		if (!TextBlock)
		{
			return nullptr;
		}

		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetAutoWrapText(true);
		FSlateFontInfo Font = TextBlock->GetFont();
		Font.Size = FontSize;
		TextBlock->SetFont(Font);
		return TextBlock;
	}

	UBorder* CreateMenuPanel(UWidgetTree* WidgetTree, const FName Name, const FLinearColor& Color, const FMargin Padding)
	{
		UBorder* Border = WidgetTree
			? WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name)
			: nullptr;
		if (Border)
		{
			Border->SetBrushColor(Color);
			Border->SetPadding(Padding);
		}
		return Border;
	}

	void AddMenuRow(UVerticalBox* Parent, UWidget* Child, const FMargin Padding = FMargin(4.0f))
	{
		if (Parent && Child)
		{
			if (UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child))
			{
				Slot->SetPadding(Padding);
			}
		}
	}

	void AddHorizontalItem(UHorizontalBox* Parent, UWidget* Child, const FMargin Padding = FMargin(4.0f))
	{
		if (Parent && Child)
		{
			if (UHorizontalBoxSlot* Slot = Parent->AddChildToHorizontalBox(Child))
			{
				Slot->SetPadding(Padding);
			}
		}
	}

	void AddCenteredWindow(UCanvasPanel* RootCanvas, UWidget* Window, const FVector2D& Size)
	{
		if (!RootCanvas || !Window)
		{
			return;
		}

		if (UCanvasPanelSlot* Slot = RootCanvas->AddChildToCanvas(Window))
		{
			Slot->SetAnchors(FAnchors(0.5f));
			Slot->SetAlignment(FVector2D(0.5f));
			Slot->SetPosition(FVector2D::ZeroVector);
			Slot->SetSize(Size);
		}
	}

	UBorder* CreateEquipmentSlot(UWidgetTree* WidgetTree, const FName Name, const FText& Label)
	{
		UBorder* SlotFrame = CreateMenuPanel(WidgetTree, Name, MenuCellColor, FMargin(10.0f, 8.0f));
		if (SlotFrame)
		{
			SlotFrame->SetContent(CreateMenuText(
				WidgetTree, FName(*(Name.ToString() + TEXT("_Text"))), Label, 13, MenuMutedTextColor));
		}
		return SlotFrame;
	}
}

TSharedRef<SWidget> URogue10mInventoryWindowWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(), TEXT("UI_InventoryCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UBorder* WindowFrame = CreateMenuPanel(
			WidgetTree, TEXT("UI_InventoryWindowFrame"), MenuFrameColor, FMargin(16.0f));
		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_InventoryContent"));
		if (WindowFrame)
		{
			WindowFrame->SetContent(Content);
		}

		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_InventoryTitleText"),
			NSLOCTEXT("Rogue10mMenu", "InventoryTitle", "인벤토리"), 24, MenuAccentColor));

		UI_BagTabContainer = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(), TEXT("UI_BagTabContainer"));
		AddHorizontalItem(UI_BagTabContainer, CreateMenuText(
			WidgetTree, TEXT("UI_DefaultBagTabText"),
			NSLOCTEXT("Rogue10mMenu", "DefaultBagTab", "기본 가방 10 x 10"), 13));
		AddHorizontalItem(UI_BagTabContainer, CreateMenuText(
			WidgetTree, TEXT("UI_AddBagTabText"),
			NSLOCTEXT("Rogue10mMenu", "AddBagTab", "+ 가방 확장"), 12, MenuMutedTextColor));
		AddMenuRow(Content, UI_BagTabContainer, FMargin(2.0f, 8.0f, 2.0f, 4.0f));

		UI_InventoryCapacityText = CreateMenuText(
			WidgetTree, TEXT("UI_InventoryCapacityText"),
			NSLOCTEXT("Rogue10mMenu", "InventoryCapacity", "사용 공간 0 / 100"), 13, MenuMutedTextColor);
		AddMenuRow(Content, UI_InventoryCapacityText);

		UOverlay* GridOverlay = WidgetTree->ConstructWidget<UOverlay>(
			UOverlay::StaticClass(), TEXT("UI_InventoryGridOverlay"));
		UI_InventoryGrid = WidgetTree->ConstructWidget<UUniformGridPanel>(
			UUniformGridPanel::StaticClass(), TEXT("UI_InventoryGrid"));
		if (UI_InventoryGrid)
		{
			UI_InventoryGrid->SetMinDesiredSlotWidth(44.0f);
			UI_InventoryGrid->SetMinDesiredSlotHeight(44.0f);
			for (int32 Row = 0; Row < 10; ++Row)
			{
				for (int32 Column = 0; Column < 10; ++Column)
				{
					const FName CellName(*FString::Printf(TEXT("UI_InventoryCell_%02d_%02d"), Row, Column));
					UBorder* Cell = CreateMenuPanel(WidgetTree, CellName, MenuCellColor, FMargin(1.0f));
					UI_InventoryGrid->AddChildToUniformGrid(Cell, Row, Column);
				}
			}
		}
		if (UOverlaySlot* GridSlot = GridOverlay->AddChildToOverlay(UI_InventoryGrid))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}

		UI_InventoryItemCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(), TEXT("UI_InventoryItemCanvas"));
		if (UOverlaySlot* ItemSlot = GridOverlay->AddChildToOverlay(UI_InventoryItemCanvas))
		{
			ItemSlot->SetHorizontalAlignment(HAlign_Fill);
			ItemSlot->SetVerticalAlignment(VAlign_Fill);
		}
		AddMenuRow(Content, GridOverlay, FMargin(4.0f, 8.0f));

		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_InventoryHintText"),
			NSLOCTEXT("Rogue10mMenu", "InventoryHint", "드래그: 이동 | R: 회전 | 바깥에 놓기: 월드에 버리기"),
			11, MenuMutedTextColor));
		AddCenteredWindow(RootCanvas, WindowFrame, FVector2D(560.0f, 610.0f));
	}
	return Super::RebuildWidget();
}

TSharedRef<SWidget> URogue10mEquipmentWindowWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(), TEXT("UI_EquipmentCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UBorder* WindowFrame = CreateMenuPanel(
			WidgetTree, TEXT("UI_EquipmentWindowFrame"), MenuFrameColor, FMargin(16.0f));
		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_EquipmentContent"));
		WindowFrame->SetContent(Content);
		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_EquipmentTitleText"),
			NSLOCTEXT("Rogue10mMenu", "EquipmentTitle", "장비"), 24, MenuAccentColor));

		UHorizontalBox* Body = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(), TEXT("UI_EquipmentBody"));
		UVerticalBox* PreviewColumn = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_CharacterPreviewColumn"));
		UBorder* PreviewFrame = CreateMenuPanel(
			WidgetTree, TEXT("UI_CharacterPreviewFrame"), MenuSectionColor, FMargin(8.0f));
		UI_CharacterPreviewImage = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(), TEXT("UI_CharacterPreviewImage"));
		UI_CharacterPreviewImage->SetDesiredSizeOverride(FVector2D(260.0f, 350.0f));
		PreviewFrame->SetContent(UI_CharacterPreviewImage);
		AddMenuRow(PreviewColumn, PreviewFrame);
		AddMenuRow(PreviewColumn, CreateMenuText(
			WidgetTree, TEXT("UI_CharacterPreviewGuideText"),
			NSLOCTEXT("Rogue10mMenu", "CharacterPreviewGuide", "SceneCapture / RenderTarget 연결 영역"),
			11, MenuMutedTextColor));
		AddHorizontalItem(Body, PreviewColumn, FMargin(4.0f, 8.0f, 12.0f, 4.0f));

		UI_EquipmentSlotContainer = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_EquipmentSlotContainer"));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_WeaponSlotFrame"), NSLOCTEXT("Rogue10mMenu", "WeaponSlot", "무기")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_HeadSlotFrame"), NSLOCTEXT("Rogue10mMenu", "HeadSlot", "머리")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_ChestSlotFrame"), NSLOCTEXT("Rogue10mMenu", "ChestSlot", "상의")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_HandsSlotFrame"), NSLOCTEXT("Rogue10mMenu", "HandsSlot", "장갑")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_LegsSlotFrame"), NSLOCTEXT("Rogue10mMenu", "LegsSlot", "하의")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_FeetSlotFrame"), NSLOCTEXT("Rogue10mMenu", "FeetSlot", "신발")));
		AddMenuRow(UI_EquipmentSlotContainer, CreateEquipmentSlot(
			WidgetTree, TEXT("UI_AccessorySlotFrame"), NSLOCTEXT("Rogue10mMenu", "AccessorySlot", "장신구")));
		AddHorizontalItem(Body, UI_EquipmentSlotContainer, FMargin(4.0f, 8.0f));
		AddMenuRow(Content, Body);

		UI_EquipmentStatsContainer = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_EquipmentStatsContainer"));
		AddMenuRow(UI_EquipmentStatsContainer, CreateMenuText(
			WidgetTree, TEXT("UI_AttackStatText"), NSLOCTEXT("Rogue10mMenu", "AttackStat", "공격력 0"), 12));
		AddMenuRow(UI_EquipmentStatsContainer, CreateMenuText(
			WidgetTree, TEXT("UI_DefenseStatText"), NSLOCTEXT("Rogue10mMenu", "DefenseStat", "방어력 0"), 12));
		AddMenuRow(Content, UI_EquipmentStatsContainer, FMargin(4.0f, 8.0f));
		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_EquipmentHintText"),
			NSLOCTEXT("Rogue10mMenu", "EquipmentHint", "P: 열기 / 닫기 | 인벤토리와 Drag & Drop"),
			11, MenuMutedTextColor));
		AddCenteredWindow(RootCanvas, WindowFrame, FVector2D(700.0f, 610.0f));
	}
	return Super::RebuildWidget();
}

TSharedRef<SWidget> URogue10mSkillTreeEntryWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UBorder* EntryFrame = CreateMenuPanel(
			WidgetTree, TEXT("UI_SkillTreeEntryFrame"), MenuSectionColor, FMargin(8.0f));
		WidgetTree->RootWidget = EntryFrame;
		UHorizontalBox* Root = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(), TEXT("UI_SkillTreeEntryRoot"));
		EntryFrame->SetContent(Root);

		UI_SkillIconImage = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(), TEXT("UI_SkillIconImage"));
		UI_SkillIconImage->SetDesiredSizeOverride(FVector2D(64.0f));
		AddHorizontalItem(Root, UI_SkillIconImage, FMargin(4.0f, 4.0f, 10.0f, 4.0f));

		UVerticalBox* TextColumn = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_SkillTextColumn"));
		UI_SkillNameText = CreateMenuText(
			WidgetTree, TEXT("UI_SkillNameText"),
			NSLOCTEXT("Rogue10mMenu", "EmptySkillName", "스킬"), 15, MenuAccentColor);
		AddMenuRow(TextColumn, UI_SkillNameText, FMargin(2.0f));
		UI_SkillDescriptionText = CreateMenuText(
			WidgetTree, TEXT("UI_SkillDescriptionText"), FText::GetEmpty(), 11, MenuMutedTextColor);
		AddMenuRow(TextColumn, UI_SkillDescriptionText, FMargin(2.0f));
		UI_SkillLockText = CreateMenuText(
			WidgetTree, TEXT("UI_SkillLockText"),
			NSLOCTEXT("Rogue10mMenu", "SkillLocked", "잠김"), 11, FLinearColor(0.85f, 0.30f, 0.24f, 1.0f));
		AddMenuRow(TextColumn, UI_SkillLockText, FMargin(2.0f));
		AddHorizontalItem(Root, TextColumn, FMargin(2.0f));
	}
	return Super::RebuildWidget();
}

TSharedRef<SWidget> URogue10mSkillTreeWindowWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(), TEXT("UI_SkillTreeCanvas"));
		WidgetTree->RootWidget = RootCanvas;
		UBorder* WindowFrame = CreateMenuPanel(
			WidgetTree, TEXT("UI_SkillTreeWindowFrame"), MenuFrameColor, FMargin(16.0f));
		UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(), TEXT("UI_SkillTreeContent"));
		WindowFrame->SetContent(Content);

		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_SkillTreeTitleText"),
			NSLOCTEXT("Rogue10mMenu", "SkillTreeTitle", "스킬 트리"), 24, MenuAccentColor));
		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_SkillTreeGuideText"),
			NSLOCTEXT("Rogue10mMenu", "SkillTreeGuide", "해금된 스킬을 HUD 입력 슬롯으로 Drag & Drop 하세요."),
			12, MenuMutedTextColor));

		UHorizontalBox* FilterBar = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(), TEXT("UI_SkillFilterBar"));
		AddHorizontalItem(FilterBar, CreateMenuText(
			WidgetTree, TEXT("UI_AllSkillsFilterText"), NSLOCTEXT("Rogue10mMenu", "AllSkills", "전체"), 12, MenuAccentColor));
		AddHorizontalItem(FilterBar, CreateMenuText(
			WidgetTree, TEXT("UI_UnlockedSkillsFilterText"), NSLOCTEXT("Rogue10mMenu", "UnlockedSkills", "해금"), 12));
		AddHorizontalItem(FilterBar, CreateMenuText(
			WidgetTree, TEXT("UI_LockedSkillsFilterText"), NSLOCTEXT("Rogue10mMenu", "LockedSkills", "잠김"), 12, MenuMutedTextColor));
		AddMenuRow(Content, FilterBar, FMargin(2.0f, 8.0f));

		UScrollBox* SkillScroll = WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(), TEXT("UI_SkillListScrollBox"));
		UWrapBox* SkillList = WidgetTree->ConstructWidget<UWrapBox>(
			UWrapBox::StaticClass(), TEXT("UI_SkillListContainer"));
		SkillList->SetInnerSlotPadding(FVector2D(8.0f));
		UI_SkillListContainer = SkillList;
		SkillScroll->AddChild(SkillList);
		AddMenuRow(Content, SkillScroll, FMargin(4.0f, 8.0f));

		AddMenuRow(Content, CreateMenuText(
			WidgetTree, TEXT("UI_SkillPointText"),
			NSLOCTEXT("Rogue10mMenu", "SkillPoint", "사용 가능 스킬 포인트: 0"),
			12, MenuMutedTextColor));
		AddCenteredWindow(RootCanvas, WindowFrame, FVector2D(820.0f, 640.0f));
	}
	return Super::RebuildWidget();
}

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