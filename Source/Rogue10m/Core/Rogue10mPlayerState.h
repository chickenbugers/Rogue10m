// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mPlayerState.generated.h"

UCLASS()
class ROGUE10M_API ARogue10mPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ARogue10mPlayerState();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterDisplayName() const { return CharacterDisplayName; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterJobName() const { return CharacterJobName; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat")
	ERogue10mWeaponType GetEquippedWeaponType() const { return EquippedWeaponType; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType);

	UFUNCTION(BlueprintPure, Category="Rogue10m|State")
	bool IsCharacterDead() const { return bCharacterDead; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|State")
	void SetCharacterDead(bool bNewCharacterDead);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Progression")
	int32 GetPlayerLevel() const { return PlayerLevel; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Progression")
	int32 GetCurrentExperience() const { return CurrentExperience; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Progression")
	int32 GetExperienceToNextLevel() const { return ExperienceToNextLevel; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Progression")
	float GetExperienceNormalized() const;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Progression")
	void AddExperience(int32 ExperienceAmount);

	UFUNCTION(BlueprintPure, Category="Rogue10m|Identity")
	int32 GetWeaponMasteryLevel(ERogue10mWeaponType WeaponType) const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Identity")
	float GetIdentityCurrent() const { return IdentityCurrent; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Identity")
	float GetIdentityMax() const { return IdentityMax; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Identity")
	float GetIdentityNormalized() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character", meta=(AllowPrivateAccess="true"))
	FText CharacterDisplayName = FText::FromString(TEXT("Rogue"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character", meta=(AllowPrivateAccess="true"))
	FText CharacterJobName = FText::FromString(TEXT("Unassigned"));

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(AllowPrivateAccess="true"))
	ERogue10mWeaponType EquippedWeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|State", meta=(AllowPrivateAccess="true"))
	bool bCharacterDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Progression", meta=(AllowPrivateAccess="true", ClampMin="1"))
	int32 PlayerLevel = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Progression", meta=(AllowPrivateAccess="true", ClampMin="0"))
	int32 CurrentExperience = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Progression", meta=(AllowPrivateAccess="true", ClampMin="1"))
	int32 ExperienceToNextLevel = 100;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Identity", meta=(AllowPrivateAccess="true", ClampMin="0"))
	float IdentityCurrent = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Identity", meta=(AllowPrivateAccess="true", ClampMin="1"))
	float IdentityMax = 100.0f;
};
