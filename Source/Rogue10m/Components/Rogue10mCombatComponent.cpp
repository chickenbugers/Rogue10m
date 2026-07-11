// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCombatComponent.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mAttackTargetInterface.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mGameplayAbility_Attack.h"
#include "Rogue10mPlayerController.h"
#include "Rogue10mPlayerFeedbackComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

URogue10mCombatComponent::URogue10mCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DefaultAttackAbilityClass = URogue10mGameplayAbility_Attack::StaticClass();
	UnlockedAttackSkillNames = {
		FName(TEXT("DA_Attack_Unarmed_Primary")),
		FName(TEXT("DA_Attack_Unarmed_JumpPrimary"))
	};

	static ConstructorHelpers::FObjectFinder<URogue10mAttackSkillData> PrimaryAsset(
		TEXT("/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_Primary"));
	if (PrimaryAsset.Succeeded())
	{
		PrimaryAttackSkill = PrimaryAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<URogue10mAttackSkillData> JumpPrimaryAsset(
		TEXT("/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_JumpPrimary"));
	if (JumpPrimaryAsset.Succeeded())
	{
		JumpPrimaryAttackSkill = JumpPrimaryAsset.Object;
	}
}

void URogue10mCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URogue10mCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		for (TPair<uint32, FRogue10mActiveAttackExecution>& Pair : ActiveAttackExecutions)
		{
			World->GetTimerManager().ClearTimer(Pair.Value.TimerHandle);
		}
	}
	ActiveAttackExecutions.Reset();
	Super::EndPlay(EndPlayReason);
}

void URogue10mCombatComponent::InitializeAbilitySystem()
{
	ARogue10mCharacter* Character = GetOwnerCharacter();
	UAbilitySystemComponent* AbilitySystem = Character ? Character->GetAbilitySystemComponent() : nullptr;
	if (!Character || !AbilitySystem)
	{
		return;
	}

	if (Character->HasAuthority() && DefaultAttackAbilityClass
		&& !AbilitySystem->FindAbilitySpecFromClass(DefaultAttackAbilityClass))
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(DefaultAttackAbilityClass, 1, INDEX_NONE, this));
	}
}

void URogue10mCombatComponent::HandleAttackPressed(bool bPrimaryAttack)
{
	if (!CanUseCombatInput() || !GetWorld())
	{
		return;
	}

	float& PressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	PressedTime = GetWorld()->GetTimeSeconds();
	AddCombatLog(
		FString::Printf(TEXT("%s 입력: 차징 확인 시작"), bPrimaryAttack ? TEXT("좌클릭") : TEXT("우클릭")),
		FLinearColor(0.72f, 0.84f, 1.0f, 1.0f));
}

void URogue10mCombatComponent::HandleAttackReleased(bool bPrimaryAttack)
{
	if (!CanUseCombatInput() || !GetWorld())
	{
		return;
	}

	float& PressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	if (PressedTime < 0.0f)
	{
		return;
	}

	const float HeldTime = GetWorld()->GetTimeSeconds() - PressedTime;
	PressedTime = -1.0f;
	const ARogue10mCharacter* Character = GetOwnerCharacter();
	const bool bJumpAttack = Character && Character->GetCharacterMovement()->IsFalling();
	const URogue10mAttackSkillData* ChargedSkill = ResolveChargedAttackSkill(bPrimaryAttack, bJumpAttack);
	const float RequiredCharge = ChargedSkill ? ChargedSkill->ChargeSeconds : DefaultChargeThreshold;
	ExecuteCombatAttack(bPrimaryAttack, ChargedSkill && HeldTime >= RequiredCharge);
}

bool URogue10mCombatComponent::ExecutePendingAttackSkillFromAbility()
{
	const URogue10mAttackSkillData* SkillData = PendingAbilityAttackSkill.Get();
	if (!SkillData)
	{
		AddCombatLog(TEXT("GAS 공격 실행 실패: 예약된 공격 Data Asset이 없습니다."), FLinearColor(1.0f, 0.35f, 0.25f, 1.0f));
		return false;
	}

	TGuardValue<bool> Guard(bExecutingAttackFromAbility, true);
	const bool bExecuted = ExecuteAttackSkill(*SkillData, bPendingAbilityComboAttack);
	PendingAbilityAttackSkill.Reset();
	bPendingAbilityComboAttack = false;
	return bExecuted;
}

bool URogue10mCombatComponent::ActivateQuickSlot(int32 SlotNumber)
{
	if (!CanUseCombatInput())
	{
		return false;
	}

	const TArray<const URogue10mAttackSkillData*> Skills = GetWeaponQuickSlotSkills();
	if (!Skills.IsValidIndex(SlotNumber - 1) || !Skills[SlotNumber - 1])
	{
		return false;
	}

	const URogue10mAttackSkillData& SkillData = *Skills[SlotNumber - 1];
	return TryActivateAttackAbility(SkillData, false) || ExecuteAttackSkill(SkillData, false);
}

void URogue10mCombatComponent::UnlockAttackSkill(URogue10mAttackSkillData* SkillData)
{
	if (SkillData)
	{
		UnlockedAttackSkillNames.Add(SkillData->GetFName());
	}
}

bool URogue10mCombatComponent::IsAttackSkillUnlocked(const URogue10mAttackSkillData* SkillData) const
{
	return SkillData && UnlockedAttackSkillNames.Contains(SkillData->GetFName());
}

TArray<URogue10mAttackSkillData*> URogue10mCombatComponent::GetUnlockedWeaponSkills() const
{
	TArray<URogue10mAttackSkillData*> Result;
	for (const URogue10mAttackSkillData* Skill : GetWeaponQuickSlotSkills())
	{
		if (Skill)
		{
			Result.Add(const_cast<URogue10mAttackSkillData*>(Skill));
		}
	}
	return Result;
}

const URogue10mAttackSkillData* URogue10mCombatComponent::ResolveAttackSkill(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const
{
	if (bChargedAttack)
	{
		return ResolveChargedAttackSkill(bPrimaryAttack, bJumpAttack);
	}
	if (bJumpAttack)
	{
		return bPrimaryAttack ? JumpPrimaryAttackSkill : JumpSpecialAttackSkill;
	}
	return bPrimaryAttack ? PrimaryAttackSkill : SpecialAttackSkill;
}

const URogue10mAttackSkillData* URogue10mCombatComponent::ResolveChargedAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const
{
	return bJumpAttack ? nullptr : (bPrimaryAttack ? ChargedPrimaryAttackSkill : ChargedSpecialAttackSkill);
}

const URogue10mAttackSkillData* URogue10mCombatComponent::ResolveComboAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const
{
	if (!bAllowAttackCombo || !GetWorld())
	{
		return nullptr;
	}

	const URogue10mAttackSkillData* Source = ActiveComboSourceSkill.Get();
	if (!Source || !Source->bEnableCombo || !Source->NextComboSkill)
	{
		return nullptr;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < ActiveComboWindowOpenTime || CurrentTime > ActiveComboWindowCloseTime
		|| Source->ComboInputSlot != GetAttackInputSlot(bPrimaryAttack, false, bJumpAttack))
	{
		return nullptr;
	}
	return Source->NextComboSkill;
}

TArray<const URogue10mAttackSkillData*> URogue10mCombatComponent::GetWeaponQuickSlotSkills() const
{
	TArray<const URogue10mAttackSkillData*> Skills;
	const URogue10mAttackSkillData* Candidates[] = {
		PrimaryAttackSkill, JumpPrimaryAttackSkill, SpecialAttackSkill,
		ChargedPrimaryAttackSkill, ChargedSpecialAttackSkill,
		JumpSpecialAttackSkill
	};
	for (const URogue10mAttackSkillData* Skill : Candidates)
	{
		if (IsAttackSkillUnlocked(Skill))
		{
			Skills.AddUnique(Skill);
		}
	}
	return Skills;
}

ERogue10mAttackInputSlot URogue10mCombatComponent::GetAttackInputSlot(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const
{
	if (bChargedAttack)
	{
		return bPrimaryAttack ? ERogue10mAttackInputSlot::ChargedPrimary : ERogue10mAttackInputSlot::ChargedSpecial;
	}
	if (bJumpAttack)
	{
		return bPrimaryAttack ? ERogue10mAttackInputSlot::JumpPrimary : ERogue10mAttackInputSlot::JumpSpecial;
	}
	return bPrimaryAttack ? ERogue10mAttackInputSlot::Primary : ERogue10mAttackInputSlot::Special;
}

bool URogue10mCombatComponent::IsAttackOnCooldown(float CurrentTime) const
{
	return CurrentTime >= AttackCooldownStartTime && CurrentTime < AttackCooldownEndTime;
}

bool URogue10mCombatComponent::IsComboSequenceActive(float CurrentTime) const
{
	return ActiveComboSourceSkill.IsValid() && CurrentTime <= ActiveComboWindowCloseTime;
}

float URogue10mCombatComponent::GetAttackCooldownRemaining() const
{
	return GetWorld() && IsAttackOnCooldown(GetWorld()->GetTimeSeconds())
		? FMath::Max(0.0f, AttackCooldownEndTime - GetWorld()->GetTimeSeconds())
		: 0.0f;
}

const URogue10mAttackSkillData* URogue10mCombatComponent::GetDisplayedAttackSkill() const
{
	const URogue10mAttackSkillData* ComboSource = ActiveComboSourceSkill.Get();
	if (GetWorld() && ComboSource && ComboSource->NextComboSkill
		&& GetWorld()->GetTimeSeconds() <= ActiveComboWindowCloseTime)
	{
		return ComboSource->NextComboSkill;
	}
	return AttackCooldownSourceSkill.IsValid() ? AttackCooldownSourceSkill.Get() : PrimaryAttackSkill;
}

ARogue10mCharacter* URogue10mCombatComponent::GetOwnerCharacter() const
{
	return Cast<ARogue10mCharacter>(GetOwner());
}

URogue10mAttributeSet* URogue10mCombatComponent::GetOwnerAttributes() const
{
	const ARogue10mCharacter* Character = GetOwnerCharacter();
	return Character ? Character->GetRogueAttributeSet() : nullptr;
}

float URogue10mCombatComponent::GetAttackSpeedMultiplier() const
{
	const URogue10mAttributeSet* Attributes = GetOwnerAttributes();
	return Attributes ? FMath::Clamp(Attributes->GetAttackSpeedMultiplier(), 0.1f, 5.0f) : 1.0f;
}

bool URogue10mCombatComponent::CanUseCombatInput() const
{
	const ARogue10mCharacter* Character = GetOwnerCharacter();
	const ARogue10mPlayerController* Controller = Character ? Cast<ARogue10mPlayerController>(Character->GetController()) : nullptr;
	return Character && !Character->IsDead() && (!Controller || !Controller->IsAnyBlockingWindowVisible());
}

void URogue10mCombatComponent::ExecuteCombatAttack(bool bPrimaryAttack, bool bChargedAttack)
{
	const ARogue10mCharacter* Character = GetOwnerCharacter();
	const bool bJumpAttack = Character && Character->GetCharacterMovement()->IsFalling();
	const URogue10mAttackSkillData* ComboSkill = bChargedAttack ? nullptr : ResolveComboAttackSkill(bPrimaryAttack, bJumpAttack);

	if (!ComboSkill && !bChargedAttack && GetWorld() && IsComboSequenceActive(GetWorld()->GetTimeSeconds()))
	{
		AddCombatLog(TEXT("콤보 입력 시간이 아닙니다."), FLinearColor(1.0f, 0.76f, 0.36f, 1.0f));
		return;
	}

	const URogue10mAttackSkillData* SkillData = ComboSkill
		? ComboSkill
		: ResolveAttackSkill(bPrimaryAttack, bChargedAttack, bJumpAttack);
	if (!SkillData)
	{
		AddCombatLog(
			FString::Printf(TEXT("%s 공격 잠김: Data Asset이 지정되지 않았습니다."), *GetAttackInputText(bPrimaryAttack, bJumpAttack)),
			FLinearColor(1.0f, 0.35f, 0.25f, 1.0f));
		return;
	}
	if (!IsAttackSkillUnlocked(SkillData))
	{
		AddCombatLog(
			FString::Printf(TEXT("%s 사용 불가: 스킬트리에서 해금되지 않았습니다."), *SkillData->SkillName.ToString()),
			FLinearColor(1.0f, 0.42f, 0.24f, 1.0f));
		ResetComboWindow();
		return;
	}

	AddCombatLog(
		FString::Printf(TEXT("%s 실행: 피해 %.0f"), *SkillData->SkillName.ToString(), SkillData->Damage),
		bPrimaryAttack ? FLinearColor(1.0f, 0.72f, 0.42f, 1.0f) : FLinearColor(0.62f, 0.82f, 1.0f));
	if (!TryActivateAttackAbility(*SkillData, ComboSkill != nullptr))
	{
		ExecuteAttackSkill(*SkillData, ComboSkill != nullptr);
	}
}

bool URogue10mCombatComponent::ExecuteAttackSkill(const URogue10mAttackSkillData& SkillData, bool bComboAttack)
{
	ARogue10mCharacter* Character = GetOwnerCharacter();
	UCameraComponent* Camera = Character ? Character->GetFirstPersonCameraComponent() : nullptr;
	if (!Character || Character->IsDead() || !Camera || !GetWorld())
	{
		return false;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (!bComboAttack && IsAttackOnCooldown(CurrentTime))
	{
		AddCombatLog(
			FString::Printf(TEXT("%s 재사용 대기 중: %.1f초"), *SkillData.SkillName.ToString(), GetAttackCooldownRemaining()),
			FLinearColor(1.0f, 0.65f, 0.35f, 1.0f));
		return false;
	}
	if (!CanPayResourceCosts(SkillData))
	{
		return false;
	}

	ConsumeResourceCosts(SkillData);
	StartSharedAttackCooldown(SkillData, bComboAttack);

	if (SkillData.AttackMontage)
	{
		UAnimInstance* AnimInstance = Character->GetFirstPersonMesh()
			? Character->GetFirstPersonMesh()->GetAnimInstance()
			: nullptr;
		if (!AnimInstance && Character->GetMesh())
		{
			AnimInstance = Character->GetMesh()->GetAnimInstance();
		}
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(SkillData.AttackMontage, GetAttackSpeedMultiplier());
		}
	}

	StartAttackHitSequence(SkillData);
	OpenComboWindow(SkillData);
	return true;
}

void URogue10mCombatComponent::StartAttackHitSequence(const URogue10mAttackSkillData& SkillData)
{
	if (!GetWorld())
	{
		return;
	}

	uint32 ExecutionId = NextAttackExecutionId++;
	if (ExecutionId == 0)
	{
		ExecutionId = NextAttackExecutionId++;
	}
	FRogue10mActiveAttackExecution& Execution = ActiveAttackExecutions.Add(ExecutionId);
	Execution.SkillData = &SkillData;
	ExecuteAttackHitPulse(ExecutionId);

	const int32 PulseCount = SkillData.HitMode == ERogue10mAttackHitMode::Single
		? 1 : FMath::Clamp(SkillData.HitCount, 1, 64);
	if (PulseCount > 1 && ActiveAttackExecutions.Contains(ExecutionId))
	{
		FTimerDelegate PulseDelegate;
		PulseDelegate.BindUObject(this, &URogue10mCombatComponent::ExecuteAttackHitPulse, ExecutionId);
		const float Interval = FMath::Max(0.01f, SkillData.HitInterval / GetAttackSpeedMultiplier());
		GetWorld()->GetTimerManager().SetTimer(
			ActiveAttackExecutions.FindChecked(ExecutionId).TimerHandle,
			PulseDelegate, Interval, true);
	}
}

void URogue10mCombatComponent::ExecuteAttackHitPulse(uint32 ExecutionId)
{
	FRogue10mActiveAttackExecution* Execution = ActiveAttackExecutions.Find(ExecutionId);
	const URogue10mAttackSkillData* SkillData = Execution ? Execution->SkillData.Get() : nullptr;
	ARogue10mCharacter* Character = GetOwnerCharacter();
	UCameraComponent* Camera = Character ? Character->GetFirstPersonCameraComponent() : nullptr;
	if (!Execution || !SkillData || !Character || Character->IsDead() || !Camera || !GetWorld())
	{
		FinishAttackHitSequence(ExecutionId);
		return;
	}

	++Execution->CompletedPulses;
	const FVector Origin = Camera->GetComponentLocation();
	const FVector Forward = Camera->GetForwardVector();
	TArray<AActor*> Targets;

	if (SkillData->HitMode == ERogue10mAttackHitMode::MultiHit && Execution->CompletedPulses > 1)
	{
		for (const TWeakObjectPtr<AActor>& LockedTarget : Execution->LockedTargets)
		{
			if (AActor* Target = LockedTarget.Get())
			{
				Targets.Add(Target);
			}
		}
	}
	else
	{
		GatherAttackTargets(*SkillData, *Execution, Origin, Forward, Camera->GetComponentQuat(), Targets);
		if (SkillData->HitMode == ERogue10mAttackHitMode::MultiHit)
		{
			Execution->LockedTargets.Reserve(Targets.Num());
			for (AActor* Target : Targets)
			{
				Execution->LockedTargets.Add(Target);
			}
		}
	}

	const int32 MaxTargets = FMath::Clamp(SkillData->MaxTargetsPerHit, 1, 64);
	int32 AppliedTargetCount = 0;
	for (AActor* Target : Targets)
	{
		if (Target && ApplyAttackDamage(*SkillData, *Execution, *Target, Forward, Execution->CompletedPulses))
		{
			if (++AppliedTargetCount >= MaxTargets)
			{
				break;
			}
		}
	}

	const int32 PulseCount = SkillData->HitMode == ERogue10mAttackHitMode::Single
		? 1 : FMath::Clamp(SkillData->HitCount, 1, 64);
	if (Execution->CompletedPulses >= PulseCount
		|| (SkillData->HitMode == ERogue10mAttackHitMode::MultiHit && Execution->LockedTargets.IsEmpty()))
	{
		FinishAttackHitSequence(ExecutionId);
	}
}

void URogue10mCombatComponent::GatherAttackTargets(
	const URogue10mAttackSkillData& SkillData, FRogue10mActiveAttackExecution& Execution,
	const FVector& Origin, const FVector& Forward, const FQuat& Rotation, TArray<AActor*>& OutTargets) const
{
	ARogue10mCharacter* Character = GetOwnerCharacter();
	if (!Character || !GetWorld())
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(Rogue10mAttackSkill), false, Character);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	TArray<FOverlapResult> Overlaps;
	FVector QueryCenter = Origin;
	FCollisionShape QueryShape;
	FQuat QueryRotation = FQuat::Identity;

	switch (SkillData.AttackShape)
	{
	case ERogue10mAttackShape::LinearBox:
		QueryCenter = Origin + Forward * (SkillData.AttackRange * 0.5f);
		QueryShape = FCollisionShape::MakeBox(FVector(
			SkillData.AttackRange * 0.5f, SkillData.BoxHalfWidth, SkillData.BoxHalfHeight));
		QueryRotation = Rotation;
		break;
	case ERogue10mAttackShape::Projectile:
	{
		const int32 PulseCount = SkillData.HitMode == ERogue10mAttackHitMode::Single
			? 1 : FMath::Clamp(SkillData.HitCount, 1, 64);
		const float SegmentLength = SkillData.AttackRange / PulseCount;
		QueryCenter = Origin + Forward * (Execution.ProjectileTravelDistance + SegmentLength * 0.5f);
		Execution.ProjectileTravelDistance += SegmentLength;
		QueryShape = FCollisionShape::MakeBox(FVector(
			SegmentLength * 0.5f, SkillData.AttackTraceRadius, SkillData.AttackTraceRadius));
		QueryRotation = Rotation;
		break;
	}
	case ERogue10mAttackShape::Arc:
		QueryCenter = Origin;
		QueryShape = FCollisionShape::MakeSphere(SkillData.AttackRange);
		break;
	case ERogue10mAttackShape::Circle:
		QueryCenter = Origin + Forward * SkillData.CircleForwardOffset;
		QueryShape = FCollisionShape::MakeSphere(SkillData.AttackRange);
		break;
	default:
		return;
	}

	GetWorld()->OverlapMultiByObjectType(
		Overlaps, QueryCenter, QueryRotation, ObjectQueryParams, QueryShape, QueryParams);

	TSet<TWeakObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!Target || Target == Character || UniqueActors.Contains(Target)
			|| !Target->GetClass()->ImplementsInterface(URogue10mAttackTargetInterface::StaticClass())
			|| !IRogue10mAttackTargetInterface::Execute_CanReceiveRogue10mAttack(Target, Character))
		{
			continue;
		}

		if (SkillData.AttackShape == ERogue10mAttackShape::Arc)
		{
			const FVector Direction = (Target->GetActorLocation() - Origin).GetSafeNormal();
			const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(
				FMath::Clamp(SkillData.ArcAngleDegrees, 1.0f, 180.0f) * 0.5f));
			if (FVector::DotProduct(Forward, Direction) < MinimumDot)
			{
				continue;
			}
		}

		UniqueActors.Add(Target);
		OutTargets.Add(Target);
	}

	OutTargets.Sort([Origin](const AActor& Left, const AActor& Right)
	{
		return FVector::DistSquared(Origin, Left.GetActorLocation())
			< FVector::DistSquared(Origin, Right.GetActorLocation());
	});

	if (SkillData.bDrawDebugAttack)
	{
		const FColor Color = SkillData.DebugColor.ToFColor(true);
		if (SkillData.AttackShape == ERogue10mAttackShape::Arc)
		{
			DrawDebugCone(GetWorld(), Origin, Forward, SkillData.AttackRange,
				FMath::DegreesToRadians(SkillData.ArcAngleDegrees * 0.5f),
				FMath::DegreesToRadians(SkillData.ArcAngleDegrees * 0.5f), 24, Color, false, 1.2f);
		}
		else if (SkillData.AttackShape == ERogue10mAttackShape::Circle)
		{
			DrawDebugSphere(GetWorld(), QueryCenter, SkillData.AttackRange, 24, Color, false, 1.2f);
		}
		else
		{
			DrawDebugBox(GetWorld(), QueryCenter, QueryShape.GetExtent(), QueryRotation, Color, false, 1.2f);
		}
	}
}

bool URogue10mCombatComponent::ApplyAttackDamage(
	const URogue10mAttackSkillData& SkillData, FRogue10mActiveAttackExecution& Execution,
	AActor& TargetActor, const FVector& DamageDirection, int32 PulseIndex)
{
	ARogue10mCharacter* Character = GetOwnerCharacter();
	if (!Character || !TargetActor.GetClass()->ImplementsInterface(URogue10mAttackTargetInterface::StaticClass())
		|| !IRogue10mAttackTargetInterface::Execute_CanReceiveRogue10mAttack(&TargetActor, Character))
	{
		return false;
	}

	const int32 AllowedHits = SkillData.HitMode == ERogue10mAttackHitMode::Single
		? 1 : FMath::Clamp(SkillData.HitCount, 1, 64);
	if (Execution.TargetHitCounts.FindRef(&TargetActor) >= AllowedHits)
	{
		return false;
	}

	bool bCriticalHit = false;
	const float RolledDamage = SkillData.RollDamage(GetOwnerAttributes(), bCriticalHit);
	const float AppliedDamage = UGameplayStatics::ApplyDamage(
		&TargetActor, RolledDamage, Character->GetController(), Character, UDamageType::StaticClass());
	if (AppliedDamage <= 0.0f)
	{
		return false;
	}

	Execution.TargetHitCounts.FindOrAdd(&TargetActor) += 1;
	if (ARogue10mPlayerController* PlayerController = Cast<ARogue10mPlayerController>(Character->GetController()))
	{
		PlayerController->AddFloatingDamageNumber(&TargetActor, AppliedDamage, bCriticalHit);
	}
	UE_LOG(LogRogue10m, Verbose, TEXT("%s %d타: %s 피해 %.1f%s"),
		*SkillData.SkillName.ToString(), PulseIndex, *GetNameSafe(&TargetActor), AppliedDamage,
		bCriticalHit ? TEXT(" (치명타)") : TEXT(""));
	return true;
}
void URogue10mCombatComponent::FinishAttackHitSequence(uint32 ExecutionId)
{
	if (FRogue10mActiveAttackExecution* Execution = ActiveAttackExecutions.Find(ExecutionId))
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(Execution->TimerHandle);
		}
	}
	ActiveAttackExecutions.Remove(ExecutionId);
}

bool URogue10mCombatComponent::TryActivateAttackAbility(const URogue10mAttackSkillData& SkillData, bool bComboAttack)
{
	if (bExecutingAttackFromAbility)
	{
		return false;
	}

	ARogue10mCharacter* Character = GetOwnerCharacter();
	UAbilitySystemComponent* AbilitySystem = Character ? Character->GetAbilitySystemComponent() : nullptr;
	const TSubclassOf<UGameplayAbility> AbilityClass = SkillData.GameplayAbilityClass
		? SkillData.GameplayAbilityClass
		: DefaultAttackAbilityClass;
	if (!Character || !AbilitySystem || !AbilityClass)
	{
		return false;
	}

	PendingAbilityAttackSkill = &SkillData;
	bPendingAbilityComboAttack = bComboAttack;
	if (!AbilitySystem->FindAbilitySpecFromClass(AbilityClass) && Character->HasAuthority())
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
	}

	if (!AbilitySystem->TryActivateAbilityByClass(AbilityClass))
	{
		PendingAbilityAttackSkill.Reset();
		bPendingAbilityComboAttack = false;
		return false;
	}
	return true;
}

bool URogue10mCombatComponent::CanPayResourceCosts(const URogue10mAttackSkillData& SkillData) const
{
	const URogue10mAttributeSet* Attributes = GetOwnerAttributes();
	if (!Attributes)
	{
		return false;
	}

	for (const FRogue10mAttackResourceCost& Cost : SkillData.ResourceCosts)
	{
		if (Cost.Cost <= 0.0f)
		{
			continue;
		}

		const bool bEnough = Cost.ResourceType == ERogue10mAttackResourceType::Health
			? Attributes->GetHealth() > Cost.Cost
			: Cost.ResourceType == ERogue10mAttackResourceType::Stamina
				? Attributes->GetStamina() >= Cost.Cost
				: Cost.ResourceType == ERogue10mAttackResourceType::Mana
					? Attributes->GetMana() >= Cost.Cost
					: Attributes->GetIdentity() >= Cost.Cost;
		if (!bEnough)
		{
			if (Cost.ResourceType == ERogue10mAttackResourceType::Stamina)
			{
				if (ARogue10mCharacter* Character = GetOwnerCharacter())
				{
					if (URogue10mPlayerFeedbackComponent* Feedback = Character->GetPlayerFeedbackComponent())
					{
						Feedback->NotifyInsufficientStamina();
					}
				}
			}
			AddCombatLog(
				FString::Printf(TEXT("%s 사용 불가: 자원이 부족합니다."), *SkillData.SkillName.ToString()),
				FLinearColor(1.0f, 0.42f, 0.24f, 1.0f));
			return false;
		}
	}
	return true;
}

void URogue10mCombatComponent::ConsumeResourceCosts(const URogue10mAttackSkillData& SkillData)
{
	URogue10mAttributeSet* Attributes = GetOwnerAttributes();
	if (!Attributes)
	{
		return;
	}

	for (const FRogue10mAttackResourceCost& Cost : SkillData.ResourceCosts)
	{
		switch (Cost.ResourceType)
		{
		case ERogue10mAttackResourceType::Health:
			Attributes->ConsumeHealth(Cost.Cost);
			break;
		case ERogue10mAttackResourceType::Stamina:
			Attributes->ConsumeStamina(Cost.Cost);
			break;
		case ERogue10mAttackResourceType::Mana:
			Attributes->ConsumeMana(Cost.Cost);
			break;
		case ERogue10mAttackResourceType::Energy:
			Attributes->ConsumeIdentity(Cost.Cost);
			break;
		default:
			break;
		}
	}
}

void URogue10mCombatComponent::StartSharedAttackCooldown(const URogue10mAttackSkillData& SkillData, bool bComboAttack)
{
	if (!GetWorld())
	{
		return;
	}

	const URogue10mAttackSkillData* CooldownSource = &SkillData;
	if (bComboAttack && ActiveComboRootSkill.IsValid())
	{
		CooldownSource = ActiveComboRootSkill.Get();
	}
	else if (!bComboAttack)
	{
		ActiveComboRootSkill = &SkillData;
	}

	AttackCooldownSourceSkill = CooldownSource;
	const float AttackSpeed = GetAttackSpeedMultiplier();
	AttackCooldownDuration = FMath::Max(0.0f, CooldownSource->AttackCooldown) / AttackSpeed;
	const bool bHasNextCombo = bAllowAttackCombo && SkillData.bEnableCombo && SkillData.NextComboSkill;
	const float StartDelay = bHasNextCombo
		? FMath::Max(SkillData.ComboWindowOpenSeconds, SkillData.ComboWindowCloseSeconds) / AttackSpeed
		: 0.0f;
	AttackCooldownStartTime = GetWorld()->GetTimeSeconds() + FMath::Max(0.0f, StartDelay);
	AttackCooldownEndTime = AttackCooldownStartTime + AttackCooldownDuration;
}

void URogue10mCombatComponent::OpenComboWindow(const URogue10mAttackSkillData& SkillData)
{
	if (!bAllowAttackCombo || !SkillData.bEnableCombo || !SkillData.NextComboSkill || !GetWorld())
	{
		ResetComboWindow();
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float AttackSpeed = GetAttackSpeedMultiplier();
	const float OpenOffset = FMath::Max(0.0f, SkillData.ComboWindowOpenSeconds) / AttackSpeed;
	const float CloseOffset = FMath::Max(OpenOffset, SkillData.ComboWindowCloseSeconds / AttackSpeed);
	ActiveComboSourceSkill = &SkillData;
	ActiveComboWindowOpenTime = CurrentTime + OpenOffset;
	ActiveComboWindowCloseTime = CurrentTime + CloseOffset;
	AddCombatLog(
		FString::Printf(TEXT("콤보 대기: %.2f초 ~ %.2f초"), OpenOffset, CloseOffset),
		FLinearColor(0.72f, 0.88f, 1.0f, 1.0f));
}

void URogue10mCombatComponent::ResetComboWindow()
{
	ActiveComboSourceSkill.Reset();
	ActiveComboRootSkill.Reset();
	ActiveComboWindowOpenTime = -1.0f;
	ActiveComboWindowCloseTime = -1.0f;
}

void URogue10mCombatComponent::DrawAttackDebug(
	const FVector& TraceStart, const FVector& TraceEnd, float TraceRadius,
	const FLinearColor& Color, bool bHit, const FHitResult& Hit) const
{
	if (!bDrawAttackDebug || !GetWorld())
	{
		return;
	}

	const FColor DrawColor = Color.ToFColor(true);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DrawColor, false, 1.2f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), TraceEnd, TraceRadius, 16, DrawColor, false, 1.2f, 0, 1.5f);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius * 1.25f, 16, FColor::Red, false, 1.2f, 0, 2.5f);
	}
}

void URogue10mCombatComponent::AddCombatLog(const FString& Message, const FLinearColor& Color) const
{
	const ARogue10mCharacter* Character = GetOwnerCharacter();
	if (ARogue10mPlayerController* Controller = Character ? Cast<ARogue10mPlayerController>(Character->GetController()) : nullptr)
	{
		Controller->AddCombatLogMessage(Message, Color);
	}
	UE_LOG(LogRogue10m, Log, TEXT("%s"), *Message);
}

FString URogue10mCombatComponent::GetAttackInputText(bool bPrimaryAttack, bool bJumpAttack) const
{
	if (bJumpAttack)
	{
		return bPrimaryAttack ? TEXT("점프 좌클릭") : TEXT("점프 우클릭");
	}
	return bPrimaryAttack ? TEXT("좌클릭") : TEXT("우클릭");
}
