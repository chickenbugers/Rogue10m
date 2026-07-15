// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Rogue10mRunHUD.h"
#include "Rogue10mHudWidgetParts.h"

class UPanelWidget;

#include "Rogue10mMainHUDWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mMainHUDWidget : public URogue10mRunHUD
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|HUD")
	void RefreshBoundWidgetData();

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mVitalBarWidget> HealthBarWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mVitalBarWidget> StaminaBarWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mVitalBarWidget> IdentityBarWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mProgressionWidget> ProgressionWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mSkillSlotPanelWidget> SkillSlotPanelWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mIdentityWidget> IdentityWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mMonsterInfoWidget> MonsterInfoWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mShortcutHintWidget> EquipmentShortcutWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mShortcutHintWidget> ItemWindowShortcutWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mShortcutHintWidget> SkillTreeShortcutWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mShortcutHintWidget> SettingsShortcutWidget;


	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Containers")
	TObjectPtr<UPanelWidget> SkillSlotContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Containers")
	TObjectPtr<UPanelWidget> ItemSlotContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Containers")
	TObjectPtr<UPanelWidget> SystemLogContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Containers")
	TObjectPtr<UPanelWidget> ItemAcquisitionContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Widget Classes")
	TSubclassOf<URogue10mQuickSlotWidget> QuickSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|HUD|Widget Classes")
	TSubclassOf<URogue10mLogLineWidget> LogLineWidgetClass;
	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Bound Widget Data Refreshed"))
	void BP_OnBoundWidgetDataRefreshed();

	/** Widget Blueprint가 아직 비어 있을 때 런타임에서 확인 가능한 임시 HUD 골격을 만든다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|HUD|Prototype")
	bool bCreatePrototypeLayoutWhenEmpty = false;

private:
	void AssignOwningMainHUDToBoundWidgets();
	void EnsurePrototypeLayout();
	void RefreshQuickSlotContainer(UPanelWidget* Container, const TArray<FRogue10mHudQuickSlotView>& Views);
	void RefreshLogContainer(UPanelWidget* Container, const TArray<FRogue10mHudLogEntryView>& Views);
	void RefreshFrequentWidgetData();
	void RefreshSlowWidgetData();

	UPROPERTY(EditDefaultsOnly, Category="Rogue10m|HUD|Performance", meta=(ClampMin="0.02", Units="s"))
	float FrequentRefreshInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="Rogue10m|HUD|Performance", meta=(ClampMin="0.05", Units="s"))
	float SlowRefreshInterval = 0.25f;

	float FrequentRefreshElapsed = 0.0f;
	float SlowRefreshElapsed = 0.0f;
};
