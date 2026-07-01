// Copyright Epic Games, Inc. All Rights Reserved.


#include "Rogue10mPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Rogue10mCameraManager.h"
#include "Rogue10mGameState.h"
#include "Rogue10mHUD.h"
#include "Rogue10mRunHUD.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10m.h"
#include "Widgets/Input/SVirtualJoystick.h"

ARogue10mPlayerController::ARogue10mPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ARogue10mCameraManager::StaticClass();

	// Content/Widget/UW_Rogue10mMainWidget을 기본 HUD Widget Blueprint로 사용한다.
	DefaultRunHUDClass = TSoftClassPtr<URogue10mRunHUD>(FSoftClassPath(TEXT("/Game/Widget/UW_Rogue10mMainWidget.UW_Rogue10mMainWidget_C")));
}

void ARogue10mPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogRogue10m, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	InitializeRunHUD();
}

void ARogue10mPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool ARogue10mPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ARogue10mPlayerController::InitializeRunHUD()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	TSubclassOf<URogue10mRunHUD> ResolvedRunHUDClass = ResolveRunHUDClass();
	if (!ResolvedRunHUDClass)
	{
		return;
	}

	RunHUD = CreateWidget<URogue10mRunHUD>(this, ResolvedRunHUDClass);
	if (!RunHUD)
	{
		UE_LOG(LogRogue10m, Error, TEXT("Could not spawn run HUD widget."));
		return;
	}

	RunHUD->AddToPlayerScreen(0);

	if (bDisableCanvasPrototypeHUDWhenRunHUDIsCreated)
	{
		if (ARogue10mHUD* RogueHUD = GetHUD<ARogue10mHUD>())
		{
			RogueHUD->SetDrawCanvasPrototypeHUD(false);
		}
	}

	if (ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr)
	{
		RogueGameState->OnRunTimerUpdated.AddDynamic(RunHUD, &URogue10mRunHUD::BP_UpdateRunTimer);
		RogueGameState->OnRunPhaseChanged.AddDynamic(RunHUD, &URogue10mRunHUD::BP_OnRunPhaseChanged);
		RunHUD->BP_UpdateRunTimer(
			RogueGameState->GetRemainingRunSeconds(),
			RogueGameState->GetRunDurationSeconds(),
			RogueGameState->GetRunProgressAlpha());
	}
}

TSubclassOf<URogue10mRunHUD> ARogue10mPlayerController::ResolveRunHUDClass()
{
	if (RunHUDClass)
	{
		return RunHUDClass;
	}

	UClass* LoadedRunHUDClass = DefaultRunHUDClass.LoadSynchronous();
	if (!LoadedRunHUDClass)
	{
		UE_LOG(LogRogue10m, Warning, TEXT("RunHUDClass is empty and the default run HUD widget could not be loaded."));
		return nullptr;
	}

	if (!LoadedRunHUDClass->IsChildOf(URogue10mRunHUD::StaticClass()))
	{
		UE_LOG(LogRogue10m, Warning, TEXT("Default run HUD widget '%s' must inherit from Rogue10mRunHUD or Rogue10mMainHUDWidget."), *LoadedRunHUDClass->GetName());
		return nullptr;
	}

	return LoadedRunHUDClass;
}
