// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rogue10mMonsterSpawner.generated.h"

class ARogue10mBasicMonster;
class UBoxComponent;
class USceneComponent;

UCLASS(Blueprintable)
class ROGUE10M_API ARogue10mMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	ARogue10mMonsterSpawner();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Spawner")
	ARogue10mBasicMonster* SpawnOneMonster();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Spawner")
	void RefillToMaximum();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Spawner")
	int32 GetAliveMonsterCount() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> SpawnArea;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner")
	TSubclassOf<ARogue10mBasicMonster> MonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner", meta=(ClampMin="0", UIMin="0"))
	int32 MaxAliveMonsters = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner", meta=(ClampMin="0.1", UIMin="0.1", Units="s"))
	float SpawnIntervalSeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner")
	bool bFillToMaximumOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner")
	bool bMaintainPopulation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner|Placement")
	bool bProjectSpawnLocationToGround = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner|Placement", meta=(ClampMin="0.0", UIMin="0.0", Units="cm", EditCondition="bProjectSpawnLocationToGround"))
	float GroundTraceDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Spawner|Placement")
	ESpawnActorCollisionHandlingMethod MonsterSpawnCollisionHandlingMethod =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

private:
	void HandleSpawnTimer();
	void CleanupTrackedMonsters();
	bool FindSpawnTransform(FTransform& OutSpawnTransform) const;

	UFUNCTION()
	void HandleSpawnedMonsterDestroyed(AActor* DestroyedActor);

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ARogue10mBasicMonster>> SpawnedMonsters;

	FTimerHandle SpawnTimerHandle;
};
