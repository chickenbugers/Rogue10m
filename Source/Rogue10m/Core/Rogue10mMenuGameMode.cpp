// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mMenuGameMode.h"

#include "Rogue10mMenuPlayerController.h"

ARogue10mMenuGameMode::ARogue10mMenuGameMode()
{
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	SpectatorClass = nullptr;
	bStartPlayersAsSpectators = true;

	PlayerControllerClass = ARogue10mMenuPlayerController::StaticClass();
}