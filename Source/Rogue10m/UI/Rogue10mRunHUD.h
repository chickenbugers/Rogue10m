// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mRunHUD.generated.h"

class ARogue10mHUD;

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
	float Current = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Max = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|HUD")
	float Normalized = 0.0f;
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
	FRogue10mHudVitalView MakeVitalView(const struct FRogue10mVitalValue& Vital, bool bVisible) const;
};
