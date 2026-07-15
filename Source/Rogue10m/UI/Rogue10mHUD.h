// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Rogue10mHUD.generated.h"

/**
 * 기존 BP_Rogue10mHUD 에셋의 부모 클래스 호환성을 위한 빈 HUD입니다.
 * 실제 화면과 UI 상태는 PlayerController와 UMG Widget이 관리합니다.
 */
UCLASS()
class ROGUE10M_API ARogue10mHUD : public AHUD
{
	GENERATED_BODY()
};