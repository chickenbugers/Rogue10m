// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Rogue10mMenuGameMode.generated.h"

/**
 * UI-only GameMode used by the front-end menu map.
 * Character spawning and gameplay HUD initialization are deferred until travel.
 */
UCLASS()
class ROGUE10M_API ARogue10mMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARogue10mMenuGameMode();
};