// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mItemDragDropOperation.h"

#include "Rogue10mInventoryComponent.h"
#include "Rogue10mItemDataAsset.h"
#include "Widgets/Rogue10mMenuWindowWidgets.h"

void URogue10mItemDragDropOperation::InitializeItemDrag(
	URogue10mInventoryComponent* InInventory, ERogue10mItemDragSource InSource, int32 InSourceIndex)
{
	Inventory = InInventory;
	Source = InSource;
	SourceIndex = InSourceIndex;
	bDropHandled = false;
}

void URogue10mItemDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);
	if (InventoryWindow && Source == ERogue10mItemDragSource::GridInventory)
	{
		InventoryWindow->EndGridItemDrag(this);
	}
	if (!bDropHandled && Source == ERogue10mItemDragSource::QuickSlot && Inventory)
	{
		Inventory->UnassignConsumableQuickSlot(SourceIndex);
	}
}

void URogue10mItemDragDropOperation::InitializeGridItemDrag(
	URogue10mInventoryComponent* InInventory,
	URogue10mInventoryWindowWidget* InInventoryWindow,
	int32 InSourceContainerIndex,
	FGuid InInstanceId,
	const URogue10mItemDataAsset* InItemData,
	float InCellSize)
{
	InitializeItemDrag(InInventory, ERogue10mItemDragSource::GridInventory, INDEX_NONE);
	InventoryWindow = InInventoryWindow;
	SourceContainerIndex = InSourceContainerIndex;
	InstanceId = InInstanceId;
	ItemData = InItemData;
	CellSize = InCellSize;
}