// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mBasicMonster.h"

#include "AIController.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mVitalsComponent.h"

ARogue10mBasicMonster::ARogue10mBasicMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	VitalsComponent = CreateDefaultSubobject<URogue10mVitalsComponent>(TEXT("Vitals Component"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	GetCharacterMovement()->MaxWalkSpeed = 260.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void ARogue10mBasicMonster::BeginPlay()
{
	Super::BeginPlay();
	UpdateTarget();
}

void ARogue10mBasicMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDead)
	{
		return;
	}

	if (!TargetCharacter.IsValid() || TargetCharacter->IsDead())
	{
		UpdateTarget();
	}

	ARogue10mCharacter* Target = TargetCharacter.Get();
	if (!Target || Target->IsDead())
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (DistanceToTarget > DetectionRange)
	{
		return;
	}

	MoveTowardTarget(DistanceToTarget);
	TryAttackTarget(DistanceToTarget);
}

float ARogue10mBasicMonster::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsDead || !VitalsComponent || DamageAmount <= 0.0f)
	{
		return AppliedDamage;
	}

	const float NewHealth = VitalsComponent->GetHealth().Current - DamageAmount;
	VitalsComponent->SetHealth(NewHealth);
	UE_LOG(LogRogue10m, Log, TEXT("%s took %.1f damage. HP %.1f / %.1f"), *GetNameSafe(this), DamageAmount, VitalsComponent->GetHealth().Current, VitalsComponent->GetHealth().Max);

	if (VitalsComponent->GetHealth().Current <= 0.0f)
	{
		Die();
	}

	return DamageAmount;
}

void ARogue10mBasicMonster::UpdateTarget()
{
	TargetCharacter = Cast<ARogue10mCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ARogue10mBasicMonster::MoveTowardTarget(float DistanceToTarget)
{
	ARogue10mCharacter* Target = TargetCharacter.Get();
	if (!Target || DistanceToTarget <= StopDistance)
	{
		return;
	}

	const FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	AddMovementInput(Direction, 1.0f);
}

void ARogue10mBasicMonster::TryAttackTarget(float DistanceToTarget)
{
	ARogue10mCharacter* Target = TargetCharacter.Get();
	UWorld* World = GetWorld();
	if (!Target || !World || DistanceToTarget > AttackRange)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackInterval)
	{
		return;
	}

	LastAttackTime = CurrentTime;
	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, UDamageType::StaticClass());
	UE_LOG(LogRogue10m, Log, TEXT("%s attacked %s for %.1f damage."), *GetNameSafe(this), *GetNameSafe(Target), AttackDamage);
}

void ARogue10mBasicMonster::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);

	UE_LOG(LogRogue10m, Log, TEXT("%s died."), *GetNameSafe(this));
	if (bDestroyOnDeath)
	{
		Destroy();
	}
}
