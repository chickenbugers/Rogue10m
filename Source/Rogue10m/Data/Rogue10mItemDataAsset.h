// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mItemDataAsset.generated.h"

class UStaticMesh;
class UTexture2D;

UENUM(BlueprintType)
enum class ERogue10mItemRarity : uint8
{
	Common UMETA(DisplayName="일반"),
	Uncommon UMETA(DisplayName="고급"),
	Rare UMETA(DisplayName="희귀"),
	Epic UMETA(DisplayName="영웅"),
	Legendary UMETA(DisplayName="전설")
};

/** 아이템 인스턴스와 분리된 정적 정의 데이터입니다. */
UCLASS(BlueprintType)
class ROGUE10M_API URogue10mItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items")
	FIntPoint GetClampedInventorySize() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Items|Bag")
	FIntPoint GetClampedBagSize() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Identity")
	FName ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display")
	TSoftObjectPtr<UTexture2D> InventoryIcon;

	/** Uniform display scale applied after the inventory icon is fitted to the item footprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display",
		meta=(ClampMin="0.1", ClampMax="2.0", UIMin="0.1", UIMax="2.0"))
	float InventoryIconScale = 1.0f;

	/** Inventory fallback color used when an item does not have an icon yet. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display")
	FLinearColor InventoryTint = FLinearColor(0.16f, 0.42f, 0.68f, 0.85f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Display")
	ERogue10mItemRarity Rarity = ERogue10mItemRarity::Common;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Classification")
	ERogue10mItemCategory Category = ERogue10mItemCategory::Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Classification")
	ERogue10mInventorySlotType EquipSlotType = ERogue10mInventorySlotType::Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Classification")
	ERogue10mWeaponType WeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Inventory", meta=(ClampMin="1", ClampMax="10"))
	FIntPoint InventorySize = FIntPoint(1, 1);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Inventory", meta=(ClampMin="1"))
	int32 MaxStackSize = 1;

	/** 아이템 한 개의 무게(kg)입니다. 총 무게는 수량을 곱해 계산합니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Inventory", meta=(ClampMin="0.0", Units="kg"))
	float UnitWeight = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|World")
	TSoftObjectPtr<UStaticMesh> DroppedWorldMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|World")
	bool bCanDrop = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Bag")
	bool bProvidesInventory = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Items|Bag", meta=(EditCondition="bProvidesInventory", ClampMin="1", ClampMax="20"))
	FIntPoint ProvidedInventorySize = FIntPoint(5, 5);
};
