// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mInventoryComponent.h"

#include "Rogue10mCharacter.h"

URogue10mInventoryComponent::URogue10mInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	LeftEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::MainWeapon, TEXT("주무기"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Head, TEXT("머리"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Armor, TEXT("갑옷"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Shoes, TEXT("신발"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false)
	};

	RightEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::SecondaryWeapon, TEXT("보조무기"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Ring, TEXT("반지"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Earring, TEXT("귀걸이"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false)
	};

	// 프로토타입 검증을 위해 현재 기획된 무기 6종을 기본 아이템으로 배치합니다.
	ItemSlots = {
		MakeWeaponItem(ERogue10mWeaponType::Dagger, TEXT("단검"), TEXT("빠른 근접 공격을 위한 한손 단검입니다."), FLinearColor(0.82f, 0.74f, 0.56f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::GreatSword, TEXT("대검"), TEXT("느리지만 강한 피해를 주는 양손 대검입니다."), FLinearColor(0.75f, 0.52f, 0.36f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::DualBlades, TEXT("쌍검"), TEXT("연속 공격과 빠른 타격에 어울리는 쌍검입니다."), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Bow, TEXT("활"), TEXT("거리 유지와 차징 사격을 위한 원거리 무기입니다."), FLinearColor(0.32f, 0.76f, 0.42f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Staff, TEXT("지팡이"), TEXT("마법 계열 액션을 연결하기 위한 촉매 무기입니다."), FLinearColor(0.58f, 0.42f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Knuckle, TEXT("너클"), TEXT("주먹 공격을 강화하는 근접 전투 장비입니다."), FLinearColor(0.95f, 0.46f, 0.24f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Dagger, TEXT("보조 단검"), TEXT("보조무기 슬롯에 장착하는 짧은 단검입니다."), FLinearColor(0.68f, 0.82f, 1.0f, 1.0f), ERogue10mInventorySlotType::SecondaryWeapon),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Head, TEXT("수련용 머리장식"), TEXT("초기 테스트용 머리 장비입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Armor, TEXT("수련용 갑옷"), TEXT("기본 방어구 슬롯 동작을 확인하기 위한 갑옷입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Shoes, TEXT("수련용 신발"), TEXT("이동 장비 슬롯 테스트용 신발입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Ring, TEXT("초심자의 반지"), TEXT("반지 슬롯에 장착 가능한 기본 장신구입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Earring, TEXT("초심자의 귀걸이"), TEXT("귀걸이 슬롯에 장착 가능한 기본 장신구입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("강화 코어"), TEXT("추후 제작과 강화에 사용할 재료입니다."), 6, FLinearColor(0.55f, 0.28f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Consumable, ERogue10mInventorySlotType::Consumable, TEXT("회복 구슬"), TEXT("소모품 자리 표시자입니다. 사용 효과는 이후 구현합니다."), 2, FLinearColor(0.15f, 0.45f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("낡은 카드"), TEXT("스토리 또는 NPC 진행에 사용할 퀘스트 아이템입니다."), 7, FLinearColor(1.0f, 0.76f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("보상 상자"), TEXT("획득 보상을 표현하기 위한 임시 상자입니다."), 1, FLinearColor(0.65f, 0.38f, 0.08f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Material, ERogue10mInventorySlotType::Material, TEXT("보상 상자"), TEXT("여러 개를 보유할 수 있는 보상 상자입니다."), 2, FLinearColor(0.1f, 0.48f, 0.72f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Currency, ERogue10mInventorySlotType::Material, TEXT("불꽃 조각"), TEXT("추후 재화 컴포넌트와 연결할 임시 재화형 자원입니다."), 35, FLinearColor(1.0f, 0.18f, 0.04f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("탐험 두루마리"), TEXT("플레이 맵 진입 스토리에 연결할 수 있는 퀘스트 문서입니다."), 5, FLinearColor(0.95f, 0.42f, 0.12f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Quest, ERogue10mInventorySlotType::Material, TEXT("기록서"), TEXT("월드 설정과 NPC 대화에 사용할 수 있는 기록 아이템입니다."), 5, FLinearColor(0.95f, 0.15f, 0.45f, 1.0f))
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

FRogue10mItemStack URogue10mInventoryComponent::MakeWeaponItem(ERogue10mWeaponType WeaponType, const TCHAR* DisplayName, const TCHAR* Description, const FLinearColor& ItemColor, ERogue10mInventorySlotType EquipSlotType, bool bLocked)
{
	FRogue10mItemStack Item = MakeItem(ERogue10mItemCategory::Equipment, EquipSlotType, DisplayName, Description, 1, ItemColor, bLocked);
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
	if (TargetSlotType == ERogue10mInventorySlotType::MainWeapon)
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

	if (TargetSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		EquipmentSlot->DisplayName = FText::FromString(TEXT("주무기"));
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
		case ERogue10mInventorySlotType::SecondaryWeapon:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("보조무기"));
			break;
		case ERogue10mInventorySlotType::Head:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("머리"));
			break;
		case ERogue10mInventorySlotType::Armor:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("갑옷"));
			break;
		case ERogue10mInventorySlotType::Shoes:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("신발"));
			break;
		case ERogue10mInventorySlotType::Ring:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("반지"));
			break;
		case ERogue10mInventorySlotType::Earring:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("귀걸이"));
			break;
		default:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("빈 슬롯"));
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
