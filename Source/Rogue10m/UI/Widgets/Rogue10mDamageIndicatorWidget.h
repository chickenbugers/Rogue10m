// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mDamageIndicatorWidget.generated.h"

class UTextBlock;

/** 몬스터 머리 위에서 상승하며 사라지는 피해량 위젯입니다. */
UCLASS(Blueprintable)
class ROGUE10M_API URogue10mDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rogue10m|HUD|Damage Indicator")
	void InitializeIndicator(float DamageAmount, FVector InWorldLocation, float InDuration = 1.2f);

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Damage Indicator")
	float GetDisplayedDamageAmount() const { return DisplayedDamageAmount; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Damage Indicator")
	bool IsAvailableForReuse() const { return !bIndicatorActive; }

	float GetActivationTime() const { return StartTime; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|HUD|Damage Indicator")
	void DeactivateIndicator();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DamageText;

	/** false?대㈃ Widget Blueprint ?좊땲硫붿씠?섏씠 ?대룞/?ш린/?щ챸?꾨? ?꾨? ?쒖뼱?⑸땲?? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator")
	bool bUseNativeAnimation = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator", meta=(ClampMin="10.0", ClampMax="160.0"))
	float VerticalTravel = 72.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator", meta=(ClampMin="0.0", ClampMax="0.95"))
	float FadeStartAlpha = 0.55f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator")
	FLinearColor LowDamageColor = FLinearColor(1.0f, 0.78f, 0.22f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator")
	FLinearColor HighDamageColor = FLinearColor(1.0f, 0.2f, 0.08f, 1.0f);

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD|Damage Indicator", meta=(DisplayName="Damage Indicator Initialized"))
	void BP_OnDamageIndicatorInitialized(float DamageAmount, float IndicatorDuration);

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD|Damage Indicator", meta=(DisplayName="Damage Indicator Progress"))
	void BP_OnDamageIndicatorProgress(float NormalizedTime);

private:
	void EnsureNativeWidget();
	void ReleaseIndicator();

	FVector IndicatorWorldLocation = FVector::ZeroVector;
	float StartTime = 0.0f;
	float Duration = 1.2f;
	float DamageScale = 1.0f;
	float DisplayedDamageAmount = 0.0f;
	bool bIndicatorActive = false;
};
