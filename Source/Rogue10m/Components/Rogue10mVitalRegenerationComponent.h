// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mVitalRegenerationComponent.generated.h"

class URogue10mAttributeSet;

/** Tick 없이 일정 주기로 Health/Stamina/Mana를 회복합니다. */
UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mVitalRegenerationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mVitalRegenerationComponent();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Regeneration")
	void ConfigureRegeneration(float HealthPerSecond, float StaminaPerSecond, float ManaPerSecond);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Regeneration", meta=(ClampMin="0.0"))
	float HealthRegenerationPerSecond = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Regeneration", meta=(ClampMin="0.0"))
	float StaminaRegenerationPerSecond = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Regeneration", meta=(ClampMin="0.0"))
	float ManaRegenerationPerSecond = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Regeneration", meta=(ClampMin="0.05", Units="s"))
	float RegenerationInterval = 1.0f;

private:
	URogue10mAttributeSet* ResolveAttributeSet() const;
	void ApplyRegeneration();

	FTimerHandle RegenerationTimerHandle;
};
