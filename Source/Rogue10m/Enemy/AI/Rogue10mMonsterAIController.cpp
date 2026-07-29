// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mMonsterAIController.h"

#include "BTTask_Rogue10mMonsterDecision.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Rogue10mBasicMonster.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mMonsterDataAsset.h"

const FName ARogue10mMonsterAIController::TargetActorKey(TEXT("TargetActor"));
const FName ARogue10mMonsterAIController::HomeLocationKey(TEXT("HomeLocation"));
const FName ARogue10mMonsterAIController::PatrolLocationKey(TEXT("PatrolLocation"));
const FName ARogue10mMonsterAIController::LastKnownLocationKey(TEXT("LastKnownLocation"));
const FName ARogue10mMonsterAIController::HasTargetKey(TEXT("HasTarget"));
const FName ARogue10mMonsterAIController::IsInAttackRangeKey(TEXT("IsInAttackRange"));
const FName ARogue10mMonsterAIController::WasDamagedKey(TEXT("WasDamaged"));

ARogue10mMonsterAIController::ARogue10mMonsterAIController()
{
	MonsterBlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Monster Blackboard"));
	MonsterBehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Monster Behavior Tree"));
	BrainComponent = MonsterBehaviorTreeComponent;

	MonsterPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Monster Perception"));
	SetPerceptionComponent(*MonsterPerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1800.0f;
	SightConfig->LoseSightRadius = 2200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	MonsterPerceptionComponent->ConfigureSense(*SightConfig);
	MonsterPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	DamageConfig->SetMaxAge(8.0f);
	MonsterPerceptionComponent->ConfigureSense(*DamageConfig);
	MonsterPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
		this, &ThisClass::HandleTargetPerceptionUpdated);
}

void ARogue10mMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeMonsterAI();
}

void ARogue10mMonsterAIController::InitializeMonsterAI()
{
	ARogue10mBasicMonster* Monster = GetMonsterPawn();
	if (!Monster || Monster->IsDead())
	{
		return;
	}

	SightConfig->SightRadius = Monster->GetDetectionRange();
	SightConfig->LoseSightRadius = FMath::Max(
		Monster->GetLoseSightRange(), Monster->GetDetectionRange());
	SightConfig->SetMaxAge(Monster->GetForgetTargetSeconds());
	DamageConfig->SetMaxAge(Monster->GetForgetTargetSeconds());
	MonsterPerceptionComponent->RequestStimuliListenerUpdate();

	if (MonsterBehaviorTreeComponent && MonsterBehaviorTreeComponent->IsRunning())
	{
		MonsterBehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
	BuildRuntimeBlackboard();
	BuildRuntimeBehaviorTree();
	UBehaviorTree* BehaviorTree = Monster->ResolveBehaviorTreeAsset();
	if (!BehaviorTree || !BehaviorTree->RootNode || !BehaviorTree->BlackboardAsset)
	{
		BehaviorTree = RuntimeBehaviorTree;
	}

	UBlackboardComponent* ResolvedBlackboard = MonsterBlackboardComponent;
	if (!BehaviorTree || !BehaviorTree->BlackboardAsset
		|| !UseBlackboard(BehaviorTree->BlackboardAsset, ResolvedBlackboard))
	{
		return;
	}
	MonsterBlackboardComponent = ResolvedBlackboard;
	MonsterBlackboardComponent->SetValueAsVector(HomeLocationKey, Monster->GetActorLocation());
	MonsterBlackboardComponent->SetValueAsVector(PatrolLocationKey, Monster->GetActorLocation());
	MonsterBlackboardComponent->SetValueAsVector(LastKnownLocationKey, Monster->GetActorLocation());
	MonsterBlackboardComponent->ClearValue(TargetActorKey);
	MonsterBlackboardComponent->SetValueAsBool(HasTargetKey, false);
	MonsterBlackboardComponent->SetValueAsBool(IsInAttackRangeKey, false);
	MonsterBlackboardComponent->SetValueAsBool(WasDamagedKey, false);
	LastTargetSenseTime = -1.0;
	NextPatrolMoveTime = 0.0;
	bWaitingForPatrolArrival = false;
	RunBehaviorTree(BehaviorTree);
}
void ARogue10mMonsterAIController::OnUnPossess()
{
	StopMonsterAI();
	Super::OnUnPossess();
}

bool ARogue10mMonsterAIController::ExecuteBehaviorDecision()
{
	ARogue10mBasicMonster* Monster = GetMonsterPawn();
	if (!Monster || Monster->IsDead() || !MonsterBlackboardComponent || !GetWorld())
	{
		return false;
	}

	ARogue10mCharacter* Target = GetBlackboardTarget();
	if (Target && !IsValidCombatTarget(Target))
	{
		ClearTarget();
		Target = nullptr;
	}

	if (Target)
	{
		const FVector HomeLocation =
			MonsterBlackboardComponent->GetValueAsVector(HomeLocationKey);
		const double CurrentTime = GetWorld()->GetTimeSeconds();
		const FAISenseID SightSenseId = UAISense::GetSenseID<UAISense_Sight>();
		const bool bHasActiveSight =
			MonsterPerceptionComponent->HasActiveStimulus(*Target, SightSenseId);
		if (bHasActiveSight)
		{
			LastTargetSenseTime = CurrentTime;
			MonsterBlackboardComponent->SetValueAsVector(
				LastKnownLocationKey, Target->GetActorLocation());
		}

		const bool bTargetOutsideLeash = FVector::Dist2D(
			HomeLocation, Target->GetActorLocation()) > Monster->GetMaxChaseDistance();
		const bool bMonsterOutsideLeash = FVector::Dist2D(
			HomeLocation, Monster->GetActorLocation()) > Monster->GetMaxChaseDistance();
		const bool bForgotTarget = !bHasActiveSight && LastTargetSenseTime >= 0.0
			&& CurrentTime - LastTargetSenseTime > Monster->GetForgetTargetSeconds();
		if (bTargetOutsideLeash || bMonsterOutsideLeash || bForgotTarget)
		{
			ClearTarget();
			Target = nullptr;
		}
	}

	if (Target)
	{
		UpdateCombatBehavior(*Monster, *Target);
	}
	else
	{
		UpdatePatrolBehavior(*Monster);
	}
	return true;
}

void ARogue10mMonsterAIController::NotifyDamageStimulus(
	AActor* SourceActor, float DamageAmount)
{
	ARogue10mCharacter* SourceCharacter = Cast<ARogue10mCharacter>(SourceActor);
	ARogue10mBasicMonster* Monster = GetMonsterPawn();
	if (!Monster || !IsValidCombatTarget(SourceCharacter))
	{
		return;
	}

	AcquireTarget(SourceCharacter, true);
	UAISense_Damage::ReportDamageEvent(
		this,
		Monster,
		SourceCharacter,
		DamageAmount,
		SourceCharacter->GetActorLocation(),
		Monster->GetActorLocation());
}

void ARogue10mMonsterAIController::StopMonsterAI()
{
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);
	if (MonsterBehaviorTreeComponent)
	{
		MonsterBehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
	ClearTarget();
}

void ARogue10mMonsterAIController::HandleTargetPerceptionUpdated(
	AActor* Actor, FAIStimulus Stimulus)
{
	ARogue10mCharacter* Target = Cast<ARogue10mCharacter>(Actor);
	if (!IsValidCombatTarget(Target) || !GetWorld())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		const bool bFromDamage =
			Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>();
		AcquireTarget(Target, bFromDamage);
	}
	else if (GetBlackboardTarget() == Target && MonsterBlackboardComponent)
	{
		MonsterBlackboardComponent->SetValueAsVector(
			LastKnownLocationKey, Target->GetActorLocation());
		LastTargetSenseTime = GetWorld()->GetTimeSeconds();
	}
}

ARogue10mBasicMonster* ARogue10mMonsterAIController::GetMonsterPawn() const
{
	return Cast<ARogue10mBasicMonster>(GetPawn());
}

ARogue10mCharacter* ARogue10mMonsterAIController::GetBlackboardTarget() const
{
	return MonsterBlackboardComponent
		? Cast<ARogue10mCharacter>(
			MonsterBlackboardComponent->GetValueAsObject(TargetActorKey))
		: nullptr;
}

bool ARogue10mMonsterAIController::IsValidCombatTarget(
	const ARogue10mCharacter* Target) const
{
	return Target && !Target->IsDead();
}

void ARogue10mMonsterAIController::AcquireTarget(
	ARogue10mCharacter* Target, bool bFromDamage)
{
	if (!IsValidCombatTarget(Target) || !MonsterBlackboardComponent || !GetWorld())
	{
		return;
	}
	MonsterBlackboardComponent->SetValueAsObject(TargetActorKey, Target);
	MonsterBlackboardComponent->SetValueAsVector(
		LastKnownLocationKey, Target->GetActorLocation());
	MonsterBlackboardComponent->SetValueAsBool(HasTargetKey, true);
	MonsterBlackboardComponent->SetValueAsBool(WasDamagedKey, bFromDamage);
	LastTargetSenseTime = GetWorld()->GetTimeSeconds();
	SetFocus(Target, EAIFocusPriority::Gameplay);
}

void ARogue10mMonsterAIController::ClearTarget()
{
	if (ARogue10mBasicMonster* Monster = GetMonsterPawn())
	{
		Monster->ClearAITarget();
	}
	if (MonsterBlackboardComponent)
	{
		MonsterBlackboardComponent->ClearValue(TargetActorKey);
		MonsterBlackboardComponent->SetValueAsBool(HasTargetKey, false);
		MonsterBlackboardComponent->SetValueAsBool(IsInAttackRangeKey, false);
		MonsterBlackboardComponent->SetValueAsBool(WasDamagedKey, false);
	}
	LastTargetSenseTime = -1.0;
	ClearFocus(EAIFocusPriority::Gameplay);
	StopMovement();
}

void ARogue10mMonsterAIController::UpdateCombatBehavior(
	ARogue10mBasicMonster& Monster, ARogue10mCharacter& Target)
{
	const float Distance = FVector::Dist2D(
		Monster.GetActorLocation(), Target.GetActorLocation());
	const float AttackRange = Monster.GetEffectiveAttackRange();
	const bool bInAttackRange = Distance <= AttackRange;
	MonsterBlackboardComponent->SetValueAsBool(IsInAttackRangeKey, bInAttackRange);
	MonsterBlackboardComponent->SetValueAsVector(
		LastKnownLocationKey, Target.GetActorLocation());
	Monster.SetAITarget(&Target);
	SetFocus(&Target, EAIFocusPriority::Gameplay);

	if (bInAttackRange)
	{
		StopMovement();
		Monster.ExecuteAICombat(&Target);
		return;
	}

	MoveToActor(
		&Target,
		FMath::Max(Monster.GetStopDistance(), AttackRange * 0.8f),
		true,
		true,
		true,
		nullptr,
		true);
}

void ARogue10mMonsterAIController::UpdatePatrolBehavior(
	ARogue10mBasicMonster& Monster)
{
	if (!GetWorld())
	{
		return;
	}

	const FVector HomeLocation =
		MonsterBlackboardComponent->GetValueAsVector(HomeLocationKey);
	const float DistanceFromHome = FVector::Dist2D(
		Monster.GetActorLocation(), HomeLocation);
	if (DistanceFromHome > Monster.GetPatrolRadius() + 100.0f)
	{
		if (GetMoveStatus() == EPathFollowingStatus::Idle)
		{
			MoveToLocation(HomeLocation, 60.0f, true, true, true, false);
			bWaitingForPatrolArrival = true;
		}
		return;
	}

	if (GetMoveStatus() != EPathFollowingStatus::Idle)
	{
		bWaitingForPatrolArrival = true;
		return;
	}

	const double CurrentTime = GetWorld()->GetTimeSeconds();
	if (bWaitingForPatrolArrival)
	{
		bWaitingForPatrolArrival = false;
		NextPatrolMoveTime = CurrentTime + Monster.GetPatrolWaitSeconds();
		return;
	}
	if (CurrentTime < NextPatrolMoveTime)
	{
		return;
	}

	UNavigationSystemV1* NavigationSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation PatrolLocation;
	if (NavigationSystem && NavigationSystem->GetRandomReachablePointInRadius(
		HomeLocation, Monster.GetPatrolRadius(), PatrolLocation))
	{
		MonsterBlackboardComponent->SetValueAsVector(
			PatrolLocationKey, PatrolLocation.Location);
		MoveToLocation(PatrolLocation.Location, 50.0f, true, true, true, false);
		bWaitingForPatrolArrival = true;
	}
	else
	{
		NextPatrolMoveTime = CurrentTime + Monster.GetPatrolWaitSeconds();
	}
}
void ARogue10mMonsterAIController::BuildRuntimeBlackboard()
{
	if (RuntimeBlackboard)
	{
		return;
	}
	RuntimeBlackboard = NewObject<UBlackboardData>(
		this, TEXT("Runtime Monster Blackboard"), RF_Transient);
	UBlackboardKeyType_Object* TargetKeyType =
		RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Object>(TargetActorKey);
	if (TargetKeyType)
	{
		TargetKeyType->BaseClass = AActor::StaticClass();
	}
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Vector>(HomeLocationKey);
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Vector>(PatrolLocationKey);
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Vector>(LastKnownLocationKey);
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Bool>(HasTargetKey);
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Bool>(IsInAttackRangeKey);
	RuntimeBlackboard->UpdatePersistentKey<UBlackboardKeyType_Bool>(WasDamagedKey);
	RuntimeBlackboard->UpdateKeyIDs();
}

void ARogue10mMonsterAIController::BuildRuntimeBehaviorTree()
{
	if (RuntimeBehaviorTree)
	{
		return;
	}
	RuntimeBehaviorTree = NewObject<UBehaviorTree>(
		this, TEXT("Runtime Monster Behavior Tree"), RF_Transient);
	RuntimeBehaviorTree->BlackboardAsset = RuntimeBlackboard;

	UBTComposite_Sequence* RootSequence = NewObject<UBTComposite_Sequence>(
		RuntimeBehaviorTree, TEXT("Monster Decision Loop"));
	UBTTask_Rogue10mMonsterDecision* DecisionTask =
		NewObject<UBTTask_Rogue10mMonsterDecision>(RuntimeBehaviorTree);
	UBTTask_Wait* WaitTask = NewObject<UBTTask_Wait>(RuntimeBehaviorTree);
	WaitTask->WaitTime = FValueOrBBKey_Float(0.25f);
	WaitTask->RandomDeviation = FValueOrBBKey_Float(0.0f);

	FBTCompositeChild DecisionChild;
	DecisionChild.ChildTask = DecisionTask;
	RootSequence->Children.Add(DecisionChild);
	FBTCompositeChild WaitChild;
	WaitChild.ChildTask = WaitTask;
	RootSequence->Children.Add(WaitChild);
	RuntimeBehaviorTree->RootNode = RootSequence;
}