// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mGameplayAbility_Attack.h"
#include "Rogue10mCharacter.h"

URogue10mGameplayAbility_Attack::URogue10mGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URogue10mGameplayAbility_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ARogue10mCharacter* RogueCharacter = ActorInfo ? Cast<ARogue10mCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	const bool bExecuted = RogueCharacter && RogueCharacter->ExecutePendingAttackSkillFromAbility();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, !bExecuted);
}
