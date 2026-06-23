// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Rogue10mGameMode.generated.h"

class ARogue10mGameState;

/**
 *  GameMode for the single-player roguelike run.
 */
UCLASS(abstract)
class ARogue10mGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARogue10mGameMode();

protected:
	virtual void StartPlay() override;
};



