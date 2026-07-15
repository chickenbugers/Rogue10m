// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Rogue10mIdentityTypes.generated.h"

// 아이덴티티 UI와 특수 자원 표시를 결정하는 타입입니다.
UENUM(BlueprintType)
enum class ERogue10mIdentityType : uint8
{
	// 아이덴티티를 사용하지 않는 상태입니다.
	None UMETA(DisplayName="없음"),

	// 기본 권/주먹 계열에서 사용하는 돌주먹 아이덴티티입니다.
	StoneFist UMETA(DisplayName="돌주먹"),

	// 스테미나와 별도로 관리되는 전투 기력 계열 자원입니다.
	Vigor UMETA(DisplayName="기력"),

	// 지팡이/마법 계열에서 사용할 마력 자원입니다.
	Mana UMETA(DisplayName="마력"),

	// 활이나 정밀 공격 계열에서 사용할 집중 자원입니다.
	Focus UMETA(DisplayName="집중"),

	// 피격/공격 누적형 무기에서 사용할 분노 자원입니다.
	Rage UMETA(DisplayName="분노")
};
