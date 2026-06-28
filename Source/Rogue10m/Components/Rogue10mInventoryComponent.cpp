// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mInventoryComponent.h"

#include "Rogue10mCharacter.h"

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

	// 프로토타입 검증을 위해 현재 기획된 무기 6종을 기본 아이템으로 배치합니다.
	ItemSlots = {
		MakeWeaponItem(ERogue10mWeaponType::Dagger, TEXT("Dagger"), TEXT("A fast one-handed blade for close-range attacks."), FLinearColor(0.82f, 0.74f, 0.56f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::GreatSword, TEXT("Great Sword"), TEXT("A heavy two-handed blade for slow, powerful attacks."), FLinearColor(0.75f, 0.52f, 0.36f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::DualBlades, TEXT("Dual Blades"), TEXT("Paired blades for rapid multi-hit attacks."), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Bow, TEXT("Bow"), TEXT("A ranged weapon for charged shots and spacing."), FLinearColor(0.32f, 0.76f, 0.42f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Staff, TEXT("Staff"), TEXT("A catalyst weapon prepared for spell actions."), FLinearColor(0.58f, 0.42f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Knuckle, TEXT("Knuckle"), TEXT("Close-range fist gear for punch-focused combat."), FLinearColor(0.95f, 0.46f, 0.24f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Relic, TEXT("Relic"), TEXT("A prototype relic that channels run energy."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Charm, TEXT("Charm"), TEXT("A light charm prepared for early build equipment tests."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Ring, TEXT("Ring"), TEXT("A simple ring that can be equipped from the item window."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("Core"), TEXT("Material used for future crafting and upgrades."), 6, FLinearColor(0.55f, 0.28f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Consumable, ERogue10mInventorySlotType::Consumable, TEXT("Orb"), TEXT("Consumable placeholder. Usage behavior is not implemented yet."), 2, FLinearColor(0.15f, 0.45f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("Card"), TEXT("Quest item placeholder for story or NPC progression."), 7, FLinearColor(1.0f, 0.76f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("Chest"), TEXT("Reward chest placeholder."), 1, FLinearColor(0.65f, 0.38f, 0.08f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("Chest"), TEXT("Stacked reward chest placeholder."), 2, FLinearColor(0.1f, 0.48f, 0.72f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("Chest"), TEXT("Stacked reward chest placeholder."), 2, FLinearColor(0.1f, 0.48f, 0.72f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Relic, TEXT("Relic"), TEXT("Locked relic. Unlock conditions will be added later."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f), true),
		MakeItem(ERogue10mItemCategory::Consumable, ERogue10mInventorySlotType::Consumable, TEXT("Bag"), TEXT("Consumable bundle placeholder."), 30, FLinearColor(0.1f, 0.45f, 0.85f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Currency, ERogue10mInventorySlotType::Material, TEXT("Fire"), TEXT("Currency-like resource placeholder."), 35, FLinearColor(1.0f, 0.18f, 0.04f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("Scroll"), TEXT("Quest scroll placeholder."), 5, FLinearColor(0.95f, 0.42f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("Book"), TEXT("Quest book placeholder."), 5, FLinearColor(0.95f, 0.15f, 0.45f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Relic, TEXT("Relic"), TEXT("Locked relic. Unlock conditions will be added later."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f), true)
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
	Slot.bHasEquippedItem = false;
	Slot.EquippedItem = MakeEmptyItem();
	return Slot;
}

FRogue10mItemStack URogue10mInventoryComponent::MakeItem(ERogue10mItemCategory Category, ERogue10mInventorySlotType EquipSlotType, const TCHAR* DisplayName, const TCHAR* Description, int32 Quantity, const FLinearColor& ItemColor, bool bLocked)
{
	FRogue10mItemStack Item;
	Item.Category = Category;
	Item.EquipSlotType = EquipSlotType;
	Item.DisplayName = FText::FromString(FString(DisplayName));
	Item.Description = FText::FromString(FString(Description));
	Item.Quantity = FMath::Max(1, Quantity);
	Item.ItemColor = ItemColor;
	Item.bLocked = bLocked;
	Item.bOccupied = true;
	return Item;
}

FRogue10mItemStack URogue10mInventoryComponent::MakeWeaponItem(ERogue10mWeaponType WeaponType, const TCHAR* DisplayName, const TCHAR* Description, const FLinearColor& ItemColor, bool bLocked)
{
	FRogue10mItemStack Item = MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Weapon, DisplayName, Description, 1, ItemColor, bLocked);
	Item.WeaponType = WeaponType;
	return Item;
}

FRogue10mItemStack URogue10mInventoryComponent::MakeEmptyItem()
{
	FRogue10mItemStack Item;
	Item.DisplayName = FText::GetEmpty();
	Item.Description = FText::GetEmpty();
	Item.Quantity = 0;
	Item.ItemColor = FLinearColor(0.03f, 0.035f, 0.04f, 1.0f);
	Item.bOccupied = false;
	return Item;
}

bool URogue10mInventoryComponent::TryEquipItemFromSlot(int32 ItemSlotIndex)
{
	if (!ItemSlots.IsValidIndex(ItemSlotIndex))
	{
		return false;
	}

	return TryEquipItemToSlot(ItemSlotIndex, ItemSlots[ItemSlotIndex].EquipSlotType);
}

bool URogue10mInventoryComponent::TryEquipItemToSlot(int32 ItemSlotIndex, ERogue10mInventorySlotType TargetSlotType)
{
	// 인덱스, 잠금 상태, 장비 카테고리, 목표 슬롯 호환성을 순서대로 검증합니다.
	if (!ItemSlots.IsValidIndex(ItemSlotIndex))
	{
		return false;
	}

	const FRogue10mItemStack Item = ItemSlots[ItemSlotIndex];
	if (!Item.bOccupied || Item.bLocked || Item.Category != ERogue10mItemCategory::Equipment)
	{
		return false;
	}

	if (Item.EquipSlotType != TargetSlotType)
	{
		return false;
	}

	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(TargetSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked)
	{
		return false;
	}

	EquipmentSlot->DisplayName = Item.DisplayName;
	EquipmentSlot->SlotColor = Item.ItemColor;
	EquipmentSlot->bEquipped = true;
	const FRogue10mItemStack PreviousEquippedItem = EquipmentSlot->EquippedItem;
	const bool bHadEquippedItem = EquipmentSlot->bHasEquippedItem && PreviousEquippedItem.bOccupied;
	EquipmentSlot->bHasEquippedItem = true;
	EquipmentSlot->EquippedItem = Item;
	ItemSlots[ItemSlotIndex] = bHadEquippedItem ? PreviousEquippedItem : MakeEmptyItem();

	// 무기 장착 성공 시 캐릭터의 현재 무기 타입도 함께 갱신합니다.
	if (TargetSlotType == ERogue10mInventorySlotType::Weapon)
	{
		if (ARogue10mCharacter* OwningCharacter = Cast<ARogue10mCharacter>(GetOwner()))
		{
			OwningCharacter->SetEquippedWeaponType(Item.WeaponType);
		}
	}

	return true;
}

bool URogue10mInventoryComponent::TryUnequipItemFromSlot(ERogue10mInventorySlotType TargetSlotType)
{
	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(TargetSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked || !EquipmentSlot->bHasEquippedItem || !EquipmentSlot->EquippedItem.bOccupied)
	{
		return false;
	}

	const int32 EmptyItemSlotIndex = FindFirstEmptyItemSlot();
	if (EmptyItemSlotIndex == INDEX_NONE)
	{
		return false;
	}

	ItemSlots[EmptyItemSlotIndex] = EquipmentSlot->EquippedItem;
	EquipmentSlot->EquippedItem = MakeEmptyItem();
	EquipmentSlot->bHasEquippedItem = false;
	EquipmentSlot->bEquipped = false;

	if (TargetSlotType == ERogue10mInventorySlotType::Weapon)
	{
		EquipmentSlot->DisplayName = FText::FromString(TEXT("Fists"));
		EquipmentSlot->SlotColor = FLinearColor(0.86f, 0.64f, 0.30f, 1.0f);
		EquipmentSlot->bEquipped = true;
		if (ARogue10mCharacter* OwningCharacter = Cast<ARogue10mCharacter>(GetOwner()))
		{
			OwningCharacter->SetEquippedWeaponType(ERogue10mWeaponType::Unarmed);
		}
	}
	else
	{
		switch (TargetSlotType)
		{
		case ERogue10mInventorySlotType::Head:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Head"));
			break;
		case ERogue10mInventorySlotType::Chest:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Chest"));
			break;
		case ERogue10mInventorySlotType::Legs:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Legs"));
			break;
		case ERogue10mInventorySlotType::Hands:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Hands"));
			break;
		case ERogue10mInventorySlotType::Charm:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Charm"));
			break;
		case ERogue10mInventorySlotType::Ring:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Ring"));
			break;
		case ERogue10mInventorySlotType::Relic:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Relic"));
			break;
		default:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("Empty"));
			break;
		}
		EquipmentSlot->SlotColor = FLinearColor(0.28f, 0.28f, 0.3f, 1.0f);
	}

	return true;
}

bool URogue10mInventoryComponent::TryMoveItemSlot(int32 SourceItemSlotIndex, int32 TargetItemSlotIndex)
{
	// 비어 있지 않은 잠금 해제 아이템만 이동하며, 대상이 차 있으면 서로 교체합니다.
	if (!ItemSlots.IsValidIndex(SourceItemSlotIndex) || !ItemSlots.IsValidIndex(TargetItemSlotIndex) || SourceItemSlotIndex == TargetItemSlotIndex)
	{
		return false;
	}

	FRogue10mItemStack& SourceItem = ItemSlots[SourceItemSlotIndex];
	FRogue10mItemStack& TargetItem = ItemSlots[TargetItemSlotIndex];
	if (!SourceItem.bOccupied || SourceItem.bLocked || TargetItem.bLocked)
	{
		return false;
	}

	if (TargetItem.bOccupied)
	{
		Swap(SourceItem, TargetItem);
	}
	else
	{
		TargetItem = SourceItem;
		SourceItem = MakeEmptyItem();
	}

	return true;
}

FRogue10mInventorySlot* URogue10mInventoryComponent::FindEquipmentSlot(ERogue10mInventorySlotType SlotType)
{
	for (FRogue10mInventorySlot& Slot : LeftEquipmentSlots)
	{
		if (Slot.SlotType == SlotType)
		{
			return &Slot;
		}
	}

	for (FRogue10mInventorySlot& Slot : RightEquipmentSlots)
	{
		if (Slot.SlotType == SlotType)
		{
			return &Slot;
		}
	}

	return nullptr;
}

int32 URogue10mInventoryComponent::FindFirstEmptyItemSlot() const
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		if (!ItemSlots[Index].bOccupied)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}
