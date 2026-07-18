// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Rogue10mItemDragDropOperation.h"

#include "Rogue10mInventoryComponent.h"
#include "Rogue10mItemDataAsset.h"
#include "Components/Border.h"
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
	if (InventoryWindow
		&& (Source == ERogue10mItemDragSource::GridInventory || Source == ERogue10mItemDragSource::Equipment))
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
void URogue10mItemDragDropOperation::InitializeEquipmentItemDrag(
	URogue10mInventoryComponent* InInventory,
	ERogue10mInventorySlotType InEquipmentSlotType,
	const URogue10mItemDataAsset* InItemData,
	int32 InQuantity,
	float InCellSize)
{
	InitializeItemDrag(InInventory, ERogue10mItemDragSource::Equipment, INDEX_NONE);
	EquipmentSlotType = InEquipmentSlotType;
	ItemData = InItemData;
	Quantity = FMath::Max(1, InQuantity);
	CellSize = InCellSize;
}

void URogue10mItemDragDropOperation::SetGridPlacementPreview(bool bPreviewing, bool bCanPlace)
{
	bPreviewCanPlace = bPreviewing && bCanPlace;
	if (PreviewWidget)
	{
		PreviewWidget->SetPlacementPreview(bPreviewing, bCanPlace);
	}
	if (PreviewBorder)
	{
		PreviewBorder->SetBrushColor(bPreviewing
			? (bCanPlace
				? FLinearColor(0.1f, 0.8f, 0.2f, 0.45f)
				: FLinearColor(0.9f, 0.08f, 0.05f, 0.55f))
			: PreviewNeutralColor);
	}
}
