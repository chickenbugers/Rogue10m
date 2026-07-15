// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mSkillLoadoutDataAsset.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class ROGUE10M_API URogue10mDodgeSkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge|Identity")
	FName DodgeId = TEXT("DefaultDodge");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge|Display")
	FText DisplayName = FText::FromString(TEXT("회피"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge|Display", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge|Display")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge", meta=(ClampMin="0.0", Units="cm"))
	float Distance = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge", meta=(ClampMin="0.05", Units="s"))
	float Duration = 0.16f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge", meta=(ClampMin="0.0", Units="s"))
	float Cooldown = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge", meta=(ClampMin="0.0"))
	float StaminaCost = 0.0f;
};

UCLASS(BlueprintType)
class ROGUE10M_API URogue10mWeaponSkillProfileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Skill Loadout")
	URogue10mAttackSkillData* FindDefaultSkill(ERogue10mAttackInputSlot InputSlot) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout")
	FName ProfileId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout|Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout|Display", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout|Display")
	TObjectPtr<UTexture2D> IdentityIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout")
	ERogue10mWeaponType WeaponType = ERogue10mWeaponType::Unarmed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Loadout")
	TMap<ERogue10mAttackInputSlot, TObjectPtr<URogue10mAttackSkillData>> DefaultSkillBindings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Tree")
	TArray<TObjectPtr<URogue10mAttackSkillData>> SkillTreeSkills;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Skill Tree")
	TArray<TObjectPtr<URogue10mAttackSkillData>> InitiallyUnlockedSkills;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Dodge")
	TObjectPtr<URogue10mDodgeSkillDataAsset> DefaultDodgeSkill;

	/** 이 프로필을 장착했을 때 허용되는 연속 점프 횟수입니다. 1은 단일 점프입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Traversal", meta=(ClampMin="1", ClampMax="4"))
	int32 MaxJumpCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Traversal|Display")
	FText ExtraJumpDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Traversal|Display", meta=(MultiLine="true"))
	FText ExtraJumpDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Traversal|Display")
	TObjectPtr<UTexture2D> ExtraJumpIcon;
};