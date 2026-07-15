// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mDamageIndicatorWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void URogue10mDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureNativeWidget();
	SetVisibility(ESlateVisibility::Collapsed);
	SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
}

void URogue10mDamageIndicatorWidget::InitializeIndicator(
	float DamageAmount, FVector InWorldLocation, float InDuration, bool bIsCriticalHit)
{
	EnsureNativeWidget();
	IndicatorWorldLocation = InWorldLocation;
	Duration = FMath::Max(0.1f, InDuration);
	StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	DamageScale = FMath::Clamp(FMath::Sqrt(FMath::Max(1.0f, DamageAmount) / 20.0f), 0.9f, 1.45f);
	DisplayedDamageAmount = DamageAmount;
	bCriticalHit = bIsCriticalHit;
	bIndicatorActive = true;
	SetRenderTranslation(FVector2D::ZeroVector);
	SetRenderScale(FVector2D(1.0f, 1.0f));
	SetRenderOpacity(1.0f);
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(DamageAmount)));
		DamageText->SetColorAndOpacity(FSlateColor(
			bCriticalHit ? CriticalDamageColor : NormalDamageColor));
		FSlateFontInfo Font = DamageText->GetFont();
		Font.OutlineSettings.OutlineSize = bCriticalHit ? CriticalOutlineSize : NormalOutlineSize;
		Font.OutlineSettings.OutlineColor = bCriticalHit ? CriticalOutlineColor : NormalOutlineColor;
		DamageText->SetFont(Font);
	}
	BP_OnDamageIndicatorInitialized(DamageAmount, Duration);
}

void URogue10mDamageIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIndicatorActive || !GetWorld())
	{
		return;
	}

	FVector2D ScreenPosition = FVector2D::ZeroVector;
	const bool bProjected = GetOwningPlayer()
		&& GetOwningPlayer()->ProjectWorldLocationToScreen(IndicatorWorldLocation, ScreenPosition, true);
	if (bProjected)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		SetPositionInViewport(ScreenPosition, true);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}

	const float Alpha = FMath::Clamp((GetWorld()->GetTimeSeconds() - StartTime) / Duration, 0.0f, 1.0f);
	if (Alpha >= 1.0f)
	{
		ReleaseIndicator();
		return;
	}

	BP_OnDamageIndicatorProgress(Alpha);
	if (bUseNativeAnimation)
	{
		const float RiseAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
		const float FadeAlpha = Alpha <= FadeStartAlpha
			? 1.0f
			: 1.0f - (Alpha - FadeStartAlpha) / FMath::Max(KINDA_SMALL_NUMBER, 1.0f - FadeStartAlpha);
		const float PopAlpha = FMath::Clamp(Alpha / 0.18f, 0.0f, 1.0f);
		const float Scale = FMath::Lerp(1.25f * DamageScale, DamageScale, PopAlpha);

		SetRenderTranslation(FVector2D(0.0f, -VerticalTravel * RiseAlpha));
		SetRenderScale(FVector2D(Scale, Scale));
		SetRenderOpacity(FMath::Clamp(FadeAlpha, 0.0f, 1.0f));
	}
}

void URogue10mDamageIndicatorWidget::EnsureNativeWidget()
{
	if (DamageText || !WidgetTree)
	{
		return;
	}

	DamageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DamageText"));
	if (!DamageText)
	{
		return;
	}

	FSlateFontInfo Font = DamageText->GetFont();
	Font.Size = 32;
	Font.OutlineSettings.OutlineSize = 2;
	Font.OutlineSettings.OutlineColor = FLinearColor::Black;
	DamageText->SetFont(Font);
	DamageText->SetJustification(ETextJustify::Center);
	DamageText->SetShadowOffset(FVector2D(2.0f, 2.0f));
	DamageText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
	WidgetTree->RootWidget = DamageText;
}

void URogue10mDamageIndicatorWidget::DeactivateIndicator()
{
	bIndicatorActive = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void URogue10mDamageIndicatorWidget::ReleaseIndicator()
{
	DeactivateIndicator();
}
