// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCombatComponent.generated.h"

class ARogue10mCharacter;
class URogue10mAttributeSet;

struct FRogue10mActiveAttackExecution
{
	TWeakObjectPtr<const URogue10mAttackSkillData> SkillData;
	TMap<TWeakObjectPtr<AActor>, int32> TargetHitCounts;
	TArray<TWeakObjectPtr<AActor>> LockedTargets;
	FTimerHandle TimerHandle;
	float ProjectileTravelDistance = 0.0f;
	int32 CompletedPulses = 0;
};

UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mCombatComponent();

	void InitializeAbilitySystem();
	void HandleAttackPressed(bool bPrimaryAttack);
	void HandleAttackReleased(bool bPrimaryAttack);
	bool ExecutePendingAttackSkillFromAbility();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	bool ActivateQuickSlot(int32 SlotNumber);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat|Skill Tree")
	void UnlockAttackSkill(URogue10mAttackSkillData* SkillData);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat|Skill Tree")
	bool IsAttackSkillUnlocked(const URogue10mAttackSkillData* SkillData) const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat")
	TArray<URogue10mAttackSkillData*> GetUnlockedWeaponSkills() const;

	const URogue10mAttackSkillData* ResolveAttackSkill(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveChargedAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveComboAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	TArray<const URogue10mAttackSkillData*> GetWeaponQuickSlotSkills() const;
	ERogue10mAttackInputSlot GetAttackInputSlot(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;

	bool IsAttackOnCooldown(float CurrentTime) const;
	bool IsComboSequenceActive(float CurrentTime) const;
	float GetAttackCooldownRemaining() const;
	float GetAttackCooldownDuration() const { return AttackCooldownDuration; }
	const URogue10mAttackSkillData* GetDisplayedAttackSkill() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	ARogue10mCharacter* GetOwnerCharacter() const;
	URogue10mAttributeSet* GetOwnerAttributes() const;
	float GetAttackSpeedMultiplier() const;
	bool CanUseCombatInput() const;
	void ExecuteCombatAttack(bool bPrimaryAttack, bool bChargedAttack);
	bool ExecuteAttackSkill(const URogue10mAttackSkillData& SkillData, bool bComboAttack);
	bool TryActivateAttackAbility(const URogue10mAttackSkillData& SkillData, bool bComboAttack);
	void StartAttackHitSequence(const URogue10mAttackSkillData& SkillData);
	void ExecuteAttackHitPulse(uint32 ExecutionId);
	void GatherAttackTargets(const URogue10mAttackSkillData& SkillData, FRogue10mActiveAttackExecution& Execution,
		const FVector& Origin, const FVector& Forward, const FQuat& Rotation, TArray<AActor*>& OutTargets) const;
	bool ApplyAttackDamage(const URogue10mAttackSkillData& SkillData, FRogue10mActiveAttackExecution& Execution,
		AActor& TargetActor, const FVector& DamageDirection, int32 PulseIndex);
	void FinishAttackHitSequence(uint32 ExecutionId);
	bool CanPayResourceCosts(const URogue10mAttackSkillData& SkillData) const;
	void ConsumeResourceCosts(const URogue10mAttackSkillData& SkillData);
	void StartSharedAttackCooldown(const URogue10mAttackSkillData& SkillData, bool bComboAttack);
	void OpenComboWindow(const URogue10mAttackSkillData& SkillData);
	void ResetComboWindow();
	void DrawAttackDebug(const FVector& TraceStart, const FVector& TraceEnd, float TraceRadius, const FLinearColor& Color, bool bHit, const FHitResult& Hit) const;
	void AddCombatLog(const FString& Message, const FLinearColor& Color = FLinearColor::White) const;
	FString GetAttackInputText(bool bPrimaryAttack, bool bJumpAttack) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> PrimaryAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> SpecialAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> JumpPrimaryAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> JumpSpecialAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> ChargedPrimaryAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Data", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URogue10mAttackSkillData> ChargedSpecialAttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Combo", meta=(AllowPrivateAccess="true"))
	bool bAllowAttackCombo = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Debug", meta=(AllowPrivateAccess="true"))
	bool bDrawAttackDebug = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Charge", meta=(AllowPrivateAccess="true", ClampMin="0.05"))
	float DefaultChargeThreshold = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|GAS", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UGameplayAbility> DefaultAttackAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Skill Tree", meta=(AllowPrivateAccess="true"))
	TSet<FName> UnlockedAttackSkillNames;

	float LeftAttackPressedTime = -1.0f;
	float RightAttackPressedTime = -1.0f;
	TWeakObjectPtr<const URogue10mAttackSkillData> ActiveComboSourceSkill;
	TWeakObjectPtr<const URogue10mAttackSkillData> ActiveComboRootSkill;
	TWeakObjectPtr<const URogue10mAttackSkillData> AttackCooldownSourceSkill;
	TWeakObjectPtr<const URogue10mAttackSkillData> PendingAbilityAttackSkill;
	float ActiveComboWindowOpenTime = -1.0f;
	float ActiveComboWindowCloseTime = -1.0f;
	float AttackCooldownStartTime = -1.0f;
	float AttackCooldownEndTime = -1.0f;
	float AttackCooldownDuration = 0.0f;
	bool bPendingAbilityComboAttack = false;
	bool bExecutingAttackFromAbility = false;
	uint32 NextAttackExecutionId = 1;
	TMap<uint32, FRogue10mActiveAttackExecution> ActiveAttackExecutions;
};