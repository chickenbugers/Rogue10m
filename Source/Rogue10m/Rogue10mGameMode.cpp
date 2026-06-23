// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mGameMode.h"

#include "GameFramework/HUD.h"
#include "Rogue10mGameState.h"
#include "Rogue10mHUD.h"
#include "UObject/ConstructorHelpers.h"

ARogue10mGameMode::ARogue10mGameMode()
{
	GameStateClass = ARogue10mGameState::StaticClass();
	HUDClass = ARogue10mHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> FirstPersonCharacterClass(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	if (FirstPersonCharacterClass.Succeeded())
	{
		DefaultPawnClass = FirstPersonCharacterClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> FirstPersonPlayerControllerClass(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"));
	if (FirstPersonPlayerControllerClass.Succeeded())
	{
		PlayerControllerClass = FirstPersonPlayerControllerClass.Class;
	}
}

void ARogue10mGameMode::StartPlay()
{
	Super::StartPlay();

	if (ARogue10mGameState* RogueGameState = GetGameState<ARogue10mGameState>())
	{
		if (bUsePrototypeRunDuration)
		{
			RogueGameState->StartRunWithDuration(PrototypeRunDurationSeconds);
		}
		else
		{
			RogueGameState->StartRun();
		}
	}
}
