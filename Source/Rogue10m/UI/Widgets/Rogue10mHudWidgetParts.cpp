// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mHudWidgetParts.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	const FLinearColor PartPanelColor(0.015f, 0.018f, 0.022f, 0.7f);
	const FLinearColor PartTitleColor(0.86f, 0.68f, 0.34f, 1.0f);
	const FLinearColor PartTextColor(0.88f, 0.86f, 0.8f, 1.0f);

	UTextBlock* CreatePartText(UWidgetTree* WidgetTree, FName WidgetName, const FText& Text, int32 FontSize, const FLinearColor& Color)
	{
		UTextBlock* TextBlock = WidgetTree ? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), WidgetName) : nullptr;
		if (!TextBlock)
		{
			return nullptr;
		}

		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetAutoWrapText(true);

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
		return TextBlock;
	}
}

void URogue10mHudPartWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	EnsurePrototypeDesignScaffold();
}

void URogue10mHudPartWidget::SetOwningMainHUD(URogue10mMainHUDWidget* InOwningMainHUD)
{
	if (OwningMainHUD == InOwningMainHUD)
	{
		return;
	}

	OwningMainHUD = InOwningMainHUD;
	BP_OnOwningMainHUDChanged();
}

FText URogue10mHudPartWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("HUD 파트"));
}

FText URogue10mHudPartWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("내부 디자인을 이 Widget Blueprint에서 제작합니다."));
}

FVector2D URogue10mHudPartWidget::GetPrototypeDesignSize() const
{
	return FVector2D(240.0f, 80.0f);
}

void URogue10mHudPartWidget::EnsurePrototypeDesignScaffold()
{
	if (!bCreatePrototypeDesignScaffold || !WidgetTree)
	{
		return;
	}

	UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if (RootCanvas && RootCanvas->GetChildrenCount() > 0)
	{
		return;
	}

	if (!RootCanvas)
	{
		RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel_Root"));
		WidgetTree->RootWidget = RootCanvas;
	}

	if (!RootCanvas || RootCanvas->GetChildrenCount() > 0)
	{
		return;
	}

	UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Border_DesignScaffold"));
	UVerticalBox* TextBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box_DesignScaffoldText"));
	if (!Background || !TextBox)
	{
		return;
	}

	Background->SetBrushColor(PartPanelColor);
	Background->SetPadding(FMargin(8.0f));
	Background->SetContent(TextBox);

	UTextBlock* TitleText = CreatePartText(WidgetTree, TEXT("Text_DesignTitle"), GetPrototypeDesignTitle(), 13, PartTitleColor);
	UTextBlock* DescriptionText = CreatePartText(WidgetTree, TEXT("Text_DesignDescription"), GetPrototypeDesignDescription(), 11, PartTextColor);
	if (TitleText)
	{
		if (UVerticalBoxSlot* TitleSlot = TextBox->AddChildToVerticalBox(TitleText))
		{
			TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
		}
	}

	if (DescriptionText)
	{
		TextBox->AddChildToVerticalBox(DescriptionText);
	}

	UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(Background);
	if (CanvasSlot)
	{
		const FVector2D DesignSize = GetPrototypeDesignSize();
		CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetSize(DesignSize);
		CanvasSlot->SetAlignment(FVector2D::ZeroVector);
	}
}

void URogue10mVitalBarWidget::SetVitalView(FText InLabel, const FRogue10mHudVitalView& InVitalView)
{
	VitalLabel = MoveTemp(InLabel);
	VitalView = InVitalView;
	BP_OnVitalViewChanged();
}

FText URogue10mVitalBarWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("체력/스테미나/마나 바"));
}

FText URogue10mVitalBarWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("ProgressBar와 수치 텍스트를 배치합니다. 메인 위젯에서는 HealthBarWidget, StaminaBarWidget, ManaBarWidget 이름으로 사용합니다."));
}

FVector2D URogue10mVitalBarWidget::GetPrototypeDesignSize() const
{
	return FVector2D(360.0f, 34.0f);
}

void URogue10mProgressionWidget::SetProgressionView(const FRogue10mHudProgressionView& InProgressionView)
{
	ProgressionView = InProgressionView;
	BP_OnProgressionViewChanged();
}

FText URogue10mProgressionWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("레벨/경험치"));
}

FText URogue10mProgressionWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("레벨 텍스트와 경험치 ProgressBar를 배치합니다."));
}

FVector2D URogue10mProgressionWidget::GetPrototypeDesignSize() const
{
	return FVector2D(300.0f, 46.0f);
}

void URogue10mIdentityWidget::SetIdentityView(const FRogue10mHudIdentityView& InIdentityView)
{
	IdentityView = InIdentityView;
	BP_OnIdentityViewChanged();
}

FText URogue10mIdentityWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("아이덴티티"));
}

FText URogue10mIdentityWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("무기 숙련도, 기력/마력 등 특수 자원 게이지를 배치합니다."));
}

FVector2D URogue10mIdentityWidget::GetPrototypeDesignSize() const
{
	return FVector2D(260.0f, 72.0f);
}

void URogue10mMonsterInfoWidget::SetMonsterInfoView(const FRogue10mHudMonsterInfoView& InMonsterInfoView)
{
	MonsterInfoView = InMonsterInfoView;
	BP_OnMonsterInfoViewChanged();
}

FText URogue10mMonsterInfoWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("몬스터 정보"));
}

FText URogue10mMonsterInfoWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("이름, 레벨, 속성, 상태, 체력 퍼센트를 표시합니다."));
}

FVector2D URogue10mMonsterInfoWidget::GetPrototypeDesignSize() const
{
	return FVector2D(420.0f, 64.0f);
}

void URogue10mQuickSlotWidget::SetQuickSlotView(const FRogue10mHudQuickSlotView& InQuickSlotView)
{
	QuickSlotView = InQuickSlotView;
	BP_OnQuickSlotViewChanged();
}

FText URogue10mQuickSlotWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("퀵 슬롯"));
}

FText URogue10mQuickSlotWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("아이콘, 입력 키, 쿨타임 오버레이, 잠금 표시를 배치합니다."));
}

FVector2D URogue10mQuickSlotWidget::GetPrototypeDesignSize() const
{
	return FVector2D(54.0f, 54.0f);
}

void URogue10mLogLineWidget::SetLogEntryView(const FRogue10mHudLogEntryView& InLogEntryView)
{
	LogEntryView = InLogEntryView;
	BP_OnLogEntryViewChanged();
}

FText URogue10mLogLineWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("로그/획득 알림 한 줄"));
}

FText URogue10mLogLineWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("시스템 로그 또는 아이템 획득 메시지 한 줄을 표시합니다."));
}

FVector2D URogue10mLogLineWidget::GetPrototypeDesignSize() const
{
	return FVector2D(340.0f, 24.0f);
}

void URogue10mMinimapMarkerWidget::SetMinimapMarkerView(const FRogue10mHudMinimapMarkerView& InMinimapMarkerView)
{
	MinimapMarkerView = InMinimapMarkerView;
	BP_OnMinimapMarkerViewChanged();
}

FText URogue10mMinimapMarkerWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("미니맵 마커"));
}

FText URogue10mMinimapMarkerWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("플레이어, 몬스터, NPC, 포탈 아이콘을 표시하는 반복 요소입니다."));
}

FVector2D URogue10mMinimapMarkerWidget::GetPrototypeDesignSize() const
{
	return FVector2D(24.0f, 24.0f);
}

void URogue10mShortcutHintWidget::SetShortcutHint(FText InKeyText, FText InLabelText, bool bInActive)
{
	KeyText = MoveTemp(InKeyText);
	LabelText = MoveTemp(InLabelText);
	bActive = bInActive;
	BP_OnShortcutHintChanged();
}

FText URogue10mShortcutHintWidget::GetPrototypeDesignTitle() const
{
	return FText::FromString(TEXT("단축키 힌트"));
}

FText URogue10mShortcutHintWidget::GetPrototypeDesignDescription() const
{
	return FText::FromString(TEXT("키 텍스트와 장비/아이템/스킬/설정 라벨을 표시합니다."));
}

FVector2D URogue10mShortcutHintWidget::GetPrototypeDesignSize() const
{
	return FVector2D(96.0f, 36.0f);
}
