// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mItemDragDropOperation.generated.h"

class UBorder;
class URogue10mInventoryComponent;
class URogue10mInventoryItemWidget;
class URogue10mInventoryWindowWidget;
class URogue10mItemDataAsset;

UENUM(BlueprintType)
enum class ERogue10mItemDragSource : uint8
{
	Inventory,
	QuickSlot,
	GridInventory,
	Equipment
};

/** Typed payload shared by inventory cells and consumable quick slots. */
UCLASS(BlueprintType)
class ROGUE10M_API URogue10mItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items|Drag Drop")
	void InitializeItemDrag(URogue10mInventoryComponent* InInventory, ERogue10mItemDragSource InSource, int32 InSourceIndex);

	void InitializeGridItemDrag(URogue10mInventoryComponent* InInventory,
		URogue10mInventoryWindowWidget* InInventoryWindow, int32 InSourceContainerIndex,
		FGuid InInstanceId, const URogue10mItemDataAsset* InItemData, float InCellSize);

	void InitializeEquipmentItemDrag(URogue10mInventoryComponent* InInventory,
		ERogue10mInventorySlotType InEquipmentSlotType, const URogue10mItemDataAsset* InItemData,
		int32 InQuantity, float InCellSize);

	void SetGridPlacementPreview(bool bPreviewing, bool bCanPlace);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items|Drag Drop")
	void MarkDropHandled() { bDropHandled = true; }

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	ERogue10mItemDragSource Source = ERogue10mItemDragSource::Inventory;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	int32 SourceIndex = INDEX_NONE;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	TObjectPtr<URogue10mInventoryComponent> Inventory;

	UPROPERTY(Transient)
	TObjectPtr<URogue10mInventoryWindowWidget> InventoryWindow;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	TObjectPtr<const URogue10mItemDataAsset> ItemData;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	int32 SourceContainerIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	FGuid InstanceId;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	ERogue10mInventorySlotType EquipmentSlotType = ERogue10mInventorySlotType::Material;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	int32 Quantity = 0;


	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	FIntPoint PreviewGridPosition = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	FIntPoint GrabCellOffset = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	bool bPreviewCanPlace = false;

	UPROPERTY(Transient)
	TObjectPtr<URogue10mInventoryItemWidget> PreviewWidget;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> PreviewBorder;

	FLinearColor PreviewNeutralColor = FLinearColor(0.05f, 0.05f, 0.05f, 0.35f);

	float CellSize = 44.0f;

protected:
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

private:
	bool bDropHandled = false;
};
