// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class ERogue10mInventorySlotType : uint8
{
	Head,
	Chest,
	Legs,
	Hands,
	Weapon,
	Charm,
	Ring,
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
};

USTRUCT(BlueprintType)
struct FRogue10mItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	ERogue10mItemCategory Category = ERogue10mItemCategory::Material;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Items")
	ERogue10mInventorySlotType EquipSlotType = ERogue10mInventorySlotType::Material;

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

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryEquipItemFromSlot(int32 ItemSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryEquipItemToSlot(int32 ItemSlotIndex, ERogue10mInventorySlotType TargetSlotType);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Items")
	bool TryMoveItemSlot(int32 SourceItemSlotIndex, int32 TargetItemSlotIndex);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TArray<FRogue10mInventorySlot> LeftEquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Inventory")
	TArray<FRogue10mInventorySlot> RightEquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items")
	TArray<FRogue10mItemStack> ItemSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="1"))
	int32 ItemGridColumns = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="0"))
	int32 Gold = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items", meta=(ClampMin="0"))
	int32 Crystals = 0;

private:
	static FRogue10mInventorySlot MakeSlot(ERogue10mInventorySlotType SlotType, const TCHAR* DisplayName, const FLinearColor& SlotColor, bool bLocked, bool bEquipped);
	static FRogue10mItemStack MakeItem(ERogue10mItemCategory Category, ERogue10mInventorySlotType EquipSlotType, const TCHAR* DisplayName, const TCHAR* Description, int32 Quantity, const FLinearColor& ItemColor, bool bLocked = false);
	static FRogue10mItemStack MakeEmptyItem();
	FRogue10mInventorySlot* FindEquipmentSlot(ERogue10mInventorySlotType SlotType);
};
