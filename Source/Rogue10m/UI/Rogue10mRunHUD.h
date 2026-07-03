// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mIdentityTypes.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mRunHUD.generated.h"

class ARogue10mHUD;
class UTexture2D;

USTRUCT(BlueprintType)
struct FRogue10mHudVitalView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Current = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Max = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Normalized = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Percent = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText ValueText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText PercentText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor FillColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = false;
};

USTRUCT(BlueprintType)
struct FRogue10mHudProgressionView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 CurrentExperience = 0;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 ExperienceToNextLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float ExperienceNormalized = 0.0f;
};

USTRUCT(BlueprintType)
struct FRogue10mHudIdentityView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText Label;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	ERogue10mWeaponType WeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	ERogue10mIdentityType IdentityType = ERogue10mIdentityType::None;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 MasteryLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float MasteryNormalized = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor OutlineColor = FLinearColor(0.42f, 0.42f, 0.42f, 1.0f);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float OutlineThickness = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText ResourceLabel;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Current = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Max = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Normalized = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bHasIdentityResource = false;
};

USTRUCT(BlueprintType)
struct FRogue10mHudQuickSlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 SlotNumber = 0;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor IconColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownRemaining = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownDuration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownNormalized = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bUnlocked = true;
};

USTRUCT(BlueprintType)
struct FRogue10mHudLogEntryView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float RemainingSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FRogue10mHudMonsterInfoView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bHasMonster = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText Name;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText AttributeText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText StateText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FRogue10mHudVitalView Health;
};

USTRUCT(BlueprintType)
struct FRogue10mHudMinimapMarkerView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FVector2D NormalizedPosition = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText Label;
};

USTRUCT(BlueprintType)
struct FRogue10mHudPanelStateView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bEquipmentVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bItemWindowVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bSkillTreeVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bSettingsVisible = false;
};

USTRUCT(BlueprintType)
struct FRogue10mHudRunTimerView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float RemainingSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float TotalSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float ProgressAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText RemainingText;
};

USTRUCT(BlueprintType)
struct FRogue10mHudRunResultView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	ERogue10mRunPhase RunPhase = ERogue10mRunPhase::WaitingToStart;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText ResultText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText DescriptionText;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor ResultColor = FLinearColor::White;
};

USTRUCT(BlueprintType)
struct FRogue10mHudAimCrossLineView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = true;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor Color = FLinearColor(0.72f, 0.92f, 1.0f, 0.86f);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float LineLength = 16.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Gap = 7.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Thickness = 2.0f;
};

USTRUCT(BlueprintType)
struct FRogue10mHudDamageFeedbackView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Alpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Strength = 0.0f;
};

USTRUCT(BlueprintType)
struct FRogue10mHudAttackCooldownView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bVisible = false;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText SkillName;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FText IconLabel;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	FLinearColor IconTint = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	TObjectPtr<UTexture2D> SkillIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownRemaining = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownDuration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float CooldownNormalized = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bOnCooldown = false;
};

/**
 * Widget Blueprint용 HUD 베이스입니다.
 * 화면 배치는 Widget Blueprint에서 만들고, 이 클래스의 Getter로 C++ 게임 데이터를 읽습니다.
 */
UCLASS(abstract)
class ROGUE10M_API URogue10mRunHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Update Run Timer"))
	void BP_UpdateRunTimer(float RemainingSeconds, float TotalSeconds, float ProgressAlpha);

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Run Phase Changed"))
	void BP_OnRunPhaseChanged(ERogue10mRunPhase NewPhase);

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="HUD Data Updated"))
	void BP_OnHudDataUpdated(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD")
	ARogue10mHUD* GetRogueHUD() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Vitals")
	FRogue10mHudVitalView GetHealthView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Vitals")
	FRogue10mHudVitalView GetStaminaView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Vitals")
	FRogue10mHudVitalView GetManaView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Progression")
	FRogue10mHudProgressionView GetProgressionView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Identity")
	FRogue10mHudIdentityView GetIdentityView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Combat")
	FRogue10mHudMonsterInfoView GetLookedAtMonsterInfoView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Quick Slot")
	TArray<FRogue10mHudQuickSlotView> GetSkillQuickSlotViews() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Quick Slot")
	TArray<FRogue10mHudQuickSlotView> GetItemQuickSlotViews() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Log")
	TArray<FRogue10mHudLogEntryView> GetSystemLogEntries() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Log")
	TArray<FRogue10mHudLogEntryView> GetItemAcquisitionEntries() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Panel")
	FRogue10mHudPanelStateView GetPanelStateView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Minimap")
	TArray<FRogue10mHudMinimapMarkerView> GetPrototypeMinimapMarkers() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Run")
	FRogue10mHudRunTimerView GetRunTimerView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Run")
	FRogue10mHudRunResultView GetRunResultView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Aim")
	FRogue10mHudAimCrossLineView GetAimCrossLineView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Combat")
	FRogue10mHudDamageFeedbackView GetDamageFeedbackView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Combat")
	FRogue10mHudAttackCooldownView GetAttackCooldownView() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Shortcut")
	FText GetEquipmentShortcutText() const { return FText::FromString(TEXT("I")); }

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Shortcut")
	FText GetItemWindowShortcutText() const { return FText::FromString(TEXT("B")); }

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Shortcut")
	FText GetSkillTreeShortcutText() const { return FText::FromString(TEXT("K")); }

	UFUNCTION(BlueprintPure, Category="Rogue10m|HUD|Shortcut")
	FText GetSettingsShortcutText() const { return FText::FromString(TEXT("O")); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|HUD")
	bool bBroadcastHudDataUpdatedEveryTick = true;

private:
	FRogue10mHudVitalView MakeVitalView(const struct FRogue10mVitalValue& Vital, bool bVisible, const FLinearColor& FillColor) const;
};
