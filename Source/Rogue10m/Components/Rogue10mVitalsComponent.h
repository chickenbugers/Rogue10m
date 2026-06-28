// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Rogue10mVitalsComponent.generated.h"

USTRUCT(BlueprintType)
struct FRogue10mVitalValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rogue10m|Vitals", meta=(ClampMin="0.0"))
	float Current = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rogue10m|Vitals", meta=(ClampMin="1.0"))
	float Max = 100.0f;

	float GetNormalized() const
	{
		return Max > 0.0f ? FMath::Clamp(Current / Max, 0.0f, 1.0f) : 0.0f;
	}
};

UCLASS(ClassGroup=(Rogue10m), meta=(BlueprintSpawnableComponent))
class ROGUE10M_API URogue10mVitalsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URogue10mVitalsComponent();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Vitals")
	const FRogue10mVitalValue& GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Vitals")
	const FRogue10mVitalValue& GetMana() const { return Mana; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Vitals")
	const FRogue10mVitalValue& GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintPure, Category="Rogue10m|Vitals")
	bool ShouldShowMana() const { return bShowMana; }

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Vitals")
	void SetShowMana(bool bNewShowMana);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Vitals")
	void SetHealth(float NewCurrent);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Vitals")
	void SetMana(float NewCurrent);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Vitals")
	void SetStamina(float NewCurrent);

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Vitals")
	void RestoreAll();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Vitals")
	FRogue10mVitalValue Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Vitals")
	FRogue10mVitalValue Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Vitals")
	FRogue10mVitalValue Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Vitals")
	bool bShowMana = false;

private:
	static void SetVitalCurrent(FRogue10mVitalValue& Vital, float NewCurrent);
};
