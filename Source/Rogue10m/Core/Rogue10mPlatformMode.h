// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Rogue10mPlatformMode.generated.h"

UENUM(BlueprintType)
enum class ERogue10mPlatformMode : uint8
{
	WindowsDesktop,
	AndroidMobile,
	Unknown
};

UCLASS()
class ROGUE10M_API URogue10mPlatformModeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Rogue10m|Platform")
	static ERogue10mPlatformMode GetCurrentPlatformMode();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Platform")
	static bool IsDesktopMode();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Platform")
	static bool IsMobileMode();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Platform")
	static FText GetCurrentPlatformModeText();
};
