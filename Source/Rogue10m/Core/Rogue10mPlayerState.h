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

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character", meta=(AllowPrivateAccess="true"))
	FText CharacterDisplayName = FText::FromString(TEXT("Rogue"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Character", meta=(AllowPrivateAccess="true"))
	FText CharacterJobName = FText::FromString(TEXT("Unassigned"));

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(AllowPrivateAccess="true"))
	ERogue10mWeaponType EquippedWeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|State", meta=(AllowPrivateAccess="true"))
	bool bCharacterDead = false;
};
