// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mMonsterDataAsset.h"

FPrimaryAssetId URogue10mMonsterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mMonster"), MonsterId.IsNone() ? GetFName() : MonsterId);
}
