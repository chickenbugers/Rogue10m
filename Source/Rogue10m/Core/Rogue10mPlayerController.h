// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/SoftObjectPtr.h"
#include "Rogue10mPlayerController.generated.h"

class UInputMappingContext;
class URogue10mRunHUD;
class UUserWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config="Game")
class ROGUE10M_API ARogue10mPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	ARogue10mPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Run HUD widget to spawn for the local player. */
	UPROPERTY(EditAnywhere, Category="Rogue10m|HUD")
	TSubclassOf<URogue10mRunHUD> RunHUDClass;

	/** RunHUDClass가 비어 있을 때 Content/Widget의 기본 메인 HUD Widget Blueprint를 로드한다. */
	UPROPERTY(EditAnywhere, Category="Rogue10m|HUD")
	TSoftClassPtr<URogue10mRunHUD> DefaultRunHUDClass;

	/** Widget HUD가 생성되면 Canvas 기반 프로토타입 HUD를 자동으로 숨겨 UI 배치 겹침을 막는다. */
	UPROPERTY(EditAnywhere, Category="Rogue10m|HUD")
	bool bDisableCanvasPrototypeHUDWhenRunHUDIsCreated = true;

	UPROPERTY()
	TObjectPtr<URogue10mRunHUD> RunHUD;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	void InitializeRunHUD();

	TSubclassOf<URogue10mRunHUD> ResolveRunHUDClass();
};
