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
