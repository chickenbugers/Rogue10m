// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mPlayerController.h"

#include "Blueprint/UserWidget.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "Rogue10m.h"
#include "Rogue10mAttackTargetInterface.h"
#include "Rogue10mBasicMonster.h"
#include "Rogue10mCameraManager.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mCombatComponent.h"
#include "Rogue10mGameState.h"
#include "Rogue10mRunHUD.h"
#include "Widgets/Rogue10mDamageIndicatorWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

ARogue10mPlayerController::ARogue10mPlayerController()
{
	PlayerCameraManagerClass = ARogue10mCameraManager::StaticClass();
	DamageIndicatorWidgetClass = URogue10mDamageIndicatorWidget::StaticClass();
	DefaultRunHUDClass = TSoftClassPtr<URogue10mRunHUD>(
		FSoftClassPath(TEXT("/Game/Widget/UW_Rogue10mMainWidget.UW_Rogue10mMainWidget_C")));
}

void ARogue10mPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogRogue10m, Error, TEXT("모바일 조작 위젯을 생성하지 못했습니다."));
		}
	}

	SetFpsLimit(CurrentFpsLimit);
	InitializeRunHUD();
	InitializeDamageIndicatorPool();
	RefreshInputMode();
}

void ARogue10mPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalPlayerController())
	{
		return;
	}

	InputComponent->BindKey(EKeys::I, IE_Pressed, this, &ARogue10mPlayerController::ToggleInventory);
	InputComponent->BindKey(EKeys::B, IE_Pressed, this, &ARogue10mPlayerController::ToggleItemWindow);
	InputComponent->BindKey(EKeys::K, IE_Pressed, this, &ARogue10mPlayerController::ToggleSkillTree);
	InputComponent->BindKey(EKeys::O, IE_Pressed, this, &ARogue10mPlayerController::ToggleSettings);
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ARogue10mPlayerController::ToggleSettings);
	InputComponent->BindKey(EKeys::F10, IE_Pressed, this, &ARogue10mPlayerController::ToggleSettings);
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ARogue10mPlayerController::HandleToggleCombatLog);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* Context : DefaultMappingContexts)
		{
			if (Context)
			{
				Subsystem->AddMappingContext(Context, 0);
			}
		}

		if (!ShouldUseTouchControls())
		{
			for (UInputMappingContext* Context : MobileExcludedMappingContexts)
			{
				if (Context)
				{
					Subsystem->AddMappingContext(Context, 0);
				}
			}
		}
	}
}

void ARogue10mPlayerController::ToggleInventory()
{
	SetPanelVisible(bInventoryVisible, !bInventoryVisible);
}

void ARogue10mPlayerController::ToggleItemWindow()
{
	SetPanelVisible(bItemWindowVisible, !bItemWindowVisible);
}

void ARogue10mPlayerController::ToggleSkillTree()
{
	SetPanelVisible(bSkillTreeVisible, !bSkillTreeVisible);
}

void ARogue10mPlayerController::ToggleSettings()
{
	SetPanelVisible(bSettingsVisible, !bSettingsVisible);
}

void ARogue10mPlayerController::CloseAllBlockingPanels()
{
	bInventoryVisible = false;
	bItemWindowVisible = false;
	bSkillTreeVisible = false;
	bSettingsVisible = false;
	RefreshInputMode();
}

bool ARogue10mPlayerController::IsAnyBlockingWindowVisible() const
{
	return bInventoryVisible || bItemWindowVisible || bSkillTreeVisible || bSettingsVisible;
}

void ARogue10mPlayerController::SetLookSensitivity(float NewSensitivityX, float NewSensitivityY)
{
	LookSensitivityX = FMath::Clamp(NewSensitivityX, 0.1f, 5.0f);
	LookSensitivityY = FMath::Clamp(NewSensitivityY, 0.1f, 5.0f);
}

void ARogue10mPlayerController::SetFpsLimit(int32 FpsValue)
{
	const int32 AllowedLimits[] = { 60, 120, 140 };
	int32 BestLimit = AllowedLimits[0];
	for (const int32 AllowedLimit : AllowedLimits)
	{
		if (FMath::Abs(FpsValue - AllowedLimit) < FMath::Abs(FpsValue - BestLimit))
		{
			BestLimit = AllowedLimit;
		}
	}
	CurrentFpsLimit = BestLimit;
	if (GEngine)
	{
		GEngine->SetMaxFPS(static_cast<float>(CurrentFpsLimit));
	}
}

void ARogue10mPlayerController::AddCombatLogMessage(const FString& Message, const FLinearColor& Color, float Duration)
{
	if (!bShowCombatLog || !GetWorld())
	{
		return;
	}

	FRogue10mRuntimeLogEntry Entry;
	Entry.Message = Message;
	Entry.Color = Color;
	Entry.ExpireTime = GetWorld()->GetTimeSeconds() + FMath::Max(0.1f, Duration);
	CombatLogEntries.Insert(MoveTemp(Entry), 0);
	if (CombatLogEntries.Num() > 40)
	{
		CombatLogEntries.SetNum(40);
	}

	if (Message.Contains(TEXT("획득")) || Message.Contains(TEXT("경험치")))
	{
		AddItemAcquisitionMessage(Message, Color);
	}
}

void ARogue10mPlayerController::AddItemAcquisitionMessage(const FString& Message, const FLinearColor& Color, float Duration)
{
	if (!GetWorld())
	{
		return;
	}

	FRogue10mRuntimeLogEntry Entry;
	Entry.Message = Message;
	Entry.Color = Color;
	Entry.ExpireTime = GetWorld()->GetTimeSeconds() + FMath::Max(0.1f, Duration);
	ItemAcquisitionEntries.Insert(MoveTemp(Entry), 0);
	if (ItemAcquisitionEntries.Num() > 8)
	{
		ItemAcquisitionEntries.SetNum(8);
	}
}

void ARogue10mPlayerController::AddFloatingDamageNumber(AActor* TargetActor, float DamageAmount)
{
	AActor* AttackSource = GetPawn();
	if (!TargetActor || !AttackSource || TargetActor == AttackSource || !GetWorld() || DamageAmount <= 0.0f
		|| !TargetActor->GetClass()->ImplementsInterface(URogue10mAttackTargetInterface::StaticClass())
		|| !IRogue10mAttackTargetInterface::Execute_CanReceiveRogue10mAttack(TargetActor, AttackSource))
	{
		return;
	}

	FVector IndicatorLocation =
		IRogue10mAttackTargetInterface::Execute_GetRogue10mDamageIndicatorLocation(TargetActor);
	IndicatorLocation += FVector(
		FMath::FRandRange(-8.0f, 8.0f),
		FMath::FRandRange(-8.0f, 8.0f),
		FMath::FRandRange(-4.0f, 8.0f));

	if (HasAuthority() && !IsLocalPlayerController())
	{
		ClientShowDamageIndicator(IndicatorLocation, DamageAmount);
		return;
	}
	if (!IsLocalPlayerController())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	FloatingDamageEntries.RemoveAll([CurrentTime](const FRogue10mFloatingDamageEntry& Entry)
	{
		return !Entry.TargetActor.IsValid() || Entry.ExpireTime <= CurrentTime;
	});
	if (FloatingDamageEntries.Num() >= 32)
	{
		FloatingDamageEntries.RemoveAt(0);
	}

	FRogue10mFloatingDamageEntry Entry;
	Entry.TargetActor = TargetActor;
	Entry.DamageAmount = DamageAmount;
	Entry.StartTime = CurrentTime;
	Entry.ExpireTime = CurrentTime + DamageIndicatorDuration;
	FloatingDamageEntries.Add(Entry);
	ShowDamageIndicatorAtLocation(IndicatorLocation, DamageAmount);
}

void ARogue10mPlayerController::ClientShowDamageIndicator_Implementation(
	FVector WorldLocation, float DamageAmount)
{
	ShowDamageIndicatorAtLocation(WorldLocation, DamageAmount);
}

void ARogue10mPlayerController::InitializeDamageIndicatorPool()
{
	if (!IsLocalPlayerController() || !DamageIndicatorWidgetClass)
	{
		return;
	}

	const int32 MaximumSize = FMath::Clamp(MaxDamageIndicatorPoolSize, 1, 128);
	const int32 InitialSize = FMath::Clamp(InitialDamageIndicatorPoolSize, 0, MaximumSize);
	DamageIndicatorPool.Reserve(MaximumSize);
	for (int32 Index = DamageIndicatorPool.Num(); Index < InitialSize; ++Index)
	{
		URogue10mDamageIndicatorWidget* Widget = CreateWidget<URogue10mDamageIndicatorWidget>(
			this, DamageIndicatorWidgetClass);
		if (!Widget)
		{
			break;
		}
		Widget->AddToPlayerScreen(100);
		Widget->DeactivateIndicator();
		DamageIndicatorPool.Add(Widget);
	}
}

URogue10mDamageIndicatorWidget* ARogue10mPlayerController::AcquireDamageIndicator()
{
	for (URogue10mDamageIndicatorWidget* Widget : DamageIndicatorPool)
	{
		if (Widget && Widget->IsAvailableForReuse())
		{
			return Widget;
		}
	}

	const int32 MaximumSize = FMath::Clamp(MaxDamageIndicatorPoolSize, 1, 128);
	if (DamageIndicatorPool.Num() < MaximumSize && DamageIndicatorWidgetClass)
	{
		if (URogue10mDamageIndicatorWidget* Widget = CreateWidget<URogue10mDamageIndicatorWidget>(
			this, DamageIndicatorWidgetClass))
		{
			Widget->AddToPlayerScreen(100);
			Widget->DeactivateIndicator();
			DamageIndicatorPool.Add(Widget);
			return Widget;
		}
	}

	URogue10mDamageIndicatorWidget* OldestWidget = nullptr;
	for (URogue10mDamageIndicatorWidget* Widget : DamageIndicatorPool)
	{
		if (Widget && (!OldestWidget || Widget->GetActivationTime() < OldestWidget->GetActivationTime()))
		{
			OldestWidget = Widget;
		}
	}
	if (OldestWidget)
	{
		OldestWidget->DeactivateIndicator();
	}
	return OldestWidget;
}

void ARogue10mPlayerController::ShowDamageIndicatorAtLocation(
	const FVector& WorldLocation, float DamageAmount)
{
	if (!IsLocalPlayerController() || !GetWorld() || !DamageIndicatorWidgetClass || DamageAmount <= 0.0f)
	{
		return;
	}

	if (URogue10mDamageIndicatorWidget* IndicatorWidget = AcquireDamageIndicator())
	{
		IndicatorWidget->InitializeIndicator(DamageAmount, WorldLocation, DamageIndicatorDuration);
	}
}

void ARogue10mPlayerController::NotifyPlayerDamaged(float DamageAmount)
{
	if (!GetWorld() || DamageAmount <= 0.0f)
	{
		return;
	}

	PlayerDamageFeedbackStrength = FMath::Clamp(DamageAmount / 35.0f, 0.25f, 1.0f);
	PlayerDamageFeedbackEndTime = GetWorld()->GetTimeSeconds() + 0.35f;
}

bool ARogue10mPlayerController::ToggleCombatLogVisible()
{
	bShowCombatLog = !bShowCombatLog;
	if (!bShowCombatLog)
	{
		CombatLogEntries.Reset();
	}
	return bShowCombatLog;
}

bool ARogue10mPlayerController::ActivateQuickSlot(int32 SlotNumber)
{
	ARogue10mCharacter* RogueCharacter = Cast<ARogue10mCharacter>(GetPawn());
	URogue10mCombatComponent* Combat = RogueCharacter ? RogueCharacter->GetCombatComponent() : nullptr;
	return Combat && Combat->ActivateQuickSlot(SlotNumber);
}

ARogue10mBasicMonster* ARogue10mPlayerController::FindLookedAtMonster() const
{
	if (!PlayerCameraManager || !GetWorld())
	{
		return nullptr;
	}

	const FVector TraceStart = PlayerCameraManager->GetCameraLocation();
	const FVector TraceEnd = TraceStart + PlayerCameraManager->GetCameraRotation().Vector() * 3000.0f;
	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(Rogue10mMonsterInfo), false, GetPawn());
	return GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, QueryParams)
		? Cast<ARogue10mBasicMonster>(Hit.GetActor())
		: nullptr;
}

float ARogue10mPlayerController::GetPlayerDamageFeedbackAlpha() const
{
	if (!GetWorld() || PlayerDamageFeedbackEndTime <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp((PlayerDamageFeedbackEndTime - GetWorld()->GetTimeSeconds()) / 0.35f, 0.0f, 1.0f);
}

bool ARogue10mPlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ARogue10mPlayerController::InitializeRunHUD()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	const TSubclassOf<URogue10mRunHUD> ResolvedClass = ResolveRunHUDClass();
	if (!ResolvedClass)
	{
		return;
	}

	RunHUD = CreateWidget<URogue10mRunHUD>(this, ResolvedClass);
	if (!RunHUD)
	{
		UE_LOG(LogRogue10m, Error, TEXT("메인 HUD 위젯을 생성하지 못했습니다."));
		return;
	}

	RunHUD->AddToPlayerScreen(0);
	if (ARogue10mGameState* GameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr)
	{
		GameState->OnRunTimerUpdated.AddDynamic(RunHUD, &URogue10mRunHUD::BP_UpdateRunTimer);
		GameState->OnRunPhaseChanged.AddDynamic(RunHUD, &URogue10mRunHUD::BP_OnRunPhaseChanged);
		RunHUD->BP_UpdateRunTimer(GameState->GetRemainingRunSeconds(), GameState->GetRunDurationSeconds(), GameState->GetRunProgressAlpha());
	}
}

TSubclassOf<URogue10mRunHUD> ARogue10mPlayerController::ResolveRunHUDClass()
{
	if (RunHUDClass)
	{
		return RunHUDClass;
	}

	UClass* LoadedClass = DefaultRunHUDClass.LoadSynchronous();
	if (!LoadedClass || !LoadedClass->IsChildOf(URogue10mRunHUD::StaticClass()))
	{
		UE_LOG(LogRogue10m, Warning, TEXT("기본 메인 HUD가 없거나 Rogue10mRunHUD를 상속하지 않았습니다."));
		return nullptr;
	}
	return LoadedClass;
}

void ARogue10mPlayerController::RefreshInputMode()
{
	const bool bBlocking = IsAnyBlockingWindowVisible();
	bShowMouseCursor = bBlocking;
	bEnableClickEvents = bBlocking;
	bEnableMouseOverEvents = bBlocking;

	if (bBlocking)
	{
		FInputModeGameAndUI InputMode;
		if (RunHUD)
		{
			InputMode.SetWidgetToFocus(RunHUD->TakeWidget());
		}
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
}

void ARogue10mPlayerController::SetPanelVisible(bool& PanelState, bool bVisible)
{
	PanelState = bVisible;
	if (bVisible)
	{
		if (ARogue10mCharacter* RogueCharacter = Cast<ARogue10mCharacter>(GetPawn()))
		{
			RogueCharacter->GetCharacterMovement()->StopMovementImmediately();
		}
	}
	RefreshInputMode();
}
void ARogue10mPlayerController::HandleToggleCombatLog()
{
	const bool bVisible = ToggleCombatLogVisible();
	if (bVisible)
	{
		AddCombatLogMessage(TEXT("전투 로그 표시 켜짐"), FLinearColor(0.58f, 0.85f, 1.0f, 1.0f));
	}
}