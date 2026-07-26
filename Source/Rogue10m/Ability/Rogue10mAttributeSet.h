// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Rogue10mAttributeSet.generated.h"

#define ROGUE10M_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** 플레이어와 전투 액터의 수치 데이터를 단일 관리하는 GAS AttributeSet입니다. */
UCLASS()
class ROGUE10M_API URogue10mAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	URogue10mAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	void InitializeVitals(float InMaxHealth, float InMaxStamina = 100.0f, float InMaxMana = 100.0f);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	void RestoreVitals();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	bool ConsumeHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	bool ConsumeMana(float Amount);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Attributes")
	bool ConsumeIdentity(float Amount);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Health;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxHealth;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Stamina;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxStamina, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxStamina;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Mana;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxMana;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Identity, Category="Rogue10m|Attributes|Identity")
	FGameplayAttributeData Identity;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Identity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxIdentity, Category="Rogue10m|Attributes|Identity")
	FGameplayAttributeData MaxIdentity;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxIdentity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PlayerLevel, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData PlayerLevel;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, PlayerLevel);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Experience, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData Experience;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Experience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ExperienceToNextLevel, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData ExperienceToNextLevel;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, ExperienceToNextLevel);

	/** 스킬 기본 피해에 더하는 최종 정액 공격력입니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackPower, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData AttackPower;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, AttackPower);

	/** 피격 피해에서 차감하는 최종 정액 방어력입니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Defense, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData Defense;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Defense);

	/** CharacterMovement에 적용하는 최종 이동 속도(cm/s)입니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MoveSpeed, Category="Rogue10m|Attributes|Movement")
	FGameplayAttributeData MoveSpeed;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MoveSpeed);

	/** 기본값은 1.0이며 공격 몽타주, 콤보 창, 공격 재사용 대기시간에 함께 적용됩니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackSpeedMultiplier, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData AttackSpeedMultiplier;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, AttackSpeedMultiplier);
	/** 기본 최소 피해 비율입니다. 0.9는 기본 피해의 90%를 의미합니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MinDamageRatio, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData MinDamageRatio;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MinDamageRatio);

	/** 기본 최대 피해 비율입니다. 1.1은 기본 피해의 110%를 의미합니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxDamageRatio, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData MaxDamageRatio;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxDamageRatio);
	/** 0.0~1.0 범위의 치명타 확률입니다. 기본값은 0%입니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalChance, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData CriticalChance;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, CriticalChance);

	/** 치명타 발생 시 최종 피해 배율입니다. 기본값은 1.5(150%)입니다. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalDamageMultiplier, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData CriticalDamageMultiplier;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, CriticalDamageMultiplier);

protected:
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Identity(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxIdentity(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_PlayerLevel(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Experience(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ExperienceToNextLevel(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Defense(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AttackSpeedMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MinDamageRatio(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxDamageRatio(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CriticalChance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CriticalDamageMultiplier(const FGameplayAttributeData& OldValue);
};
