// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mGameMode.h"

#include "Rogue10mGameState.h"

ARogue10mGameMode::ARogue10mGameMode()
{
	GameStateClass = ARogue10mGameState::StaticClass();
}

void ARogue10mGameMode::StartPlay()
{
	Super::StartPlay();

	if (ARogue10mGameState* RogueGameState = GetGameState<ARogue10mGameState>())
	{
		RogueGameState->StartRun();
	}
}
