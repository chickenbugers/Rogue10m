// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class ERogue10mInventorySlotType : uint8
{
	MainWeapon,
	SecondaryWeapon,
	Head,
	Armor,
	Shoes,
	Chest,
	Legs,
	Hands,
	Weapon,
	Charm,
	Ring,
	Earring,
	Relic,
	Consumable,
	Material
};

UENUM(BlueprintType)
enum class ERogue10mItemCategory : uint8
{
	Equipment,
	Consumable,
	Material,
	Currency,
	Quest
};

USTRUCT(BlueprintType)
struct FRogue10mItemStack
{
	GENERATED_BODY()

	// 아이템의 큰 분류입니다. 장비/소모품/재료/재화/퀘스트 아이템을 구분합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	ERogue10mItemCategory Category = ERogue10mItemCategory::Material;

	// 장비 아이템일 때 들어갈 장비 슬롯입니다. 무기는 Weapon 슬롯을 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	ERogue10mInventorySlotType EquipSlotType = ERogue10mInventorySlotType::Material;

	// 무기 장비일 때 캐릭터의 현재 무기 타입으로 반영할 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	ERogue10mWeaponType WeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="0"))
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	FLinearColor ItemColor = FLinearColor(0.25f, 0.85f, 0.95f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	bool bOccupied = true;
};

USTRUCT(BlueprintType)
struct FRogue10mInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	ERogue10mInventorySlotType SlotType = ERogue10mInventorySlotType::Material;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FLinearColor SlotColor = FLinearColor(0.85f, 0.68f, 0.32f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	bool bEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	bool bHasEquippedItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Inventory")
	FRogue10mItemStack EquippedItem;
};

/**
 * Prototype inventory/equipment data for the first-person character.
 */
UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mInventoryComponent();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	const TArray<FRogue10mInventorySlot>& GetLeftEquipmentSlots() const { return LeftEquipmentSlots; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Inventory")
	const TArray<FRogue10mInventorySlot>& GetRightEquipmentSlots() const { return RightEquipmentSlots; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items")
	const TArray<FRogue10mItemStack>& GetItemSlots() const { return ItemSlots; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items")
	int32 GetItemGridColumns() const { return ItemGridColumns; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items")
	int32 GetGold() const { return Gold; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items")
	int32 GetCrystals() const { return Crystals; }

	// 아이템 슬롯의 장비를 기본 장착 슬롯으로 장착합니다.
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryEquipItemFromSlot(int32 ItemSlotIndex);

	// 아이템 슬롯의 장비를 지정한 장착 슬롯으로 장착합니다.
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryEquipItemToSlot(int32 ItemSlotIndex, ERogue10mInventorySlotType TargetSlotType);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryUnequipItemFromSlot(ERogue10mInventorySlotType TargetSlotType);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryUnequipItemFromSlotToItemSlot(ERogue10mInventorySlotType TargetSlotType, int32 TargetItemSlotIndex);

	// 아이템 창 내부에서 드래그 앤 드랍으로 아이템 위치를 이동하거나 교체합니다.
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryMoveItemSlot(int32 SourceItemSlotIndex, int32 TargetItemSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool RemoveItemFromSlot(int32 ItemSlotIndex, FRogue10mItemStack& OutRemovedItem);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool RemoveEquippedItemFromSlot(ERogue10mInventorySlotType TargetSlotType, FRogue10mItemStack& OutRemovedItem);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TArray<FRogue10mInventorySlot> LeftEquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TArray<FRogue10mInventorySlot> RightEquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items")
	TArray<FRogue10mItemStack> ItemSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="1"))
	int32 ItemGridColumns = 10;

	// 현재는 UI 표시용 0 고정값입니다. 이후 재화 관리 컴포넌트에서 받아오도록 교체합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="0"))
	int32 Gold = 0;

	// 현재는 UI 표시용 0 고정값입니다. 이후 재화 관리 컴포넌트에서 받아오도록 교체합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="0"))
	int32 Crystals = 0;

private:
	// 장비창의 개별 슬롯 표시 데이터를 만듭니다.
	static FRogue10mInventorySlot MakeSlot(ERogue10mInventorySlotType SlotType, const TCHAR* DisplayName, const FLinearColor& SlotColor, bool bLocked, bool bEquipped);

	// 아이템 창에 들어갈 일반 아이템 데이터를 만듭니다.
	static FRogue10mItemStack MakeItem(ERogue10mItemCategory Category, ERogue10mInventorySlotType EquipSlotType, const TCHAR* DisplayName, const TCHAR* Description, int32 Quantity, const FLinearColor& ItemColor, bool bLocked = false);

	// 무기 슬롯에 장착 가능한 무기 아이템 데이터를 만듭니다.
	static FRogue10mItemStack MakeWeaponItem(ERogue10mWeaponType WeaponType, const TCHAR* DisplayName, const TCHAR* Description, const FLinearColor& ItemColor, ERogue10mInventorySlotType EquipSlotType = ERogue10mInventorySlotType::MainWeapon, bool bLocked = false);

	// 비어 있는 인벤토리 칸을 표현하는 아이템 데이터를 만듭니다.
	static FRogue10mItemStack MakeEmptyItem();

	// 좌/우 장비 슬롯 배열에서 목표 슬롯 타입을 찾습니다.
	FRogue10mInventorySlot* FindEquipmentSlot(ERogue10mInventorySlotType SlotType);
	int32 FindFirstEmptyItemSlot() const;
	void ResetEquipmentSlotDisplay(FRogue10mInventorySlot& EquipmentSlot, ERogue10mInventorySlotType TargetSlotType);
};
