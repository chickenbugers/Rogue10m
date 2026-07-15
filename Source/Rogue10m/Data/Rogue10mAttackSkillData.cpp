// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mAttackSkillData.h"

#include "Rogue10mAttributeSet.h"

URogue10mAttackSkillData::URogue10mAttackSkillData()
{
	ResourceCosts =
	{
		{ ERogue10mAttackResourceType::Health, 0.0f },
		{ ERogue10mAttackResourceType::Stamina, 0.0f }
	};
}

float URogue10mAttackSkillData::RollDamage(const URogue10mAttributeSet* SourceAttributes, bool& bOutCritical) const
{
	const float CharacterMinRatio = SourceAttributes ? SourceAttributes->GetMinDamageRatio() : 0.9f;
	const float CharacterMaxRatio = SourceAttributes ? SourceAttributes->GetMaxDamageRatio() : 1.1f;
	const float AdjustedA = FMath::Max(0.0f, CharacterMinRatio * MinDamageRatioMultiplier);
	const float AdjustedB = FMath::Max(0.0f, CharacterMaxRatio * MaxDamageRatioMultiplier);
	const float MinimumRatio = FMath::Min(AdjustedA, AdjustedB);
	const float MaximumRatio = FMath::Max(AdjustedA, AdjustedB);
	float RolledDamage = FMath::Max(0.0f, Damage) * FMath::FRandRange(MinimumRatio, MaximumRatio);

	const float CharacterCriticalChance = SourceAttributes ? SourceAttributes->GetCriticalChance() : 0.0f;
	const float FinalCriticalChance = FMath::Clamp(CharacterCriticalChance + CriticalChanceBonus, 0.0f, 1.0f);
	bOutCritical = FMath::FRand() < FinalCriticalChance;
	if (bOutCritical)
	{
		const float CharacterCriticalMultiplier = SourceAttributes ? SourceAttributes->GetCriticalDamageMultiplier() : 1.5f;
		const float FinalCriticalMultiplier = FMath::Max(0.0f, CharacterCriticalMultiplier + CriticalDamageMultiplierBonus);
		RolledDamage *= FinalCriticalMultiplier;
	}
	return RolledDamage;
}