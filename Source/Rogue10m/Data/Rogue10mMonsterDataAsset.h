// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mMonsterDataAsset.generated.h"

class UAnimInstance;
class USkeletalMesh;
class UTexture2D;
class URogue10mAttackSkillData;

/** 몬스터의 외형, 능력치, 보상과 전투 설정을 정의합니다. */
UCLASS(BlueprintType)
class ROGUE10M_API URogue10mMonsterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Identity")
	FName MonsterId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Identity", meta=(ClampMin="1"))
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Identity")
	bool bIsBoss = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Identity")
	TSoftObjectPtr<UTexture2D> PortraitIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Rewards", meta=(ClampMin="0"))
	int32 ExperienceReward = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Vitals", meta=(ClampMin="1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Vitals", meta=(ClampMin="0.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Vitals", meta=(ClampMin="0.0"))
	float MaxMana = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Regeneration", meta=(ClampMin="0.0"))
	float HealthRegenerationPerSecond = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Regeneration", meta=(ClampMin="0.0"))
	float StaminaRegenerationPerSecond = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Regeneration", meta=(ClampMin="0.0"))
	float ManaRegenerationPerSecond = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Visual")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Visual")
	TSubclassOf<UAnimInstance> AnimationBlueprintClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Visual")
	FVector MeshRelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Visual")
	FRotator MeshRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Visual")
	FVector MeshRelativeScale = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Movement", meta=(ClampMin="0.0"))
	float WalkSpeed = 260.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Movement", meta=(ClampMin="0.0"))
	float DetectionRange = 1800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Movement", meta=(ClampMin="0.0"))
	float StopDistance = 140.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Combat")
	TObjectPtr<URogue10mAttackSkillData> AttackSkill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Monster|Lifecycle")
	bool bDestroyOnDeath = true;
};
