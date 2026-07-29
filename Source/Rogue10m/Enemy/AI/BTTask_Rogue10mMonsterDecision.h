// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Rogue10mMonsterDecision.generated.h"

UCLASS()
class ROGUE10M_API UBTTask_Rogue10mMonsterDecision : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Rogue10mMonsterDecision();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};