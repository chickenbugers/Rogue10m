// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mPlayerFeedbackComponent.generated.h"

class UAbilitySystemComponent;
class UPostProcessComponent;
struct FOnAttributeChangeData;

/** 피해와 낮은 스테미나 상태를 카메라 포스트프로세스로 표현합니다. */
UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mPlayerFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mPlayerFeedbackComponent();

	void InitializeAbilitySystem();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Feedback")
	void NotifyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Feedback")
	void NotifyInsufficientStamina();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Feedback")
	void SetSprinting(bool bNewSprinting);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void EnsurePostProcessComponent();
	void UnbindAbilitySystem();
	void HandleStaminaChanged(const FOnAttributeChangeData& ChangeData);
	void HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData);
	void RefreshStaminaIntensity();
	void StartFeedbackTimer();
	void UpdateFeedback();
	void ApplyPostProcess();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Stamina", meta=(AllowPrivateAccess="true", ClampMin="0.01", ClampMax="1.0"))
	float LowStaminaThreshold = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Stamina", meta=(AllowPrivateAccess="true", ClampMin="0.05"))
	float InsufficientStaminaPulseDuration = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Stamina", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float StaminaDesaturationStrength = 0.55f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Stamina", meta=(AllowPrivateAccess="true"))
	FLinearColor StaminaTint = FLinearColor(0.55f, 0.68f, 0.9f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Sprint", meta=(AllowPrivateAccess="true"))
	FLinearColor SprintTint = FLinearColor(0.86f, 0.94f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Sprint", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float SprintVignetteIntensity = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Sprint", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float SprintMotionBlurAmount = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Sprint", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="5.0"))
	float SprintChromaticAberration = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Damage", meta=(AllowPrivateAccess="true", ClampMin="0.05", Units="s"))
	float DamagePulseDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Damage", meta=(AllowPrivateAccess="true", ClampMin="1.0"))
	float FullDamagePulseAmount = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Damage", meta=(AllowPrivateAccess="true"))
	FLinearColor DamageTint = FLinearColor(1.0f, 0.16f, 0.1f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Lens", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float DamageVignetteIntensity = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Lens", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float StaminaVignetteIntensity = 0.55f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback|Lens", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="5.0"))
	float DamageChromaticAberration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Feedback", meta=(AllowPrivateAccess="true", ClampMin="0.016", ClampMax="0.1"))
	float FeedbackUpdateInterval = 0.033f;

	UPROPERTY(Transient)
	TObjectPtr<UPostProcessComponent> EffectPostProcess;

	TWeakObjectPtr<UAbilitySystemComponent> BoundAbilitySystem;
	FDelegateHandle StaminaChangedHandle;
	FDelegateHandle MaxStaminaChangedHandle;
	FTimerHandle FeedbackTimerHandle;
	float DamagePulse = 0.0f;
	float InsufficientStaminaPulse = 0.0f;
	float LowStaminaIntensity = 0.0f;
	bool bSprinting = false;
};
