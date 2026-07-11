// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mBasicMonster.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mAbilitySystemComponent.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mPlayerController.h"

ARogue10mBasicMonster::ARogue10mBasicMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<URogue10mAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<URogue10mAttributeSet>(TEXT("Attribute Set"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	GetCharacterMovement()->MaxWalkSpeed = 260.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

UAbilitySystemComponent* ARogue10mBasicMonster::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARogue10mBasicMonster::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AttributeSet->InitializeVitals(MaxHealth, 1.0f, 1.0f);
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

	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance <= DetectionRange)
	{
		MoveTowardTarget(Distance);
		TryAttackTarget(Distance);
	}
}

float ARogue10mBasicMonster::TakeDamage(
	float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsDead || !AttributeSet || AppliedDamage <= 0.0f)
	{
		return AppliedDamage;
	}

	AttributeSet->SetHealth(AttributeSet->GetHealth() - AppliedDamage);
	UE_LOG(
		LogRogue10m, Log, TEXT("%s 피해 %.1f, 체력 %.1f / %.1f"),
		*GetNameSafe(this), AppliedDamage, AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());

	ARogue10mPlayerController* PlayerController = EventInstigator
		? Cast<ARogue10mPlayerController>(EventInstigator)
		: Cast<ARogue10mPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->AddCombatLogMessage(
			FString::Printf(TEXT("몬스터에게 피해 %.0f"), AppliedDamage),
			FLinearColor(1.0f, 0.72f, 0.22f, 1.0f));
	}

	if (AttributeSet->GetHealth() <= 0.0f)
	{
		Die();
	}
	return AppliedDamage;
}

bool ARogue10mBasicMonster::CanReceiveRogue10mAttack_Implementation(AActor* AttackSource) const
{
	return !bIsDead && AttackSource != this;
}

FVector ARogue10mBasicMonster::GetRogue10mDamageIndicatorLocation_Implementation() const
{
	FVector BoundsOrigin;
	FVector BoundsExtent;
	GetActorBounds(true, BoundsOrigin, BoundsExtent);
	return BoundsOrigin + FVector(0.0f, 0.0f, BoundsExtent.Z + 30.0f);
}

void ARogue10mBasicMonster::UpdateTarget()
{
	TargetCharacter = Cast<ARogue10mCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ARogue10mBasicMonster::MoveTowardTarget(float DistanceToTarget)
{
	ARogue10mCharacter* Target = TargetCharacter.Get();
	if (Target && DistanceToTarget > StopDistance)
	{
		AddMovementInput((Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D(), 1.0f);
	}
}

void ARogue10mBasicMonster::TryAttackTarget(float DistanceToTarget)
{
	ARogue10mCharacter* Target = TargetCharacter.Get();
	if (!Target || !GetWorld() || DistanceToTarget > AttackRange)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackInterval)
	{
		return;
	}

	LastAttackTime = CurrentTime;
	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, UDamageType::StaticClass());
	UE_LOG(LogRogue10m, Log, TEXT("%s이(가) %s에게 %.1f 피해"), *GetNameSafe(this), *GetNameSafe(Target), AttackDamage);

	if (ARogue10mPlayerController* PlayerController = Cast<ARogue10mPlayerController>(Target->GetController()))
	{
		PlayerController->AddCombatLogMessage(
			FString::Printf(TEXT("몬스터 공격: 플레이어 피해 %.0f"), AttackDamage),
			FLinearColor(1.0f, 0.42f, 0.36f, 1.0f));
	}
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
	UE_LOG(LogRogue10m, Log, TEXT("%s 사망"), *GetNameSafe(this));

	if (bDestroyOnDeath)
	{
		Destroy();
	}
}