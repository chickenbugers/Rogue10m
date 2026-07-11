// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mMainHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	const FLinearColor PrototypePanelColor(0.02f, 0.025f, 0.03f, 0.68f);
	const FLinearColor PrototypeAccentColor(0.72f, 0.58f, 0.28f, 1.0f);
	const FLinearColor PrototypeTextColor(0.92f, 0.90f, 0.84f, 1.0f);

	FRogue10mHudVitalView MakeIdentityResourceVitalView(const FRogue10mHudIdentityView& IdentityView)
	{
		FRogue10mHudVitalView VitalView;
		VitalView.Current = IdentityView.Current;
		VitalView.Max = IdentityView.Max;
		VitalView.Normalized = IdentityView.Normalized;
		VitalView.Percent = IdentityView.Normalized * 100.0f;
		VitalView.ValueText = FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), IdentityView.Current, IdentityView.Max));
		VitalView.PercentText = FText::FromString(FString::Printf(TEXT("%.0f%%"), VitalView.Percent));
		VitalView.FillColor = FLinearColor(0.52f, 0.54f, 0.58f, 1.0f);
		VitalView.bVisible = IdentityView.bHasIdentityResource;
		return VitalView;
	}

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

	const TArray<URogue10mShortcutHintWidget*> HiddenShortcuts =
	{
		EquipmentShortcutWidget, ItemWindowShortcutWidget, SkillTreeShortcutWidget, SettingsShortcutWidget
	};
	for (URogue10mShortcutHintWidget* ShortcutWidget : HiddenShortcuts)
	{
		if (ShortcutWidget)
		{
			ShortcutWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void URogue10mMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FrequentRefreshElapsed += InDeltaTime;
	SlowRefreshElapsed += InDeltaTime;
	if (FrequentRefreshElapsed >= FrequentRefreshInterval)
	{
		FrequentRefreshElapsed = FMath::Fmod(FrequentRefreshElapsed, FrequentRefreshInterval);
		RefreshFrequentWidgetData();
	}
	if (SlowRefreshElapsed >= SlowRefreshInterval)
	{
		SlowRefreshElapsed = FMath::Fmod(SlowRefreshElapsed, SlowRefreshInterval);
		RefreshSlowWidgetData();
	}
}

void URogue10mMainHUDWidget::RefreshBoundWidgetData()
{
	RefreshFrequentWidgetData();
	RefreshSlowWidgetData();
}

void URogue10mMainHUDWidget::RefreshFrequentWidgetData()
{
	static const FText HealthLabel = NSLOCTEXT("Rogue10mHUD", "HealthLabel", "체력");
	static const FText StaminaLabel = NSLOCTEXT("Rogue10mHUD", "StaminaLabel", "스테미나");
	static const FText IdentityLabel = NSLOCTEXT("Rogue10mHUD", "IdentityLabel", "아이덴티티");

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVitalView(HealthLabel, GetHealthView());
	}
	if (StaminaBarWidget)
	{
		StaminaBarWidget->SetVitalView(StaminaLabel, GetStaminaView());
	}
	if (IdentityBarWidget)
	{
		const FRogue10mHudIdentityView IdentityView = GetIdentityView();
		const ESlateVisibility DesiredVisibility = IdentityView.bHasIdentityResource
			? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
		if (IdentityBarWidget->GetVisibility() != DesiredVisibility)
		{
			IdentityBarWidget->SetVisibility(DesiredVisibility);
		}
		IdentityBarWidget->SetVitalView(IdentityLabel, MakeIdentityResourceVitalView(IdentityView));
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
	RefreshMinimapMarkerContainer();
	BP_OnBoundWidgetDataRefreshed();
}

void URogue10mMainHUDWidget::RefreshSlowWidgetData()
{
	RefreshQuickSlotContainer(SkillSlotContainer, GetSkillQuickSlotViews());
	RefreshQuickSlotContainer(ItemSlotContainer, GetItemQuickSlotViews());
	RefreshLogContainer(SystemLogContainer, GetSystemLogEntries());
	RefreshLogContainer(ItemAcquisitionContainer, GetItemAcquisitionEntries());
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

	if (IdentityBarWidget)
	{
		IdentityBarWidget->SetOwningMainHUD(this);
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

void URogue10mMainHUDWidget::RefreshQuickSlotContainer(UPanelWidget* Container, const TArray<FRogue10mHudQuickSlotView>& Views)
{
	if (!Container || !QuickSlotWidgetClass)
	{
		return;
	}

	while (Container->GetChildrenCount() > Views.Num())
	{
		Container->RemoveChildAt(Container->GetChildrenCount() - 1);
	}
	while (Container->GetChildrenCount() < Views.Num())
	{
		URogue10mQuickSlotWidget* SlotWidget = CreateWidget<URogue10mQuickSlotWidget>(GetOwningPlayer(), QuickSlotWidgetClass);
		if (!SlotWidget)
		{
			break;
		}
		SlotWidget->SetOwningMainHUD(this);
		Container->AddChild(SlotWidget);
	}
	for (int32 Index = 0; Index < Views.Num(); ++Index)
	{
		if (URogue10mQuickSlotWidget* SlotWidget = Cast<URogue10mQuickSlotWidget>(Container->GetChildAt(Index)))
		{
			SlotWidget->SetQuickSlotView(Views[Index]);
		}
	}
}
void URogue10mMainHUDWidget::RefreshLogContainer(UPanelWidget* Container, const TArray<FRogue10mHudLogEntryView>& Views)
{
	if (!Container || !LogLineWidgetClass)
	{
		return;
	}

	while (Container->GetChildrenCount() > Views.Num())
	{
		Container->RemoveChildAt(Container->GetChildrenCount() - 1);
	}
	while (Container->GetChildrenCount() < Views.Num())
	{
		URogue10mLogLineWidget* LogWidget = CreateWidget<URogue10mLogLineWidget>(GetOwningPlayer(), LogLineWidgetClass);
		if (!LogWidget)
		{
			break;
		}
		LogWidget->SetOwningMainHUD(this);
		Container->AddChild(LogWidget);
	}
	for (int32 Index = 0; Index < Views.Num(); ++Index)
	{
		if (URogue10mLogLineWidget* LogWidget = Cast<URogue10mLogLineWidget>(Container->GetChildAt(Index)))
		{
			LogWidget->SetLogEntryView(Views[Index]);
		}
	}
}
void URogue10mMainHUDWidget::RefreshMinimapMarkerContainer()
{
	if (!MinimapMarkerContainer || !MinimapMarkerWidgetClass)
	{
		return;
	}

	const TArray<FRogue10mHudMinimapMarkerView> Views = GetPrototypeMinimapMarkers();
	if (MinimapMarkerContainer->GetChildrenCount() != Views.Num())
	{
		MinimapMarkerContainer->ClearChildren();
		for (const FRogue10mHudMinimapMarkerView& View : Views)
		{
			URogue10mMinimapMarkerWidget* MarkerWidget = CreateWidget<URogue10mMinimapMarkerWidget>(GetOwningPlayer(), MinimapMarkerWidgetClass);
			if (!MarkerWidget)
			{
				continue;
			}

			MarkerWidget->SetOwningMainHUD(this);
			MarkerWidget->SetMinimapMarkerView(View);
			MinimapMarkerContainer->AddChild(MarkerWidget);
		}
		return;
	}

	for (int32 Index = 0; Index < Views.Num(); ++Index)
	{
		if (URogue10mMinimapMarkerWidget* MarkerWidget = Cast<URogue10mMinimapMarkerWidget>(MinimapMarkerContainer->GetChildAt(Index)))
		{
			MarkerWidget->SetMinimapMarkerView(Views[Index]);
		}
	}
}
void URogue10mMainHUDWidget::EnsurePrototypeLayout()
{
	if (!bCreatePrototypeLayoutWhenEmpty || !WidgetTree)
	{
		return;
	}

	// Blueprint에서 이미 UI를 직접 배치했다면 C++ 임시 골격은 만들지 않는다.
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

}
