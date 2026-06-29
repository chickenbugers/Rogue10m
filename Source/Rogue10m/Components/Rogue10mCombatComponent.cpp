// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCombatComponent.h"

#include "UObject/ConstructorHelpers.h"

URogue10mCombatComponent::URogue10mCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<URogue10mAttackSkillData> PrimaryAttackAsset(TEXT("/Game/DataAsset/DA_Attack_Unarmed_Primary"));
	if (PrimaryAttackAsset.Succeeded())
	{
		PrimaryAttackSkill = PrimaryAttackAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<URogue10mAttackSkillData> JumpPrimaryAttackAsset(TEXT("/Game/DataAsset/DA_Attack_Unarmed_JumpPrimary"));
	if (JumpPrimaryAttackAsset.Succeeded())
	{
		JumpPrimaryAttackSkill = JumpPrimaryAttackAsset.Object;
	}
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
	if (bJumpAttack)
	{
		return nullptr;
	}

	return bPrimaryAttack ? ChargedPrimaryAttackSkill : ChargedSpecialAttackSkill;
}

const URogue10mAttackSkillData* URogue10mCombatComponent::ResolveComboAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const
{
	if (!bAllowAttackCombo)
	{
		return nullptr;
	}

	const UWorld* World = GetWorld();
	const URogue10mAttackSkillData* SourceSkill = ActiveComboSourceSkill.Get();
	if (!World || !SourceSkill || !SourceSkill->bEnableCombo || !SourceSkill->NextComboSkill)
	{
		return nullptr;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime < ActiveComboWindowOpenTime || CurrentTime > ActiveComboWindowCloseTime)
	{
		return nullptr;
	}

	const ERogue10mAttackInputSlot CurrentInputSlot = GetAttackInputSlot(bPrimaryAttack, false, bJumpAttack);
	if (SourceSkill->ComboInputSlot != CurrentInputSlot)
	{
		return nullptr;
	}

	return SourceSkill->NextComboSkill;
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

void URogue10mCombatComponent::RecordAttackPressed(bool bPrimaryAttack, float PressedTime)
{
	float& StoredPressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	StoredPressedTime = PressedTime;
}

float URogue10mCombatComponent::ConsumeAttackHeldTime(bool bPrimaryAttack, float CurrentTime)
{
	float& StoredPressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	if (StoredPressedTime < 0.0f)
	{
		return -1.0f;
	}

	const float HeldTime = CurrentTime - StoredPressedTime;
	StoredPressedTime = -1.0f;
	return HeldTime;
}

bool URogue10mCombatComponent::IsAttackOnCooldown(float CurrentTime) const
{
	return CurrentTime < AttackCooldownEndTime;
}

float URogue10mCombatComponent::GetAttackCooldownRemaining() const
{
	const UWorld* World = GetWorld();
	return World ? FMath::Max(0.0f, AttackCooldownEndTime - World->GetTimeSeconds()) : 0.0f;
}

void URogue10mCombatComponent::StartSharedAttackCooldown(const URogue10mAttackSkillData& SkillData, bool bComboAttack)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const URogue10mAttackSkillData* CooldownSource = &SkillData;
	if (bComboAttack)
	{
		if (const URogue10mAttackSkillData* ComboRootSkill = ActiveComboRootSkill.Get())
		{
			CooldownSource = ComboRootSkill;
		}
	}
	else
	{
		ActiveComboRootSkill = &SkillData;
	}

	AttackCooldownSourceSkill = CooldownSource;
	AttackCooldownDuration = FMath::Max(0.0f, CooldownSource->AttackCooldown);
	AttackCooldownEndTime = World->GetTimeSeconds() + AttackCooldownDuration;
}

void URogue10mCombatComponent::OpenComboWindow(const URogue10mAttackSkillData& SkillData)
{
	if (!bAllowAttackCombo || !SkillData.bEnableCombo || !SkillData.NextComboSkill || !GetWorld())
	{
		ResetComboWindow();
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float OpenOffset = FMath::Max(0.0f, SkillData.ComboWindowOpenSeconds);
	const float CloseOffset = FMath::Max(OpenOffset, SkillData.ComboWindowCloseSeconds);

	ActiveComboSourceSkill = &SkillData;
	ActiveComboWindowOpenTime = CurrentTime + OpenOffset;
	ActiveComboWindowCloseTime = CurrentTime + CloseOffset;
}

void URogue10mCombatComponent::ResetComboWindow()
{
	ActiveComboSourceSkill.Reset();
	ActiveComboRootSkill.Reset();
	ActiveComboWindowOpenTime = -1.0f;
	ActiveComboWindowCloseTime = -1.0f;
}

const URogue10mAttackSkillData* URogue10mCombatComponent::GetDisplayedAttackSkill() const
{
	const UWorld* World = GetWorld();
	const URogue10mAttackSkillData* ComboSourceSkill = ActiveComboSourceSkill.Get();
	if (World && ComboSourceSkill && ComboSourceSkill->NextComboSkill && World->GetTimeSeconds() <= ActiveComboWindowCloseTime)
	{
		return ComboSourceSkill->NextComboSkill;
	}

	if (const URogue10mAttackSkillData* CooldownSkill = AttackCooldownSourceSkill.Get())
	{
		return CooldownSkill;
	}

	return PrimaryAttackSkill;
}
