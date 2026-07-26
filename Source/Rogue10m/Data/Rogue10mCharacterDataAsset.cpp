// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacterDataAsset.h"

FPrimaryAssetId URogue10mCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mCharacter"), CharacterId.IsNone() ? GetFName() : CharacterId);
}
FRogue10mCharacterBaseStats URogue10mCharacterDataAsset::GetBaseStats() const
{
	FRogue10mCharacterBaseStats Stats;
	Stats.MaxHealth = FMath::Max(1.0f, MaxHealth);
	Stats.MaxStamina = FMath::Max(1.0f, MaxStamina);
	Stats.MaxMana = FMath::Max(1.0f, MaxMana);
	Stats.AttackPower = FMath::Max(0.0f, AttackPower);
	Stats.Defense = FMath::Max(0.0f, Defense);
	Stats.CriticalChance = FMath::Clamp(CriticalChance, 0.0f, 1.0f);
	Stats.CriticalDamageMultiplier = FMath::Max(1.0f, CriticalDamageMultiplier);
	Stats.AttackSpeedMultiplier = FMath::Clamp(AttackSpeedMultiplier, 0.1f, 5.0f);
	Stats.MoveSpeed = FMath::Max(0.0f, WalkSpeed);
	Stats.SprintSpeed = FMath::Max(0.0f, SprintSpeed);
	return Stats;
}
