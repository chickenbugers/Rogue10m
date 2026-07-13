// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mMenuWindowWidgets.generated.h"

class URogue10mAttackSkillData;
class URogue10mCombatComponent;
class URogue10mInventoryComponent;
class UImage;
class UCanvasPanel;
class UHorizontalBox;
class UPanelWidget;
class UTextBlock;
class UUniformGridPanel;
class UVerticalBox;

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mMenuWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	virtual void InitializeMenuWindow(URogue10mInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|UI")
	virtual void SetWindowOpen(bool bOpen);

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	URogue10mInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|UI")
	bool IsWindowOpen() const { return bWindowOpen; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|UI", meta=(DisplayName="Menu Window Initialized"))
	void BP_OnMenuWindowInitialized();

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|UI", meta=(DisplayName="Menu Window Open Changed"))
	void BP_OnWindowOpenChanged(bool bOpen);

private:
	UPROPERTY(Transient)
	TObjectPtr<URogue10mInventoryComponent> InventoryComponent;

	bool bWindowOpen = false;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mInventoryWindowWidget : public URogue10mMenuWindowWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Inventory")
	TObjectPtr<UUniformGridPanel> UI_InventoryGrid;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Inventory")
	TObjectPtr<UTextBlock> UI_InventoryCapacityText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Inventory")
	TObjectPtr<UHorizontalBox> UI_BagTabContainer;

	/** NxM 아이템 위젯을 셀 좌표와 크기에 맞춰 배치하는 Grid 상단 레이어입니다. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Inventory")
	TObjectPtr<UCanvasPanel> UI_InventoryItemCanvas;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mEquipmentWindowWidget : public URogue10mMenuWindowWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Equipment")
	TObjectPtr<UVerticalBox> UI_EquipmentSlotContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Equipment")
	TObjectPtr<UImage> UI_CharacterPreviewImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Equipment")
	TObjectPtr<UVerticalBox> UI_EquipmentStatsContainer;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mSkillTreeEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	void SetSkillData(URogue10mAttackSkillData* InSkillData, bool bInUnlocked);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Skill Tree")
	TObjectPtr<UTextBlock> UI_SkillNameText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Skill Tree")
	TObjectPtr<UImage> UI_SkillIconImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Skill Tree")
	TObjectPtr<UTextBlock> UI_SkillDescriptionText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Skill Tree")
	TObjectPtr<UTextBlock> UI_SkillLockText;

private:
	UPROPERTY(Transient)
	TObjectPtr<URogue10mAttackSkillData> SkillData;

	bool bUnlocked = false;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mSkillTreeWindowWidget : public URogue10mMenuWindowWidget
{
	GENERATED_BODY()

public:
	virtual void InitializeMenuWindow(URogue10mInventoryComponent* InInventoryComponent) override;
	virtual void SetWindowOpen(bool bOpen) override;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	void RefreshSkillTree();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category="Rogue10m|Skill Tree")
	TObjectPtr<UPanelWidget> UI_SkillListContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Tree")
	TSubclassOf<URogue10mSkillTreeEntryWidget> SkillTreeEntryWidgetClass;

private:
	URogue10mCombatComponent* GetCombatComponent() const;
};