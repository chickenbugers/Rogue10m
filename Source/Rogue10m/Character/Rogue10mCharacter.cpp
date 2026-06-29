// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Rogue10m.h"
#include "Rogue10mAttackSkillData.h"
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

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ARogue10mCharacter::DoPrimaryAttackPressed);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ARogue10mCharacter::DoPrimaryAttackReleased);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ARogue10mCharacter::DoSpecialAttackPressed);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ARogue10mCharacter::DoSpecialAttackReleased);
	PlayerInputComponent->BindKey(EKeys::I, IE_Pressed, this, &ARogue10mCharacter::DoToggleInventory);
	PlayerInputComponent->BindKey(EKeys::B, IE_Pressed, this, &ARogue10mCharacter::DoToggleItemWindow);
	PlayerInputComponent->BindKey(EKeys::K, IE_Pressed, this, &ARogue10mCharacter::DoToggleSkillTree);
	PlayerInputComponent->BindKey(EKeys::O, IE_Pressed, this, &ARogue10mCharacter::DoToggleSettings);

	// 숫자 1~5 키를 하단 퀵 슬롯 UI와 연결합니다.
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot1);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot2);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot3);
	PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot4);
	PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ARogue10mCharacter::DoQuickSlot5);
}

float ARogue10mCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsDead || !VitalsComponent || DamageAmount <= 0.0f)
	{
		return AppliedDamage;
	}

	const float NewHealth = VitalsComponent->GetHealth().Current - DamageAmount;
	VitalsComponent->SetHealth(NewHealth);
	UE_LOG(LogRogue10m, Log, TEXT("%s took %.1f damage. HP %.1f / %.1f"), *GetNameSafe(this), DamageAmount, VitalsComponent->GetHealth().Current, VitalsComponent->GetHealth().Max);
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
		{
			RogueHUD->NotifyPlayerDamaged(DamageAmount);
		}
	}

	if (VitalsComponent->GetHealth().Current <= 0.0f)
	{
		Die();
	}

	return DamageAmount;
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
	if (bIsDead || IsInventoryWindowBlockingMovement() || !GetController())
	{
		return;
	}

	float SensitivityX = 1.0f;
	float SensitivityY = 1.0f;
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (const ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
		{
			SensitivityX = RogueHUD->GetLookSensitivityX();
			SensitivityY = RogueHUD->GetLookSensitivityY();
		}
	}

	AddControllerYawInput(Yaw * SensitivityX);
	AddControllerPitchInput(Pitch * SensitivityY);
}

void ARogue10mCharacter::DoMove(float Right, float Forward)
{
	if (IsInventoryWindowBlockingMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		return;
	}

	if (!bIsDead && GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ARogue10mCharacter::DoJumpStart()
{
	if (bIsDead || IsInventoryWindowBlockingMovement())
	{
		return;
	}

	// pass Jump to the character
	Jump();
}

void ARogue10mCharacter::DoJumpEnd()
{
	if (bIsDead || IsInventoryWindowBlockingMovement())
	{
		return;
	}

	// pass StopJumping to the character
	StopJumping();
}

void ARogue10mCharacter::DoPrimaryAttackPressed()
{
	BeginCombatAttack(true);
}

void ARogue10mCharacter::DoPrimaryAttackReleased()
{
	EndCombatAttack(true);
}

void ARogue10mCharacter::DoSpecialAttackPressed()
{
	BeginCombatAttack(false);
}

void ARogue10mCharacter::DoSpecialAttackReleased()
{
	EndCombatAttack(false);
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
		if (RogueHUD->IsAnyBlockingWindowVisible())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
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
		AddCombatScreenLog(TEXT("공격 입력 확인: 기본 공격 재사용 대기 중입니다."), FLinearColor(1.0f, 0.65f, 0.35f, 1.0f));
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
		AddCombatScreenLog(FString::Printf(TEXT("기본 공격 명중: %s / 피해 %.0f"), *GetNameSafe(HitResult.GetActor()), UnarmedDamage), FLinearColor(0.35f, 1.0f, 0.62f, 1.0f));
	}
	else
	{
		UE_LOG(LogRogue10m, Verbose, TEXT("Unarmed attack missed."));
		AddCombatScreenLog(TEXT("기본 공격 빗나감"), FLinearColor(0.85f, 0.86f, 0.9f, 1.0f));
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
		if (RogueHUD->IsAnyBlockingWindowVisible())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

void ARogue10mCharacter::DoToggleSkillTree()
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
		RogueHUD->ToggleSkillTree();
		if (RogueHUD->IsAnyBlockingWindowVisible())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

void ARogue10mCharacter::DoToggleSettings()
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
		RogueHUD->ToggleSettings();
		if (RogueHUD->IsAnyBlockingWindowVisible())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
	}
}

void ARogue10mCharacter::DoQuickSlot1()
{
	ActivateQuickSlot(1);
}

void ARogue10mCharacter::DoQuickSlot2()
{
	ActivateQuickSlot(2);
}

void ARogue10mCharacter::DoQuickSlot3()
{
	ActivateQuickSlot(3);
}

void ARogue10mCharacter::DoQuickSlot4()
{
	ActivateQuickSlot(4);
}

void ARogue10mCharacter::DoQuickSlot5()
{
	ActivateQuickSlot(5);
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
			RogueHUD->SetSkillTreeVisible(false);
			RogueHUD->SetSettingsVisible(false);
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogRogue10m, Log, TEXT("%s died."), *GetNameSafe(this));
	BP_OnRunDeath();
}

void ARogue10mCharacter::SetEquippedWeaponType(ERogue10mWeaponType NewWeaponType)
{
	// 무기 아이템 장착 성공 시 인벤토리 컴포넌트가 호출하는 갱신 지점입니다.
	EquippedWeaponType = NewWeaponType;
	UE_LOG(LogRogue10m, Log, TEXT("%s equipped weapon type %d."), *GetNameSafe(this), static_cast<int32>(EquippedWeaponType));
}

bool ARogue10mCharacter::IsInventoryWindowBlockingMovement() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	const ARogue10mHUD* RogueHUD = PlayerController ? PlayerController->GetHUD<ARogue10mHUD>() : nullptr;
	return RogueHUD && RogueHUD->IsAnyBlockingWindowVisible();
}

bool ARogue10mCharacter::CanUseCombatInput() const
{
	return !bIsDead && !IsInventoryWindowBlockingMovement();
}

void ARogue10mCharacter::BeginCombatAttack(bool bPrimaryAttack)
{
	if (!CanUseCombatInput())
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float& PressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	PressedTime = World->GetTimeSeconds();

	const FString ButtonText = bPrimaryAttack ? TEXT("좌클릭") : TEXT("우클릭");
	AddCombatScreenLog(FString::Printf(TEXT("%s 입력: 차징 확인 시작"), *ButtonText), FLinearColor(0.72f, 0.84f, 1.0f, 1.0f));
}

void ARogue10mCharacter::EndCombatAttack(bool bPrimaryAttack)
{
	if (!CanUseCombatInput())
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float& PressedTime = bPrimaryAttack ? LeftAttackPressedTime : RightAttackPressedTime;
	if (PressedTime < 0.0f)
	{
		return;
	}

	const float HeldTime = World->GetTimeSeconds() - PressedTime;
	PressedTime = -1.0f;
	const bool bJumpAttack = GetCharacterMovement() && GetCharacterMovement()->IsFalling();
	const URogue10mAttackSkillData* ChargedSkill = ResolveChargedAttackSkill(bPrimaryAttack, bJumpAttack);
	const float RequiredChargeSeconds = ChargedSkill ? ChargedSkill->ChargeSeconds : ChargeAttackThreshold;
	ExecuteCombatAttack(bPrimaryAttack, ChargedSkill && HeldTime >= RequiredChargeSeconds);
}

void ARogue10mCharacter::ExecuteCombatAttack(bool bPrimaryAttack, bool bChargedAttack)
{
	const bool bJumpAttack = GetCharacterMovement() && GetCharacterMovement()->IsFalling();
	const URogue10mAttackSkillData* SkillData = ResolveAttackSkill(bPrimaryAttack, bChargedAttack, bJumpAttack);
	if (!SkillData)
	{
		AddCombatScreenLog(FString::Printf(TEXT("%s 공격 잠김: 스킬 Data Asset이 지정되지 않았습니다."), *GetAttackInputText(bPrimaryAttack, bJumpAttack)), FLinearColor(1.0f, 0.35f, 0.25f, 1.0f));
		return;
	}

	const FString ActionText = GetCombatActionText(bPrimaryAttack, bChargedAttack, bJumpAttack);
	const FLinearColor LogColor = bPrimaryAttack
		? FLinearColor(1.0f, 0.72f, 0.42f, 1.0f)
		: FLinearColor(0.62f, 0.82f, 1.0f, 1.0f);

	AddCombatScreenLog(ActionText, LogColor);
	UE_LOG(LogRogue10m, Log, TEXT("%s"), *ActionText);
	ExecuteAttackSkill(*SkillData);
}

void ARogue10mCharacter::ExecuteAttackSkill(const URogue10mAttackSkillData& SkillData)
{
	if (bIsDead || !FirstPersonCameraComponent)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < SkillData.AttackCooldown)
	{
		AddCombatScreenLog(FString::Printf(TEXT("%s 재사용 대기 중입니다."), *SkillData.SkillName.ToString()), FLinearColor(1.0f, 0.65f, 0.35f, 1.0f));
		return;
	}

	LastAttackTime = CurrentTime;

	if (SkillData.AttackMontage)
	{
		if (UAnimInstance* FirstPersonAnimInstance = FirstPersonMesh ? FirstPersonMesh->GetAnimInstance() : nullptr)
		{
			FirstPersonAnimInstance->Montage_Play(SkillData.AttackMontage);
		}
		else if (UAnimInstance* ThirdPersonAnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			ThirdPersonAnimInstance->Montage_Play(SkillData.AttackMontage);
		}
	}

	const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
	const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * SkillData.AttackRange;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(Rogue10mAttackSkill), false, this);
	FHitResult HitResult;
	const bool bHit = World->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SkillData.AttackTraceRadius),
		QueryParams);

	if (SkillData.bDrawDebugAttack)
	{
		DrawAttackDebug(TraceStart, TraceEnd, SkillData.AttackTraceRadius, SkillData.DebugColor, bHit, HitResult);
	}

	if (bHit && HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), SkillData.Damage, GetController(), this, UDamageType::StaticClass());
		AddCombatScreenLog(FString::Printf(TEXT("%s 명중: %s / 피해 %.0f"), *SkillData.SkillName.ToString(), *GetNameSafe(HitResult.GetActor()), SkillData.Damage), FLinearColor(0.35f, 1.0f, 0.62f, 1.0f));
	}
	else
	{
		AddCombatScreenLog(FString::Printf(TEXT("%s 빗나감"), *SkillData.SkillName.ToString()), FLinearColor(0.85f, 0.86f, 0.9f, 1.0f));
	}
}

void ARogue10mCharacter::DrawAttackDebug(const FVector& TraceStart, const FVector& TraceEnd, float TraceRadius, const FLinearColor& DebugColor, bool bHit, const FHitResult& HitResult) const
{
	if (!bDrawAttackDebug)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FColor DrawColor = DebugColor.ToFColor(true);
	DrawDebugLine(World, TraceStart, TraceEnd, DrawColor, false, 1.2f, 0, 2.0f);
	DrawDebugSphere(World, TraceEnd, TraceRadius, 16, DrawColor, false, 1.2f, 0, 1.5f);
	if (bHit)
	{
		DrawDebugSphere(World, HitResult.ImpactPoint, TraceRadius * 1.25f, 16, FColor::Red, false, 1.2f, 0, 2.5f);
	}
}

const URogue10mAttackSkillData* ARogue10mCharacter::ResolveAttackSkill(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const
{
	if (bChargedAttack)
	{
		return ResolveChargedAttackSkill(bPrimaryAttack, bJumpAttack);
	}

	if (bJumpAttack)
	{
		return bPrimaryAttack ? JumpPrimaryAttackSkill : JumpSpecialAttackSkill;
	}

	return bPrimaryAttack ? PrimaryAttackSkill : SpecialAttackSkill;
}

const URogue10mAttackSkillData* ARogue10mCharacter::ResolveChargedAttackSkill(bool bPrimaryAttack, bool bJumpAttack) const
{
	if (bJumpAttack)
	{
		return nullptr;
	}

	return bPrimaryAttack ? ChargedPrimaryAttackSkill : ChargedSpecialAttackSkill;
}

void ARogue10mCharacter::AddCombatScreenLog(const FString& Message, const FLinearColor& Color) const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ARogue10mHUD* RogueHUD = PlayerController->GetHUD<ARogue10mHUD>())
		{
			RogueHUD->AddCombatLogMessage(Message, Color);
		}
	}
}

FString ARogue10mCharacter::GetCombatActionText(bool bPrimaryAttack, bool bChargedAttack, bool bJumpAttack) const
{
	const FString ButtonText = GetAttackInputText(bPrimaryAttack, bJumpAttack);
	const FString AttackText = bPrimaryAttack ? TEXT("기본 공격") : TEXT("특수 공격");
	const FString ChargePrefix = bChargedAttack ? TEXT("차징 ") : TEXT("");
	const URogue10mAttackSkillData* SkillData = ResolveAttackSkill(bPrimaryAttack, bChargedAttack, bJumpAttack);
	const float DamagePreview = SkillData ? SkillData->Damage : 0.0f;
	return FString::Printf(TEXT("%s%s 실행: %s / 피해 %.0f"), *ChargePrefix, *AttackText, *ButtonText, DamagePreview);
}

FString ARogue10mCharacter::GetAttackInputText(bool bPrimaryAttack, bool bJumpAttack) const
{
	if (bJumpAttack)
	{
		return bPrimaryAttack ? TEXT("점프 좌클릭") : TEXT("점프 우클릭");
	}

	return bPrimaryAttack ? TEXT("좌클릭") : TEXT("우클릭");
}

bool ARogue10mCharacter::ActivateQuickSlot(int32 SlotNumber)
{
	// 사망 상태나 UI 조작 중에는 전투용 퀵 슬롯 입력을 무시합니다.
	if (bIsDead || IsInventoryWindowBlockingMovement())
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	ARogue10mHUD* RogueHUD = PlayerController ? PlayerController->GetHUD<ARogue10mHUD>() : nullptr;
	if (!RogueHUD)
	{
		return false;
	}

	const bool bActivated = RogueHUD->ActivateQuickSlot(SlotNumber);
	if (bActivated)
	{
		UE_LOG(LogRogue10m, Verbose, TEXT("Quick slot %d activated."), SlotNumber);
	}

	return bActivated;
}
