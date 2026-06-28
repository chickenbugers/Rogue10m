// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rogue10mBasicMonster.generated.h"

class ARogue10mCharacter;
class URogue10mVitalsComponent;

UCLASS(Blueprintable)
class ROGUE10M_API ARogue10mBasicMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ARogue10mBasicMonster();

	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	URogue10mVitalsComponent* GetVitalsComponent() const { return VitalsComponent; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	FText GetMonsterDisplayName() const { return MonsterDisplayName; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Monster")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<URogue10mVitalsComponent> VitalsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Monster")
	FText MonsterDisplayName = FText::FromString(TEXT("Basic Monster"));

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

	UPROPERTY(Transient)
	TWeakObjectPtr<ARogue10mCharacter> TargetCharacter;

	float LastAttackTime = -1000.0f;
	bool bIsDead = false;
};
