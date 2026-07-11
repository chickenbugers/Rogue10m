// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mAttributeSet.h"

#include "Net/UnrealNetwork.h"

URogue10mAttributeSet::URogue10mAttributeSet()
{
	InitMaxHealth(100.0f);
	InitHealth(100.0f);
	InitMaxStamina(100.0f);
	InitStamina(100.0f);
	InitMaxMana(100.0f);
	InitMana(100.0f);
	InitMaxIdentity(100.0f);
	InitIdentity(0.0f);
	InitPlayerLevel(1.0f);
	InitExperience(0.0f);
	InitExperienceToNextLevel(100.0f);
	InitAttackSpeedMultiplier(1.0f);
	InitMinDamageRatio(0.9f);
	InitMaxDamageRatio(1.1f);
	InitCriticalChance(0.0f);
	InitCriticalDamageMultiplier(1.5f);
}

void URogue10mAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetIdentityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxIdentity());
	}
	else if (Attribute == GetMaxHealthAttribute() || Attribute == GetMaxStaminaAttribute()
		|| Attribute == GetMaxManaAttribute() || Attribute == GetMaxIdentityAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
	else if (Attribute == GetPlayerLevelAttribute() || Attribute == GetExperienceToNextLevelAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
	else if (Attribute == GetExperienceAttribute())
	{
		NewValue = FMath::Max(0.0f, NewValue);
	}
	else if (Attribute == GetAttackSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 5.0f);
	}
	else if (Attribute == GetMinDamageRatioAttribute() || Attribute == GetMaxDamageRatioAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 10.0f);
	}
	else if (Attribute == GetCriticalChanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	else if (Attribute == GetCriticalDamageMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 10.0f);
	}
}

void URogue10mAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, Identity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MaxIdentity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, PlayerLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, ExperienceToNextLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, AttackSpeedMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MinDamageRatio, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, MaxDamageRatio, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URogue10mAttributeSet, CriticalDamageMultiplier, COND_None, REPNOTIFY_Always);
}

void URogue10mAttributeSet::InitializeVitals(float InMaxHealth, float InMaxStamina, float InMaxMana)
{
	SetMaxHealth(FMath::Max(1.0f, InMaxHealth));
	SetMaxStamina(FMath::Max(1.0f, InMaxStamina));
	SetMaxMana(FMath::Max(1.0f, InMaxMana));
	RestoreVitals();
}

void URogue10mAttributeSet::RestoreVitals()
{
	SetHealth(GetMaxHealth());
	SetStamina(GetMaxStamina());
	SetMana(GetMaxMana());
}

bool URogue10mAttributeSet::ConsumeHealth(float Amount)
{
	if (Amount <= 0.0f || GetHealth() <= Amount)
	{
		return Amount <= 0.0f;
	}
	SetHealth(GetHealth() - Amount);
	return true;
}

bool URogue10mAttributeSet::ConsumeStamina(float Amount)
{
	if (Amount <= 0.0f || GetStamina() < Amount)
	{
		return Amount <= 0.0f;
	}
	SetStamina(GetStamina() - Amount);
	return true;
}

bool URogue10mAttributeSet::ConsumeMana(float Amount)
{
	if (Amount <= 0.0f || GetMana() < Amount)
	{
		return Amount <= 0.0f;
	}
	SetMana(GetMana() - Amount);
	return true;
}

bool URogue10mAttributeSet::ConsumeIdentity(float Amount)
{
	if (Amount <= 0.0f || GetIdentity() < Amount)
	{
		return Amount <= 0.0f;
	}
	SetIdentity(GetIdentity() - Amount);
	return true;
}

void URogue10mAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, Health, OldValue); }
void URogue10mAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MaxHealth, OldValue); }
void URogue10mAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, Stamina, OldValue); }
void URogue10mAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MaxStamina, OldValue); }
void URogue10mAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, Mana, OldValue); }
void URogue10mAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MaxMana, OldValue); }
void URogue10mAttributeSet::OnRep_Identity(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, Identity, OldValue); }
void URogue10mAttributeSet::OnRep_MaxIdentity(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MaxIdentity, OldValue); }
void URogue10mAttributeSet::OnRep_PlayerLevel(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, PlayerLevel, OldValue); }
void URogue10mAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, Experience, OldValue); }
void URogue10mAttributeSet::OnRep_ExperienceToNextLevel(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, ExperienceToNextLevel, OldValue); }
void URogue10mAttributeSet::OnRep_AttackSpeedMultiplier(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, AttackSpeedMultiplier, OldValue); }
void URogue10mAttributeSet::OnRep_MinDamageRatio(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MinDamageRatio, OldValue); }
void URogue10mAttributeSet::OnRep_MaxDamageRatio(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, MaxDamageRatio, OldValue); }

void URogue10mAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, CriticalChance, OldValue); }
void URogue10mAttributeSet::OnRep_CriticalDamageMultiplier(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(URogue10mAttributeSet, CriticalDamageMultiplier, OldValue); }
