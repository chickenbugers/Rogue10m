// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Rogue10mAbilitySystemComponent.generated.h"

/**
 * Rogue10m 전용 AbilitySystemComponent입니다.
 * 현재는 PlayerState에 붙여 장기 상태와 스킬 권한을 관리하고,
 * 이후 입력 바인딩, 태그, 공통 디버그 기능을 이 클래스에 모읍니다.
 */
UCLASS()
class ROGUE10M_API URogue10mAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
};
