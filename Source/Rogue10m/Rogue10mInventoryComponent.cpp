// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mInventoryComponent.h"

URogue10mInventoryComponent::URogue10mInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LeftEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::Head, TEXT("Head"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), true, false),
		MakeSlot(ERogue10mInventorySlotType::Chest, TEXT("Chest"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Legs, TEXT("Legs"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), true, false),
		MakeSlot(ERogue10mInventorySlotType::Hands, TEXT("Hands"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Weapon, TEXT("Fists"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, true)
	};

	RightEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::Charm, TEXT("Charm"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Ring, TEXT("Ring"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Relic, TEXT("Relic"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Consumable, TEXT("Potion"), FLinearColor(0.12f, 0.85f, 0.22f, 1.0f), true, false),
		MakeSlot(ERogue10mInventorySlotType::Material, TEXT("Core"), FLinearColor(0.72f, 0.42f, 1.0f, 1.0f), false, true)
	};

	ItemSlots = {
		MakeItem(ERogue10mItemCategory::Equipment, TEXT("Relic"), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, TEXT("Charm"), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, TEXT("Ring"), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, TEXT("Core"), 6, FLinearColor(0.55f, 0.28f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Consumable, TEXT("Orb"), 2, FLinearColor(0.15f, 0.45f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, TEXT("Card"), 7, FLinearColor(1.0f, 0.76f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, TEXT("Chest"), 1, FLinearColor(0.65f, 0.38f, 0.08f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, TEXT("Chest"), 2, FLinearColor(0.1f, 0.48f, 0.72f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, TEXT("Chest"), 2, FLinearColor(0.1f, 0.48f, 0.72f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, TEXT("Relic"), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f), true),
		MakeItem(ERogue10mItemCategory::Consumable, TEXT("Bag"), 30, FLinearColor(0.1f, 0.45f, 0.85f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Currency, TEXT("Fire"), 35, FLinearColor(1.0f, 0.18f, 0.04f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, TEXT("Scroll"), 5, FLinearColor(0.95f, 0.42f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, TEXT("Book"), 5, FLinearColor(0.95f, 0.15f, 0.45f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, TEXT("Relic"), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f), true)
	};

	while (ItemSlots.Num() < 80)
	{
		ItemSlots.Add(MakeEmptyItem());
	}
}

FRogue10mInventorySlot URogue10mInventoryComponent::MakeSlot(ERogue10mInventorySlotType SlotType, const TCHAR* DisplayName, const FLinearColor& SlotColor, bool bLocked, bool bEquipped)
{
	FRogue10mInventorySlot Slot;
	Slot.SlotType = SlotType;
	Slot.DisplayName = FText::FromString(FString(DisplayName));
	Slot.SlotColor = SlotColor;
	Slot.bLocked = bLocked;
	Slot.bEquipped = bEquipped;
	return Slot;
}

FRogue10mItemStack URogue10mInventoryComponent::MakeItem(ERogue10mItemCategory Category, const TCHAR* DisplayName, int32 Quantity, const FLinearColor& ItemColor, bool bLocked)
{
	FRogue10mItemStack Item;
	Item.Category = Category;
	Item.DisplayName = FText::FromString(FString(DisplayName));
	Item.Quantity = FMath::Max(1, Quantity);
	Item.ItemColor = ItemColor;
	Item.bLocked = bLocked;
	Item.bOccupied = true;
	return Item;
}

FRogue10mItemStack URogue10mInventoryComponent::MakeEmptyItem()
{
	FRogue10mItemStack Item;
	Item.DisplayName = FText::GetEmpty();
	Item.Quantity = 0;
	Item.ItemColor = FLinearColor(0.03f, 0.035f, 0.04f, 1.0f);
	Item.bOccupied = false;
	return Item;
}
