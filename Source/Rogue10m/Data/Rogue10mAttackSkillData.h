// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Rogue10mAttackSkillData.generated.h"

class UAnimMontage;
class UGameplayAbility;
class UTexture2D;
class URogue10mAttributeSet;

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

UENUM(BlueprintType)
enum class ERogue10mAttackResourceType : uint8
{
	Health,
	Stamina,
	Mana,
	Energy
};

UENUM(BlueprintType)
enum class ERogue10mAttackShape : uint8
{
	LinearBox UMETA(DisplayName="직선 박스"),
	Projectile UMETA(DisplayName="투사체 경로"),
	Arc UMETA(DisplayName="부채꼴"),
	Circle UMETA(DisplayName="원형")
};

UENUM(BlueprintType)
enum class ERogue10mAttackHitMode : uint8
{
	Single UMETA(DisplayName="단타"),
	Continuous UMETA(DisplayName="연속 공격"),
	MultiHit UMETA(DisplayName="적중 후 다단히트")
};

USTRUCT(BlueprintType)
struct FRogue10mAttackResourceCost
{
	GENERATED_BODY()

	FRogue10mAttackResourceCost() = default;

	FRogue10mAttackResourceCost(ERogue10mAttackResourceType InResourceType, float InCost)
		: ResourceType(InResourceType)
		, Cost(InCost)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Cost")
	ERogue10mAttackResourceType ResourceType = ERogue10mAttackResourceType::Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Cost", meta=(ClampMin="0.0"))
	float Cost = 0.0f;
};

UCLASS(BlueprintType)
class ROGUE10M_API URogue10mAttackSkillData : public UDataAsset
{
	GENERATED_BODY()

public:
	URogue10mAttackSkillData();

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat|Damage")
	float RollDamage(const URogue10mAttributeSet* SourceAttributes, bool& bOutCritical) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill")
	FText SkillName = FText::FromString(TEXT("공격 스킬"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill", meta=(MultiLine="true"))
	FText SkillDescription = FText::FromString(TEXT("공격 스킬 설명을 입력하세요."));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Skill")
	ERogue10mAttackInputSlot InputSlot = ERogue10mAttackInputSlot::Primary;

	/** 이 스킬 Data Asset을 실행할 GAS Ability입니다. 비어 있으면 캐릭터의 기본 공격 Ability를 사용합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|GAS")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0"))
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Shape")
	ERogue10mAttackShape AttackShape = ERogue10mAttackShape::LinearBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Hit Mode")
	ERogue10mAttackHitMode HitMode = ERogue10mAttackHitMode::Single;
	/** 캐릭터 최소 피해 비율에 곱하는 스킬 보정값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Damage", meta=(ClampMin="0.0"))
	float MinDamageRatioMultiplier = 1.0f;

	/** 캐릭터 최대 피해 비율에 곱하는 스킬 보정값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Damage", meta=(ClampMin="0.0"))
	float MaxDamageRatioMultiplier = 1.0f;
	/** 캐릭터 치명타 확률에 더하는 스킬 고유 보정입니다. 0.1은 10%p 증가입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Critical", meta=(ClampMin="-1.0", ClampMax="1.0"))
	float CriticalChanceBonus = 0.0f;

	/** 캐릭터 치명타 피해 배율에 더하는 스킬 고유 보정입니다. 0.2는 150%를 170%로 만듭니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Critical", meta=(ClampMin="-10.0", ClampMax="10.0"))
	float CriticalDamageMultiplierBonus = 0.0f;

	/** 공격 한 번을 구성하는 판정 펄스 수입니다. 1이면 기존 단일 타격과 같습니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Multi Hit", meta=(ClampMin="1", ClampMax="64"))
	int32 HitCount = 1;

	/** 다단히트 판정 펄스 사이의 간격입니다. 공격속도 배율에 따라 짧아집니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Multi Hit", meta=(EditCondition="HitCount > 1", ClampMin="0.01", Units="s"))
	float HitInterval = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Multi Hit", meta=(ClampMin="1", ClampMax="64"))
	int32 MaxTargetsPerHit = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Multi Hit", meta=(ClampMin="1", ClampMax="64"))
	int32 MaxHitsPerTarget = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="1.0", Units="cm"))
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="1.0", Units="cm"))
	float AttackTraceRadius = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Shape", meta=(ClampMin="1.0", Units="cm"))
	float BoxHalfWidth = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Shape", meta=(ClampMin="1.0", Units="cm"))
	float BoxHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Shape", meta=(ClampMin="1.0", ClampMax="180.0", Units="deg"))
	float ArcAngleDegrees = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat|Shape", meta=(ClampMin="0.0", Units="cm"))
	float CircleForwardOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0", Units="s"))
	float AttackCooldown = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.0", Units="s"))
	float ChargeSeconds = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Cost")
	TArray<FRogue10mAttackResourceCost> ResourceCosts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combo")
	bool bEnableCombo = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combo", meta=(EditCondition="bEnableCombo", ClampMin="0.0", Units="s"))
	float ComboWindowOpenSeconds = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combo", meta=(EditCondition="bEnableCombo", ClampMin="0.0", Units="s"))
	float ComboWindowCloseSeconds = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combo", meta=(EditCondition="bEnableCombo"))
	ERogue10mAttackInputSlot ComboInputSlot = ERogue10mAttackInputSlot::Primary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Combo", meta=(EditCondition="bEnableCombo"))
	TObjectPtr<URogue10mAttackSkillData> NextComboSkill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual")
	TObjectPtr<UTexture2D> SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual")
	FText IconLabel = FText::FromString(TEXT("공"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual")
	FLinearColor IconTint = FLinearColor(1.0f, 0.72f, 0.22f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Visual", meta=(AllowedClasses="/Script/Niagara.NiagaraSystem,/Script/Engine.ParticleSystem"))
	TSoftObjectPtr<UObject> AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Debug")
	bool bDrawDebugAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rogue10m|Debug")
	FLinearColor DebugColor = FLinearColor(1.0f, 0.72f, 0.22f, 1.0f);
};
