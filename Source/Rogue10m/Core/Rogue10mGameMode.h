// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Rogue10mGameMode.generated.h"

class ARogue10mGameState;
class APawn;
class APlayerController;
class AHUD;
enum class ERogue10mRunPhase : uint8;

/**
 *  GameMode for the single-player roguelike run.
 */
UCLASS()
class ARogue10mGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARogue10mGameMode();

protected:
	virtual void StartPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Run")
	bool bStartRunOnStartPlay = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Prototype")
	bool bUsePrototypeRunDuration = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Prototype", meta=(EditCondition="bUsePrototypeRunDuration", ClampMin="1.0"))
	float PrototypeRunDurationSeconds = 30.0f;

private:
	UFUNCTION()
	void HandleRunPhaseChanged(ERogue10mRunPhase NewPhase);

	bool ShouldStartRunFromTravelOptions() const;
	void StartConfiguredRun(ARogue10mGameState& RogueGameState) const;
	void HandleRunDefeat();
};



