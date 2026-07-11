// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Rogue10mAttackTargetInterface.generated.h"

UINTERFACE(BlueprintType)
class ROGUE10M_API URogue10mAttackTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/** 몬스터와 파괴 오브젝트가 플레이어 공격 판정에 참여하기 위한 공통 계약입니다. */
class ROGUE10M_API IRogue10mAttackTargetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Rogue10m|Combat|Target")
	bool CanReceiveRogue10mAttack(AActor* AttackSource) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Rogue10m|Combat|Target")
	FVector GetRogue10mDamageIndicatorLocation() const;
};
