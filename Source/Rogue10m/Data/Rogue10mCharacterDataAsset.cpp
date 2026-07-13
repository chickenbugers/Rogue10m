// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacterDataAsset.h"

FPrimaryAssetId URogue10mCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mCharacter"), CharacterId.IsNone() ? GetFName() : CharacterId);
}