// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mMainHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	const FLinearColor PrototypePanelColor(0.02f, 0.025f, 0.03f, 0.68f);
	const FLinearColor PrototypeAccentColor(0.72f, 0.58f, 0.28f, 1.0f);
	const FLinearColor PrototypeTextColor(0.92f, 0.90f, 0.84f, 1.0f);

	UTextBlock* CreatePrototypeText(UWidgetTree* WidgetTree, FName WidgetName, const FString& Text, float FontSize = 12.0f)
	{
		UTextBlock* TextBlock = WidgetTree ? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName) : nullptr;
		if (!TextBlock)
		{
			return nullptr;
		}

		TextBlock->SetText(FText::FromString(Text));
		TextBlock->SetColorAndOpacity(FSlateColor(PrototypeTextColor));

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = static_cast<int32>(FontSize);
		TextBlock->SetFont(FontInfo);

		return TextBlock;
	}

	UBorder* CreatePrototypePanel(UWidgetTree* WidgetTree, FName WidgetName)
	{
		UBorder* Border = WidgetTree ? WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), WidgetName) : nullptr;
		if (!Border)
		{
			return nullptr;
		}

		Border->SetBrushColor(PrototypePanelColor);
		Border->SetPadding(FMargin(8.0f));
		return Border;
	}

	void AddCanvasChild(UCanvasPanel* RootCanvas, UWidget* Widget, const FVector2D& AnchorsMin, const FVector2D& AnchorsMax, const FVector2D& Position, const FVector2D& Size, const FVector2D& Alignment)
	{
		if (!RootCanvas || !Widget)
		{
			return;
		}

		UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(Widget);
		if (!CanvasSlot)
		{
			return;
		}

		CanvasSlot->SetAnchors(FAnchors(AnchorsMin.X, AnchorsMin.Y, AnchorsMax.X, AnchorsMax.Y));
		CanvasSlot->SetAlignment(Alignment);
		CanvasSlot->SetPosition(Position);
		CanvasSlot->SetSize(Size);
	}
}

void URogue10mMainHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AssignOwningMainHUDToBoundWidgets();
	RefreshBoundWidgetData();
}

void URogue10mMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnsurePrototypeLayout();
}

void URogue10mMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	RefreshBoundWidgetData();

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void URogue10mMainHUDWidget::RefreshBoundWidgetData()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVitalView(FText::FromString(TEXT("체력")), GetHealthView());
	}

	if (StaminaBarWidget)
	{
		StaminaBarWidget->SetVitalView(FText::FromString(TEXT("스테미나")), GetStaminaView());
	}

	if (ManaBarWidget)
	{
		ManaBarWidget->SetVitalView(FText::FromString(TEXT("마나")), GetManaView());
	}

	if (ProgressionWidget)
	{
		ProgressionWidget->SetProgressionView(GetProgressionView());
	}

	if (IdentityWidget)
	{
		IdentityWidget->SetIdentityView(GetIdentityView());
	}

	if (MonsterInfoWidget)
	{
		MonsterInfoWidget->SetMonsterInfoView(GetLookedAtMonsterInfoView());
	}

	const FRogue10mHudPanelStateView PanelState = GetPanelStateView();
	if (EquipmentShortcutWidget)
	{
		EquipmentShortcutWidget->SetShortcutHint(GetEquipmentShortcutText(), FText::FromString(TEXT("장비")), PanelState.bEquipmentVisible);
	}

	if (ItemWindowShortcutWidget)
	{
		ItemWindowShortcutWidget->SetShortcutHint(GetItemWindowShortcutText(), FText::FromString(TEXT("아이템")), PanelState.bItemWindowVisible);
	}

	if (SkillTreeShortcutWidget)
	{
		SkillTreeShortcutWidget->SetShortcutHint(GetSkillTreeShortcutText(), FText::FromString(TEXT("스킬")), PanelState.bSkillTreeVisible);
	}

	if (SettingsShortcutWidget)
	{
		SettingsShortcutWidget->SetShortcutHint(GetSettingsShortcutText(), FText::FromString(TEXT("설정")), PanelState.bSettingsVisible);
	}

	BP_OnBoundWidgetDataRefreshed();
}

void URogue10mMainHUDWidget::AssignOwningMainHUDToBoundWidgets()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetOwningMainHUD(this);
	}

	if (StaminaBarWidget)
	{
		StaminaBarWidget->SetOwningMainHUD(this);
	}

	if (ManaBarWidget)
	{
		ManaBarWidget->SetOwningMainHUD(this);
	}

	if (ProgressionWidget)
	{
		ProgressionWidget->SetOwningMainHUD(this);
	}

	if (IdentityWidget)
	{
		IdentityWidget->SetOwningMainHUD(this);
	}

	if (MonsterInfoWidget)
	{
		MonsterInfoWidget->SetOwningMainHUD(this);
	}

	if (EquipmentShortcutWidget)
	{
		EquipmentShortcutWidget->SetOwningMainHUD(this);
	}

	if (ItemWindowShortcutWidget)
	{
		ItemWindowShortcutWidget->SetOwningMainHUD(this);
	}

	if (SkillTreeShortcutWidget)
	{
		SkillTreeShortcutWidget->SetOwningMainHUD(this);
	}

	if (SettingsShortcutWidget)
	{
		SettingsShortcutWidget->SetOwningMainHUD(this);
	}
}

void URogue10mMainHUDWidget::EnsurePrototypeLayout()
{
	if (!bCreatePrototypeLayoutWhenEmpty || !WidgetTree)
	{
		return;
	}

	// Blueprint에서 대표 위젯을 직접 배치했으면 C++ 임시 골격은 만들지 않는다.
	if (HealthBarWidget || StaminaBarWidget || MonsterInfoWidget || GetWidgetFromName(TEXT("BottomHUDPanel")))
	{
		return;
	}

	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if (!RootCanvas)
	{
		RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel_Root"));
		WidgetTree->RootWidget = RootCanvas;
	}

	if (!RootCanvas || RootCanvas->GetChildrenCount() > 0)
	{
		return;
	}

	UBorder* MonsterPanel = CreatePrototypePanel(WidgetTree, TEXT("MonsterInfoWidget_PrototypePanel"));
	if (MonsterPanel)
	{
		MonsterPanel->SetContent(CreatePrototypeText(WidgetTree, TEXT("Text_MonsterInfoPlaceholder"), TEXT("몬스터 정보: 이름 / 레벨 / 속성 / 체력 %"), 12.0f));
		AddCanvasChild(RootCanvas, MonsterPanel, FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f), FVector2D(0.0f, 24.0f), FVector2D(420.0f, 52.0f), FVector2D(0.5f, 0.0f));
	}

	UBorder* MinimapPanel = CreatePrototypePanel(WidgetTree, TEXT("Panel_Minimap"));
	if (MinimapPanel)
	{
		MinimapPanel->SetContent(CreatePrototypeText(WidgetTree, TEXT("Text_MinimapPlaceholder"), TEXT("미니맵 / NPC / 몬스터 / 포탈"), 12.0f));
		AddCanvasChild(RootCanvas, MinimapPanel, FVector2D(1.0f, 0.0f), FVector2D(1.0f, 0.0f), FVector2D(-24.0f, 24.0f), FVector2D(260.0f, 180.0f), FVector2D(1.0f, 0.0f));
	}

	UBorder* ItemAcquisitionPanel = CreatePrototypePanel(WidgetTree, TEXT("Panel_ItemAcquisition"));
	if (ItemAcquisitionPanel)
	{
		ItemAcquisitionPanel->SetContent(CreatePrototypeText(WidgetTree, TEXT("Text_ItemAcquisitionPlaceholder"), TEXT("아이템 획득 알림\n최근 획득 항목부터 위에 표시"), 12.0f));
		AddCanvasChild(RootCanvas, ItemAcquisitionPanel, FVector2D(1.0f, 0.35f), FVector2D(1.0f, 0.35f), FVector2D(-24.0f, 0.0f), FVector2D(260.0f, 96.0f), FVector2D(1.0f, 0.5f));
	}

	UBorder* SystemLogPanel = CreatePrototypePanel(WidgetTree, TEXT("Panel_SystemLog"));
	if (SystemLogPanel)
	{
		SystemLogPanel->SetContent(CreatePrototypeText(WidgetTree, TEXT("Text_SystemLogPlaceholder"), TEXT("시스템 로그\n아이템, 경험치, 전투 메시지 표시"), 12.0f));
		AddCanvasChild(RootCanvas, SystemLogPanel, FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f), FVector2D(24.0f, -24.0f), FVector2D(360.0f, 190.0f), FVector2D(0.0f, 1.0f));
	}

	UBorder* BottomPanel = CreatePrototypePanel(WidgetTree, TEXT("BottomHUDPanel"));
	UVerticalBox* BottomBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box_BottomHUD"));
	if (BottomPanel && BottomBox)
	{
		BottomPanel->SetContent(BottomBox);

		UTextBlock* HealthText = CreatePrototypeText(WidgetTree, TEXT("HealthBarWidget_PrototypeText"), TEXT("HealthBarWidget - 체력 100 / 100"), 12.0f);
		UTextBlock* SkillText = CreatePrototypeText(WidgetTree, TEXT("Box_SkillSlots_PrototypeText"), TEXT("SkillSlot 영역 - 좌클릭 / 우클릭 / 점프공격 / 차징"), 12.0f);
		UTextBlock* IdentityText = CreatePrototypeText(WidgetTree, TEXT("IdentityWidget_PrototypeText"), TEXT("IdentityWidget - 아이덴티티 / 무기 숙련도 / 기력"), 12.0f);
		UTextBlock* StaminaText = CreatePrototypeText(WidgetTree, TEXT("StaminaBarWidget_PrototypeText"), TEXT("StaminaBarWidget - 스테미나 100 / 100"), 12.0f);
		UTextBlock* ItemText = CreatePrototypeText(WidgetTree, TEXT("Box_ItemSlots_PrototypeText"), TEXT("ItemSlot 영역 - 사용 아이템 슬롯"), 12.0f);

		TArray<UWidget*> BottomChildren = { HealthText, SkillText, IdentityText, StaminaText, ItemText };
		for (UWidget* ChildWidget : BottomChildren)
		{
			if (ChildWidget)
			{
				if (UVerticalBoxSlot* ChildSlot = BottomBox->AddChildToVerticalBox(ChildWidget))
				{
					ChildSlot->SetPadding(FMargin(0.0f, 2.0f));
				}
			}
		}

		AddCanvasChild(RootCanvas, BottomPanel, FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f), FVector2D(0.0f, -24.0f), FVector2D(700.0f, 148.0f), FVector2D(0.5f, 1.0f));
	}

	UBorder* ShortcutPanel = CreatePrototypePanel(WidgetTree, TEXT("Box_ShortcutHints"));
	UHorizontalBox* ShortcutBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Box_ShortcutHintItems"));
	if (ShortcutPanel && ShortcutBox)
	{
		ShortcutPanel->SetContent(ShortcutBox);

		const TArray<FString> ShortcutLabels = { TEXT("장비[I]"), TEXT("아이템[O]"), TEXT("스킬[K]"), TEXT("설정[Esc]") };
		for (const FString& ShortcutLabel : ShortcutLabels)
		{
			UTextBlock* ShortcutText = CreatePrototypeText(WidgetTree, NAME_None, ShortcutLabel, 12.0f);
			if (ShortcutText)
			{
				ShortcutText->SetColorAndOpacity(FSlateColor(PrototypeAccentColor));
				if (UHorizontalBoxSlot* ChildSlot = ShortcutBox->AddChildToHorizontalBox(ShortcutText))
				{
					ChildSlot->SetPadding(FMargin(6.0f, 0.0f));
				}
			}
		}

		AddCanvasChild(RootCanvas, ShortcutPanel, FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f), FVector2D(-24.0f, -24.0f), FVector2D(280.0f, 42.0f), FVector2D(1.0f, 1.0f));
	}
}
