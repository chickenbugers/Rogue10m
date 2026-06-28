// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Rogue10mCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class URogue10mInventoryComponent;
class URogue10mVitalsComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class ERogue10mWeaponType : uint8
{
	Unarmed,
	Sword,
	Staff,
	Bow
};

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

	/** Current weapon type. The character starts unarmed. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|Combat")
	ERogue10mWeaponType EquippedWeaponType = ERogue10mWeaponType::Unarmed;

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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Rogue10m|State")
	bool bIsDead = false;

	float LastAttackTime = -1000.0f;
	
public:
	ARogue10mCharacter();

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
	virtual void DoPrimaryAttack();

	/** Performs the default unarmed attack. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Combat")
	virtual void DoUnarmedAttack();

	/** Toggles the prototype inventory panel. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleInventory();

	/** Toggles the prototype item window. */
	UFUNCTION(BlueprintCallable, Category="Rogue10m|Inventory")
	virtual void DoToggleItemWindow();

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

	/** Returns whether the run failure has killed this character. **/
	UFUNCTION(BlueprintPure, Category="Rogue10m|State")
	bool IsDead() const { return bIsDead; }

};

