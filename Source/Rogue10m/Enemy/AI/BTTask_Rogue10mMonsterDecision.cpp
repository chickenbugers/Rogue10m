// Copyright Epic Games, Inc. All Rights Reserved.

#include "BTTask_Rogue10mMonsterDecision.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Rogue10mMonsterAIController.h"

UBTTask_Rogue10mMonsterDecision::UBTTask_Rogue10mMonsterDecision()
{
	NodeName = TEXT("Rogue10m Monster Decision");
}

EBTNodeResult::Type UBTTask_Rogue10mMonsterDecision::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARogue10mMonsterAIController* MonsterController =
		Cast<ARogue10mMonsterAIController>(OwnerComp.GetAIOwner());
	return MonsterController && MonsterController->ExecuteBehaviorDecision()
		? EBTNodeResult::Succeeded
		: EBTNodeResult::Failed;
}