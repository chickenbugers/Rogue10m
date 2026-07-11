// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Rogue10mAttackTargetInterface.h"
#include "Rogue10mBasicMonster.generated.h"

class ARogue10mCharacter;
class UAbilitySystemComponent;
class URogue10mAbilitySystemComponent;
class URogue10mAttributeSet;

UCLASS(Blueprintable)
class ROGUE10M_API ARogue10mBasicMonster : public ACharacter, public IAbilitySystemInterface, public IRogue10mAttackTargetInterface
{
	GENERATED_BODY()

public:
	ARogue10mBasicMonster();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool CanReceiveRogue10mAttack_Implementation(AActor* AttackSource) const override;
	virtual FVector GetRogue10mDamageIndicatorLocation_Implementation() const override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	URogue10mAttributeSet* GetRogueAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	FText GetMonsterDisplayName() const { return MonsterDisplayName; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	int32 GetMonsterLevel() const { return MonsterLevel; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	FText GetMonsterAttributeText() const { return MonsterAttributeText; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rogue10m|GAS")
	TObjectPtr<URogue10mAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rogue10m|GAS")
	TObjectPtr<URogue10mAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster")
	FText MonsterDisplayName = FText::FromString(TEXT("기본 몬스터"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster", meta=(ClampMin="1"))
	int32 MonsterLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster")
	FText MonsterAttributeText = FText::FromString(TEXT("일반"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Vitals", meta=(ClampMin="1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Movement", meta=(ClampMin="0.0"))
	float DetectionRange = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Movement", meta=(ClampMin="0.0"))
	float StopDistance = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Combat", meta=(ClampMin="0.0"))
	float AttackRange = 170.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Combat", meta=(ClampMin="0.0"))
	float AttackDamage = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Combat", meta=(ClampMin="0.01"))
	float AttackInterval = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster|Combat")
	bool bDestroyOnDeath = true;

private:
	void UpdateTarget();
	void MoveTowardTarget(float DistanceToTarget);
	void TryAttackTarget(float DistanceToTarget);
	void Die();

	TWeakObjectPtr<ARogue10mCharacter> TargetCharacter;
	float LastAttackTime = -1000.0f;
	bool bIsDead = false;
};