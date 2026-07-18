// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mItemDataAsset.h"

FPrimaryAssetId URogue10mItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mItem"), ItemId.IsNone() ? GetFName() : ItemId);
}

FIntPoint URogue10mItemDataAsset::GetClampedInventorySize() const
{
	return FIntPoint(FMath::Clamp(InventorySize.X, 1, 10), FMath::Clamp(InventorySize.Y, 1, 10));
}

FIntPoint URogue10mItemDataAsset::GetClampedBagSize() const
{
	return FIntPoint(FMath::Clamp(ProvidedInventorySize.X, 1, 20), FMath::Clamp(ProvidedInventorySize.Y, 1, 20));
}

FLinearColor URogue10mItemDataAsset::GetInventoryRarityBackgroundColor() const
{
	switch (Rarity)
	{
	case ERogue10mItemRarity::Rare:
		return FLinearColor(0.08f, 0.28f, 1.0f, 0.34f);
	case ERogue10mItemRarity::Epic:
		return FLinearColor(0.52f, 0.12f, 0.9f, 0.36f);
	case ERogue10mItemRarity::Unique:
		return FLinearColor(1.0f, 0.32f, 0.025f, 0.38f);
	case ERogue10mItemRarity::Mythic:
		return FLinearColor(0.92f, 0.025f, 0.02f, 0.40f);
	case ERogue10mItemRarity::Common:
	default:
		return FLinearColor(0.92f, 0.92f, 0.92f, 0.22f);
	}
}

FLinearColor URogue10mItemDataAsset::GetRarityTextColor() const
{
	switch (Rarity)
	{
	case ERogue10mItemRarity::Rare:
		return FLinearColor(0.22f, 0.52f, 1.0f, 1.0f);
	case ERogue10mItemRarity::Epic:
		return FLinearColor(0.72f, 0.32f, 1.0f, 1.0f);
	case ERogue10mItemRarity::Unique:
		return FLinearColor(1.0f, 0.46f, 0.08f, 1.0f);
	case ERogue10mItemRarity::Mythic:
		return FLinearColor(1.0f, 0.12f, 0.08f, 1.0f);
	case ERogue10mItemRarity::Common:
	default:
		return FLinearColor(0.92f, 0.92f, 0.92f, 1.0f);
	}
}
