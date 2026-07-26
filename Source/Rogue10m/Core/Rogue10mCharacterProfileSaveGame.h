// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Rogue10mCharacterCustomizationTypes.h"
#include "Rogue10mCharacterProfileSaveGame.generated.h"

UCLASS()
class ROGUE10M_API URogue10mCharacterProfileSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	int32 SaveVersion = 1;

	UPROPERTY(SaveGame)
	TArray<FRogue10mCharacterProfile> Profiles;

	UPROPERTY(SaveGame)
	FGuid SelectedProfileId;
};
