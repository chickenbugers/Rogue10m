// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class URogue10mCombatComponent;
class URogue10mInventoryComponent;
class URogue10mVitalsComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ARogue10mCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Inventory and equipment data for the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URogue10mInventoryComponent> InventoryComponent;

	/** Health, mana, and stamina data for the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URogue10mVitalsComponent> VitalsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URogue10mCombatComponent> CombatComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	/** Damage applied by the default unarmed attack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Unarmed", meta=(ClampMin="0.0"))
	float UnarmedDamage = 5.0f;

	/** Forward reach of the default unarmed attack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Unarmed", meta=(ClampMin="1.0"))
	float UnarmedRange = 180.0f;

	/** Trace radius for the default unarmed attack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Unarmed", meta=(ClampMin="1.0"))
	float UnarmedTraceRadius = 24.0f;

	/** Minimum time between unarmed attacks. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Unarmed", meta=(ClampMin="0.01"))
	float UnarmedAttackInterval = 0.55f;

	/** 마우스 버튼을 이 시간 이상 누르고 떼면 차징 공격으로 판정합니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat", meta=(ClampMin="0.05"))
	float ChargeAttackThreshold = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rogue10m|Combat|Debug")
	bool bDrawAttackDebug = true;

	float LastAttackTime = -1000.0f;
	
public:
	ARogue10mCharacter();
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Handles primary attack inputs from controls. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoPrimaryAttackPressed();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoPrimaryAttackReleased();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoSpecialAttackPressed();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoSpecialAttackReleased();

	/** Performs the default unarmed attack. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoUnarmedAttack();

	/** Toggles the prototype inventory panel. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleInventory();

	/** Toggles the prototype item window. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleItemWindow();

	/** K 키로 스킬트리 창을 열고 닫습니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	virtual void DoToggleSkillTree();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	virtual void DoToggleSettings();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Debug")
	virtual void DoToggleCombatLog();

	/** 1번 퀵 슬롯을 사용합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot1();

	/** 2번 퀵 슬롯을 사용합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot2();

	/** 3번 퀵 슬롯을 사용합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot3();

	/** 4번 퀵 슬롯을 사용합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot4();

	/** 5번 퀵 슬롯을 사용합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot5();

public:

	/** Kills the character and locks player actions for the ended run. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|State")
	virtual void Die();

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|State", meta=(DisplayName="On Run Death"))
	void BP_OnRunDeath();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns inventory component **/
	URogue10mInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	/** Returns vitals component **/
	URogue10mVitalsComponent* GetVitalsComponent() const { return VitalsComponent; }

	URogue10mCombatComponent* GetCombatComponent() const { return CombatComponent; }

	/** Returns whether the run failure has killed this character. **/
	UFUNCTION(BlueprintPure, Category="Rogue10m|State")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterJobName() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat")
	ERogue10mWeaponType GetEquippedWeaponType() const;

	/** 장비 아이템 장착 결과를 캐릭터의 현재 무기 타입에 반영합니다. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType);

	const URogue10mAttackSkillData* GetDisplayedAttackSkillForHUD() const;
	float GetAttackCooldownRemaining() const;
	float GetAttackCooldownDuration() const;

private:
	// 인벤토리/아이템 창이 열려 있어 캐릭터 이동을 막아야 하는지 확인합니다.
	bool IsInventoryWindowBlockingMovement() const;

	bool CanUseCombatInput() const;
	void BeginCombatAttack(bool bPrimaryAttack);
	void EndCombatAttack(bool bPrimaryAttack);
	void ExecuteCombatAttack(bool bPrimaryAttack, bool bChargedAttack);
	bool ExecuteAttackSkill(const URogue10mAttackSkillData& SkillData, bool bIgnoreCooldown = false);
	void DrawAttackDebug(const FVector& TraceStart, const FVector& TraceEnd, float TraceRadius, const FLinearColor& DebugColor, bool bHit, const FHitResult& HitResult) const;
	const URogue10mAttackSkillData* ResolveAttackSkill(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveChargedAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	const URogue10mAttackSkillData* ResolveComboAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const;
	bool IsAttackSkillUnlockedBySkillTree(const URogue10mAttackSkillData& SkillData) const;
	ERogue10mAttackInputSlot GetAttackInputSlot(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;
	bool CanPayAttackResourceCosts(const URogue10mAttackSkillData& SkillData) const;
	void ConsumeAttackResourceCosts(const URogue10mAttackSkillData& SkillData);
	void StartSharedAttackCooldown(const URogue10mAttackSkillData& SkillData, bool bComboAttack);
	void OpenComboWindow(const URogue10mAttackSkillData& SkillData);
	void ResetComboWindow();
	void AddCombatScreenLog(const FString& Message, const FLinearColor& Color = FLinearColor::White) const;
	FString GetCombatActionText(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const;
	FString GetAttackInputText(bool bPrimaryAttack, bool bJumpAttack) const;

	// 숫자 키 입력을 HUD 퀵 슬롯 활성화로 전달합니다.
	bool ActivateQuickSlot(int32 SlotNumber);

};

