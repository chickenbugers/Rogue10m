// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mGameMode.h"

#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mCharacter.h"
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
		RogueGameState->OnRunPhaseChanged.AddDynamic(this, &ARogue10mGameMode::HandleRunPhaseChanged);

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

void ARogue10mGameMode::HandleRunPhaseChanged(ERogue10mRunPhase NewPhase)
{
	if (NewPhase == ERogue10mRunPhase::Defeat)
	{
		HandleRunDefeat();
	}
}

void ARogue10mGameMode::HandleRunDefeat()
{
	ARogue10mCharacter* RogueCharacter = Cast<ARogue10mCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!RogueCharacter)
	{
		UE_LOG(LogRogue10m, Warning, TEXT("Run ended in defeat, but no Rogue10m character was found to kill."));
		return;
	}

	RogueCharacter->Die();
}
