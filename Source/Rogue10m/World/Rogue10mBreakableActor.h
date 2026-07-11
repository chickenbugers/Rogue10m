// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rogue10mAttackTargetInterface.h"
#include "Rogue10mBreakableActor.generated.h"

class UStaticMeshComponent;

/** 벽, 상자, 기둥 등 공격으로 파괴되는 오브젝트의 C++ 기본 클래스입니다. */
UCLASS(Blueprintable)
class ROGUE10M_API ARogue10mBreakableActor : public AActor, public IRogue10mAttackTargetInterface
{
	GENERATED_BODY()

public:
	ARogue10mBreakableActor();

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanReceiveRogue10mAttack_Implementation(AActor* AttackSource) const override;
	virtual FVector GetRogue10mDamageIndicatorLocation_Implementation() const override;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Breakable")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Breakable")
	bool IsBroken() const { return bBroken; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|Breakable", meta=(DisplayName="On Broken"))
	void BP_OnBroken(AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Breakable", meta=(ClampMin="1.0"))
	float MaxHealth = 50.0f;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Breakable")
	float CurrentHealth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Breakable")
	bool bHideWhenBroken = true;

private:
	void Break(AActor* DamageCauser);
	void ApplyBrokenState();

	UFUNCTION()
	void OnRep_Broken();

	UPROPERTY(ReplicatedUsing=OnRep_Broken)
	bool bBroken = false;
};
