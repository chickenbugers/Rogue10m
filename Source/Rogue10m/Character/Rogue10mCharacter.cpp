// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Rogue10m.h"
#include "Rogue10mAttributeSet.h"
#include "Rogue10mCombatComponent.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mPlayerController.h"
#include "Rogue10mPlayerFeedbackComponent.h"
#include "Rogue10mPlayerState.h"

ARogue10mCharacter::ARogue10mCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	InventoryComponent = CreateDefaultSubobject<URogue10mInventoryComponent>(TEXT("Inventory Component"));
	CombatComponent = CreateDefaultSubobject<URogue10mCombatComponent>(TEXT("Combat Component"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName(TEXT("head")));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(
		FVector(-2.8f, 5.89f, 0.0f),
		FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	PlayerFeedbackComponent = CreateDefaultSubobject<URogue10mPlayerFeedbackComponent>(TEXT("Player Feedback Component"));

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

UAbilitySystemComponent* ARogue10mCharacter::GetAbilitySystemComponent() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State ? State->GetAbilitySystemComponent() : nullptr;
}

void ARogue10mCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilityActorInfo();
	if (CombatComponent)
	{
		CombatComponent->InitializeAbilitySystem();
	}
	if (PlayerFeedbackComponent)
	{
		PlayerFeedbackComponent->InitializeAbilitySystem();
	}
}

void ARogue10mCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeAbilityActorInfo();
	if (PlayerFeedbackComponent)
	{
		PlayerFeedbackComponent->InitializeAbilitySystem();
	}
}

float ARogue10mCharacter::TakeDamage(
	float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	URogue10mAttributeSet* Attributes = GetRogueAttributeSet();
	if (IsDead() || !Attributes || AppliedDamage <= 0.0f)
	{
		return AppliedDamage;
	}

	Attributes->SetHealth(Attributes->GetHealth() - AppliedDamage);
	if (PlayerFeedbackComponent)
	{
		PlayerFeedbackComponent->NotifyDamage(AppliedDamage);
	}
	UE_LOG(
		LogRogue10m, Log, TEXT("%s 피해 %.1f, 체력 %.1f / %.1f"),
		*GetNameSafe(this), AppliedDamage, Attributes->GetHealth(), Attributes->GetMaxHealth());

	if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
	{
		RogueController->NotifyPlayerDamaged(AppliedDamage);
		RogueController->AddCombatLogMessage(
			FString::Printf(TEXT("플레이어 피해 %.0f"), AppliedDamage),
			FLinearColor(1.0f, 0.42f, 0.36f, 1.0f));
	}

	if (Attributes->GetHealth() <= 0.0f)
	{
		Die();
	}
	return AppliedDamage;
}

void ARogue10mCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ARogue10mCharacter::DoJumpStart);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARogue10mCharacter::DoJumpEnd);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::MoveInput);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::LookInput);
		EnhancedInput->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogRogue10m, Error, TEXT("Enhanced Input Component를 찾지 못했습니다."));
	}

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARogue10mCharacter::DoPrimaryAttackPressed);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ARogue10mCharacter::DoPrimaryAttackReleased);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ARogue10mCharacter::DoSpecialAttackPressed);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ARogue10mCharacter::DoSpecialAttackReleased);

	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot1);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot2);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot3);
	PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot4);
	PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot5);
}

void ARogue10mCharacter::MoveInput(const FInputActionValue& Value)
{
	const FVector2D Movement = Value.Get<FVector2D>();
	DoMove(Movement.X, Movement.Y);
}

void ARogue10mCharacter::LookInput(const FInputActionValue& Value)
{
	const FVector2D Look = Value.Get<FVector2D>();
	DoAim(Look.X, Look.Y);
}

void ARogue10mCharacter::DoAim(float Yaw, float Pitch)
{
	if (IsDead() || IsBlockingWindowVisible() || !GetController())
	{
		return;
	}

	const ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController());
	const float SensitivityX = RogueController ? RogueController->GetLookSensitivityX() : 1.0f;
	const float SensitivityY = RogueController ? RogueController->GetLookSensitivityY() : 1.0f;
	AddControllerYawInput(Yaw * SensitivityX);
	AddControllerPitchInput(Pitch * SensitivityY);
}

void ARogue10mCharacter::DoMove(float Right, float Forward)
{
	if (IsBlockingWindowVisible())
	{
		GetCharacterMovement()->StopMovementImmediately();
		return;
	}
	if (!IsDead() && GetController())
	{
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ARogue10mCharacter::DoJumpStart()
{
	if (!IsDead() && !IsBlockingWindowVisible())
	{
		Jump();
	}
}

void ARogue10mCharacter::DoJumpEnd()
{
	if (!IsDead() && !IsBlockingWindowVisible())
	{
		StopJumping();
	}
}

void ARogue10mCharacter::DoPrimaryAttackPressed()
{
	if (CombatComponent)
	{
		CombatComponent->HandleAttackPressed(true);
	}
}

void ARogue10mCharacter::DoPrimaryAttackReleased()
{
	if (CombatComponent)
	{
		CombatComponent->HandleAttackReleased(true);
	}
}

void ARogue10mCharacter::DoSpecialAttackPressed()
{
	if (CombatComponent)
	{
		CombatComponent->HandleAttackPressed(false);
	}
}

void ARogue10mCharacter::DoSpecialAttackReleased()
{
	if (CombatComponent)
	{
		CombatComponent->HandleAttackReleased(false);
	}
}

void ARogue10mCharacter::DoUnarmedAttack()
{
	if (CombatComponent)
	{
		CombatComponent->ActivateQuickSlot(1);
	}
}

void ARogue10mCharacter::DoToggleInventory()
{
	if (!IsDead())
	{
		if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
		{
			RogueController->ToggleInventory();
		}
	}
}

void ARogue10mCharacter::DoToggleItemWindow()
{
	if (!IsDead())
	{
		if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
		{
			RogueController->ToggleItemWindow();
		}
	}
}

void ARogue10mCharacter::DoToggleSkillTree()
{
	if (!IsDead())
	{
		if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
		{
			RogueController->ToggleSkillTree();
		}
	}
}

void ARogue10mCharacter::DoToggleSettings()
{
	if (!IsDead())
	{
		if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
		{
			RogueController->ToggleSettings();
		}
	}
}

void ARogue10mCharacter::DoToggleCombatLog()
{
	if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
	{
		const bool bVisible = RogueController->ToggleCombatLogVisible();
		if (bVisible)
		{
			RogueController->AddCombatLogMessage(TEXT("전투 로그 표시 켜짐"), FLinearColor(0.58f, 0.85f, 1.0f, 1.0f));
		}
	}
}

void ARogue10mCharacter::DoQuickSlot1() { ActivateQuickSlot(1); }
void ARogue10mCharacter::DoQuickSlot2() { ActivateQuickSlot(2); }
void ARogue10mCharacter::DoQuickSlot3() { ActivateQuickSlot(3); }
void ARogue10mCharacter::DoQuickSlot4() { ActivateQuickSlot(4); }
void ARogue10mCharacter::DoQuickSlot5() { ActivateQuickSlot(5); }

void ARogue10mCharacter::Die()
{
	if (IsDead())
	{
		return;
	}

	if (ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>())
	{
		State->SetCharacterDead(true);
	}
	if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
	{
		RogueController->CloseAllBlockingPanels();
		DisableInput(RogueController);
	}
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	BP_OnRunDeath();
	UE_LOG(LogRogue10m, Log, TEXT("%s 사망"), *GetNameSafe(this));
}

bool ARogue10mCharacter::IsDead() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State && State->IsCharacterDead();
}

FText ARogue10mCharacter::GetCharacterDisplayName() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State ? State->GetCharacterDisplayName() : FText::FromString(TEXT("로그"));
}

FText ARogue10mCharacter::GetCharacterJobName() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State ? State->GetCharacterJobName() : FText::FromString(TEXT("미정"));
}

ERogue10mWeaponType ARogue10mCharacter::GetEquippedWeaponType() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State ? State->GetEquippedWeaponType() : ERogue10mWeaponType::Unarmed;
}

void ARogue10mCharacter::SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType)
{
	if (ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>())
	{
		State->SetEquippedWeaponType(NewWeaponType);
	}
}

URogue10mAttributeSet* ARogue10mCharacter::GetRogueAttributeSet() const
{
	const ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	return State ? State->GetRogueAttributeSet() : nullptr;
}

const URogue10mAttackSkillData* ARogue10mCharacter::GetDisplayedAttackSkillForHUD() const
{
	return CombatComponent ? CombatComponent->GetDisplayedAttackSkill() : nullptr;
}

float ARogue10mCharacter::GetAttackCooldownRemaining() const
{
	return CombatComponent ? CombatComponent->GetAttackCooldownRemaining() : 0.0f;
}

float ARogue10mCharacter::GetAttackCooldownDuration() const
{
	return CombatComponent ? CombatComponent->GetAttackCooldownDuration() : 0.0f;
}

bool ARogue10mCharacter::ExecutePendingAttackSkillFromAbility()
{
	return CombatComponent && CombatComponent->ExecutePendingAttackSkillFromAbility();
}

bool ARogue10mCharacter::IsBlockingWindowVisible() const
{
	const ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController());
	return RogueController && RogueController->IsAnyBlockingWindowVisible();
}

void ARogue10mCharacter::ActivateQuickSlot(int32 SlotNumber)
{
	if (ARogue10mPlayerController* RogueController = Cast<ARogue10mPlayerController>(GetController()))
	{
		RogueController->ActivateQuickSlot(SlotNumber);
	}
}

void ARogue10mCharacter::InitializeAbilityActorInfo()
{
	ARogue10mPlayerState* State = GetPlayerState<ARogue10mPlayerState>();
	if (UAbilitySystemComponent* AbilitySystem = State ? State->GetAbilitySystemComponent() : nullptr)
	{
		AbilitySystem->InitAbilityActorInfo(State, this);
	}
}