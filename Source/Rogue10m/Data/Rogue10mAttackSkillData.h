// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mAttackSkillData.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class ERogue10mAttackInputSlot : uint8
{
	Primary,
	Special,
	JumpPrimary,
	JumpSpecial,
	ChargedPrimary,
	ChargedSpecial
};

UCLASS(BlueprintType)
class ROGUE10M_API URogue10mAttackSkillData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill")
	FText SkillName = FText::FromString(TEXT("공격 스킬"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill", meta=(MultiLine="true"))
	FText SkillDescription = FText::FromString(TEXT("공격 스킬 설명을 입력하세요."));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill")
	ERogue10mAttackInputSlot InputSlot = ERogue10mAttackInputSlot::Primary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0"))
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="1.0", Units="cm"))
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="1.0", Units="cm"))
	float AttackTraceRadius = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0", Units="s"))
	float AttackCooldown = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0", Units="s"))
	float ChargeSeconds = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual", meta=(AllowedClasses="/Script/Niagara.NiagaraSystem,/Script/Engine.ParticleSystem"))
	TSoftObjectPtr<UObject> AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Debug")
	bool bDrawDebugAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Debug")
	FLinearColor DebugColor = FLinearColor(1.0f, 0.72f, 0.22f, 1.0f);
};
