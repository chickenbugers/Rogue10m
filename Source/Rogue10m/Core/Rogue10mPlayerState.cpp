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

float ARogue10mPlayerState::GetExperienceNormalized() const
{
	return ExperienceToNextLevel > 0 ? FMath::Clamp(static_cast<float>(CurrentExperience) / static_cast<float>(ExperienceToNextLevel), 0.0f, 1.0f) : 0.0f;
}

void ARogue10mPlayerState::AddExperience(int32 ExperienceAmount)
{
	if (ExperienceAmount <= 0)
	{
		return;
	}

	CurrentExperience += ExperienceAmount;
	while (CurrentExperience >= ExperienceToNextLevel)
	{
		CurrentExperience -= ExperienceToNextLevel;
		++PlayerLevel;
		ExperienceToNextLevel = FMath::Max(100, FMath::RoundToInt(ExperienceToNextLevel * 1.18f));
	}
}

int32 ARogue10mPlayerState::GetWeaponMasteryLevel(ERogue10mWeaponType WeaponType) const
{
	// 현재는 기본 권 숙련만 개방합니다. 이후 무기별 숙련도 컴포넌트나 Data Asset으로 교체합니다.
	return WeaponType == ERogue10mWeaponType::Knuckle || WeaponType == ERogue10mWeaponType::Unarmed ? 1 : 0;
}

float ARogue10mPlayerState::GetIdentityNormalized() const
{
	return IdentityMax > 0.0f ? FMath::Clamp(IdentityCurrent / IdentityMax, 0.0f, 1.0f) : 0.0f;
}
