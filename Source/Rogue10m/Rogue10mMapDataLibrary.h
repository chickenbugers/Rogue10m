// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Rogue10mMapDataLibrary.generated.h"

UENUM(BlueprintType)
enum class ERogue10mMapRole : uint8
{
	StartHub,
	CombatRun
};

USTRUCT(BlueprintType)
struct FRogue10mNpcMapEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FText Purpose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FVector SuggestedLocation = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FRogue10mMapDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	ERogue10mMapRole Role = ERogue10mMapRole::StartHub;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	FVector2D TargetWorldSizeMeters = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Map")
	TArray<FRogue10mNpcMapEntry> Npcs;
};

UCLASS()
class ROGUE10M_API URogue10mMapDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Rogue10m|Map")
	static TArray<FRogue10mMapDefinition> GetPrototypeMapDefinitions();
};
