// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mBasicMonster.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mAbilitySystemComponent.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mPlayerController.h"
#include "Rogue10mPlayerState.h"
#include "Rogue10mMonsterDataAsset.h"
#include "Rogue10mVitalRegenerationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

ARogue10mBasicMonster::ARogue10mBasicMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<URogue10mAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<URogue10mAttributeSet>(TEXT("Attribute Set"));
	VitalRegenerationComponent = CreateDefaultSubobject<URogue10mVitalRegenerationComponent>(TEXT("Vital Regeneration Component"));

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
	float InitialStamina = 1.0f;
	float InitialMana = 1.0f;
	if (MonsterData)
	{
		MonsterDisplayName = MonsterData->DisplayName;
		MonsterLevel = MonsterData->Level;
		switch (MonsterData->MonsterRank)
		{
		case ERogue10mMonsterRank::MidBoss:
			MonsterAttributeText = NSLOCTEXT("Rogue10mMonster", "MidBossRank", "중간 보스");
			break;
		case ERogue10mMonsterRank::FinalBoss:
			MonsterAttributeText = NSLOCTEXT("Rogue10mMonster", "FinalBossRank", "최종 보스");
			break;
		default:
			MonsterAttributeText = NSLOCTEXT("Rogue10mMonster", "NormalRank", "일반");
			break;
		}
		MaxHealth = MonsterData->MaxHealth;
		InitialStamina = MonsterData->MaxStamina;
		InitialMana = MonsterData->MaxMana;
		ExperienceReward = MonsterData->ExperienceReward;
		DetectionRange = MonsterData->DetectionRange;
		StopDistance = MonsterData->StopDistance;
		AttackSkillData = MonsterData->AttackSkill;
		AttackRange = MonsterData->AttackRange;
		AttackDamage = MonsterData->AttackDamage;
		AttackInterval = MonsterData->AttackInterval;
		bDestroyOnDeath = MonsterData->bDestroyOnDeath;
		GetCharacterMovement()->MaxWalkSpeed = MonsterData->WalkSpeed;
		if (!MonsterData->SkeletalMesh.IsNull()) GetMesh()->SetSkeletalMeshAsset(MonsterData->SkeletalMesh.LoadSynchronous());
		if (MonsterData->AnimationBlueprintClass) GetMesh()->SetAnimInstanceClass(MonsterData->AnimationBlueprintClass);
		GetMesh()->SetRelativeLocation(MonsterData->MeshRelativeLocation);
		GetMesh()->SetRelativeRotation(MonsterData->MeshRelativeRotation);
		GetMesh()->SetRelativeScale3D(MonsterData->MeshRelativeScale);
		VitalRegenerationComponent->ConfigureRegeneration(
			MonsterData->HealthRegenerationPerSecond,
			MonsterData->StaminaRegenerationPerSecond,
			MonsterData->ManaRegenerationPerSecond);
	}
	AttributeSet->InitializeVitals(MaxHealth, InitialStamina, InitialMana);
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

	LastDamageInstigator = EventInstigator;
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
	const float Range = AttackSkillData ? AttackSkillData->AttackRange : AttackRange;
	const float Cooldown = AttackSkillData ? AttackSkillData->AttackCooldown : AttackInterval;
	if (!Target || !GetWorld() || DistanceToTarget > Range || GetWorld()->GetTimerManager().IsTimerActive(AttackSequenceTimer)) return;
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < Cooldown) return;
	LastAttackTime = Now;
	StartMonsterAttackSequence();
}

void ARogue10mBasicMonster::StartMonsterAttackSequence()
{
	CompletedAttackPulses = 0;
	LockedAttackTarget.Reset();
	ExecuteMonsterAttackPulse();
	const int32 Count = !AttackSkillData || AttackSkillData->HitMode == ERogue10mAttackHitMode::Single ? 1 : FMath::Clamp(AttackSkillData->HitCount, 1, 64);
	if (Count > 1 && GetWorld()) GetWorld()->GetTimerManager().SetTimer(AttackSequenceTimer, this, &ARogue10mBasicMonster::ExecuteMonsterAttackPulse, FMath::Max(0.01f, AttackSkillData->HitInterval), true);
}

void ARogue10mBasicMonster::ExecuteMonsterAttackPulse()
{
	ARogue10mCharacter* Target = LockedAttackTarget.IsValid() ? LockedAttackTarget.Get() : TargetCharacter.Get();
	if (!Target || Target->IsDead() || !GetWorld()) { if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimer); return; }
	++CompletedAttackPulses;
	const int32 Count = !AttackSkillData || AttackSkillData->HitMode == ERogue10mAttackHitMode::Single ? 1 : FMath::Clamp(AttackSkillData->HitCount, 1, 64);
	const bool bLockedMultiHit = AttackSkillData && AttackSkillData->HitMode == ERogue10mAttackHitMode::MultiHit && LockedAttackTarget.IsValid();
	const bool bCanHit = !AttackSkillData || bLockedMultiHit || IsTargetInsideAttackShape(*Target, *AttackSkillData);
	if (bCanHit)
	{
		if (AttackSkillData && AttackSkillData->HitMode == ERogue10mAttackHitMode::MultiHit && !LockedAttackTarget.IsValid()) LockedAttackTarget = Target;
		float Damage = 0.0f;
		bool bCriticalHit = false;
		if (AttackSkillData)
		{
			Damage = AttackSkillData->RollDamage(AttributeSet, bCriticalHit);
		}
		else
		{
			const float RatioA = AttributeSet ? AttributeSet->GetMinDamageRatio() : 0.9f;
			const float RatioB = AttributeSet ? AttributeSet->GetMaxDamageRatio() : 1.1f;
			Damage = AttackDamage * FMath::FRandRange(FMath::Min(RatioA, RatioB), FMath::Max(RatioA, RatioB));
			const float CriticalChance = AttributeSet ? AttributeSet->GetCriticalChance() : 0.0f;
			bCriticalHit = FMath::FRand() < FMath::Clamp(CriticalChance, 0.0f, 1.0f);
			if (bCriticalHit)
			{
				Damage *= AttributeSet ? FMath::Max(0.0f, AttributeSet->GetCriticalDamageMultiplier()) : 1.5f;
			}
		}
		UGameplayStatics::ApplyDamage(Target, Damage, GetController(), this, UDamageType::StaticClass());
		UE_LOG(LogRogue10m, Log, TEXT("%s 공격 %d/%d: %s에게 %.1f 피해%s"), *GetNameSafe(this), CompletedAttackPulses, Count, *GetNameSafe(Target), Damage, bCriticalHit ? TEXT(" (치명타)") : TEXT(""));
	}
	if (CompletedAttackPulses >= Count || (AttackSkillData && AttackSkillData->HitMode == ERogue10mAttackHitMode::MultiHit && !LockedAttackTarget.IsValid())) GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimer);
}

bool ARogue10mBasicMonster::IsTargetInsideAttackShape(const ARogue10mCharacter& Target, const URogue10mAttackSkillData& SkillData) const
{
	const FVector Offset = Target.GetActorLocation() - GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const float ForwardDistance = FVector::DotProduct(Forward, Offset);
	switch (SkillData.AttackShape)
	{
	case ERogue10mAttackShape::LinearBox:
		return ForwardDistance >= 0.0f && ForwardDistance <= SkillData.AttackRange && FMath::Abs(FVector::DotProduct(GetActorRightVector(), Offset)) <= SkillData.BoxHalfWidth && FMath::Abs(Offset.Z) <= SkillData.BoxHalfHeight;
	case ERogue10mAttackShape::Projectile:
		return ForwardDistance >= 0.0f && ForwardDistance <= SkillData.AttackRange && FVector::CrossProduct(Offset, Forward).Size() <= SkillData.AttackTraceRadius;
	case ERogue10mAttackShape::Arc:
		return Offset.Size2D() <= SkillData.AttackRange && FVector::DotProduct(Forward, Offset.GetSafeNormal()) >= FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(SkillData.ArcAngleDegrees, 1.0f, 180.0f) * 0.5f));
	case ERogue10mAttackShape::Circle:
		return FVector::Dist2D(GetActorLocation() + Forward * SkillData.CircleForwardOffset, Target.GetActorLocation()) <= SkillData.AttackRange;
	default: return false;
	}
}
void ARogue10mBasicMonster::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	if (HasAuthority() && ExperienceReward > 0)
	{
		if (ARogue10mPlayerController* RewardController =
			Cast<ARogue10mPlayerController>(LastDamageInstigator.Get()))
		{
			if (ARogue10mPlayerState* RewardState =
				RewardController->GetPlayerState<ARogue10mPlayerState>())
			{
				RewardState->AddExperience(ExperienceReward);
				RewardController->AddCombatLogMessage(
					FString::Printf(TEXT("%s 처치: 경험치 +%d"),
						*MonsterDisplayName.ToString(), ExperienceReward),
					FLinearColor(0.42f, 0.9f, 0.58f, 1.0f));
				UE_LOG(LogRogue10m, Log, TEXT("%s 처치 보상: 경험치 +%d"),
					*MonsterDisplayName.ToString(), ExperienceReward);
			}
		}
	}
	if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimer);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	UE_LOG(LogRogue10m, Log, TEXT("%s 사망"), *GetNameSafe(this));

	if (bDestroyOnDeath)
	{
		Destroy();
	}
}