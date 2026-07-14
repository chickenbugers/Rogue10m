// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mMenuWindowWidgets.generated.h"

class URogue10mAttackSkillData;
class URogue10mCombatComponent;
class URogue10mInventoryComponent;
class URogue10mItemDataAsset;
class URogue10mItemDragDropOperation;
class UImage;
class UCanvasPanel;
class UBorder;
class UButton;
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
class ROGUE10M_API URogue10mInventoryCellWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGridPosition(FIntPoint InGridPosition) { GridPosition = InGridPosition; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	FIntPoint GetGridPosition() const { return GridPosition; }

private:
	UPROPERTY(Transient)
	FIntPoint GridPosition = FIntPoint::ZeroValue;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeGridItem(URogue10mInventoryComponent* InInventory, class URogue10mInventoryWindowWidget* InInventoryWindow,
		int32 InContainerIndex, const struct FRogue10mInventoryGridEntry& InEntry, float InCellSize);
	void SetPlacementPreview(bool bPreviewing, bool bCanPlace);
	void SetPreviewSize(FIntPoint GridSize, float InCellSize);
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory") TObjectPtr<UImage> UI_InventoryItemIcon;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory") TObjectPtr<UTextBlock> UI_InventoryItemQuantityText;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory") TObjectPtr<UBorder> UI_InventoryItemPreviewBorder;
private:
	UPROPERTY(Transient) TObjectPtr<URogue10mInventoryComponent> Inventory;
	UPROPERTY(Transient) TObjectPtr<class URogue10mInventoryWindowWidget> InventoryWindow;
	UPROPERTY(Transient) TObjectPtr<const URogue10mItemDataAsset> ItemData;
	int32 ContainerIndex = INDEX_NONE;
	FGuid InstanceId;
	int32 Quantity = 0;
	bool bRotatedClockwise = false;
	float CellSize = 44.0f;
	FIntPoint DragGrabOffset = FIntPoint::ZeroValue;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mBagTabWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeBagTab(class URogue10mInventoryWindowWidget* InInventoryWindow, int32 InContainerIndex,
		const FText& DisplayName, bool bSelected);
protected:
	virtual void NativeConstruct() override;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory") TObjectPtr<UButton> UI_BagTabButton;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory") TObjectPtr<UTextBlock> UI_BagNameText;

private:
	UFUNCTION() void HandleBagTabClicked();
	UPROPERTY(Transient) TObjectPtr<class URogue10mInventoryWindowWidget> InventoryWindow;
	int32 ContainerIndex = INDEX_NONE;
};
UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mInventoryWindowWidget : public URogue10mMenuWindowWidget
{
	GENERATED_BODY()

public:
	virtual void InitializeMenuWindow(URogue10mInventoryComponent* InInventoryComponent) override;
	virtual void SetWindowOpen(bool bOpen) override;

	void BeginGridItemDrag(URogue10mItemDragDropOperation* Operation);
	void EndGridItemDrag(URogue10mItemDragDropOperation* Operation);
	void SelectInventoryContainer(int32 ContainerIndex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory")
	TObjectPtr<UUniformGridPanel> UI_InventoryGrid;

	/** NxM 아이템 위젯을 셀 좌표와 크기에 맞춰 배치하는 Grid 상단 레이어입니다. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory")
	TObjectPtr<UCanvasPanel> UI_InventoryItemCanvas;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory")
	TObjectPtr<UHorizontalBox> UI_BagTabContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory")
	TObjectPtr<UTextBlock> UI_InventoryMoneyText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Inventory")
	TObjectPtr<UTextBlock> UI_InventoryWeightText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TSubclassOf<URogue10mInventoryCellWidget> InventoryCellWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TSubclassOf<URogue10mInventoryItemWidget> InventoryItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TSubclassOf<URogue10mBagTabWidget> BagTabWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory", meta=(ClampMin="16.0"))
	float InventoryCellSize = 44.0f;

private:
	UFUNCTION()
	void HandleInventoryGridChanged();

	void RefreshInventoryDisplay();
	void RebuildInventoryCells(FIntPoint GridSize);
	void RebuildInventoryItems();
	void RebuildBagTabs();
	bool UpdateGridDropPreview(const FDragDropEvent& DragDropEvent, URogue10mItemDragDropOperation* Operation);
	void ClearGridDropPreview();

	FIntPoint CachedGridSize = FIntPoint::ZeroValue;
	int32 DisplayedContainerIndex = 0;
	TWeakObjectPtr<URogue10mItemDragDropOperation> ActiveGridDragOperation;
};

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mEquipmentWindowWidget : public URogue10mMenuWindowWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Equipment")
	TObjectPtr<UVerticalBox> UI_EquipmentSlotContainer;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Equipment")
	TObjectPtr<UImage> UI_CharacterPreviewImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Equipment")
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
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Skill Tree")
	TObjectPtr<UTextBlock> UI_SkillNameText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Skill Tree")
	TObjectPtr<UImage> UI_SkillIconImage;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Skill Tree")
	TObjectPtr<UTextBlock> UI_SkillDescriptionText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Skill Tree")
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
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Skill Tree")
	TObjectPtr<UPanelWidget> UI_SkillListContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Tree")
	TSubclassOf<URogue10mSkillTreeEntryWidget> SkillTreeEntryWidgetClass;

private:
	URogue10mCombatComponent* GetCombatComponent() const;
};
