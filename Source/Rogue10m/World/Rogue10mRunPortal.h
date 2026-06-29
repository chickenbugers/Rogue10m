// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rogue10mRunPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class ROGUE10M_API ARogue10mRunPortal : public AActor
{
	GENERATED_BODY()

public:
	ARogue10mRunPortal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> PortalCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> PortalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Portal")
	FName TargetLevelName = FName(TEXT("OpenRunMap"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Portal")
	bool bStartRunTimerOnTravel = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Portal")
	FString AdditionalTravelOptions;

private:
	UFUNCTION()
	void HandlePortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FString BuildTravelOptions() const;
	bool bTravelStarted = false;
};
