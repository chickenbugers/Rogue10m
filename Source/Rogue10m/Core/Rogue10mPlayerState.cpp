// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Rogue10mAbilitySystemComponent.h"
#include "Rogue10mAttributeSet.h"

ARogue10mPlayerState::ARogue10mPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<URogue10mAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<URogue10mAttributeSet>(TEXT("Attribute Set"));
}

UAbilitySystemComponent* ARogue10mPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARogue10mPlayerState::SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType)
{
	EquippedWeaponType = NewWeaponType;
}

void ARogue10mPlayerState::SetCharacterDead(bool bNewCharacterDead)
{
	bCharacterDead = bNewCharacterDead;
}

void ARogue10mPlayerState::SetIdentityType(ERogue10mIdentityType NewIdentityType)
{
	IdentityType = NewIdentityType;
}

int32 ARogue10mPlayerState::GetPlayerLevel() const
{
	return AttributeSet ? FMath::Max(1, FMath::RoundToInt(AttributeSet->GetPlayerLevel())) : 1;
}

int32 ARogue10mPlayerState::GetCurrentExperience() const
{
	return AttributeSet ? FMath::Max(0, FMath::RoundToInt(AttributeSet->GetExperience())) : 0;
}

int32 ARogue10mPlayerState::GetExperienceToNextLevel() const
{
	return AttributeSet ? FMath::Max(1, FMath::RoundToInt(AttributeSet->GetExperienceToNextLevel())) : 100;
}

float ARogue10mPlayerState::GetExperienceNormalized() const
{
	const int32 RequiredExperience = GetExperienceToNextLevel();
	return RequiredExperience > 0
		? FMath::Clamp(static_cast<float>(GetCurrentExperience()) / static_cast<float>(RequiredExperience), 0.0f, 1.0f)
		: 0.0f;
}

void ARogue10mPlayerState::AddExperience(int32 ExperienceAmount)
{
	if (!AttributeSet || ExperienceAmount <= 0)
	{
		return;
	}

	int32 CurrentExperience = GetCurrentExperience() + ExperienceAmount;
	int32 RequiredExperience = GetExperienceToNextLevel();
	int32 Level = GetPlayerLevel();

	while (CurrentExperience >= RequiredExperience)
	{
		CurrentExperience -= RequiredExperience;
		++Level;
		RequiredExperience = FMath::Max(100, FMath::RoundToInt(RequiredExperience * 1.18f));
	}

	AttributeSet->SetPlayerLevel(static_cast<float>(Level));
	AttributeSet->SetExperience(static_cast<float>(CurrentExperience));
	AttributeSet->SetExperienceToNextLevel(static_cast<float>(RequiredExperience));
}

int32 ARogue10mPlayerState::GetWeaponMasteryLevel(ERogue10mWeaponType WeaponType) const
{
	return WeaponType == ERogue10mWeaponType::Knuckle || WeaponType == ERogue10mWeaponType::Unarmed ? 1 : 0;
}

float ARogue10mPlayerState::GetIdentityCurrent() const
{
	return AttributeSet ? AttributeSet->GetIdentity() : 0.0f;
}

float ARogue10mPlayerState::GetIdentityMax() const
{
	return AttributeSet ? AttributeSet->GetMaxIdentity() : 100.0f;
}

float ARogue10mPlayerState::GetIdentityNormalized() const
{
	const float MaxIdentity = GetIdentityMax();
	return MaxIdentity > 0.0f ? FMath::Clamp(GetIdentityCurrent() / MaxIdentity, 0.0f, 1.0f) : 0.0f;
}