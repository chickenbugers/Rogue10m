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

/**
 * GAS에서 관리할 플레이어 전투/성장 속성 집합입니다.
 * 기존 VitalsComponent와 PlayerState 값은 당분간 유지하고, 신규 스킬은 이 AttributeSet으로 점진 이전합니다.
 */
UCLASS()
class ROGUE10M_API URogue10mAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Health;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxHealth;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Stamina;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxStamina;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData Mana;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Vitals")
	FGameplayAttributeData MaxMana;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Identity")
	FGameplayAttributeData Identity;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Identity);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Identity")
	FGameplayAttributeData MaxIdentity;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, MaxIdentity);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData PlayerLevel;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, PlayerLevel);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData Experience;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, Experience);

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Attributes|Progression")
	FGameplayAttributeData ExperienceToNextLevel;
	ROGUE10M_ATTRIBUTE_ACCESSORS(URogue10mAttributeSet, ExperienceToNextLevel);
};
