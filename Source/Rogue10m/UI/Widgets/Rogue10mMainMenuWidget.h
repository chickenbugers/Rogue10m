// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Rogue10mMainMenuWidget.generated.h"

class UButton;

UCLASS(Abstract, Blueprintable)
class ROGUE10M_API URogue10mMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Main Menu")
	TObjectPtr<UButton> UI_StartGameButton;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Rogue10m|Main Menu")
	TObjectPtr<UButton> UI_QuitGameButton;

private:
	UFUNCTION()
	void HandleStartGameClicked();

	UFUNCTION()
	void HandleQuitGameClicked();
};