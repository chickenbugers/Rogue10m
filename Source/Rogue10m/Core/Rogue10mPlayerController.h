// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/SoftObjectPtr.h"
#include "Rogue10mPlayerController.generated.h"

class ARogue10mBasicMonster;
class UInputMappingContext;
class URogue10mDamageIndicatorWidget;
class URogue10mRunHUD;
class UUserWidget;

struct FRogue10mRuntimeLogEntry
{
	FString Message;
	FLinearColor Color = FLinearColor::White;
	float ExpireTime = 0.0f;
};

struct FRogue10mFloatingDamageEntry
{
	TWeakObjectPtr<AActor> TargetActor;
	float DamageAmount = 0.0f;
	float StartTime = 0.0f;
	float ExpireTime = 0.0f;
};

/** 로컬 입력, UMG HUD와 플레이어 UI 상태를 관리합니다. */
UCLASS(abstract, config="Game")
class ROGUE10M_API ARogue10mPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARogue10mPlayerController();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	void ToggleItemWindow();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	void ToggleSkillTree();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	void ToggleSettings();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	void CloseAllBlockingPanels();

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsInventoryVisible() const { return bInventoryVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsItemWindowVisible() const { return bItemWindowVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsSkillTreeVisible() const { return bSkillTreeVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsSettingsVisible() const { return bSettingsVisible; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsAnyBlockingWindowVisible() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	float GetLookSensitivityX() const { return LookSensitivityX; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	float GetLookSensitivityY() const { return LookSensitivityY; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	void SetLookSensitivity(float NewSensitivityX, float NewSensitivityY);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Settings")
	int32 GetCurrentFpsLimit() const { return CurrentFpsLimit; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	void SetFpsLimit(int32 FpsValue);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void AddCombatLogMessage(const FString& Message, const FLinearColor& Color = FLinearColor::White, float Duration = 4.0f);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void AddItemAcquisitionMessage(const FString& Message, const FLinearColor& Color = FLinearColor::White, float Duration = 3.0f);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void AddFloatingDamageNumber(AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void NotifyPlayerDamaged(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Debug")
	bool ToggleCombatLogVisible();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Debug")
	bool IsCombatLogVisible() const { return bShowCombatLog; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	bool ActivateQuickSlot(int32 SlotNumber);

	const TArray<FRogue10mRuntimeLogEntry>& GetCombatLogEntries() const { return CombatLogEntries; }
	const TArray<FRogue10mRuntimeLogEntry>& GetItemAcquisitionEntries() const { return ItemAcquisitionEntries; }
	const TArray<FRogue10mFloatingDamageEntry>& GetFloatingDamageEntries() const { return FloatingDamageEntries; }

	ARogue10mBasicMonster* FindLookedAtMonster() const;
	float GetPlayerDamageFeedbackAlpha() const;
	float GetPlayerDamageFeedbackStrength() const { return PlayerDamageFeedbackStrength; }
	FLinearColor GetAimCrossLineColor() const { return AimCrossLineColor; }
	float GetAimCrossLineLength() const { return AimCrossLineLength; }
	float GetAimCrossLineGap() const { return AimCrossLineGap; }
	float GetAimCrossLineThickness() const { return AimCrossLineThickness; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(EditAnywhere, Config, Category="Input|Touch Controls")
	bool bForceTouchControls = false;

	UPROPERTY(EditAnywhere, Category="Rogue10m|HUD")
	TSubclassOf<URogue10mRunHUD> RunHUDClass;

	UPROPERTY(EditDefaultsOnly, Category="Rogue10m|HUD|Damage Indicator")
	TSubclassOf<URogue10mDamageIndicatorWidget> DamageIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator", meta=(ClampMin="0.2", ClampMax="3.0"))
	float DamageIndicatorDuration = 1.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator", meta=(ClampMin="0.0", ClampMax="200.0"))
	float DamageIndicatorHeightOffset = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator|Pool", meta=(ClampMin="0", ClampMax="64"))
	int32 InitialDamageIndicatorPoolSize = 24;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Damage Indicator|Pool", meta=(ClampMin="1", ClampMax="128"))
	int32 MaxDamageIndicatorPoolSize = 48;

	/** RunHUDClass가 비어 있으면 기본 메인 HUD Widget Blueprint를 로드합니다. */
	UPROPERTY(EditAnywhere, Category="Rogue10m|HUD")
	TSoftClassPtr<URogue10mRunHUD> DefaultRunHUDClass;

	UPROPERTY()
	TObjectPtr<URogue10mRunHUD> RunHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Aim")
	FLinearColor AimCrossLineColor = FLinearColor(0.72f, 0.92f, 1.0f, 0.86f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Aim", meta=(ClampMin="4.0", ClampMax="48.0"))
	float AimCrossLineLength = 16.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Aim", meta=(ClampMin="0.0", ClampMax="32.0"))
	float AimCrossLineGap = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Aim", meta=(ClampMin="1.0", ClampMax="6.0"))
	float AimCrossLineThickness = 2.0f;

private:
	bool ShouldUseTouchControls() const;
	void InitializeRunHUD();
	TSubclassOf<URogue10mRunHUD> ResolveRunHUDClass();
	void RefreshInputMode();
	void SetPanelVisible(bool& PanelState, bool bVisible);
	void HandleToggleCombatLog();
	void InitializeDamageIndicatorPool();
	URogue10mDamageIndicatorWidget* AcquireDamageIndicator();
	void ShowDamageIndicatorAtLocation(const FVector& WorldLocation, float DamageAmount);

	UFUNCTION(Client, Reliable)
	void ClientShowDamageIndicator(FVector WorldLocation, float DamageAmount);

	bool bInventoryVisible = false;
	bool bItemWindowVisible = false;
	bool bSkillTreeVisible = false;
	bool bSettingsVisible = false;
	bool bShowCombatLog = true;
	float LookSensitivityX = 1.0f;
	float LookSensitivityY = 1.0f;
	int32 CurrentFpsLimit = 120;
	float PlayerDamageFeedbackEndTime = 0.0f;
	float PlayerDamageFeedbackStrength = 0.0f;

	TArray<FRogue10mRuntimeLogEntry> CombatLogEntries;
	TArray<FRogue10mRuntimeLogEntry> ItemAcquisitionEntries;
	TArray<FRogue10mFloatingDamageEntry> FloatingDamageEntries;

	UPROPERTY(Transient)
	TArray<TObjectPtr<URogue10mDamageIndicatorWidget>> DamageIndicatorPool;
};