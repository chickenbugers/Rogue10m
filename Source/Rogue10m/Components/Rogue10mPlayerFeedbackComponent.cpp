// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mPlayerFeedbackComponent.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/Actor.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mCharacter.h"
#include "TimerManager.h"

URogue10mPlayerFeedbackComponent::URogue10mPlayerFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URogue10mPlayerFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsurePostProcessComponent();
	InitializeAbilitySystem();
}

void URogue10mPlayerFeedbackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilitySystem();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FeedbackTimerHandle);
	}
	if (EffectPostProcess)
	{
		EffectPostProcess->DestroyComponent();
		EffectPostProcess = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void URogue10mPlayerFeedbackComponent::EnsurePostProcessComponent()
{
	ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner());
	if (EffectPostProcess || !Character || !Character->IsLocallyControlled()
		|| !Character->GetFirstPersonCameraComponent())
	{
		return;
	}

	EffectPostProcess = NewObject<UPostProcessComponent>(Character, TEXT("Player Feedback Post Process"));
	Character->AddInstanceComponent(EffectPostProcess);
	EffectPostProcess->SetupAttachment(Character->GetFirstPersonCameraComponent());
	EffectPostProcess->bUnbound = true;
	EffectPostProcess->BlendRadius = 100.0f;
	EffectPostProcess->BlendWeight = 0.0f;
	EffectPostProcess->Priority = 50.0f;
	EffectPostProcess->Settings.bOverride_SceneColorTint = true;
	EffectPostProcess->Settings.bOverride_ColorSaturation = true;
	EffectPostProcess->Settings.bOverride_VignetteIntensity = true;
	EffectPostProcess->Settings.bOverride_SceneFringeIntensity = true;
	EffectPostProcess->RegisterComponent();
}

void URogue10mPlayerFeedbackComponent::InitializeAbilitySystem()
{
	EnsurePostProcessComponent();
	const ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner());
	UAbilitySystemComponent* AbilitySystem = Character ? Character->GetAbilitySystemComponent() : nullptr;
	if (!Character || !Character->IsLocallyControlled() || !AbilitySystem)
	{
		return;
	}

	if (BoundAbilitySystem.Get() != AbilitySystem)
	{
		UnbindAbilitySystem();
		BoundAbilitySystem = AbilitySystem;
		StaminaChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
			URogue10mAttributeSet::GetStaminaAttribute()).AddUObject(
				this, &URogue10mPlayerFeedbackComponent::HandleStaminaChanged);
		MaxStaminaChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
			URogue10mAttributeSet::GetMaxStaminaAttribute()).AddUObject(
				this, &URogue10mPlayerFeedbackComponent::HandleMaxStaminaChanged);
	}

	RefreshStaminaIntensity();
}

void URogue10mPlayerFeedbackComponent::NotifyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	const float DamageStrength = FMath::Clamp(DamageAmount / FMath::Max(1.0f, FullDamagePulseAmount), 0.2f, 1.0f);
	DamagePulse = FMath::Max(DamagePulse, DamageStrength);
	ApplyPostProcess();
	StartFeedbackTimer();
}

void URogue10mPlayerFeedbackComponent::NotifyInsufficientStamina()
{
	InsufficientStaminaPulse = 1.0f;
	ApplyPostProcess();
	StartFeedbackTimer();
}

void URogue10mPlayerFeedbackComponent::UnbindAbilitySystem()
{
	if (UAbilitySystemComponent* AbilitySystem = BoundAbilitySystem.Get())
	{
		if (StaminaChangedHandle.IsValid())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(
				URogue10mAttributeSet::GetStaminaAttribute()).Remove(StaminaChangedHandle);
		}
		if (MaxStaminaChangedHandle.IsValid())
		{
			AbilitySystem->GetGameplayAttributeValueChangeDelegate(
				URogue10mAttributeSet::GetMaxStaminaAttribute()).Remove(MaxStaminaChangedHandle);
		}
	}

	BoundAbilitySystem.Reset();
	StaminaChangedHandle.Reset();
	MaxStaminaChangedHandle.Reset();
}

void URogue10mPlayerFeedbackComponent::HandleStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	RefreshStaminaIntensity();
}

void URogue10mPlayerFeedbackComponent::HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	RefreshStaminaIntensity();
}

void URogue10mPlayerFeedbackComponent::RefreshStaminaIntensity()
{
	const ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner());
	const URogue10mAttributeSet* Attributes = Character ? Character->GetRogueAttributeSet() : nullptr;
	if (!Attributes)
	{
		LowStaminaIntensity = 0.0f;
		ApplyPostProcess();
		return;
	}

	const float NormalizedStamina = Attributes->GetMaxStamina() > 0.0f
		? FMath::Clamp(Attributes->GetStamina() / Attributes->GetMaxStamina(), 0.0f, 1.0f)
		: 0.0f;
	LowStaminaIntensity = NormalizedStamina < LowStaminaThreshold
		? 1.0f - (NormalizedStamina / FMath::Max(KINDA_SMALL_NUMBER, LowStaminaThreshold))
		: 0.0f;
	ApplyPostProcess();
}

void URogue10mPlayerFeedbackComponent::StartFeedbackTimer()
{
	if (UWorld* World = GetWorld(); World && !World->GetTimerManager().IsTimerActive(FeedbackTimerHandle))
	{
		World->GetTimerManager().SetTimer(
			FeedbackTimerHandle, this, &URogue10mPlayerFeedbackComponent::UpdateFeedback,
			FeedbackUpdateInterval, true);
	}
}

void URogue10mPlayerFeedbackComponent::UpdateFeedback()
{
	DamagePulse = FMath::Max(0.0f, DamagePulse - FeedbackUpdateInterval / FMath::Max(0.05f, DamagePulseDuration));
	InsufficientStaminaPulse = FMath::Max(
		0.0f, InsufficientStaminaPulse - FeedbackUpdateInterval / FMath::Max(0.05f, InsufficientStaminaPulseDuration));
	ApplyPostProcess();

	if (DamagePulse <= KINDA_SMALL_NUMBER && InsufficientStaminaPulse <= KINDA_SMALL_NUMBER)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(FeedbackTimerHandle);
		}
	}
}

void URogue10mPlayerFeedbackComponent::ApplyPostProcess()
{
	if (!EffectPostProcess)
	{
		return;
	}

	const float StaminaIntensity = FMath::Max(LowStaminaIntensity, InsufficientStaminaPulse);
	const float CombinedIntensity = FMath::Max(DamagePulse, StaminaIntensity);
	EffectPostProcess->BlendWeight = CombinedIntensity > KINDA_SMALL_NUMBER ? 1.0f : 0.0f;

	FLinearColor Tint = FLinearColor::White;
	Tint = FMath::Lerp(Tint, StaminaTint, StaminaIntensity * 0.45f);
	Tint = FMath::Lerp(Tint, DamageTint, DamagePulse * 0.65f);
	EffectPostProcess->Settings.SceneColorTint = Tint;

	const float Saturation = FMath::Clamp(
		1.0f - StaminaIntensity * StaminaDesaturationStrength - DamagePulse * 0.1f,
		0.25f, 1.0f);
	EffectPostProcess->Settings.ColorSaturation = FVector4(Saturation, Saturation, Saturation, 1.0f);
	EffectPostProcess->Settings.VignetteIntensity = FMath::Max(
		DamagePulse * DamageVignetteIntensity,
		StaminaIntensity * StaminaVignetteIntensity);
	EffectPostProcess->Settings.SceneFringeIntensity = DamagePulse * DamageChromaticAberration;
}
