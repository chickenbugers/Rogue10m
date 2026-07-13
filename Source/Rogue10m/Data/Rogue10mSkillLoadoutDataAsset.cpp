// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mSkillLoadoutDataAsset.h"

FPrimaryAssetId URogue10mDodgeSkillDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mDodgeSkill"), GetFName());
}

FPrimaryAssetId URogue10mWeaponSkillProfileDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Rogue10mWeaponSkillProfile"), GetFName());
}

URogue10mAttackSkillData* URogue10mWeaponSkillProfileDataAsset::FindDefaultSkill(
	ERogue10mAttackInputSlot InputSlot) const
{
	const TObjectPtr<URogue10mAttackSkillData>* Skill = DefaultSkillBindings.Find(InputSlot);
	return Skill ? Skill->Get() : nullptr;
}