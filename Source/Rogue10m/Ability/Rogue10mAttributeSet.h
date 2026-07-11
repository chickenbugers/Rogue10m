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

	/** 1.0??湲곕낯 ?ъ깮 ?띾룄?대ŉ 怨듦꺽 紐쏀?二? 肄ㅻ낫 李? 怨듦꺽 荑⑤떎?댁뿉 ?④퍡 ?곸슜?⑸땲?? */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AttackSpeedMultiplier, Category="Rogue10m|Attributes|Combat")
	FGameplayAttributeData AttackSpeedMultiplier;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, AttackSpeedMultiplier);

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
	UFUNCTION() void OnRep_AttackSpeedMultiplier(const FGameplayAttributeData& OldValue);
};