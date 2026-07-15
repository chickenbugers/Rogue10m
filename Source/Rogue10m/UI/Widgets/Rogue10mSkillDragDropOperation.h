// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Rogue10mSkillDragDropOperation.generated.h"

class URogue10mAttackSkillData;

UCLASS()
class ROGUE10M_API URogue10mSkillDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Skill Drag Drop")
	TObjectPtr<URogue10mAttackSkillData> SkillData;

	UPROPERTY(BlueprintReadOnly, Category="Rogue10m|Skill Drag Drop")
	bool bUnlocked = false;
};