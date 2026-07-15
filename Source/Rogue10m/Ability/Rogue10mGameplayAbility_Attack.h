// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Rogue10mGameplayAbility_Attack.generated.h"

/**
 * 기존 공격 Data Asset을 GAS 실행 흐름으로 호출하는 전환용 기본 공격 Ability입니다.
 * 비용과 쿨타임은 다음 단계에서 GameplayEffect로 옮기고, 지금은 기존 검증/트레이스 로직을 재사용합니다.
 */
UCLASS()
class ROGUE10M_API URogue10mGameplayAbility_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URogue10mGameplayAbility_Attack();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
