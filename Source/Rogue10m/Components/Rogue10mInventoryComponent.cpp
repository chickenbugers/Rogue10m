// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mInventoryComponent.h"

#include "Rogue10m.h"

#include "Rogue10mCharacter.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mDroppedItem.h"
#include "Rogue10mItemDataAsset.h"
#include "Rogue10mPlayerController.h"
#include "Engine/World.h"

URogue10mInventoryComponent::URogue10mInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DroppedItemClass = ARogue10mDroppedItem::StaticClass();

	FRogue10mInventoryContainer BaseContainer;
	BaseContainer.ContainerId = TEXT("BaseInventory");
	BaseContainer.DisplayName = FText::FromString(TEXT("기본 인벤토리"));
	BaseContainer.GridSize = BaseInventorySize;
	InventoryContainers.Add(MoveTemp(BaseContainer));
	ConsumableQuickSlotItemIndices.Init(INDEX_NONE, 5);
	ConsumableQuickSlotCooldownEndTimes.Init(0.0f, 5);
	PrototypeStartingItems = {
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Prototype/DA_Item_Prototype_1x1.DA_Item_Prototype_1x1"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Prototype/DA_Item_Prototype_2x3.DA_Item_Prototype_2x3"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Prototype/DA_Item_Prototype_4x3.DA_Item_Prototype_4x3"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_HealthPotion.DA_Item_HealthPotion"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_IronLongsword.DA_Item_IronLongsword"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_IronHelmet.DA_Item_IronHelmet"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_LeatherArmor.DA_Item_LeatherArmor"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_LeatherBoots.DA_Item_LeatherBoots"))),
		TSoftObjectPtr<URogue10mItemDataAsset>(FSoftObjectPath(TEXT("/Game/DataAsset/Item/Starter/DA_Item_NoviceRing.DA_Item_NoviceRing")))
	};

	LeftEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::MainWeapon, TEXT("무기"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, true),
		MakeSlot(ERogue10mInventorySlotType::Head, TEXT("투구"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Armor, TEXT("갑옷"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Hands, TEXT("장갑"), FLinearColor(0.86f, 0.64f, 0.30f, 1.0f), false, false)
	};

	RightEquipmentSlots = {
		MakeSlot(ERogue10mInventorySlotType::Shoes, TEXT("신발"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Ring, TEXT("반지"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false),
		MakeSlot(ERogue10mInventorySlotType::Necklace, TEXT("목걸이"), FLinearColor(0.05f, 0.68f, 1.0f, 1.0f), false, false)
	};

	// 프로토타입 검증을 위해 현재 기획된 무기 6종을 기본 아이템으로 배치합니다.
	ItemSlots = {
		MakeWeaponItem(ERogue10mWeaponType::Dagger, TEXT("단검"), TEXT("빠른 근접 공격을 위한 한손 단검입니다."), FLinearColor(0.82f, 0.74f, 0.56f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::GreatSword, TEXT("대검"), TEXT("느리지만 강한 피해를 주는 양손 대검입니다."), FLinearColor(0.75f, 0.52f, 0.36f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::DualBlades, TEXT("쌍검"), TEXT("연속 공격과 빠른 타격에 어울리는 쌍검입니다."), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Bow, TEXT("활"), TEXT("거리 유지와 차징 사격을 위한 원거리 무기입니다."), FLinearColor(0.32f, 0.76f, 0.42f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Staff, TEXT("지팡이"), TEXT("마법 계열 액션을 연결하기 위한 촉매 무기입니다."), FLinearColor(0.58f, 0.42f, 1.0f, 1.0f)),
		MakeWeaponItem(ERogue10mWeaponType::Knuckle, TEXT("권"), TEXT("주먹 공격을 강화하는 근접 전투 장비입니다."), FLinearColor(0.95f, 0.46f, 0.24f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Hands, TEXT("수련용 장갑"), TEXT("장갑 슬롯에 장착하는 기본 방어구입니다."), 1, FLinearColor(0.68f, 0.82f, 1.0f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Head, TEXT("수련용 머리장식"), TEXT("초기 테스트용 머리 장비입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Armor, TEXT("수련용 갑옷"), TEXT("기본 방어구 슬롯 동작을 확인하기 위한 갑옷입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Shoes, TEXT("수련용 신발"), TEXT("이동 장비 슬롯 테스트용 신발입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Ring, TEXT("초심자의 반지"), TEXT("반지 슬롯에 장착 가능한 기본 장신구입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
		MakeItem(ERogue10mItemCategory::Equipment, ERogue10mInventorySlotType::Necklace, TEXT("초심자의 목걸이"), TEXT("목걸이 슬롯에 장착 가능한 기본 장신구입니다."), 1, FLinearColor(0.16f, 0.8f, 0.95f, 1.0f)),
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

float URogue10mInventoryComponent::GetTotalInventoryWeight() const
{
	float TotalWeight = 0.0f;
	for (const FRogue10mInventoryContainer& Container : InventoryContainers)
	{
		for (const FRogue10mInventoryGridEntry& Entry : Container.Entries)
		{
			if (Entry.ItemData)
			{
				TotalWeight += FMath::Max(0.0f, Entry.ItemData->UnitWeight)
					* static_cast<float>(FMath::Max(0, Entry.Quantity));
			}
		}
	}
	return TotalWeight;
}

void URogue10mInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseInventorySize.X = FMath::Clamp(BaseInventorySize.X, 1, 20);
	BaseInventorySize.Y = FMath::Clamp(BaseInventorySize.Y, 1, 20);
	if (InventoryContainers.IsEmpty())
	{
		FRogue10mInventoryContainer BaseContainer;
		BaseContainer.ContainerId = TEXT("BaseInventory");
		BaseContainer.DisplayName = FText::FromString(TEXT("기본 인벤토리"));
		BaseContainer.GridSize = BaseInventorySize;
		InventoryContainers.Add(MoveTemp(BaseContainer));
	}
	else
	{
		InventoryContainers[0].GridSize = BaseInventorySize;
	}

	TArray<FRogue10mItemStack> StarterSlotItems;
	if (bAddPrototypeStartingItems && InventoryContainers[0].Entries.IsEmpty())
	{
		for (const TSoftObjectPtr<URogue10mItemDataAsset>& ItemReference : PrototypeStartingItems)
		{
			const URogue10mItemDataAsset* ItemData = ItemReference.LoadSynchronous();
			if (!ItemData)
			{
				UE_LOG(LogRogue10m, Warning, TEXT("Prototype inventory item could not be loaded: %s"),
					*ItemReference.ToSoftObjectPath().ToString());
				continue;
			}

			const int32 StartingQuantity = ItemData->MaxStackSize > 1
				? FMath::Min(5, ItemData->MaxStackSize)
				: 1;
			if (ItemData->Category == ERogue10mItemCategory::Equipment
				|| ItemData->Category == ERogue10mItemCategory::Consumable)
			{
				StarterSlotItems.Add(MakeItemFromDataAsset(ItemData, StartingQuantity));
			}


			int32 ContainerIndex = INDEX_NONE;
			FGuid InstanceId;
			if (!TryAddGridItem(ItemData, StartingQuantity, ContainerIndex, InstanceId))
			{
				UE_LOG(LogRogue10m, Warning, TEXT("Prototype inventory item could not be placed: %s"),
					*ItemData->GetName());
			}
		}

		for (int32 Index = StarterSlotItems.Num() - 1; Index >= 0; --Index)
		{
			ItemSlots.Insert(StarterSlotItems[Index], 0);
		}
		if (ItemSlots.Num() > 80)
		{
			ItemSlots.SetNum(80);
		}

		if (bEquipPrototypeStarterEquipment)
		{
			bool bRemovedEquippedStarterFromGrid = false;
			for (int32 ItemSlotIndex = 0; ItemSlotIndex < StarterSlotItems.Num(); ++ItemSlotIndex)
			{
				if (ItemSlots.IsValidIndex(ItemSlotIndex)
					&& ItemSlots[ItemSlotIndex].Category == ERogue10mItemCategory::Equipment)
				{
					const URogue10mItemDataAsset* EquippedItemData = ItemSlots[ItemSlotIndex].ItemData.Get();
					if (TryEquipItemFromSlot(ItemSlotIndex) && EquippedItemData)
					{
						const int32 GridEntryIndex = InventoryContainers[0].Entries.IndexOfByPredicate(
							[EquippedItemData](const FRogue10mInventoryGridEntry& Entry)
							{
								return Entry.ItemData == EquippedItemData;
							});
						if (GridEntryIndex != INDEX_NONE)
						{
							InventoryContainers[0].Entries.RemoveAt(GridEntryIndex);
							bRemovedEquippedStarterFromGrid = true;
						}
					}
				}
			}
			if (bRemovedEquippedStarterFromGrid)
			{
				OnInventoryGridChanged.Broadcast();
			}
		}
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

FRogue10mItemStack URogue10mInventoryComponent::MakeItemFromDataAsset(const URogue10mItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData)
	{
		return MakeEmptyItem();
	}

	const FString DisplayName = ItemData->DisplayName.ToString();
	const FString Description = ItemData->Description.ToString();
	FRogue10mItemStack Item = MakeItem(
		ItemData->Category,
		ItemData->EquipSlotType,
		*DisplayName,
		*Description,
		Quantity,
		ItemData->InventoryTint);
	Item.ItemData = ItemData;
	Item.WeaponType = ItemData->WeaponType;
	Item.RestoreHealth = FMath::Max(0.0f, ItemData->RestoreHealth);
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

	OnEquipmentChanged.Broadcast();
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
		EquipmentSlot->DisplayName = FText::FromString(TEXT("무기"));
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
			EquipmentSlot->DisplayName = FText::FromString(TEXT("투구"));
			break;
		case ERogue10mInventorySlotType::Armor:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("갑옷"));
			break;
		case ERogue10mInventorySlotType::Hands:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("장갑"));
			break;
		case ERogue10mInventorySlotType::Shoes:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("신발"));
			break;
		case ERogue10mInventorySlotType::Ring:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("반지"));
			break;
		case ERogue10mInventorySlotType::Necklace:
			EquipmentSlot->DisplayName = FText::FromString(TEXT("목걸이"));
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

	OnEquipmentChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::TryUnequipItemFromSlotToItemSlot(ERogue10mInventorySlotType TargetSlotType, int32 TargetItemSlotIndex)
{
	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(TargetSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked || !EquipmentSlot->bHasEquippedItem || !EquipmentSlot->EquippedItem.bOccupied)
	{
		return false;
	}

	if (!ItemSlots.IsValidIndex(TargetItemSlotIndex))
	{
		TargetItemSlotIndex = FindFirstEmptyItemSlot();
	}

	if (!ItemSlots.IsValidIndex(TargetItemSlotIndex) || ItemSlots[TargetItemSlotIndex].bLocked || ItemSlots[TargetItemSlotIndex].bOccupied)
	{
		return false;
	}

	ItemSlots[TargetItemSlotIndex] = EquipmentSlot->EquippedItem;
	EquipmentSlot->EquippedItem = MakeEmptyItem();
	EquipmentSlot->bHasEquippedItem = false;
	EquipmentSlot->bEquipped = false;
	ResetEquipmentSlotDisplay(*EquipmentSlot, TargetSlotType);

	if (TargetSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		if (ARogue10mCharacter* OwningCharacter = Cast<ARogue10mCharacter>(GetOwner()))
		{
			OwningCharacter->SetEquippedWeaponType(ERogue10mWeaponType::Unarmed);
		}
	}

	OnEquipmentChanged.Broadcast();
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

	for (int32& Assigned : ConsumableQuickSlotItemIndices)
	{
		if (Assigned == SourceItemSlotIndex) Assigned = TargetItemSlotIndex;
		else if (Assigned == TargetItemSlotIndex) Assigned = SourceItemSlotIndex;
	}
	return true;
}

bool URogue10mInventoryComponent::RemoveItemFromSlot(int32 ItemSlotIndex, FRogue10mItemStack& OutRemovedItem)
{
	if (!ItemSlots.IsValidIndex(ItemSlotIndex))
	{
		return false;
	}

	FRogue10mItemStack& SourceItem = ItemSlots[ItemSlotIndex];
	if (!SourceItem.bOccupied || SourceItem.bLocked)
	{
		return false;
	}

	OutRemovedItem = SourceItem;
	SourceItem = MakeEmptyItem();
	ClearQuickSlotAssignmentsForItemSlot(ItemSlotIndex);
	return true;
}

bool URogue10mInventoryComponent::RemoveEquippedItemFromSlot(ERogue10mInventorySlotType TargetSlotType, FRogue10mItemStack& OutRemovedItem)
{
	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(TargetSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked || !EquipmentSlot->bHasEquippedItem || !EquipmentSlot->EquippedItem.bOccupied)
	{
		return false;
	}

	OutRemovedItem = EquipmentSlot->EquippedItem;
	EquipmentSlot->EquippedItem = MakeEmptyItem();
	EquipmentSlot->bHasEquippedItem = false;
	EquipmentSlot->bEquipped = false;
	ResetEquipmentSlotDisplay(*EquipmentSlot, TargetSlotType);

	if (TargetSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		if (ARogue10mCharacter* OwningCharacter = Cast<ARogue10mCharacter>(GetOwner()))
		{
			OwningCharacter->SetEquippedWeaponType(ERogue10mWeaponType::Unarmed);
		}
	}

	OnEquipmentChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::AssignConsumableToQuickSlot(int32 ItemSlotIndex, int32 QuickSlotIndex)
{
	if (!ItemSlots.IsValidIndex(ItemSlotIndex) || !ConsumableQuickSlotItemIndices.IsValidIndex(QuickSlotIndex)) return false;
	const FRogue10mItemStack& Item=ItemSlots[ItemSlotIndex];
	if (!Item.bOccupied || Item.bLocked || Item.Category!=ERogue10mItemCategory::Consumable || Item.Quantity<=0) return false;
	for (int32& Assigned:ConsumableQuickSlotItemIndices) if (Assigned==ItemSlotIndex) Assigned=INDEX_NONE;
	ConsumableQuickSlotItemIndices[QuickSlotIndex]=ItemSlotIndex; ConsumableQuickSlotCooldownEndTimes[QuickSlotIndex]=0.0f; return true;
}
bool URogue10mInventoryComponent::UnassignConsumableQuickSlot(int32 Index){if(!ConsumableQuickSlotItemIndices.IsValidIndex(Index))return false;const bool b=ConsumableQuickSlotItemIndices[Index]!=INDEX_NONE;ConsumableQuickSlotItemIndices[Index]=INDEX_NONE;ConsumableQuickSlotCooldownEndTimes[Index]=0;return b;}
bool URogue10mInventoryComponent::MoveConsumableQuickSlot(int32 A,int32 B){if(!ConsumableQuickSlotItemIndices.IsValidIndex(A)||!ConsumableQuickSlotItemIndices.IsValidIndex(B)||A==B)return false;Swap(ConsumableQuickSlotItemIndices[A],ConsumableQuickSlotItemIndices[B]);Swap(ConsumableQuickSlotCooldownEndTimes[A],ConsumableQuickSlotCooldownEndTimes[B]);return true;}
int32 URogue10mInventoryComponent::GetConsumableQuickSlotInventoryIndex(int32 Index) const{return ConsumableQuickSlotItemIndices.IsValidIndex(Index)?ConsumableQuickSlotItemIndices[Index]:INDEX_NONE;}
FRogue10mItemStack URogue10mInventoryComponent::GetConsumableQuickSlotItem(int32 Index) const{const int32 I=GetConsumableQuickSlotInventoryIndex(Index);return ItemSlots.IsValidIndex(I)?ItemSlots[I]:MakeEmptyItem();}
float URogue10mInventoryComponent::GetConsumableQuickSlotCooldownRemaining(int32 Index) const{return ConsumableQuickSlotCooldownEndTimes.IsValidIndex(Index)&&GetWorld()?FMath::Max(0.f,ConsumableQuickSlotCooldownEndTimes[Index]-GetWorld()->GetTimeSeconds()):0.f;}
bool URogue10mInventoryComponent::UseConsumableQuickSlot(int32 Index){const int32 I=GetConsumableQuickSlotInventoryIndex(Index);if(!ItemSlots.IsValidIndex(I)||GetConsumableQuickSlotCooldownRemaining(Index)>0)return false;FRogue10mItemStack& Item=ItemSlots[I];if(!Item.bOccupied||Item.Category!=ERogue10mItemCategory::Consumable||Item.Quantity<=0)return false;if(ARogue10mCharacter* Ch=Cast<ARogue10mCharacter>(GetOwner()))if(URogue10mAttributeSet* A=Ch->GetRogueAttributeSet())A->SetHealth(FMath::Min(A->GetMaxHealth(),A->GetHealth()+Item.RestoreHealth));ConsumableQuickSlotCooldownEndTimes[Index]=GetWorld()?GetWorld()->GetTimeSeconds()+Item.UseCooldown:0;--Item.Quantity;if(Item.Quantity<=0){Item=MakeEmptyItem();ClearQuickSlotAssignmentsForItemSlot(I);}return true;}
void URogue10mInventoryComponent::ClearQuickSlotAssignmentsForItemSlot(int32 ItemIndex){for(int32 I=0;I<ConsumableQuickSlotItemIndices.Num();++I)if(ConsumableQuickSlotItemIndices[I]==ItemIndex)UnassignConsumableQuickSlot(I);}
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

const URogue10mItemDataAsset* URogue10mInventoryComponent::GetEquippedItemData(
	ERogue10mInventorySlotType SlotType) const
{
	const auto FindInSlots = [SlotType](const TArray<FRogue10mInventorySlot>& Slots)
		-> const URogue10mItemDataAsset*
	{
		for (const FRogue10mInventorySlot& Slot : Slots)
		{
			if (Slot.SlotType == SlotType)
			{
				return !Slot.bLocked && Slot.bHasEquippedItem && Slot.EquippedItem.bOccupied
					? Slot.EquippedItem.ItemData.Get()
					: nullptr;
			}
		}
		return nullptr;
	};

	if (const URogue10mItemDataAsset* ItemData = FindInSlots(LeftEquipmentSlots))
	{
		return ItemData;
	}
	return FindInSlots(RightEquipmentSlots);
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

void URogue10mInventoryComponent::ResetEquipmentSlotDisplay(FRogue10mInventorySlot& EquipmentSlot, ERogue10mInventorySlotType TargetSlotType)
{
	if (TargetSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		EquipmentSlot.DisplayName = FText::FromString(TEXT("무기"));
		EquipmentSlot.SlotColor = FLinearColor(0.86f, 0.64f, 0.30f, 1.0f);
		EquipmentSlot.bEquipped = true;
		return;
	}

	switch (TargetSlotType)
	{
	case ERogue10mInventorySlotType::SecondaryWeapon:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("보조무기"));
		break;
	case ERogue10mInventorySlotType::Head:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("투구"));
		break;
	case ERogue10mInventorySlotType::Armor:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("갑옷"));
		break;
	case ERogue10mInventorySlotType::Hands:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("장갑"));
		break;
	case ERogue10mInventorySlotType::Shoes:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("신발"));
		break;
	case ERogue10mInventorySlotType::Ring:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("반지"));
		break;
	case ERogue10mInventorySlotType::Necklace:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("목걸이"));
		break;
	case ERogue10mInventorySlotType::Earring:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("귀걸이"));
		break;
	default:
		EquipmentSlot.DisplayName = FText::FromString(TEXT("빈 슬롯"));
		break;
	}
	EquipmentSlot.SlotColor = FLinearColor(0.28f, 0.28f, 0.3f, 1.0f);
}

bool URogue10mInventoryComponent::CanPlaceGridItem(int32 ContainerIndex, const URogue10mItemDataAsset* ItemData,
	FIntPoint Position, FGuid IgnoredInstanceId) const
{
	if (!InventoryContainers.IsValidIndex(ContainerIndex) || !ItemData)
	{
		return false;
	}

	const FRogue10mInventoryContainer& Container = InventoryContainers[ContainerIndex];
	const FIntPoint ItemSize = ItemData->GetClampedInventorySize();
	if (Position.X < 0 || Position.Y < 0
		|| Position.X + ItemSize.X > Container.GridSize.X
		|| Position.Y + ItemSize.Y > Container.GridSize.Y)
	{
		return false;
	}

	for (const FRogue10mInventoryGridEntry& Entry : Container.Entries)
	{
		if (Entry.InstanceId == IgnoredInstanceId || !Entry.ItemData)
		{
			continue;
		}

		const FIntPoint OtherSize = Entry.ItemData->GetClampedInventorySize();
		const bool bOverlaps = Position.X < Entry.Position.X + OtherSize.X
			&& Position.X + ItemSize.X > Entry.Position.X
			&& Position.Y < Entry.Position.Y + OtherSize.Y
			&& Position.Y + ItemSize.Y > Entry.Position.Y;
		if (bOverlaps)
		{
			return false;
		}
	}
	return true;
}

bool URogue10mInventoryComponent::FindFirstGridPosition(int32 ContainerIndex,
	const URogue10mItemDataAsset* ItemData, FIntPoint& OutPosition, FGuid IgnoredInstanceId) const
{
	if (!InventoryContainers.IsValidIndex(ContainerIndex) || !ItemData)
	{
		return false;
	}

	const FIntPoint GridSize = InventoryContainers[ContainerIndex].GridSize;
	for (int32 Y = 0; Y < GridSize.Y; ++Y)
	{
		for (int32 X = 0; X < GridSize.X; ++X)
		{
			const FIntPoint Candidate(X, Y);
			if (CanPlaceGridItem(ContainerIndex, ItemData, Candidate, IgnoredInstanceId))
			{
				OutPosition = Candidate;
				return true;
			}
		}
	}
	return false;
}

bool URogue10mInventoryComponent::TryAddGridItem(const URogue10mItemDataAsset* ItemData, int32 Quantity,
	int32& OutContainerIndex, FGuid& OutInstanceId)
{
	OutContainerIndex = INDEX_NONE;
	OutInstanceId.Invalidate();
	if (!ItemData || Quantity <= 0 || Quantity > FMath::Max(1, ItemData->MaxStackSize))
	{
		return false;
	}

	for (int32 ContainerIndex = 0; ContainerIndex < InventoryContainers.Num(); ++ContainerIndex)
	{
		FIntPoint Position;
		if (!FindFirstGridPosition(ContainerIndex, ItemData, Position))
		{
			continue;
		}

		FRogue10mInventoryGridEntry Entry;
		Entry.InstanceId = FGuid::NewGuid();
		Entry.ItemData = ItemData;
		Entry.Quantity = Quantity;
		Entry.Position = Position;
		InventoryContainers[ContainerIndex].Entries.Add(Entry);
		OutContainerIndex = ContainerIndex;
		OutInstanceId = Entry.InstanceId;
		OnInventoryGridChanged.Broadcast();
		if (const ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner()))
		{
			if (ARogue10mPlayerController* Controller = Cast<ARogue10mPlayerController>(Character->GetController()))
			{
				Controller->AddItemAcquisitionItem(ItemData, Quantity);
			}
		}
		return true;
	}
	return false;
}

FRogue10mInventoryGridEntry* URogue10mInventoryComponent::FindGridEntry(int32 ContainerIndex, FGuid InstanceId)
{
	if (!InventoryContainers.IsValidIndex(ContainerIndex) || !InstanceId.IsValid())
	{
		return nullptr;
	}
	return InventoryContainers[ContainerIndex].Entries.FindByPredicate(
		[InstanceId](const FRogue10mInventoryGridEntry& Entry) { return Entry.InstanceId == InstanceId; });
}

const FRogue10mInventoryGridEntry* URogue10mInventoryComponent::FindGridEntry(int32 ContainerIndex, FGuid InstanceId) const
{
	if (!InventoryContainers.IsValidIndex(ContainerIndex) || !InstanceId.IsValid())
	{
		return nullptr;
	}
	return InventoryContainers[ContainerIndex].Entries.FindByPredicate(
		[InstanceId](const FRogue10mInventoryGridEntry& Entry) { return Entry.InstanceId == InstanceId; });
}

bool URogue10mInventoryComponent::TryMoveGridItem(int32 SourceContainerIndex, FGuid InstanceId,
	int32 TargetContainerIndex, FIntPoint TargetPosition)
{
	const FRogue10mInventoryGridEntry* SourceEntry = FindGridEntry(SourceContainerIndex, InstanceId);
	if (!SourceEntry || !SourceEntry->ItemData || !InventoryContainers.IsValidIndex(TargetContainerIndex))
	{
		return false;
	}

	const FGuid IgnoredId = SourceContainerIndex == TargetContainerIndex ? InstanceId : FGuid();
	if (!CanPlaceGridItem(TargetContainerIndex, SourceEntry->ItemData, TargetPosition, IgnoredId))
	{
		return false;
	}

	if (SourceContainerIndex == TargetContainerIndex)
	{
		FRogue10mInventoryGridEntry* MutableEntry = InventoryContainers[SourceContainerIndex].Entries.FindByPredicate(
			[InstanceId](const FRogue10mInventoryGridEntry& Entry) { return Entry.InstanceId == InstanceId; });
		MutableEntry->Position = TargetPosition;
		OnInventoryGridChanged.Broadcast();
		return true;
	}

	FRogue10mInventoryGridEntry MovedEntry = *SourceEntry;
	MovedEntry.Position = TargetPosition;
	InventoryContainers[SourceContainerIndex].Entries.RemoveAll(
		[InstanceId](const FRogue10mInventoryGridEntry& Entry) { return Entry.InstanceId == InstanceId; });
	InventoryContainers[TargetContainerIndex].Entries.Add(MoveTemp(MovedEntry));
	OnInventoryGridChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::TryUseGridConsumable(int32 ContainerIndex, FGuid InstanceId)
{
	FRogue10mInventoryGridEntry* Entry = FindGridEntry(ContainerIndex, InstanceId);
	if (!Entry || !Entry->ItemData || Entry->ItemData->Category != ERogue10mItemCategory::Consumable
		|| Entry->Quantity <= 0)
	{
		return false;
	}

	ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner());
	URogue10mAttributeSet* Attributes = Character ? Character->GetRogueAttributeSet() : nullptr;
	const float RestoreHealth = FMath::Max(0.0f, Entry->ItemData->RestoreHealth);
	if (!Attributes || RestoreHealth <= 0.0f || Attributes->GetHealth() >= Attributes->GetMaxHealth())
	{
		return false;
	}

	const FText ItemName = Entry->ItemData->DisplayName;
	const float PreviousHealth = Attributes->GetHealth();
	Attributes->SetHealth(FMath::Min(Attributes->GetMaxHealth(), PreviousHealth + RestoreHealth));
	const float AppliedHealth = Attributes->GetHealth() - PreviousHealth;
	if (AppliedHealth <= 0.0f)
	{
		return false;
	}

	--Entry->Quantity;
	if (Entry->Quantity <= 0)
	{
		InventoryContainers[ContainerIndex].Entries.RemoveAll(
			[InstanceId](const FRogue10mInventoryGridEntry& Candidate)
			{
				return Candidate.InstanceId == InstanceId;
			});
	}
	OnInventoryGridChanged.Broadcast();

	if (ARogue10mPlayerController* Controller = Character
		? Cast<ARogue10mPlayerController>(Character->GetController()) : nullptr)
	{
		Controller->AddCombatLogMessage(FString::Printf(TEXT("%s 사용: 체력 +%.0f"),
			*ItemName.ToString(), AppliedHealth), FLinearColor(0.25f, 0.9f, 0.35f, 1.0f));
	}
	return true;
}

bool URogue10mInventoryComponent::TryEquipGridItem(int32 ContainerIndex, FGuid InstanceId)
{
	FRogue10mInventoryGridEntry* SourceEntry = FindGridEntry(ContainerIndex, InstanceId);
	if (!SourceEntry || !SourceEntry->ItemData
		|| SourceEntry->ItemData->Category != ERogue10mItemCategory::Equipment
		|| SourceEntry->Quantity != 1)
	{
		return false;
	}

	const FRogue10mInventoryGridEntry SourceEntryCopy = *SourceEntry;
	const URogue10mItemDataAsset* NewItemData = SourceEntryCopy.ItemData.Get();
	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(NewItemData->EquipSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked)
	{
		return false;
	}

	const FRogue10mItemStack PreviousEquippedItem = EquipmentSlot->EquippedItem;
	const bool bHadEquippedItem = EquipmentSlot->bHasEquippedItem
		&& PreviousEquippedItem.bOccupied;
	int32 ReplacementContainerIndex = INDEX_NONE;
	FIntPoint ReplacementPosition = FIntPoint::ZeroValue;
	if (bHadEquippedItem)
	{
		const URogue10mItemDataAsset* PreviousItemData = PreviousEquippedItem.ItemData.Get();
		if (!PreviousItemData)
		{
			return false;
		}

		if (CanPlaceGridItem(ContainerIndex, PreviousItemData, SourceEntryCopy.Position, InstanceId))
		{
			ReplacementContainerIndex = ContainerIndex;
			ReplacementPosition = SourceEntryCopy.Position;
		}
		else
		{
			for (int32 CandidateContainerIndex = 0;
				CandidateContainerIndex < InventoryContainers.Num(); ++CandidateContainerIndex)
			{
				const FGuid IgnoredId = CandidateContainerIndex == ContainerIndex ? InstanceId : FGuid();
				if (FindFirstGridPosition(CandidateContainerIndex, PreviousItemData,
					ReplacementPosition, IgnoredId))
				{
					ReplacementContainerIndex = CandidateContainerIndex;
					break;
				}
			}
		}

		if (ReplacementContainerIndex == INDEX_NONE)
		{
			return false;
		}
	}

	const FRogue10mItemStack NewEquippedItem = MakeItemFromDataAsset(NewItemData, 1);
	if (!NewEquippedItem.bOccupied)
	{
		return false;
	}

	if (bHadEquippedItem)
	{
		FRogue10mInventoryGridEntry ReplacementEntry;
		ReplacementEntry.InstanceId = InstanceId;
		ReplacementEntry.ItemData = PreviousEquippedItem.ItemData;
		ReplacementEntry.Quantity = FMath::Clamp(PreviousEquippedItem.Quantity, 1,
			FMath::Max(1, PreviousEquippedItem.ItemData->MaxStackSize));
		ReplacementEntry.Position = ReplacementPosition;
		if (ReplacementContainerIndex == ContainerIndex)
		{
			*SourceEntry = MoveTemp(ReplacementEntry);
		}
		else
		{
			InventoryContainers[ContainerIndex].Entries.RemoveAll(
				[InstanceId](const FRogue10mInventoryGridEntry& Candidate)
				{
					return Candidate.InstanceId == InstanceId;
				});
			InventoryContainers[ReplacementContainerIndex].Entries.Add(MoveTemp(ReplacementEntry));
		}
	}
	else
	{
		InventoryContainers[ContainerIndex].Entries.RemoveAll(
			[InstanceId](const FRogue10mInventoryGridEntry& Candidate)
			{
				return Candidate.InstanceId == InstanceId;
			});
	}

	EquipmentSlot->DisplayName = NewEquippedItem.DisplayName;
	EquipmentSlot->SlotColor = NewEquippedItem.ItemColor;
	EquipmentSlot->bEquipped = true;
	EquipmentSlot->bHasEquippedItem = true;
	EquipmentSlot->EquippedItem = NewEquippedItem;
	if (NewItemData->EquipSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		if (ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner()))
		{
			Character->SetEquippedWeaponType(NewEquippedItem.WeaponType);
		}
	}

	OnInventoryGridChanged.Broadcast();
	OnEquipmentChanged.Broadcast();

	if (ARogue10mCharacter* Character = Cast<ARogue10mCharacter>(GetOwner()))
	{
		if (ARogue10mPlayerController* Controller =
			Cast<ARogue10mPlayerController>(Character->GetController()))
		{
			const FString Message = bHadEquippedItem
				? FString::Printf(TEXT("%s 장착 (%s 교체)"), *NewEquippedItem.DisplayName.ToString(),
					*PreviousEquippedItem.DisplayName.ToString())
				: FString::Printf(TEXT("%s 장착"), *NewEquippedItem.DisplayName.ToString());
			Controller->AddCombatLogMessage(Message, FLinearColor(0.45f, 0.75f, 1.0f, 1.0f));
		}
	}
	return true;
}

bool URogue10mInventoryComponent::TryUnequipItemToGrid(
	ERogue10mInventorySlotType SourceSlotType,
	int32 TargetContainerIndex,
	FIntPoint TargetPosition)
{
	FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(SourceSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked || !EquipmentSlot->bHasEquippedItem
		|| !EquipmentSlot->EquippedItem.bOccupied || !EquipmentSlot->EquippedItem.ItemData)
	{
		return false;
	}

	const URogue10mItemDataAsset* ItemData = EquipmentSlot->EquippedItem.ItemData.Get();
	if (!InventoryContainers.IsValidIndex(TargetContainerIndex)
		|| !CanPlaceGridItem(TargetContainerIndex, ItemData, TargetPosition, FGuid()))
	{
		return false;
	}

	FRogue10mInventoryGridEntry Entry;
	Entry.InstanceId = FGuid::NewGuid();
	Entry.ItemData = ItemData;
	Entry.Quantity = FMath::Clamp(EquipmentSlot->EquippedItem.Quantity, 1, FMath::Max(1, ItemData->MaxStackSize));
	Entry.Position = TargetPosition;
	InventoryContainers[TargetContainerIndex].Entries.Add(MoveTemp(Entry));

	EquipmentSlot->EquippedItem = MakeEmptyItem();
	EquipmentSlot->bHasEquippedItem = false;
	EquipmentSlot->bEquipped = false;
	ResetEquipmentSlotDisplay(*EquipmentSlot, SourceSlotType);

	if (SourceSlotType == ERogue10mInventorySlotType::MainWeapon)
	{
		if (ARogue10mCharacter* OwningCharacter = Cast<ARogue10mCharacter>(GetOwner()))
		{
			OwningCharacter->SetEquippedWeaponType(ERogue10mWeaponType::Unarmed);
		}
	}

	OnInventoryGridChanged.Broadcast();
	OnEquipmentChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::TryUnequipItemToFirstAvailableGrid(
	ERogue10mInventorySlotType SourceSlotType)
{
	const FRogue10mInventorySlot* EquipmentSlot = FindEquipmentSlot(SourceSlotType);
	if (!EquipmentSlot || EquipmentSlot->bLocked || !EquipmentSlot->bHasEquippedItem
		|| !EquipmentSlot->EquippedItem.bOccupied || !EquipmentSlot->EquippedItem.ItemData)
	{
		return false;
	}

	const URogue10mItemDataAsset* ItemData = EquipmentSlot->EquippedItem.ItemData.Get();
	for (int32 ContainerIndex = 0; ContainerIndex < InventoryContainers.Num(); ++ContainerIndex)
	{
		FIntPoint Position = FIntPoint::ZeroValue;
		if (!FindFirstGridPosition(ContainerIndex, ItemData, Position))
		{
			continue;
		}

		if (TryUnequipItemToGrid(SourceSlotType, ContainerIndex, Position))
		{
			return true;
		}
	}

	return false;
}

bool URogue10mInventoryComponent::RemoveGridItem(int32 ContainerIndex, FGuid InstanceId,
	FRogue10mInventoryGridEntry& OutRemovedEntry)
{
	const FRogue10mInventoryGridEntry* Entry = FindGridEntry(ContainerIndex, InstanceId);
	if (!Entry)
	{
		return false;
	}
	OutRemovedEntry = *Entry;
	InventoryContainers[ContainerIndex].Entries.RemoveAll(
		[InstanceId](const FRogue10mInventoryGridEntry& Candidate) { return Candidate.InstanceId == InstanceId; });
	OnInventoryGridChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::AddInventoryFromBag(const URogue10mItemDataAsset* BagItemData,
	FName RequestedContainerId)
{
	if (!BagItemData || !BagItemData->bProvidesInventory)
	{
		return false;
	}

	const FName ContainerId = RequestedContainerId.IsNone() ? BagItemData->ItemId : RequestedContainerId;
	if (ContainerId.IsNone() || InventoryContainers.ContainsByPredicate(
		[ContainerId](const FRogue10mInventoryContainer& Container) { return Container.ContainerId == ContainerId; }))
	{
		return false;
	}

	FRogue10mInventoryContainer Container;
	Container.ContainerId = ContainerId;
	Container.DisplayName = BagItemData->DisplayName;
	Container.GridSize = BagItemData->GetClampedBagSize();
	Container.SourceBagItem = BagItemData;
	InventoryContainers.Add(MoveTemp(Container));
	OnInventoryGridChanged.Broadcast();
	return true;
}

bool URogue10mInventoryComponent::DropGridItem(int32 ContainerIndex, FGuid InstanceId, FVector WorldLocation)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !GetWorld())
	{
		return false;
	}

	const FRogue10mInventoryGridEntry* ExistingEntry = FindGridEntry(ContainerIndex, InstanceId);
	if (!ExistingEntry || !ExistingEntry->ItemData || !ExistingEntry->ItemData->bCanDrop)
	{
		return false;
	}

	FRogue10mInventoryGridEntry RemovedEntry;
	if (!RemoveGridItem(ContainerIndex, InstanceId, RemovedEntry))
	{
		return false;
	}

	TSubclassOf<ARogue10mDroppedItem> SpawnClass = DroppedItemClass;
	if (!SpawnClass)
	{
		SpawnClass = ARogue10mDroppedItem::StaticClass();
	}
	ARogue10mDroppedItem* DroppedItem = GetWorld()->SpawnActor<ARogue10mDroppedItem>(
		SpawnClass, WorldLocation, FRotator::ZeroRotator);
	if (!DroppedItem)
	{
		InventoryContainers[ContainerIndex].Entries.Add(RemovedEntry);
		OnInventoryGridChanged.Broadcast();
		return false;
	}

	FRogue10mItemStack Stack;
	Stack.ItemData = RemovedEntry.ItemData;
	Stack.Category = RemovedEntry.ItemData->Category;
	Stack.EquipSlotType = RemovedEntry.ItemData->EquipSlotType;
	Stack.WeaponType = RemovedEntry.ItemData->WeaponType;
	Stack.DisplayName = RemovedEntry.ItemData->DisplayName;
	Stack.Description = RemovedEntry.ItemData->Description;
	Stack.Quantity = RemovedEntry.Quantity;
	Stack.bOccupied = true;
	DroppedItem->InitializeDroppedItem(Stack);
	return true;
}
