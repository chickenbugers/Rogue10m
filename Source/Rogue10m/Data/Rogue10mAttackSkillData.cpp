// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mAttackSkillData.h"

URogue10mAttackSkillData::URogue10mAttackSkillData()
{
	ResourceCosts =
	{
		{ ERogue10mAttackResourceType::Health, 0.0f },
		{ ERogue10mAttackResourceType::Stamina, 0.0f }
	};
}
