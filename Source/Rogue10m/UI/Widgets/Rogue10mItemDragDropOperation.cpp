// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mItemDragDropOperation.h"

#include "Rogue10mInventoryComponent.h"

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
	if (!bDropHandled && Source == ERogue10mItemDragSource::QuickSlot && Inventory)
	{
		Inventory->UnassignConsumableQuickSlot(SourceIndex);
	}
}
