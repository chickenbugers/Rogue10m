// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCombatComponent.generated.h"

UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mCombatComponent();

	const URogue10mAttackSkillData* ResolveAttackSkill(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveChargedAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveComboAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	ERogue10mAttackInputSlot GetAttackInputSlot(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;

	void RecordAttackPressed(bool bPrimaryAttack, float PressedTime);
	float ConsumeAttackHeldTime(bool bPrimaryAttack, float CurrentTime);

	bool IsAttackOnCooldown(float CurrentTime) const;
	float GetAttackCooldownRemaining() const;
	float GetAttackCooldownDuration() const { return AttackCooldownDuration; }
	void StartSharedAttackCooldown(const URogue10mAttackSkillData& SkillData, bool bComboAttack);

	void OpenComboWindow(const URogue10mAttackSkillData& SkillData);
	void ResetComboWindow();
	const URogue10mAttackSkillData* GetDisplayedAttackSkill() const;

	bool IsComboEnabled() const { return bAllowAttackCombo; }

private:
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

	float LeftAttackPressedTime = -1.0f;
	float RightAttackPressedTime = -1.0f;
	TWeakObjectPtr<const URogue10mAttackSkillData> ActiveComboSourceSkill;
	TWeakObjectPtr<const URogue10mAttackSkillData> ActiveComboRootSkill;
	TWeakObjectPtr<const URogue10mAttackSkillData> AttackCooldownSourceSkill;
	float ActiveComboWindowOpenTime = -1.0f;
	float ActiveComboWindowCloseTime = -1.0f;
	float AttackCooldownEndTime = -1.0f;
	float AttackCooldownDuration = 0.0f;
};
