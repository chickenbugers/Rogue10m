// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mCharacterStats.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mCharacterDataAsset.generated.h"

class URogue10mWeaponSkillProfileDataAsset;

/** GDD의 Character 항목을 데이터 주도 방식으로 구성하는 기본 캐릭터 정의입니다. */
UCLASS(BlueprintType)
class ROGUE10M_API URogue10mCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character|Stats")
	FRogue10mCharacterBaseStats GetBaseStats() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Identity")
	FName CharacterId = TEXT("DefaultCharacter");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Identity")
	FText DisplayName = FText::FromString(TEXT("기본 캐릭터"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Vitals", meta=(ClampMin="1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Vitals", meta=(ClampMin="0.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Vitals", meta=(ClampMin="0.0"))
	float MaxMana = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat", meta=(ClampMin="0.0"))
	float AttackPower = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat", meta=(ClampMin="0.0"))
	float Defense = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat",
		meta=(ClampMin="0.0", ClampMax="1.0"))
	float CriticalChance = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat", meta=(ClampMin="1.0"))
	float CriticalDamageMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat",
		meta=(ClampMin="0.1", ClampMax="5.0"))
	float AttackSpeedMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float WalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float SprintSpeed = 900.0f;

	/** 캐릭터가 사용할 수 있는 무기별 공격/회피 프로필입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat")
	ERogue10mWeaponType DefaultWeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character|Combat")
	TArray<TObjectPtr<URogue10mWeaponSkillProfileDataAsset>> WeaponSkillProfiles;
};