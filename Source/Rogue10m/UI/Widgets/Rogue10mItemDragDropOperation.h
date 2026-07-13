// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Rogue10mItemDragDropOperation.generated.h"

class URogue10mInventoryComponent;

UENUM(BlueprintType)
enum class ERogue10mItemDragSource : uint8
{
	Inventory,
	QuickSlot
};

/** Typed payload shared by inventory cells and consumable quick slots. */
UCLASS(BlueprintType)
class ROGUE10M_API URogue10mItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items|Drag Drop")
	void InitializeItemDrag(URogue10mInventoryComponent* InInventory, ERogue10mItemDragSource InSource, int32 InSourceIndex);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items|Drag Drop")
	void MarkDropHandled() { bDropHandled = true; }

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	ERogue10mItemDragSource Source = ERogue10mItemDragSource::Inventory;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	int32 SourceIndex = INDEX_NONE;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Rogue10m|Items|Drag Drop")
	TObjectPtr<URogue10mInventoryComponent> Inventory;

protected:
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

private:
	bool bDropHandled = false;
};
