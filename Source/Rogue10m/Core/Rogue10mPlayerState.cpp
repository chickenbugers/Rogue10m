// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mPlayerState.h"

ARogue10mPlayerState::ARogue10mPlayerState()
{
}

void ARogue10mPlayerState::SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType)
{
	EquippedWeaponType = NewWeaponType;
}

void ARogue10mPlayerState::SetCharacterDead(bool bNewCharacterDead)
{
	bCharacterDead = bNewCharacterDead;
}
