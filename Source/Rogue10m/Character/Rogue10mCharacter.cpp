// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mHUD.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mVitalsComponent.h"

ARogue10mCharacter::ARogue10mCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	InventoryComponent = CreateDefaultSubobject<URogue10mInventoryComponent>(TEXT("Inventory Component"));
	VitalsComponent = CreateDefaultSubobject<URogue10mVitalsComponent>(TEXT("Vitals Component"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ARogue10mCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARogue10mCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARogue10mCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ARogue10mCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogRogue10m, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARogue10mCharacter::DoPrimaryAttack);
	PlayerInputComponent->BindKey(EKeys::I, IE_Pressed, this, &ARogue10mCharacter::DoToggleInventory);
	PlayerInputComponent->BindKey(EKeys::B, IE_Pressed, this, &ARogue10mCharacter::DoToggleItemWindow);
}


void ARogue10mCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ARogue10mCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ARogue10mCharacter::DoAim(float Yaw, float Pitch)
{
	if (!bIsDead && GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ARogue10mCharacter::DoMove(float Right, float Forward)
{
	if (!bIsDead && GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ARogue10mCharacter::DoJumpStart()
{
	if (bIsDead)
	{
		return;
	}

	// pass Jump to the character
	Jump();
}

void ARogue10mCharacter::DoJumpEnd()
{
	if (bIsDead)
	{
		return;
	}

	// pass StopJumping to the character
	StopJumping();
}

void ARogue10mCharacter::DoPrimaryAttack()
{
	if (bIsDead)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (const ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>(); RogueHUD && RogueHUD->IsAnyInventoryWindowVisible())
		{
			return;
		}
	}

	switch (EquippedWeaponType)
	{
	case ERogue10mWeaponType::Unarmed:
		DoUnarmedAttack();
		break;
	default:
		UE_LOG(LogRogue10m, Verbose, TEXT("Weapon attack is not implemented yet."));
		break;
	}
}

void ARogue10mCharacter::DoToggleItemWindow()
{
	if (bIsDead)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	if (ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
	{
		RogueHUD->ToggleItemWindow();
	}
}

void ARogue10mCharacter::DoUnarmedAttack()
{
	if (bIsDead)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !FirstPersonCameraComponent)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < UnarmedAttackInterval)
	{
		return;
	}

	LastAttackTime = CurrentTime;

	const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
	const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * UnarmedRange;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UnarmedAttack), false, this);
	FHitResult HitResult;
	const bool bHit = World->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(UnarmedTraceRadius),
		QueryParams);

	if (bHit && HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), UnarmedDamage, GetController(), this, UDamageType::StaticClass());
		UE_LOG(LogRogue10m, Log, TEXT("Unarmed attack hit %s."), *GetNameSafe(HitResult.GetActor()));
	}
	else
	{
		UE_LOG(LogRogue10m, Verbose, TEXT("Unarmed attack missed."));
	}
}

void ARogue10mCharacter::DoToggleInventory()
{
	if (bIsDead)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	if (ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
	{
		RogueHUD->ToggleInventory();
	}
}

void ARogue10mCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);

		if (ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
		{
			RogueHUD->SetInventoryVisible(false);
			RogueHUD->SetItemWindowVisible(false);
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogRogue10m, Log, TEXT("%s died after the run timer expired."), *GetNameSafe(this));
	BP_OnRunDeath();
}
