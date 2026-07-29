// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Rogue10mPlayerController.h"
#include "Rogue10mMenuPlayerController.generated.h"

/** Concrete UI-only controller used by the front-end menu map. */
UCLASS()
class ROGUE10M_API ARogue10mMenuPlayerController final
	: public ARogue10mPlayerController
{
	GENERATED_BODY()
};