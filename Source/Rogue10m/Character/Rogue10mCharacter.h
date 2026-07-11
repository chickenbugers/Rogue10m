// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Rogue10mWeaponTypes.h"
#include "Rogue10mCharacter.generated.h"

class UAbilitySystemComponent;
class UCameraComponent;
class UInputAction;
class URogue10mAttackSkillData;
class URogue10mAttributeSet;
class URogue10mCombatComponent;
class URogue10mInventoryComponent;
class URogue10mPlayerFeedbackComponent;
class USkeletalMeshComponent;
struct FInputActionValue;

/** 1인칭 이동과 입력 전달, 캐릭터 생명주기를 담당합니다. */
UCLASS(abstract)
class ROGUE10M_API ARogue10mCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARogue10mCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|State")
	virtual void Die();

	UFUNCTION(BlueprintImplementableEvent, Category="Rogue10m|State", meta=(DisplayName="On Run Death"))
	void BP_OnRunDeath();

	UFUNCTION(BlueprintPure, Category="Rogue10m|State")
	bool IsDead() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterDisplayName() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Character")
	FText GetCharacterJobName() const;

	UFUNCTION(BlueprintPure, Category="Rogue10m|Combat")
	ERogue10mWeaponType GetEquippedWeaponType() const;

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	void SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType);

	UFUNCTION(BlueprintPure, Category="Rogue10m|GAS")
	URogue10mAttributeSet* GetRogueAttributeSet() const;

	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	URogue10mInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	URogue10mCombatComponent* GetCombatComponent() const { return CombatComponent; }
	URogue10mPlayerFeedbackComponent* GetPlayerFeedbackComponent() const { return PlayerFeedbackComponent; }

	const URogue10mAttackSkillData* GetDisplayedAttackSkillForHUD() const;
	float GetAttackCooldownRemaining() const;
	float GetAttackCooldownDuration() const;
	bool ExecutePendingAttackSkillFromAbility();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoPrimaryAttackPressed();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoPrimaryAttackReleased();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoSpecialAttackPressed();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoSpecialAttackReleased();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoUnarmedAttack();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleInventory();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleItemWindow();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Skill Tree")
	virtual void DoToggleSkillTree();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Settings")
	virtual void DoToggleSettings();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Debug")
	virtual void DoToggleCombatLog();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot1();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot2();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot3();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot4();

	UFUNCTION(BlueprintCallable, Category="Rogue10m|Quick Slot")
	virtual void DoQuickSlot5();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<URogue10mInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<URogue10mCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<URogue10mPlayerFeedbackComponent> PlayerFeedbackComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MouseLookAction;

private:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	bool IsBlockingWindowVisible() const;
	void ActivateQuickSlot(int32 SlotNumber);
	void InitializeAbilityActorInfo();
};