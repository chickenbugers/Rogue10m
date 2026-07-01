// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Rogue10mRunHUD.h"
#include "Rogue10mHudWidgetParts.h"
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
	TObjectPtr<URogue10mVitalBarWidget> ManaBarWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|HUD|Bound Widgets")
	TObjectPtr<URogue10mProgressionWidget> ProgressionWidget;

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

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Bound Widget Data Refreshed"))
	void BP_OnBoundWidgetDataRefreshed();

	/** Widget Blueprint가 아직 비어 있을 때 런타임에서 확인 가능한 임시 HUD 골격을 만든다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|HUD|Prototype")
	bool bCreatePrototypeLayoutWhenEmpty = true;

private:
	void AssignOwningMainHUDToBoundWidgets();
	void EnsurePrototypeLayout();
};
