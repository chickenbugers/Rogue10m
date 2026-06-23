// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mGameState.h"
#include "Rogue10mRunHUD.generated.h"

/**
 * Blueprint-facing HUD base for run timer and run result UI.
 */
UCLASS(abstract)
class ROGUE10M_API URogue10mRunHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Update Run Timer"))
	void BP_UpdateRunTimer(float RemainingSeconds, float TotalSeconds, float ProgressAlpha);

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|HUD", meta=(DisplayName="Run Phase Changed"))
	void BP_OnRunPhaseChanged(ERogue10mRunPhase NewPhase);
};
