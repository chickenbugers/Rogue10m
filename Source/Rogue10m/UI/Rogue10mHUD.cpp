// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mHUD.h"

#include "Camera/CameraComponent.h"
#include "CanvasItem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Canvas.h"
#include "Engine/EngineTypes.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Fonts/CompositeFont.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Misc/Paths.h"
#include "Rogue10mBasicMonster.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mPlayerState.h"
#include "Rogue10mVitalsComponent.h"
#include "Rogue10mDroppedItem.h"

ARogue10mHUD::ARogue10mHUD()
{
	CharacterPreviewYaw = 90.0f;

	// 임시 퀵 슬롯 데이터입니다. 실제 스킬/아이템 시스템이 생기면 데이터 기반으로 교체합니다.
	QuickSlots =
	{
		{ 1, FText::FromString(TEXT("주먹")), FLinearColor(0.9f, 0.36f, 0.22f, 1.0f), 1.8f, 0.0f },
		{ 2, FText::FromString(TEXT("회피")), FLinearColor(0.28f, 0.65f, 1.0f, 1.0f), 4.0f, 0.0f },
		{ 3, FText::FromString(TEXT("방어")), FLinearColor(0.32f, 0.86f, 0.46f, 1.0f), 3.0f, 0.0f },
		{ 4, FText::FromString(TEXT("스킬")), FLinearColor(0.74f, 0.48f, 1.0f, 1.0f), 6.0f, 0.0f },
		{ 5, FText::FromString(TEXT("아이템")), FLinearColor(0.95f, 0.75f, 0.26f, 1.0f), 8.0f, 0.0f }
	};
}

void ARogue10mHUD::BeginPlay()
{
	Super::BeginPlay();

	ApplyFpsLimit(CurrentFpsLimit);
}

void ARogue10mHUD::DrawText(const FString& Text, FLinearColor TextColor, float ScreenX, float ScreenY, UFont* Font, float Scale, bool bScalePosition)
{
	if (!Canvas)
	{
		return;
	}

	if (!bUseCustomUIFont)
	{
		Super::DrawText(Text, TextColor, ScreenX, ScreenY, Font, Scale, bScalePosition);
		return;
	}

	// HUD 전체 텍스트를 같은 한글 지원 Slate 폰트로 그려 UI 인상을 통일합니다.
	const TSharedPtr<const FCompositeFont> CompositeFont = GetUIFont();
	if (!CompositeFont.IsValid())
	{
		Super::DrawText(Text, TextColor, ScreenX, ScreenY, Font, Scale, bScalePosition);
		return;
	}

	const FSlateFontInfo SlateFontInfo(CompositeFont, UIFontBaseSize);
	const FVector2D DrawPosition = bScalePosition ? FVector2D(ScreenX * Scale, ScreenY * Scale) : FVector2D(ScreenX, ScreenY);

	FCanvasTextItem TextItem(DrawPosition, FText::FromString(Text), SlateFontInfo, TextColor);
	TextItem.Scale = FVector2D(Scale, Scale);
	TextItem.EnableShadow(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), FVector2D(1.0f, 1.0f));
	Canvas->DrawItem(TextItem);
}

TSharedPtr<const FCompositeFont> ARogue10mHUD::GetUIFont() const
{
	const FString FontPath = FPaths::EngineContentDir() / UIFontRelativePath;
	if (!CachedUIFont.IsValid() || CachedUIFontAbsolutePath != FontPath)
	{
		CachedUIFontAbsolutePath = FontPath;
		CachedUIFont = MakeShared<FCompositeFont>(
			TEXT("Rogue10mUIFont"),
			FString(FontPath),
			EFontHinting::Default,
			EFontLoadingPolicy::LazyLoad);
	}

	return CachedUIFont;
}

void ARogue10mHUD::DrawHUD()
{
	Super::DrawHUD();

	bHasHoveredItem = false;
	HoveredItemSlotIndex = INDEX_NONE;
	EquipmentSlotHitAreas.Reset();
	ItemSlotHitAreas.Reset();
	SkillWeaponHitAreas.Reset();

	if (!bDrawCanvasPrototypeHUD)
	{
		DrawPlayerDamageFeedback();
		DrawDraggedItem();
		return;
	}

	UpdateWindowDrag();
	DrawRunTimer();
	DrawVitals();
	DrawMiniMap();
	DrawLookedAtMonsterInfo();
	DrawItemAcquisitionPanel();
	DrawAimCrossLine();
	DrawCombatLog();
	DrawFloatingDamageNumbers();
	DrawAttackCooldownSlot();
	DrawQuickSlots();
	DrawPanelShortcutHints();
	DrawRunResult();
	DrawInventory();
	DrawItemWindow();
	DrawSkillTree();
	DrawSettingsPanel();
	DrawPlayerDamageFeedback();
	DrawHoveredItemTooltip();
	DrawDraggedItem();
}

void ARogue10mHUD::ToggleInventory()
{
	SetInventoryVisible(!bInventoryVisible);
}

void ARogue10mHUD::ToggleItemWindow()
{
	SetItemWindowVisible(!bItemWindowVisible);
}

void ARogue10mHUD::ToggleSkillTree()
{
	SetSkillTreeVisible(!bSkillTreeVisible);
}

void ARogue10mHUD::ToggleSettings()
{
	SetSettingsVisible(!bSettingsVisible);
}

void ARogue10mHUD::SetInventoryVisible(bool bNewInventoryVisible)
{
	bInventoryVisible = bNewInventoryVisible;
	UpdateInventoryCursor();
}

void ARogue10mHUD::SetItemWindowVisible(bool bNewItemWindowVisible)
{
	bItemWindowVisible = bNewItemWindowVisible;
	UpdateInventoryCursor();
}

void ARogue10mHUD::SetSkillTreeVisible(bool bNewSkillTreeVisible)
{
	bSkillTreeVisible = bNewSkillTreeVisible;
	if (bSkillTreeVisible)
	{
		SkillTreeView = ERogue10mSkillTreeView::WeaponSelect;
	}
	UpdateInventoryCursor();
}

void ARogue10mHUD::SetSettingsVisible(bool bNewSettingsVisible)
{
	bSettingsVisible = bNewSettingsVisible;
	if (!bSettingsVisible)
	{
		DraggedSettingsSlider = ERogue10mSettingsSlider::None;
	}
	UpdateInventoryCursor();
}

void ARogue10mHUD::SetFpsLimit(int32 FpsValue)
{
	ApplyFpsLimit(FpsValue);
}

bool ARogue10mHUD::ActivateQuickSlot(int32 SlotNumber)
{
	if (!GetWorld())
	{
		return false;
	}

	FRogue10mQuickSlotView* QuickSlot = QuickSlots.FindByPredicate([SlotNumber](const FRogue10mQuickSlotView& Candidate)
	{
		return Candidate.SlotNumber == SlotNumber;
	});

	// 쿨타임 중인 슬롯은 다시 사용할 수 없습니다.
	if (!QuickSlot || GetQuickSlotCooldownRemaining(*QuickSlot) > 0.0f)
	{
		return false;
	}

	// 현재는 실제 효과 없이 쿨타임만 시작해 UI와 입력 흐름을 검증합니다.
	if (QuickSlot->CooldownDuration > 0.0f)
	{
		QuickSlot->CooldownEndTime = GetWorld()->GetTimeSeconds() + QuickSlot->CooldownDuration;
	}

	return true;
}

void ARogue10mHUD::AddCombatLogMessage(const FString& Message, const FLinearColor& Color, float Duration)
{
	if (!bShowCombatLog || !GetWorld())
	{
		return;
	}

	FRogue10mCombatLogEntry Entry;
	Entry.Message = Message;
	Entry.Color = Color;
	Entry.ExpireTime = GetWorld()->GetTimeSeconds() + FMath::Max(4.0f, Duration);
	CombatLogEntries.Insert(Entry, 0);

	if (Message.Contains(TEXT("획득")) || Message.Contains(TEXT("얻었습니다")) || Message.Contains(TEXT("경험치")))
	{
		AddItemAcquisitionMessage(Message, Color);
	}

	constexpr int32 MaxCombatLogCount = 40;
	if (CombatLogEntries.Num() > MaxCombatLogCount)
	{
		CombatLogEntries.SetNum(MaxCombatLogCount);
	}
}

bool ARogue10mHUD::ToggleCombatLogVisible()
{
	SetCombatLogVisible(!bShowCombatLog);
	return bShowCombatLog;
}

void ARogue10mHUD::SetCombatLogVisible(bool bNewVisible)
{
	bShowCombatLog = bNewVisible;
	if (!bShowCombatLog)
	{
		CombatLogEntries.Reset();
	}
}

void ARogue10mHUD::SetFloatingDamageNumbersVisible(bool bNewVisible)
{
	bShowFloatingDamageNumbers = bNewVisible;
	if (!bShowFloatingDamageNumbers)
	{
		FloatingDamageEntries.Reset();
	}
}

void ARogue10mHUD::SetPlayerDamageFeedbackVisible(bool bNewVisible)
{
	bShowPlayerDamageFeedback = bNewVisible;
	if (!bShowPlayerDamageFeedback)
	{
		PlayerDamageFeedbackEndTime = 0.0f;
		PlayerDamageFeedbackStrength = 0.0f;
	}
}

void ARogue10mHUD::SetAllCombatDebugVisible(bool bNewVisible)
{
	SetCombatLogVisible(bNewVisible);
	SetFloatingDamageNumbersVisible(bNewVisible);
	SetPlayerDamageFeedbackVisible(bNewVisible);
}

void ARogue10mHUD::NotifyPlayerDamaged(float DamageAmount)
{
	if (!GetWorld() || DamageAmount <= 0.0f)
	{
		return;
	}

	if (bShowPlayerDamageFeedback)
	{
		PlayerDamageFeedbackEndTime = GetWorld()->GetTimeSeconds() + 0.35f;
		PlayerDamageFeedbackStrength = FMath::Clamp(DamageAmount / 25.0f, 0.25f, 1.0f);
	}
	AddCombatLogMessage(FString::Printf(TEXT("플레이어 피격: 피해 %.0f"), DamageAmount), FLinearColor(1.0f, 0.42f, 0.36f, 1.0f), 1.6f);
}

void ARogue10mHUD::AddFloatingDamageNumber(AActor* TargetActor, float DamageAmount)
{
	if (!bShowFloatingDamageNumbers || !GetWorld() || !TargetActor || DamageAmount <= 0.0f)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	FRogue10mFloatingDamageEntry Entry;
	Entry.TargetActor = TargetActor;
	Entry.DamageAmount = DamageAmount;
	Entry.StartTime = CurrentTime;
	Entry.ExpireTime = CurrentTime + 0.85f;
	FloatingDamageEntries.Add(Entry);
}

void ARogue10mHUD::DrawRunTimer()
{
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!Canvas || !RogueGameState || !RogueGameState->IsRunActive())
	{
		return;
	}

	const FString TimerText = FString::Printf(TEXT("남은 시간  %s"), *RogueGameState->GetRemainingTimeText().ToString());
	DrawText(TimerText, TimerColor, 32.0f, 32.0f, nullptr, TimerScale, false);

	const float ProgressWidth = 360.0f;
	const float ProgressHeight = 8.0f;
	const float ProgressX = 34.0f;
	const float ProgressY = 78.0f;
	const float ProgressAlpha = RogueGameState->GetRunProgressAlpha();

	DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.75f), ProgressX, ProgressY, ProgressWidth, ProgressHeight);
	DrawRect(TimerColor, ProgressX, ProgressY, ProgressWidth * ProgressAlpha, ProgressHeight);
}

void ARogue10mHUD::DrawRunResult()
{
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!Canvas || !RogueGameState || RogueGameState->IsRunActive())
	{
		return;
	}

	FString ResultText;
	FLinearColor ResultColor = TimerColor;

	switch (RogueGameState->GetRunPhase())
	{
	case ERogue10mRunPhase::Victory:
		ResultText = TEXT("클리어");
		ResultColor = FLinearColor(0.2f, 1.0f, 0.35f, 1.0f);
		break;
	case ERogue10mRunPhase::Defeat:
		ResultText = TEXT("게임 오버");
		ResultColor = WarningColor;
		break;
	default:
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f - 150.0f;
	const float CenterY = Canvas->SizeY * 0.5f - 40.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), CenterX - 48.0f, CenterY - 28.0f, 396.0f, 116.0f);
	DrawText(ResultText, ResultColor, CenterX, CenterY, nullptr, ResultScale, false);
	DrawText(TEXT("제한 시간이 끝나 캐릭터가 사망했습니다."), TimerColor, CenterX - 20.0f, CenterY + 52.0f, nullptr, 1.1f, false);
}

void ARogue10mHUD::DrawVitals()
{
	if (!Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	const ARogue10mPlayerState* RoguePlayerState = GetOwningPlayerController() ? GetOwningPlayerController()->GetPlayerState<ARogue10mPlayerState>() : nullptr;
	if (!VitalsComponent)
	{
		return;
	}

	const float BarWidth = FMath::Clamp(Canvas->SizeX * 0.145f, 260.0f, 390.0f);
	const FVector2D BarSize(BarWidth, 16.0f);
	const FVector2D HealthPosition(Canvas->SizeX * 0.305f, Canvas->SizeY * 0.836f);
	const FVector2D StaminaPosition(Canvas->SizeX * 0.535f, Canvas->SizeY * 0.836f);

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.28f), HealthPosition.X - 8.0f, HealthPosition.Y - 8.0f, BarWidth + 16.0f, 32.0f);
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.28f), StaminaPosition.X - 8.0f, StaminaPosition.Y - 8.0f, BarWidth + 16.0f, VitalsComponent->ShouldShowMana() ? 54.0f : 32.0f);
	DrawVitalBar(TEXT("체력"), VitalsComponent->GetHealth(), HealthPosition, BarSize, FLinearColor(0.92f, 0.12f, 0.12f, 1.0f));
	DrawVitalBar(TEXT("스테미나"), VitalsComponent->GetStamina(), StaminaPosition, BarSize, FLinearColor(0.16f, 0.86f, 0.32f, 1.0f));

	if (VitalsComponent->ShouldShowMana())
	{
		DrawVitalBar(TEXT("마나"), VitalsComponent->GetMana(), StaminaPosition + FVector2D(0.0f, 22.0f), BarSize, FLinearColor(0.15f, 0.42f, 1.0f, 1.0f));
	}

	if (RoguePlayerState && RogueCharacter)
	{
		const FVector2D LevelPosition(0.0f, Canvas->SizeY * 0.9684f);
		const FVector2D LevelSize(Canvas->SizeX * 0.3915f, Canvas->SizeY * 0.0316f);
		const float ExperienceBarHeight = FMath::Max(6.0f, LevelSize.Y - 14.0f);
		DrawRect(FLinearColor(0.015f, 0.02f, 0.03f, 0.88f), LevelPosition.X, LevelPosition.Y, LevelSize.X, LevelSize.Y);
		DrawText(FString::Printf(TEXT("Lv.%d"), RoguePlayerState->GetPlayerLevel()), FLinearColor(0.92f, 0.86f, 0.56f, 1.0f), LevelPosition.X + 14.0f, LevelPosition.Y + 5.0f, nullptr, 0.68f, false);
		DrawRect(FLinearColor(0.04f, 0.055f, 0.07f, 0.96f), LevelPosition.X + 92.0f, LevelPosition.Y + 7.0f, LevelSize.X - 108.0f, ExperienceBarHeight);
		DrawRect(FLinearColor(0.45f, 0.72f, 1.0f, 0.95f), LevelPosition.X + 92.0f, LevelPosition.Y + 7.0f, (LevelSize.X - 108.0f) * RoguePlayerState->GetExperienceNormalized(), ExperienceBarHeight);

		const FVector2D IdentityPosition(Canvas->SizeX * 0.445f, Canvas->SizeY * 0.858f);
		const FVector2D IdentitySize(FMath::Clamp(Canvas->SizeX * 0.075f, 92.0f, 134.0f), FMath::Clamp(Canvas->SizeY * 0.095f, 68.0f, 92.0f));
		const float IdentityBarHeight = 8.0f;
		DrawRect(FLinearColor(0.02f, 0.024f, 0.032f, 0.88f), IdentityPosition.X, IdentityPosition.Y, IdentitySize.X, IdentitySize.Y);
		DrawRect(FLinearColor(0.58f, 0.64f, 0.72f, 0.42f), IdentityPosition.X + 2.0f, IdentityPosition.Y + 2.0f, IdentitySize.X - 4.0f, IdentitySize.Y - 4.0f);
		DrawText(TEXT("아이덴티티"), FLinearColor(0.72f, 0.94f, 0.9f, 1.0f), IdentityPosition.X + 12.0f, IdentityPosition.Y + 10.0f, nullptr, 0.58f, false);
		DrawText(FString::Printf(TEXT("%s 숙련 %d"), *GetWeaponTypeText(RogueCharacter->GetEquippedWeaponType()), RoguePlayerState->GetWeaponMasteryLevel(RogueCharacter->GetEquippedWeaponType())), InventoryTextColor, IdentityPosition.X + 10.0f, IdentityPosition.Y + 34.0f, nullptr, 0.52f, false);
		DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), IdentityPosition.X + 8.0f, IdentityPosition.Y + IdentitySize.Y - 16.0f, IdentitySize.X - 16.0f, IdentityBarHeight);
		DrawRect(FLinearColor(0.18f, 0.88f, 0.72f, 0.95f), IdentityPosition.X + 8.0f, IdentityPosition.Y + IdentitySize.Y - 16.0f, (IdentitySize.X - 16.0f) * RoguePlayerState->GetIdentityNormalized(), IdentityBarHeight);
		DrawText(FString::Printf(TEXT("EXP %d / %d"), RoguePlayerState->GetCurrentExperience(), RoguePlayerState->GetExperienceToNextLevel()), FLinearColor(0.62f, 0.75f, 0.95f, 1.0f), LevelPosition.X + 106.0f, LevelPosition.Y + 5.0f, nullptr, 0.56f, false);
	}
}

void ARogue10mHUD::DrawVitalBar(const FString& Label, const FRogue10mVitalValue& Vital, const FVector2D& Position, const FVector2D& Size, const FLinearColor& FillColor)
{
	const float FillWidth = Size.X * Vital.GetNormalized();
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.58f), Position.X - 4.0f, Position.Y - 4.0f, Size.X + 8.0f, Size.Y + 8.0f);
	DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), Position.X, Position.Y, Size.X, Size.Y);
	DrawRect(FillColor, Position.X, Position.Y, FillWidth, Size.Y);
	DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.18f), Position.X, Position.Y, Size.X, 2.0f);

	DrawText(Label, InventoryTextColor, Position.X + 8.0f, Position.Y + 2.0f, nullptr, 0.68f, false);
	DrawText(FString::Printf(TEXT("%.0f / %.0f"), Vital.Current, Vital.Max), InventoryTextColor, Position.X + Size.X - 92.0f, Position.Y + 2.0f, nullptr, 0.68f, false);
}

void ARogue10mHUD::DrawMiniMap()
{
	if (!Canvas)
	{
		return;
	}

	const APawn* OwningPawn = GetOwningPawn();
	const FVector2D MapSize(Canvas->SizeX * 0.2012f, Canvas->SizeY * 0.3041f);
	const FVector2D MapPosition(Canvas->SizeX * 0.7988f, 0.0f);
	const FVector PawnLocation = OwningPawn ? OwningPawn->GetActorLocation() : FVector::ZeroVector;
	const float PawnYaw = OwningPawn ? OwningPawn->GetActorRotation().Yaw : 0.0f;

	DrawRect(FLinearColor(0.01f, 0.014f, 0.018f, 0.78f), MapPosition.X, MapPosition.Y, MapSize.X, MapSize.Y);
	DrawRect(FLinearColor(0.42f, 0.52f, 0.58f, 0.65f), MapPosition.X, MapPosition.Y, MapSize.X, 2.0f);
	DrawRect(FLinearColor(0.42f, 0.52f, 0.58f, 0.65f), MapPosition.X, MapPosition.Y + MapSize.Y - 2.0f, MapSize.X, 2.0f);
	DrawRect(FLinearColor(0.42f, 0.52f, 0.58f, 0.65f), MapPosition.X, MapPosition.Y, 2.0f, MapSize.Y);
	DrawRect(FLinearColor(0.42f, 0.52f, 0.58f, 0.65f), MapPosition.X + MapSize.X - 2.0f, MapPosition.Y, 2.0f, MapSize.Y);

	const float CenterX = MapPosition.X + MapSize.X * 0.5f;
	const float CenterY = MapPosition.Y + MapSize.Y * 0.5f;
	DrawRect(FLinearColor(0.55f, 0.62f, 0.68f, 0.18f), CenterX - 1.0f, MapPosition.Y + 14.0f, 2.0f, MapSize.Y - 28.0f);
	DrawRect(FLinearColor(0.55f, 0.62f, 0.68f, 0.18f), MapPosition.X + 14.0f, CenterY - 1.0f, MapSize.X - 28.0f, 2.0f);

	DrawText(TEXT("N"), FLinearColor(0.72f, 0.9f, 1.0f, 1.0f), CenterX - 4.0f, MapPosition.Y + 7.0f, nullptr, 0.62f, false);
	DrawRect(FLinearColor(0.28f, 0.86f, 1.0f, 0.95f), CenterX - 5.0f, CenterY - 5.0f, 10.0f, 10.0f);
	DrawText(FString::Printf(TEXT("%.0f"), PawnYaw), FLinearColor(0.72f, 0.78f, 0.86f, 1.0f), MapPosition.X + 10.0f, MapPosition.Y + MapSize.Y - 22.0f, nullptr, 0.52f, false);
	DrawText(FString::Printf(TEXT("X %.0f  Y %.0f"), PawnLocation.X, PawnLocation.Y), FLinearColor(0.72f, 0.78f, 0.86f, 1.0f), MapPosition.X + 10.0f, MapPosition.Y + MapSize.Y - 42.0f, nullptr, 0.56f, false);

	struct FMinimapMarker
	{
		FVector2D Offset;
		FLinearColor Color;
		float Size;
	};

	const FMinimapMarker Markers[] =
	{
		{ FVector2D(-0.27f, -0.18f), FLinearColor(0.92f, 0.18f, 0.12f, 0.95f), 7.0f },
		{ FVector2D(0.31f, -0.06f), FLinearColor(0.35f, 0.9f, 0.42f, 0.95f), 6.0f },
		{ FVector2D(-0.12f, 0.28f), FLinearColor(0.96f, 0.78f, 0.28f, 0.95f), 6.0f },
		{ FVector2D(0.18f, 0.24f), FLinearColor(0.55f, 0.68f, 1.0f, 0.95f), 5.0f }
	};
	for (const FMinimapMarker& Marker : Markers)
	{
		DrawRect(Marker.Color, CenterX + Marker.Offset.X * MapSize.X - Marker.Size * 0.5f, CenterY + Marker.Offset.Y * MapSize.Y - Marker.Size * 0.5f, Marker.Size, Marker.Size);
	}

	DrawText(TEXT("적"), FLinearColor(0.92f, 0.18f, 0.12f, 1.0f), MapPosition.X + 12.0f, MapPosition.Y + MapSize.Y - 64.0f, nullptr, 0.46f, false);
	DrawText(TEXT("NPC"), FLinearColor(0.35f, 0.9f, 0.42f, 1.0f), MapPosition.X + 46.0f, MapPosition.Y + MapSize.Y - 64.0f, nullptr, 0.46f, false);
}

void ARogue10mHUD::DrawCombatLog()
{
	if (!Canvas || !GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	for (int32 Index = CombatLogEntries.Num() - 1; Index >= 0; --Index)
	{
		if (CombatLogEntries[Index].ExpireTime <= CurrentTime)
		{
			CombatLogEntries.RemoveAt(Index);
		}
	}

	const FVector2D PanelPosition(Canvas->SizeX * 0.008f, Canvas->SizeY * 0.69f);
	const float LineHeight = FMath::Clamp(Canvas->SizeY * 0.018f, 16.0f, 22.0f);
	const float PanelWidth = FMath::Clamp(Canvas->SizeX * 0.205f, 300.0f, 430.0f);
	const float PanelHeight = FMath::Clamp(Canvas->SizeY * 0.245f, 180.0f, 270.0f);
	if (!bShowCombatLog)
	{
		return;
	}

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.34f), PanelPosition.X, PanelPosition.Y, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.6f, 0.64f, 0.68f, 0.28f), PanelPosition.X, PanelPosition.Y, PanelWidth, 1.0f);
	DrawText(TEXT("시스템"), FLinearColor(0.86f, 0.92f, 1.0f, 1.0f), PanelPosition.X + 10.0f, PanelPosition.Y + 8.0f, nullptr, 0.62f, false);
	if (CombatLogEntries.Num() <= 0)
	{
		DrawText(TEXT("표시할 로그가 없습니다."), FLinearColor(0.62f, 0.68f, 0.76f, 1.0f), PanelPosition.X + 10.0f, PanelPosition.Y + 34.0f, nullptr, 0.58f, false);
		return;
	}

	const int32 MaxVisibleLines = FMath::Max(1, FMath::FloorToInt((PanelHeight - 42.0f) / LineHeight));
	const int32 VisibleLineCount = FMath::Min(MaxVisibleLines, CombatLogEntries.Num());
	for (int32 Index = 0; Index < VisibleLineCount; ++Index)
	{
		const FRogue10mCombatLogEntry& Entry = CombatLogEntries[Index];
		DrawText(Entry.Message, Entry.Color, PanelPosition.X + 10.0f, PanelPosition.Y + 34.0f + Index * LineHeight, nullptr, 0.58f, false);
	}

	if (CombatLogEntries.Num() > MaxVisibleLines)
	{
		const float TrackX = PanelPosition.X + PanelWidth - 8.0f;
		const float TrackY = PanelPosition.Y + 32.0f;
		const float TrackHeight = PanelHeight - 42.0f;
		const float ThumbHeight = FMath::Clamp(TrackHeight * static_cast<float>(MaxVisibleLines) / static_cast<float>(CombatLogEntries.Num()), 24.0f, TrackHeight);
		DrawRect(FLinearColor(0.35f, 0.38f, 0.42f, 0.42f), TrackX, TrackY, 3.0f, TrackHeight);
		DrawRect(FLinearColor(0.8f, 0.86f, 0.92f, 0.62f), TrackX, TrackY, 3.0f, ThumbHeight);
	}
}

void ARogue10mHUD::DrawFloatingDamageNumbers()
{
	if (!Canvas || !GetWorld())
	{
		return;
	}

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!OwningPlayerController)
	{
		return;
	}

	if (!bShowFloatingDamageNumbers)
	{
		FloatingDamageEntries.Reset();
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	for (int32 Index = FloatingDamageEntries.Num() - 1; Index >= 0; --Index)
	{
		FRogue10mFloatingDamageEntry& Entry = FloatingDamageEntries[Index];
		AActor* TargetActor = Entry.TargetActor.Get();
		if (!TargetActor || Entry.ExpireTime <= CurrentTime)
		{
			FloatingDamageEntries.RemoveAt(Index);
			continue;
		}

		const float LifeAlpha = FMath::Clamp((CurrentTime - Entry.StartTime) / FMath::Max(0.01f, Entry.ExpireTime - Entry.StartTime), 0.0f, 1.0f);
		const FVector WorldLocation = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, 120.0f + LifeAlpha * 45.0f);
		FVector2D ScreenPosition;
		if (!OwningPlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			continue;
		}

		const float FadeAlpha = 1.0f - LifeAlpha;
		const FString DamageText = FString::Printf(TEXT("-%.0f"), Entry.DamageAmount);
		const FLinearColor ShadowColor(0.0f, 0.0f, 0.0f, 0.65f * FadeAlpha);
		const FLinearColor DamageColor(1.0f, 0.72f, 0.22f, FadeAlpha);
		DrawText(DamageText, ShadowColor, ScreenPosition.X + 2.0f, ScreenPosition.Y + 2.0f, nullptr, 1.2f, false);
		DrawText(DamageText, DamageColor, ScreenPosition.X, ScreenPosition.Y, nullptr, 1.2f, false);
	}
}

void ARogue10mHUD::DrawPlayerDamageFeedback()
{
	if (!Canvas || !GetWorld())
	{
		return;
	}

	if (!bShowPlayerDamageFeedback)
	{
		PlayerDamageFeedbackEndTime = 0.0f;
		PlayerDamageFeedbackStrength = 0.0f;
		return;
	}

	if (PlayerDamageFeedbackEndTime <= 0.0f)
	{
		return;
	}

	const float RemainingTime = PlayerDamageFeedbackEndTime - GetWorld()->GetTimeSeconds();
	if (RemainingTime <= 0.0f)
	{
		PlayerDamageFeedbackEndTime = 0.0f;
		PlayerDamageFeedbackStrength = 0.0f;
		return;
	}

	const float Alpha = FMath::Clamp(RemainingTime / 0.35f, 0.0f, 1.0f) * PlayerDamageFeedbackStrength;
	DrawRect(FLinearColor(0.8f, 0.02f, 0.02f, 0.16f * Alpha), 0.0f, 0.0f, Canvas->SizeX, Canvas->SizeY);
	DrawRect(FLinearColor(0.9f, 0.03f, 0.02f, 0.26f * Alpha), 0.0f, 0.0f, Canvas->SizeX, 28.0f);
	DrawRect(FLinearColor(0.9f, 0.03f, 0.02f, 0.26f * Alpha), 0.0f, Canvas->SizeY - 28.0f, Canvas->SizeX, 28.0f);
	DrawRect(FLinearColor(0.9f, 0.03f, 0.02f, 0.2f * Alpha), 0.0f, 0.0f, 24.0f, Canvas->SizeY);
	DrawRect(FLinearColor(0.9f, 0.03f, 0.02f, 0.2f * Alpha), Canvas->SizeX - 24.0f, 0.0f, 24.0f, Canvas->SizeY);
}

void ARogue10mHUD::DrawAimCrossLine()
{
	if (!Canvas || IsAnyBlockingWindowVisible())
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	if (!RogueCharacter || RogueCharacter->IsDead())
	{
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f;
	const float CenterY = Canvas->SizeY * 0.5f;
	const float LineLength = AimCrossLineLength;
	const float Gap = AimCrossLineGap;
	const float Thickness = AimCrossLineThickness;
	const FLinearColor ShadowColor(0.0f, 0.0f, 0.0f, 0.42f);

	// 중앙 공격점을 비워두고 네 방향 선만 그려 실제 타격 지점을 가리지 않게 합니다.
	DrawRect(ShadowColor, CenterX - Gap - LineLength + 1.0f, CenterY - Thickness * 0.5f + 1.0f, LineLength, Thickness);
	DrawRect(ShadowColor, CenterX + Gap + 1.0f, CenterY - Thickness * 0.5f + 1.0f, LineLength, Thickness);
	DrawRect(ShadowColor, CenterX - Thickness * 0.5f + 1.0f, CenterY - Gap - LineLength + 1.0f, Thickness, LineLength);
	DrawRect(ShadowColor, CenterX - Thickness * 0.5f + 1.0f, CenterY + Gap + 1.0f, Thickness, LineLength);

	DrawRect(AimCrossLineColor, CenterX - Gap - LineLength, CenterY - Thickness * 0.5f, LineLength, Thickness);
	DrawRect(AimCrossLineColor, CenterX + Gap, CenterY - Thickness * 0.5f, LineLength, Thickness);
	DrawRect(AimCrossLineColor, CenterX - Thickness * 0.5f, CenterY - Gap - LineLength, Thickness, LineLength);
	DrawRect(AimCrossLineColor, CenterX - Thickness * 0.5f, CenterY + Gap, Thickness, LineLength);
	DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.55f), CenterX - 1.0f, CenterY - 1.0f, 2.0f, 2.0f);
}

void ARogue10mHUD::DrawPanelShortcutHints()
{
	if (!Canvas)
	{
		return;
	}

	struct FMainHudButton
	{
		const TCHAR* Key;
		const TCHAR* Label;
		FLinearColor Color;
		bool bActive;
	};

	const FVector2D ButtonSize(FMath::Clamp(Canvas->SizeX * 0.042f, 54.0f, 74.0f), FMath::Clamp(Canvas->SizeY * 0.052f, 38.0f, 52.0f));
	const float ButtonGap = FMath::Clamp(Canvas->SizeX * 0.008f, 8.0f, 18.0f);
	const float ButtonY = Canvas->SizeY - ButtonSize.Y - 22.0f;
	const FMainHudButton Buttons[] =
	{
		{ TEXT("I"), TEXT("장비"), FLinearColor(0.92f, 0.72f, 0.35f, 1.0f), bInventoryVisible },
		{ TEXT("B"), TEXT("인벤"), FLinearColor(0.35f, 0.92f, 0.72f, 1.0f), bItemWindowVisible },
		{ TEXT("K"), TEXT("스킬"), FLinearColor(0.58f, 0.72f, 1.0f, 1.0f), bSkillTreeVisible },
		{ TEXT("O"), TEXT("설정"), FLinearColor(0.72f, 0.58f, 1.0f, 1.0f), bSettingsVisible }
	};
	const float TotalWidth = UE_ARRAY_COUNT(Buttons) * ButtonSize.X + (UE_ARRAY_COUNT(Buttons) - 1) * ButtonGap;
	const float StartX = Canvas->SizeX - TotalWidth - FMath::Clamp(Canvas->SizeX * 0.018f, 18.0f, 38.0f);

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Buttons); ++Index)
	{
		const FMainHudButton& Button = Buttons[Index];
		const float ButtonX = StartX + Index * (ButtonSize.X + ButtonGap);
		const FLinearColor FillColor = Button.bActive ? FLinearColor(0.12f, 0.18f, 0.26f, 0.94f) : FLinearColor(0.025f, 0.03f, 0.04f, 0.82f);

		DrawRect(Button.Color, ButtonX - 2.0f, ButtonY - 2.0f, ButtonSize.X + 4.0f, ButtonSize.Y + 4.0f);
		DrawRect(FillColor, ButtonX, ButtonY, ButtonSize.X, ButtonSize.Y);
		DrawText(Button.Key, Button.Color, ButtonX + 8.0f, ButtonY + 6.0f, nullptr, 0.62f, false);
		DrawText(Button.Label, InventoryTextColor, ButtonX + 8.0f, ButtonY + ButtonSize.Y - 20.0f, nullptr, 0.56f, false);
	}
}

void ARogue10mHUD::DrawQuickSlots()
{
	if (!Canvas || QuickSlots.Num() <= 0)
	{
		return;
	}

	const float SlotSize = FMath::Clamp(Canvas->SizeY * 0.052f, 42.0f, 56.0f);
	const float SlotGap = FMath::Max(6.0f, Canvas->SizeX * 0.004f);
	const int32 SkillSlotCount = GetVisibleSkillSlotCount();
	const int32 ItemSlotCount = 4;
	const float SkillGroupWidth = SkillSlotCount * SlotSize + FMath::Max(0, SkillSlotCount - 1) * SlotGap;
	const float ItemGroupWidth = ItemSlotCount * SlotSize + FMath::Max(0, ItemSlotCount - 1) * SlotGap;
	const float StartX = Canvas->SizeX * 0.305f;
	const float StartY = Canvas->SizeY * 0.8643f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.42f), StartX - 12.0f, StartY - 22.0f, SkillGroupWidth + 24.0f, SlotSize + 34.0f);
	DrawText(TEXT("스킬"), FLinearColor(0.68f, 0.78f, 0.9f, 1.0f), StartX, StartY - 18.0f, nullptr, 0.54f, false);

	for (int32 Index = 0; Index < SkillSlotCount; ++Index)
	{
		const FVector2D SlotPosition(StartX + Index * (SlotSize + SlotGap), StartY);
		if (QuickSlots.IsValidIndex(Index))
		{
			DrawQuickSlot(QuickSlots[Index], SlotPosition, SlotSize, FString::FromInt(Index + 1));
		}
		else
		{
			DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 0.32f), SlotPosition.X - 2.0f, SlotPosition.Y - 2.0f, SlotSize + 4.0f, SlotSize + 4.0f);
			DrawRect(FLinearColor(0.015f, 0.018f, 0.024f, 0.78f), SlotPosition.X, SlotPosition.Y, SlotSize, SlotSize);
			DrawText(TEXT("잠김"), FLinearColor(0.58f, 0.62f, 0.68f, 1.0f), SlotPosition.X + 8.0f, SlotPosition.Y + SlotSize - 17.0f, nullptr, 0.52f, false);
		}
	}

	if (ItemSlotCount > 0)
	{
		const float ItemStartX = Canvas->SizeX * 0.535f;
		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.42f), ItemStartX - 12.0f, StartY - 22.0f, ItemGroupWidth + 24.0f, SlotSize + 34.0f);
		DrawText(TEXT("사용 아이템"), FLinearColor(0.92f, 0.82f, 0.58f, 1.0f), ItemStartX, StartY - 18.0f, nullptr, 0.54f, false);

		for (int32 Index = 0; Index < ItemSlotCount; ++Index)
		{
			const int32 QuickSlotIndex = SkillSlotCount + Index;
			const FVector2D SlotPosition(ItemStartX + Index * (SlotSize + SlotGap), StartY);
			if (QuickSlots.IsValidIndex(QuickSlotIndex))
			{
				DrawQuickSlot(QuickSlots[QuickSlotIndex], SlotPosition, SlotSize, FString::FromInt(QuickSlots[QuickSlotIndex].SlotNumber));
			}
			else
			{
				DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 0.42f), SlotPosition.X - 2.0f, SlotPosition.Y - 2.0f, SlotSize + 4.0f, SlotSize + 4.0f);
				DrawRect(FLinearColor(0.025f, 0.03f, 0.04f, 0.78f), SlotPosition.X, SlotPosition.Y, SlotSize, SlotSize);
			}
		}
	}
}

void ARogue10mHUD::DrawQuickSlot(const FRogue10mQuickSlotView& QuickSlot, const FVector2D& Position, float SlotSize, const FString& KeyLabel)
{
	const float CooldownRemaining = GetQuickSlotCooldownRemaining(QuickSlot);
	const bool bOnCooldown = CooldownRemaining > 0.0f;
	const float InnerPadding = 6.0f;
	const float KeyBoxSize = 15.0f;

	DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 0.88f), Position.X - 2.0f, Position.Y - 2.0f, SlotSize + 4.0f, SlotSize + 4.0f);
	DrawRect(FLinearColor(0.025f, 0.03f, 0.04f, 0.94f), Position.X, Position.Y, SlotSize, SlotSize);
	DrawRect(QuickSlot.IconColor, Position.X + InnerPadding, Position.Y + InnerPadding, SlotSize - InnerPadding * 2.0f, SlotSize - InnerPadding * 2.0f);
	DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.14f), Position.X + InnerPadding, Position.Y + InnerPadding, SlotSize - InnerPadding * 2.0f, 3.0f);

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.72f), Position.X + 3.0f, Position.Y + 3.0f, KeyBoxSize, KeyBoxSize);
	DrawText(KeyLabel, InventoryTextColor, Position.X + 7.0f, Position.Y + 4.0f, nullptr, 0.62f, false);

	if (bOnCooldown)
	{
		const float CooldownAlpha = QuickSlot.CooldownDuration > 0.0f ? FMath::Clamp(CooldownRemaining / QuickSlot.CooldownDuration, 0.0f, 1.0f) : 0.0f;
		// 남은 쿨타임 비율만큼 어두운 오버레이를 덮고 숫자를 표시합니다.
		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.62f), Position.X, Position.Y, SlotSize, SlotSize * CooldownAlpha);
		DrawText(FString::Printf(TEXT("%.1f"), CooldownRemaining), FLinearColor(1.0f, 0.92f, 0.58f, 1.0f), Position.X + SlotSize * 0.5f - 12.0f, Position.Y + SlotSize * 0.5f - 7.0f, nullptr, 0.72f, false);
	}

	DrawText(QuickSlot.DisplayName.ToString(), InventoryTextColor, Position.X + 6.0f, Position.Y + SlotSize - 15.0f, nullptr, 0.56f, false);
}

float ARogue10mHUD::GetQuickSlotCooldownRemaining(const FRogue10mQuickSlotView& QuickSlot) const
{
	const UWorld* World = GetWorld();
	return World ? FMath::Max(0.0f, QuickSlot.CooldownEndTime - World->GetTimeSeconds()) : 0.0f;
}

void ARogue10mHUD::DrawAttackCooldownSlot()
{
	if (!Canvas)
	{
		return;
	}

	const APlayerController* PlayerController = GetOwningPlayerController();
	const ARogue10mCharacter* RogueCharacter = PlayerController ? Cast<ARogue10mCharacter>(PlayerController->GetPawn()) : nullptr;
	if (!RogueCharacter)
	{
		return;
	}

	const URogue10mAttackSkillData* DisplaySkill = RogueCharacter->GetDisplayedAttackSkillForHUD();
	if (!DisplaySkill)
	{
		return;
	}

	const float SlotSize = FMath::Clamp(Canvas->SizeY * 0.058f, 48.0f, 60.0f);
	const float PanelWidth = SlotSize + 150.0f;
	const float PanelHeight = SlotSize + 18.0f;
	const float PanelX = Canvas->SizeX * 0.4152f - (PanelWidth - Canvas->SizeX * 0.1072f) * 0.5f;
	const float PanelY = Canvas->SizeY * 0.656f;
	const FVector2D IconPosition(PanelX + 9.0f, PanelY + 9.0f);
	const float CooldownRemaining = RogueCharacter->GetAttackCooldownRemaining();
	const float CooldownDuration = RogueCharacter->GetAttackCooldownDuration();
	const bool bOnCooldown = CooldownRemaining > 0.0f && CooldownDuration > 0.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.44f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.34f, 0.38f, 0.45f, 0.88f), IconPosition.X - 2.0f, IconPosition.Y - 2.0f, SlotSize + 4.0f, SlotSize + 4.0f);
	DrawRect(FLinearColor(0.025f, 0.03f, 0.04f, 0.96f), IconPosition.X, IconPosition.Y, SlotSize, SlotSize);

	if (DisplaySkill->SkillIcon)
	{
		DrawTexture(DisplaySkill->SkillIcon, IconPosition.X + 5.0f, IconPosition.Y + 5.0f, SlotSize - 10.0f, SlotSize - 10.0f, 0.0f, 0.0f, 1.0f, 1.0f, DisplaySkill->IconTint, BLEND_Translucent);
	}
	else
	{
		DrawRect(DisplaySkill->IconTint, IconPosition.X + 6.0f, IconPosition.Y + 6.0f, SlotSize - 12.0f, SlotSize - 12.0f);
		DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.16f), IconPosition.X + 6.0f, IconPosition.Y + 6.0f, SlotSize - 12.0f, 4.0f);
		DrawText(DisplaySkill->IconLabel.ToString(), InventoryTextColor, IconPosition.X + SlotSize * 0.5f - 9.0f, IconPosition.Y + SlotSize * 0.5f - 9.0f, nullptr, 0.78f, false);
	}

	if (bOnCooldown)
	{
		const float CooldownAlpha = FMath::Clamp(CooldownRemaining / CooldownDuration, 0.0f, 1.0f);
		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.64f), IconPosition.X, IconPosition.Y, SlotSize, SlotSize * CooldownAlpha);
		DrawText(FString::Printf(TEXT("%.1f"), CooldownRemaining), FLinearColor(1.0f, 0.92f, 0.58f, 1.0f), IconPosition.X + SlotSize * 0.5f - 13.0f, IconPosition.Y + SlotSize * 0.5f + 8.0f, nullptr, 0.72f, false);
	}

	const float TextX = IconPosition.X + SlotSize + 12.0f;
	DrawText(TEXT("공격"), FLinearColor(0.68f, 0.76f, 0.86f, 1.0f), TextX, PanelY + 13.0f, nullptr, 0.58f, false);
	DrawText(DisplaySkill->SkillName.ToString(), InventoryTextColor, TextX, PanelY + 31.0f, nullptr, 0.72f, false);
	DrawText(bOnCooldown ? TEXT("쿨타임") : TEXT("사용 가능"), bOnCooldown ? FLinearColor(1.0f, 0.76f, 0.36f, 1.0f) : FLinearColor(0.58f, 1.0f, 0.66f, 1.0f), TextX, PanelY + 53.0f, nullptr, 0.58f, false);
}

void ARogue10mHUD::DrawCharacterInfo()
{
	if (!Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!RogueCharacter || !VitalsComponent)
	{
		return;
	}

	const float PanelWidth = 300.0f;
	const float PanelHeight = VitalsComponent->ShouldShowMana() ? 166.0f : 148.0f;
	const float PanelX = Canvas->SizeX - PanelWidth - 28.0f;
	const float PanelY = FMath::Clamp(Canvas->SizeY * 0.18f, 150.0f, 196.0f) + 58.0f;
	const float TextScale = 0.72f;
	const float LineHeight = 18.0f;
	float TextY = PanelY + 14.0f;

	const FString StateText = RogueCharacter->IsDead()
		? TEXT("사망")
		: (IsAnyBlockingWindowVisible() ? TEXT("UI 조작 중") : TEXT("준비"));

	DrawRect(FLinearColor(0.01f, 0.014f, 0.02f, 0.72f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.55f, 0.72f, 0.95f, 0.85f), PanelX, PanelY, PanelWidth, 2.0f);

	DrawText(TEXT("캐릭터 정보"), FLinearColor(0.78f, 0.86f, 1.0f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight + 4.0f;
	DrawText(FString::Printf(TEXT("이름: %s"), *RogueCharacter->GetCharacterDisplayName().ToString()), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("직업: %s"), *RogueCharacter->GetCharacterJobName().ToString()), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("상태: %s"), *StateText), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("무기: %s"), *GetWeaponTypeText(RogueCharacter->GetEquippedWeaponType())), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("체력: %.0f / %.0f"), VitalsComponent->GetHealth().Current, VitalsComponent->GetHealth().Max), FLinearColor(1.0f, 0.72f, 0.72f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	if (VitalsComponent->ShouldShowMana())
	{
		DrawText(FString::Printf(TEXT("마나: %.0f / %.0f"), VitalsComponent->GetMana().Current, VitalsComponent->GetMana().Max), FLinearColor(0.68f, 0.8f, 1.0f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
		TextY += LineHeight;
	}
	DrawText(FString::Printf(TEXT("스테미나: %.0f / %.0f"), VitalsComponent->GetStamina().Current, VitalsComponent->GetStamina().Max), FLinearColor(0.7f, 1.0f, 0.72f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
}

void ARogue10mHUD::DrawLookedAtMonsterInfo()
{
	if (!Canvas || IsAnyBlockingWindowVisible())
	{
		return;
	}

	const ARogue10mBasicMonster* Monster = FindLookedAtMonster();
	const URogue10mVitalsComponent* VitalsComponent = Monster ? Monster->GetVitalsComponent() : nullptr;
	if (!Monster || !VitalsComponent)
	{
		return;
	}

	const FRogue10mVitalValue& Health = VitalsComponent->GetHealth();
	const float PanelWidth = Canvas->SizeX * 0.4184f;
	const float PanelHeight = FMath::Max(56.0f, Canvas->SizeY * 0.0608f);
	const float PanelX = Canvas->SizeX * 0.2566f;
	const float PanelY = Canvas->SizeY * 0.0222f;
	const float BarX = PanelX + 18.0f;
	const float BarY = PanelY + 44.0f;
	const float BarWidth = PanelWidth - 36.0f;
	const float BarHeight = 12.0f;
	const FString StateText = Monster->IsDead() ? TEXT("사망") : TEXT("적대");
	const float HealthPercent = Health.GetNormalized() * 100.0f;

	DrawRect(FLinearColor(0.01f, 0.012f, 0.016f, 0.78f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.84f, 0.22f, 0.18f, 0.95f), PanelX, PanelY, PanelWidth, 3.0f);
	DrawText(Monster->GetMonsterDisplayName().ToString(), InventoryTextColor, PanelX + 18.0f, PanelY + 14.0f, nullptr, 0.9f, false);
	DrawText(FString::Printf(TEXT("Lv.%d  %s  %s"), Monster->GetMonsterLevel(), *Monster->GetMonsterAttributeText().ToString(), *StateText), FLinearColor(1.0f, 0.72f, 0.5f, 1.0f), PanelX + PanelWidth - 210.0f, PanelY + 15.0f, nullptr, 0.68f, false);

	DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), BarX, BarY, BarWidth, BarHeight);
	DrawRect(FLinearColor(0.92f, 0.1f, 0.08f, 1.0f), BarX, BarY, BarWidth * Health.GetNormalized(), BarHeight);
	DrawText(FString::Printf(TEXT("체력 %.0f / %.0f  %.0f%%"), Health.Current, Health.Max, HealthPercent), InventoryTextColor, BarX + BarWidth - 178.0f, BarY - 2.0f, nullptr, 0.62f, false);
}

void ARogue10mHUD::DrawItemAcquisitionPanel()
{
	if (!Canvas || IsAnyBlockingWindowVisible())
	{
		return;
	}

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	for (int32 Index = ItemAcquisitionEntries.Num() - 1; Index >= 0; --Index)
	{
		if (ItemAcquisitionEntries[Index].ExpireTime <= CurrentTime)
		{
			ItemAcquisitionEntries.RemoveAt(Index);
		}
	}

	if (ItemAcquisitionEntries.Num() <= 0)
	{
		return;
	}

	const FVector2D PanelPosition(Canvas->SizeX * 0.535f, Canvas->SizeY * 0.30f);
	const float PanelWidth = FMath::Clamp(Canvas->SizeX * 0.18f, 240.0f, 360.0f);
	const float RowHeight = FMath::Clamp(Canvas->SizeY * 0.035f, 28.0f, 38.0f);
	const int32 VisibleCount = FMath::Min(5, ItemAcquisitionEntries.Num());

	for (int32 Index = 0; Index < VisibleCount; ++Index)
	{
		const FRogue10mItemAcquisitionEntry& Entry = ItemAcquisitionEntries[Index];
		const float RemainingAlpha = FMath::Clamp((Entry.ExpireTime - CurrentTime) / 3.0f, 0.0f, 1.0f);
		const float RowY = PanelPosition.Y + Index * (RowHeight + 5.0f);
		const FLinearColor BackColor(0.0f, 0.0f, 0.0f, 0.16f + 0.24f * RemainingAlpha);
		const FLinearColor TextColor(Entry.Color.R, Entry.Color.G, Entry.Color.B, FMath::Clamp(0.45f + 0.55f * RemainingAlpha, 0.0f, 1.0f));

		DrawRect(BackColor, PanelPosition.X, RowY, PanelWidth, RowHeight);
		DrawRect(FLinearColor(0.95f, 0.78f, 0.3f, 0.58f * RemainingAlpha), PanelPosition.X, RowY, 3.0f, RowHeight);
		DrawText(Entry.Message, TextColor, PanelPosition.X + 12.0f, RowY + 7.0f, nullptr, 0.58f, false);
	}
}

void ARogue10mHUD::DrawInventory()
{
	if (!bInventoryVisible || !Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.5f;
	const float PanelHeight = Canvas->SizeY * 0.5f;
	if (InventoryWindowPosition.X < 0.0f || InventoryWindowPosition.Y < 0.0f)
	{
		InventoryWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f, (Canvas->SizeY - PanelHeight) * 0.5f);
	}

	InventoryWindowPosition = ClampWindowPosition(InventoryWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = InventoryWindowPosition.X;
	const float PanelY = InventoryWindowPosition.Y;
	const float SlotSize = FMath::Clamp(PanelHeight * 0.105f, 44.0f, 64.0f);
	const float SlotGap = FMath::Clamp(PanelHeight * 0.022f, 8.0f, 12.0f);

	DrawRect(InventoryPanelColor, PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.1f, 0.95f), PanelX, PanelY, PanelWidth, 56.0f);
	DrawText(TEXT("장비창"), InventoryTextColor, PanelX + 28.0f, PanelY + 22.0f, nullptr, 1.35f, false);
	DrawText(TEXT("I : 닫기  |  제목줄 드래그"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 26.0f, nullptr, 0.85f, false);

	DrawCharacterPreview(RogueCharacter, PanelX + PanelWidth * 0.27f, PanelY + 62.0f, PanelWidth * 0.46f, PanelHeight - 78.0f);
	DrawEquipmentCharacterStats(RogueCharacter, PanelX + PanelWidth * 0.30f, PanelY + PanelHeight - 118.0f, PanelWidth * 0.40f, 92.0f);
	DrawInventorySlots(InventoryComponent->GetLeftEquipmentSlots(), PanelX + 34.0f, PanelY + 74.0f, SlotSize, SlotGap, false);
	DrawInventorySlots(InventoryComponent->GetRightEquipmentSlots(), PanelX + PanelWidth - SlotSize - 34.0f, PanelY + 74.0f, SlotSize, SlotGap, true);
}

void ARogue10mHUD::DrawItemWindow()
{
	if (!bItemWindowVisible || !Canvas)
	{
		return;
	}

	ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	if (!InventoryComponent)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.3f;
	const float PanelHeight = Canvas->SizeY * 0.5f;
	if (ItemWindowPosition.X < 0.0f || ItemWindowPosition.Y < 0.0f)
	{
		ItemWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f + 48.0f, (Canvas->SizeY - PanelHeight) * 0.5f + 36.0f);
	}

	ItemWindowPosition = ClampWindowPosition(ItemWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = ItemWindowPosition.X;
	const float PanelY = ItemWindowPosition.Y;
	const float HeaderHeight = 58.0f;

	DrawRect(InventoryPanelColor, PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.1f, 0.96f), PanelX, PanelY, PanelWidth, HeaderHeight);
	DrawText(TEXT("아이템"), InventoryTextColor, PanelX + PanelWidth * 0.5f - 36.0f, PanelY + 18.0f, nullptr, 1.35f, false);
	DrawText(TEXT("B : 닫기"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 112.0f, PanelY + 22.0f, nullptr, 0.85f, false);

	const float TabY = PanelY + HeaderHeight + 10.0f;
	const float TabSize = 38.0f;
	for (int32 Index = 0; Index < 8; ++Index)
	{
		const float TabX = PanelX + 22.0f + Index * (TabSize + 10.0f);
		const FLinearColor TabColor = Index == 1 ? FLinearColor(0.92f, 0.74f, 0.42f, 1.0f) : FLinearColor(0.18f, 0.2f, 0.22f, 1.0f);
		DrawRect(TabColor, TabX, TabY, TabSize, TabSize);
	}

	DrawItemGrid(InventoryComponent, PanelX + 18.0f, PanelY + 112.0f, 48.0f, 6.0f, PanelWidth - 36.0f, PanelHeight - 166.0f);

	const float FooterY = PanelY + PanelHeight - 44.0f;
	DrawRect(FLinearColor(0.03f, 0.04f, 0.05f, 0.96f), PanelX, FooterY, PanelWidth, 44.0f);
	DrawText(FString::Printf(TEXT("골드  %d"), InventoryComponent->GetGold()), FLinearColor(0.35f, 1.0f, 0.72f, 1.0f), PanelX + PanelWidth * 0.44f, FooterY + 12.0f, nullptr, 1.0f, false);
	DrawText(FString::Printf(TEXT("수정  %d"), InventoryComponent->GetCrystals()), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f), PanelX + PanelWidth * 0.72f, FooterY + 12.0f, nullptr, 1.0f, false);
}

void ARogue10mHUD::DrawSkillTree()
{
	if (!bSkillTreeVisible || !Canvas)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.56f;
	const float PanelHeight = Canvas->SizeY * 0.62f;
	if (SkillTreeWindowPosition.X < 0.0f || SkillTreeWindowPosition.Y < 0.0f)
	{
		SkillTreeWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f, (Canvas->SizeY - PanelHeight) * 0.5f);
	}

	SkillTreeWindowPosition = ClampWindowPosition(SkillTreeWindowPosition, FVector2D(PanelWidth, PanelHeight));
	const float PanelX = SkillTreeWindowPosition.X;
	const float PanelY = SkillTreeWindowPosition.Y;

	DrawRect(FLinearColor(0.012f, 0.016f, 0.024f, 0.96f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.105f, 0.98f), PanelX, PanelY, PanelWidth, 58.0f);
	DrawText(TEXT("스킬트리"), InventoryTextColor, PanelX + 24.0f, PanelY + 19.0f, nullptr, 1.35f, false);
	DrawText(TEXT("K : 닫기  |  제목줄 드래그"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 260.0f, PanelY + 23.0f, nullptr, 0.84f, false);

	if (SkillTreeView == ERogue10mSkillTreeView::WeaponSelect)
	{
		DrawSkillTreeWeaponSelect(PanelX, PanelY, PanelWidth, PanelHeight);
	}
	else
	{
		DrawSkillTreeWeaponDetail(PanelX, PanelY, PanelWidth, PanelHeight);
	}
}

void ARogue10mHUD::DrawSkillTreeWeaponSelect(float PanelX, float PanelY, float PanelWidth, float PanelHeight)
{
	const TArray<ERogue10mWeaponType> WeaponTypes =
	{
		ERogue10mWeaponType::Knuckle,
		ERogue10mWeaponType::Dagger,
		ERogue10mWeaponType::GreatSword,
		ERogue10mWeaponType::DualBlades,
		ERogue10mWeaponType::Bow,
		ERogue10mWeaponType::Staff
	};

	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bLeftClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bLeftClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	}

	DrawText(TEXT("무기 선택"), FLinearColor(0.82f, 0.88f, 0.96f, 1.0f), PanelX + 28.0f, PanelY + 74.0f, nullptr, 0.95f, false);

	// 무기 이미지는 아직 에셋이 없으므로 색상 카드로 대체합니다. 배치는 가로 3칸, 세로 최대 4칸까지 확장 가능합니다.
	const int32 Columns = 3;
	const float Gap = 18.0f;
	const float AvailableWidth = PanelWidth - 56.0f;
	const float CardWidth = (AvailableWidth - Gap * (Columns - 1)) / Columns;
	const float CardHeight = FMath::Min(PanelHeight * 0.26f, 150.0f);
	const float StartX = PanelX + 28.0f;
	const float StartY = PanelY + 104.0f;

	for (int32 Index = 0; Index < WeaponTypes.Num(); ++Index)
	{
		const int32 Row = Index / Columns;
		const int32 Column = Index % Columns;
		const FVector2D CardPosition(StartX + Column * (CardWidth + Gap), StartY + Row * (CardHeight + Gap));
		const FVector2D CardSize(CardWidth, CardHeight);
		const ERogue10mWeaponType WeaponType = WeaponTypes[Index];
		const bool bUnlocked = IsWeaponSkillTreeUnlocked(WeaponType);
		const int32 ProficiencyLevel = GetWeaponProficiencyLevel(WeaponType);
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, CardPosition, CardSize);

		SkillWeaponHitAreas.Add({ WeaponType, CardPosition, CardSize });

		DrawRect(bHovered && bUnlocked ? FLinearColor(0.95f, 0.82f, 0.35f, 0.9f) : FLinearColor(0.34f, 0.38f, 0.45f, 0.9f), CardPosition.X - 2.0f, CardPosition.Y - 2.0f, CardSize.X + 4.0f, CardSize.Y + 4.0f);
		DrawRect(FLinearColor(0.025f, 0.03f, 0.04f, 0.96f), CardPosition.X, CardPosition.Y, CardSize.X, CardSize.Y);
		DrawRect(bUnlocked ? GetWeaponTypeColor(WeaponType) : FLinearColor(0.16f, 0.17f, 0.19f, 1.0f), CardPosition.X + 12.0f, CardPosition.Y + 12.0f, CardSize.X - 24.0f, CardSize.Y - 46.0f);
		DrawText(GetWeaponTypeText(WeaponType), InventoryTextColor, CardPosition.X + 14.0f, CardPosition.Y + CardSize.Y - 28.0f, nullptr, 0.9f, false);
		DrawText(FString::Printf(TEXT("숙련도 %d / 10"), ProficiencyLevel), bUnlocked ? FLinearColor(0.72f, 1.0f, 0.78f, 1.0f) : FLinearColor(0.6f, 0.64f, 0.7f, 1.0f), CardPosition.X + 14.0f, CardPosition.Y + CardSize.Y - 50.0f, nullptr, 0.7f, false);
		if (!bUnlocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.58f), CardPosition.X, CardPosition.Y, CardSize.X, CardSize.Y);
			DrawText(TEXT("잠김"), FLinearColor(0.9f, 0.9f, 0.9f, 1.0f), CardPosition.X + CardSize.X * 0.5f - 18.0f, CardPosition.Y + CardSize.Y * 0.5f - 8.0f, nullptr, 0.9f, false);
		}

		if (bHovered && bUnlocked && bLeftClickPressed)
		{
			SelectedSkillTreeWeapon = WeaponType;
			SkillTreeView = ERogue10mSkillTreeView::WeaponDetail;
		}
	}
}

void ARogue10mHUD::DrawSkillTreeWeaponDetail(float PanelX, float PanelY, float PanelWidth, float PanelHeight)
{
	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bLeftClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bLeftClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	}

	const FVector2D BackPosition(PanelX + 24.0f, PanelY + 76.0f);
	const FVector2D BackSize(86.0f, 32.0f);
	const bool bBackHovered = bHasMousePosition && IsPointInRect(MousePosition, BackPosition, BackSize);
	DrawRect(bBackHovered ? FLinearColor(0.32f, 0.48f, 0.72f, 0.96f) : FLinearColor(0.16f, 0.22f, 0.32f, 0.96f), BackPosition.X, BackPosition.Y, BackSize.X, BackSize.Y);
	DrawText(TEXT("Back"), InventoryTextColor, BackPosition.X + 22.0f, BackPosition.Y + 8.0f, nullptr, 0.8f, false);

	if (bBackHovered && bLeftClickPressed)
	{
		SkillTreeView = ERogue10mSkillTreeView::WeaponSelect;
		return;
	}

	const FString WeaponName = GetWeaponTypeText(SelectedSkillTreeWeapon);
	const bool bTreeUnlocked = IsWeaponSkillTreeUnlocked(SelectedSkillTreeWeapon);
	const int32 ProficiencyLevel = GetWeaponProficiencyLevel(SelectedSkillTreeWeapon);
	DrawText(FString::Printf(TEXT("%s 숙련도 %d / 10"), *WeaponName, ProficiencyLevel), InventoryTextColor, PanelX + 130.0f, PanelY + 80.0f, nullptr, 1.15f, false);

	if (!bTreeUnlocked)
	{
		DrawText(TEXT("해당 무기의 숙련도가 아직 열리지 않았습니다."), FLinearColor(0.68f, 0.74f, 0.82f, 1.0f), PanelX + 28.0f, PanelY + 142.0f, nullptr, 0.9f, false);
		DrawText(TEXT("무기를 얻거나 스킬서/NPC를 통해 스킬을 습득하면 접근할 수 있습니다."), FLinearColor(0.68f, 0.74f, 0.82f, 1.0f), PanelX + 28.0f, PanelY + 174.0f, nullptr, 0.8f, false);
		return;
	}

	if (SelectedSkillTreeWeapon != ERogue10mWeaponType::Knuckle)
	{
		DrawText(TEXT("현재 프로토타입에서는 권 스킬트리만 열려 있습니다."), FLinearColor(0.68f, 0.74f, 0.82f, 1.0f), PanelX + 28.0f, PanelY + 142.0f, nullptr, 0.9f, false);
		return;
	}

	struct FKnuckleSkillView
	{
		int32 SkillIndex;
		int32 Row;
		int32 Column;
		const TCHAR* SkillName;
		const TCHAR* AssetName;
		const TCHAR* IconText;
		FLinearColor IconColor;
	};

	const FKnuckleSkillView SkillViews[] =
	{
		{ 0, 0, 0, TEXT("기본 권격"), TEXT("Primary"), TEXT("좌"), FLinearColor(1.0f, 0.58f, 0.24f, 1.0f) },
		{ 2, 0, 1, TEXT("연속 권격 2"), TEXT("Combo2"), TEXT("연"), FLinearColor(1.0f, 0.68f, 0.28f, 1.0f) },
		{ 3, 0, 2, TEXT("연속 권격 3"), TEXT("Combo3"), TEXT("3"), FLinearColor(1.0f, 0.42f, 0.24f, 1.0f) },
		{ 4, 0, 3, TEXT("권 마무리"), TEXT("추가 예정"), TEXT("끝"), FLinearColor(0.95f, 0.3f, 0.24f, 1.0f) },
		{ 1, 1, 0, TEXT("도약 권격"), TEXT("JumpPrimary"), TEXT("도"), FLinearColor(1.0f, 0.82f, 0.34f, 1.0f) },
		{ 5, 1, 1, TEXT("공중 연계"), TEXT("추가 예정"), TEXT("공"), FLinearColor(0.62f, 0.82f, 1.0f, 1.0f) },
		{ 6, 1, 2, TEXT("낙하 권격"), TEXT("추가 예정"), TEXT("낙"), FLinearColor(0.48f, 0.64f, 1.0f, 1.0f) }
	};

	const TCHAR* RowCommands[] = { TEXT("좌"), TEXT("점프 좌") };
	const float CommandX = PanelX + 48.0f;
	const float NodeStartX = PanelX + PanelWidth * 0.2f;
	const float StartY = PanelY + 150.0f;
	const float RowGap = 116.0f;
	const float NodeGap = 18.0f;
	const FVector2D CommandBoxSize(86.0f, 64.0f);
	const FVector2D NodeSize(122.0f, 64.0f);
	const FLinearColor WeaponColor = GetWeaponTypeColor(SelectedSkillTreeWeapon);

	DrawText(TEXT("커맨드"), FLinearColor(0.72f, 0.78f, 0.88f, 1.0f), CommandX, StartY - 32.0f, nullptr, 0.8f, false);
	DrawText(TEXT("스킬 오브젝트"), FLinearColor(0.72f, 0.78f, 0.88f, 1.0f), NodeStartX, StartY - 32.0f, nullptr, 0.8f, false);

	for (int32 Row = 0; Row < UE_ARRAY_COUNT(RowCommands); ++Row)
	{
		const FVector2D CommandPosition(CommandX, StartY + Row * RowGap);
		DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 1.0f), CommandPosition.X - 2.0f, CommandPosition.Y - 2.0f, CommandBoxSize.X + 4.0f, CommandBoxSize.Y + 4.0f);
		DrawRect(FLinearColor(0.03f, 0.035f, 0.045f, 0.98f), CommandPosition.X, CommandPosition.Y, CommandBoxSize.X, CommandBoxSize.Y);
		DrawText(RowCommands[Row], InventoryTextColor, CommandPosition.X + 18.0f, CommandPosition.Y + 20.0f, nullptr, 1.0f, false);

		const FVector2D FirstNodePosition(NodeStartX, CommandPosition.Y);
		DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 0.9f), CommandPosition.X + CommandBoxSize.X + 10.0f, CommandPosition.Y + CommandBoxSize.Y * 0.5f - 2.0f, FirstNodePosition.X - (CommandPosition.X + CommandBoxSize.X + 10.0f), 4.0f);
	}

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(SkillViews); ++Index)
	{
		const FKnuckleSkillView& SkillView = SkillViews[Index];
		const FVector2D NodePosition(NodeStartX + SkillView.Column * (NodeSize.X + NodeGap), StartY + SkillView.Row * RowGap);
		const bool bUnlocked = IsKnuckleSkillUnlocked(SkillView.SkillIndex);
		const bool bCanUnlock = CanUnlockKnuckleSkill(SkillView.SkillIndex);
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, NodePosition, NodeSize);
		const FLinearColor NodeBorder = bUnlocked ? WeaponColor : (bCanUnlock ? FLinearColor(0.95f, 0.82f, 0.35f, 1.0f) : FLinearColor(0.24f, 0.26f, 0.3f, 1.0f));
		const FLinearColor NodeFill = bUnlocked ? FLinearColor(0.16f, 0.22f, 0.18f, 0.96f) : (bCanUnlock ? FLinearColor(0.18f, 0.16f, 0.08f, 0.96f) : FLinearColor(0.055f, 0.06f, 0.07f, 0.96f));

		if (SkillView.Column > 0)
		{
			DrawRect(FLinearColor(0.42f, 0.46f, 0.52f, 0.9f), NodePosition.X - NodeGap, NodePosition.Y + NodeSize.Y * 0.5f - 2.0f, NodeGap, 4.0f);
		}

		DrawRect(NodeBorder, NodePosition.X - 2.0f, NodePosition.Y - 2.0f, NodeSize.X + 4.0f, NodeSize.Y + 4.0f);
		DrawRect(NodeFill, NodePosition.X, NodePosition.Y, NodeSize.X, NodeSize.Y);
		if (bHovered && bCanUnlock)
		{
			DrawRect(FLinearColor(1.0f, 0.92f, 0.45f, 0.18f), NodePosition.X, NodePosition.Y, NodeSize.X, NodeSize.Y);
		}

		const FVector2D IconPosition(NodePosition.X + 8.0f, NodePosition.Y + 10.0f);
		const FVector2D IconSize(34.0f, 34.0f);
		const FLinearColor IconFrameColor = bUnlocked ? SkillView.IconColor : FLinearColor(0.28f, 0.3f, 0.34f, 1.0f);
		const FLinearColor IconFillColor = bUnlocked ? FLinearColor(SkillView.IconColor.R * 0.18f, SkillView.IconColor.G * 0.18f, SkillView.IconColor.B * 0.18f, 0.98f) : FLinearColor(0.08f, 0.085f, 0.095f, 0.98f);
		DrawRect(IconFrameColor, IconPosition.X - 1.0f, IconPosition.Y - 1.0f, IconSize.X + 2.0f, IconSize.Y + 2.0f);
		DrawRect(IconFillColor, IconPosition.X, IconPosition.Y, IconSize.X, IconSize.Y);
		DrawText(SkillView.IconText, bUnlocked ? InventoryTextColor : FLinearColor(0.62f, 0.66f, 0.72f, 1.0f), IconPosition.X + 8.0f, IconPosition.Y + 9.0f, nullptr, 0.68f, false);

		DrawText(SkillView.SkillName, bUnlocked ? InventoryTextColor : FLinearColor(0.62f, 0.66f, 0.72f, 1.0f), NodePosition.X + 48.0f, NodePosition.Y + 8.0f, nullptr, 0.62f, false);
		DrawText(SkillView.AssetName, FLinearColor(0.56f, 0.64f, 0.74f, 1.0f), NodePosition.X + 48.0f, NodePosition.Y + 27.0f, nullptr, 0.42f, false);
		DrawText(bUnlocked ? TEXT("습득됨") : (bCanUnlock ? TEXT("해금") : TEXT("잠김")), bUnlocked ? FLinearColor(0.56f, 1.0f, 0.62f, 1.0f) : (bCanUnlock ? FLinearColor(1.0f, 0.86f, 0.35f, 1.0f) : FLinearColor(0.7f, 0.72f, 0.76f, 1.0f)), NodePosition.X + 48.0f, NodePosition.Y + 45.0f, nullptr, 0.52f, false);

		if (bHovered && bLeftClickPressed && bCanUnlock)
		{
			UnlockKnuckleSkillForTest(SkillView.SkillIndex);
		}
	}

	DrawText(TEXT("습득되지 않은 노드는 실제 공격에서도 사용 불가능합니다. 테스트용으로 연결된 앞 노드를 해금하면 다음 노드를 클릭할 수 있습니다."), FLinearColor(0.68f, 0.74f, 0.82f, 1.0f), PanelX + 28.0f, PanelY + PanelHeight - 44.0f, nullptr, 0.72f, false);
}

void ARogue10mHUD::DrawSettingsPanel()
{
	if (!bSettingsVisible || !Canvas)
	{
		return;
	}

	const float PanelWidth = Canvas->SizeX * 0.42f;
	const float PanelHeight = Canvas->SizeY * 0.44f;
	if (SettingsWindowPosition.X < 0.0f || SettingsWindowPosition.Y < 0.0f)
	{
		SettingsWindowPosition = FVector2D((Canvas->SizeX - PanelWidth) * 0.5f, (Canvas->SizeY - PanelHeight) * 0.5f);
	}

	SettingsWindowPosition = ClampWindowPosition(SettingsWindowPosition, FVector2D(PanelWidth, PanelHeight));
	UpdateSettingsInteraction();

	const float PanelX = SettingsWindowPosition.X;
	const float PanelY = SettingsWindowPosition.Y;
	DrawRect(FLinearColor(0.012f, 0.016f, 0.024f, 0.97f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.08f, 0.09f, 0.105f, 0.98f), PanelX, PanelY, PanelWidth, 56.0f);
	DrawText(TEXT("설정"), InventoryTextColor, PanelX + 24.0f, PanelY + 18.0f, nullptr, 1.25f, false);
	DrawText(TEXT("O : 닫기  |  제목줄 드래그"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 22.0f, nullptr, 0.78f, false);

	const float SliderX = PanelX + 44.0f;
	const float SliderWidth = PanelWidth - 88.0f;
	DrawSettingsSlider(TEXT("음성 볼륨"), ERogue10mSettingsSlider::MasterVolume, MasterVolume, 0.0f, 1.0f, FVector2D(SliderX, PanelY + 94.0f), SliderWidth);
	DrawSettingsSlider(TEXT("시점 감도 X"), ERogue10mSettingsSlider::LookSensitivityX, LookSensitivityX, 0.2f, 3.0f, FVector2D(SliderX, PanelY + 154.0f), SliderWidth);
	DrawSettingsSlider(TEXT("시점 감도 Y"), ERogue10mSettingsSlider::LookSensitivityY, LookSensitivityY, 0.2f, 3.0f, FVector2D(SliderX, PanelY + 214.0f), SliderWidth);

	DrawText(FString::Printf(TEXT("FPS 제한  |  현재: %s"), CurrentFpsLimit > 0 ? *FString::Printf(TEXT("%d FPS"), CurrentFpsLimit) : TEXT("제한 없음")), InventoryTextColor, SliderX, PanelY + 274.0f, nullptr, 0.86f, false);
	const FVector2D ButtonSize(78.0f, 34.0f);
	DrawSettingsFpsButton(60, TEXT("60"), FVector2D(SliderX, PanelY + 304.0f), ButtonSize);
	DrawSettingsFpsButton(120, TEXT("120"), FVector2D(SliderX + 94.0f, PanelY + 304.0f), ButtonSize);
	DrawSettingsFpsButton(140, TEXT("140"), FVector2D(SliderX + 188.0f, PanelY + 304.0f), ButtonSize);
	DrawSettingsFpsButton(0, TEXT("해제"), FVector2D(SliderX + 282.0f, PanelY + 304.0f), ButtonSize);
}

void ARogue10mHUD::DrawSettingsSlider(const FString& Label, ERogue10mSettingsSlider SliderType, float Value, float MinValue, float MaxValue, const FVector2D& Position, float Width)
{
	const float TrackY = Position.Y + 26.0f;
	const float Alpha = FMath::Clamp((Value - MinValue) / (MaxValue - MinValue), 0.0f, 1.0f);
	const float KnobX = Position.X + Width * Alpha;

	DrawText(FString::Printf(TEXT("%s  %.2f"), *Label, Value), InventoryTextColor, Position.X, Position.Y, nullptr, 0.82f, false);
	DrawRect(FLinearColor(0.07f, 0.08f, 0.095f, 0.95f), Position.X, TrackY, Width, 8.0f);
	DrawRect(FLinearColor(0.26f, 0.58f, 0.95f, 1.0f), Position.X, TrackY, Width * Alpha, 8.0f);
	DrawRect(DraggedSettingsSlider == SliderType ? FLinearColor(1.0f, 0.9f, 0.42f, 1.0f) : FLinearColor(0.88f, 0.92f, 1.0f, 1.0f), KnobX - 6.0f, TrackY - 6.0f, 12.0f, 20.0f);
}

void ARogue10mHUD::DrawSettingsFpsButton(int32 FpsValue, const FString& Label, const FVector2D& Position, const FVector2D& Size)
{
	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHovered = false;
	bool bClicked = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		if (OwningPlayerController->GetMousePosition(MouseX, MouseY))
		{
			MousePosition = FVector2D(MouseX, MouseY);
			bHovered = IsPointInRect(MousePosition, Position, Size);
			bClicked = bHovered && OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
		}
	}

	if (bClicked)
	{
		SetFpsLimit(FpsValue);
	}

	const bool bSelected = CurrentFpsLimit == FpsValue;
	const FLinearColor ButtonColor = bSelected ? FLinearColor(0.24f, 0.64f, 0.36f, 0.96f) : (bHovered ? FLinearColor(0.26f, 0.38f, 0.56f, 0.96f) : FLinearColor(0.14f, 0.18f, 0.25f, 0.96f));
	DrawRect(ButtonColor, Position.X, Position.Y, Size.X, Size.Y);
	DrawText(Label, InventoryTextColor, Position.X + 18.0f, Position.Y + 9.0f, nullptr, 0.82f, false);
}

void ARogue10mHUD::AddItemAcquisitionMessage(const FString& Message, const FLinearColor& Color)
{
	if (!GetWorld())
	{
		return;
	}

	FRogue10mItemAcquisitionEntry Entry;
	Entry.Message = Message;
	Entry.Color = Color;
	Entry.StartTime = GetWorld()->GetTimeSeconds();
	Entry.ExpireTime = Entry.StartTime + 3.0f;
	ItemAcquisitionEntries.Insert(Entry, 0);

	constexpr int32 MaxAcquisitionCount = 8;
	if (ItemAcquisitionEntries.Num() > MaxAcquisitionCount)
	{
		ItemAcquisitionEntries.SetNum(MaxAcquisitionCount);
	}
}

void ARogue10mHUD::DrawInventorySlots(const TArray<FRogue10mInventorySlot>& Slots, float X, float Y, float SlotSize, float Gap, bool bRightSide)
{
	ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bLeftClickPressed = false;
	bool bLeftClickReleased = false;
	bool bRightClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bLeftClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
		bLeftClickReleased = OwningPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton);
		bRightClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
	}

	if (bLeftClickReleased && bIsDraggingItem && DraggedItemSource == ERogue10mDraggedItemSource::EquipmentSlot && !bItemWindowVisible)
	{
		ClearDraggedItem();
	}

	const float PulseAlpha = GetWorld() ? (0.35f + 0.35f * FMath::Abs(FMath::Sin(GetWorld()->GetTimeSeconds() * 7.5f))) : 0.45f;

	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FRogue10mInventorySlot& Slot = Slots[Index];
		const float SlotY = Y + Index * (SlotSize + Gap);
		EquipmentSlotHitAreas.Add({ Slot.SlotType, FVector2D(X, SlotY), FVector2D(SlotSize, SlotSize), Slot.bLocked });
		const bool bDragCompatible = bIsDraggingItem && DraggedItemSource == ERogue10mDraggedItemSource::ItemSlot && IsItemCompatibleWithSlot(DraggedItem, Slot.SlotType) && !Slot.bLocked;
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, FVector2D(X, SlotY), FVector2D(SlotSize, SlotSize));

		DrawRect(Slot.SlotColor, X - 3.0f, SlotY - 3.0f, SlotSize + 6.0f, SlotSize + 6.0f);
		DrawRect(InventorySlotFillColor, X, SlotY, SlotSize, SlotSize);
		if (bDragCompatible)
		{
			DrawRect(FLinearColor(0.2f, 1.0f, 0.55f, PulseAlpha), X - 4.0f, SlotY - 4.0f, SlotSize + 8.0f, SlotSize + 8.0f);
			DrawRect(FLinearColor(0.2f, 1.0f, 0.55f, 0.24f), X, SlotY, SlotSize, SlotSize);
		}

		if (bHovered && bRightClickPressed && InventoryComponent && Slot.bHasEquippedItem)
		{
			InventoryComponent->TryUnequipItemFromSlot(Slot.SlotType);
		}

		if (bHovered && bLeftClickPressed && Slot.bHasEquippedItem && Slot.EquippedItem.bOccupied && !Slot.EquippedItem.bLocked)
		{
			bIsDraggingItem = true;
			DraggedItemSource = ERogue10mDraggedItemSource::EquipmentSlot;
			DraggedItem = Slot.EquippedItem;
			DraggedItemSlotIndex = INDEX_NONE;
			DraggedEquipmentSlotType = Slot.SlotType;
			DraggedItemMousePosition = MousePosition;
			bHasHoveredItem = false;
		}

		const FString SlotName = Slot.DisplayName.ToString();
		DrawText(SlotName, InventoryTextColor, X + 8.0f, SlotY + 10.0f, nullptr, 0.85f, false);

		if (Slot.bLocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), X, SlotY, SlotSize, SlotSize);
			DrawText(TEXT("잠김"), FLinearColor(0.9f, 0.9f, 0.9f, 1.0f), X + 12.0f, SlotY + 38.0f, nullptr, 0.75f, false);
		}
		else if (Slot.bEquipped)
		{
			DrawRect(FLinearColor(0.1f, 0.9f, 0.95f, 0.16f), X + 8.0f, SlotY + 34.0f, SlotSize - 16.0f, 22.0f);
			DrawText(Slot.bHasEquippedItem ? TEXT("EQUIP") : TEXT("BASE"), FLinearColor(0.1f, 0.95f, 1.0f, 1.0f), X + 12.0f, SlotY + 38.0f, nullptr, 0.72f, false);
		}

		const float MarkerX = bRightSide ? X - 18.0f : X + SlotSize + 8.0f;
		DrawText(TEXT("<"), Slot.SlotColor, MarkerX, SlotY + 20.0f, nullptr, 1.0f, false);
	}
}

void ARogue10mHUD::DrawItemGrid(URogue10mInventoryComponent* InventoryComponent, float X, float Y, float SlotSize, float Gap, float MaxWidth, float MaxHeight)
{
	if (!InventoryComponent)
	{
		return;
	}

	const TArray<FRogue10mItemStack>& Items = InventoryComponent->GetItemSlots();
	const int32 Columns = InventoryComponent->GetItemGridColumns();
	const int32 ColumnsByWidth = FMath::Max(1, FMath::FloorToInt((MaxWidth + Gap) / (SlotSize + Gap)));
	const int32 SafeColumns = FMath::Max(1, FMath::Min(Columns, ColumnsByWidth));
	const int32 SafeRows = FMath::Max(1, FMath::FloorToInt((MaxHeight + Gap) / (SlotSize + Gap)));
	const int32 VisibleItemCount = FMath::Min(Items.Num(), SafeColumns * SafeRows);

	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bLeftClickPressed = false;
	bool bLeftClickHeld = false;
	bool bLeftClickReleased = false;
	bool bRightClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bLeftClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
		bLeftClickHeld = OwningPlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
		bLeftClickReleased = OwningPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton);
		bRightClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
	}

	if (bHasMousePosition && bIsDraggingItem)
	{
		DraggedItemMousePosition = MousePosition;
		if (bLeftClickReleased)
		{
			bool bHandledDrop = false;
			if (DraggedItemSource == ERogue10mDraggedItemSource::ItemSlot)
			{
				if (const FRogue10mEquipmentSlotHitArea* DropTarget = FindEquipmentSlotHitArea(MousePosition))
				{
					bHandledDrop = InventoryComponent->TryEquipItemToSlot(DraggedItemSlotIndex, DropTarget->SlotType);
				}
				else
				{
					const float LocalX = MousePosition.X - X;
					const float LocalY = MousePosition.Y - Y;
					const int32 TargetColumn = FMath::FloorToInt(LocalX / (SlotSize + Gap));
					const int32 TargetRow = FMath::FloorToInt(LocalY / (SlotSize + Gap));
					const float TargetSlotX = X + TargetColumn * (SlotSize + Gap);
					const float TargetSlotY = Y + TargetRow * (SlotSize + Gap);
					if (TargetColumn >= 0
						&& TargetColumn < SafeColumns
						&& TargetRow >= 0
						&& TargetRow < SafeRows
						&& IsPointInRect(MousePosition, FVector2D(TargetSlotX, TargetSlotY), FVector2D(SlotSize, SlotSize)))
					{
						const int32 TargetItemSlotIndex = TargetRow * SafeColumns + TargetColumn;
						bHandledDrop = InventoryComponent->TryMoveItemSlot(DraggedItemSlotIndex, TargetItemSlotIndex);
					}
				}
			}
			else if (DraggedItemSource == ERogue10mDraggedItemSource::EquipmentSlot)
			{
				int32 TargetItemSlotIndex = INDEX_NONE;
				const float LocalX = MousePosition.X - X;
				const float LocalY = MousePosition.Y - Y;
				const int32 TargetColumn = FMath::FloorToInt(LocalX / (SlotSize + Gap));
				const int32 TargetRow = FMath::FloorToInt(LocalY / (SlotSize + Gap));
				const float TargetSlotX = X + TargetColumn * (SlotSize + Gap);
				const float TargetSlotY = Y + TargetRow * (SlotSize + Gap);
				if (TargetColumn >= 0
					&& TargetColumn < SafeColumns
					&& TargetRow >= 0
					&& TargetRow < SafeRows
					&& IsPointInRect(MousePosition, FVector2D(TargetSlotX, TargetSlotY), FVector2D(SlotSize, SlotSize)))
				{
					TargetItemSlotIndex = TargetRow * SafeColumns + TargetColumn;
				}
				if (TargetItemSlotIndex != INDEX_NONE)
				{
					bHandledDrop = InventoryComponent->TryUnequipItemFromSlotToItemSlot(DraggedEquipmentSlotType, TargetItemSlotIndex);
				}
			}

			if (!bHandledDrop && !IsMouseOverInventoryOrItemWindow(MousePosition))
			{
				TryDropDraggedItemToWorld(InventoryComponent);
			}

			ClearDraggedItem();
		}
	}
	else if (!bLeftClickHeld)
	{
		ClearDraggedItem();
	}

	for (int32 Index = 0; Index < VisibleItemCount; ++Index)
	{
		const int32 Row = Index / SafeColumns;
		const int32 Column = Index % SafeColumns;
		const float SlotX = X + Column * (SlotSize + Gap);
		const float SlotY = Y + Row * (SlotSize + Gap);
		const FRogue10mItemStack& Item = Items[Index];
		const FVector2D SlotPosition(SlotX, SlotY);
		const FVector2D SlotExtent(SlotSize, SlotSize);
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, SlotPosition, SlotExtent);
		ItemSlotHitAreas.Add({ Index, SlotPosition, SlotExtent, Item.bLocked });

		DrawRect(FLinearColor(0.01f, 0.012f, 0.015f, 0.95f), SlotX, SlotY, SlotSize, SlotSize);
		DrawRect(FLinearColor(0.13f, 0.16f, 0.19f, 0.75f), SlotX + 1.0f, SlotY + 1.0f, SlotSize - 2.0f, SlotSize - 2.0f);

		if (bIsDraggingItem && Index != DraggedItemSlotIndex)
		{
			DrawRect(FLinearColor(0.2f, 0.65f, 1.0f, 0.1f), SlotX, SlotY, SlotSize, SlotSize);
		}

		if (!Item.bOccupied)
		{
			continue;
		}

		DrawRect(Item.ItemColor, SlotX + 5.0f, SlotY + 5.0f, SlotSize - 10.0f, SlotSize - 10.0f);
		DrawText(Item.DisplayName.ToString(), InventoryTextColor, SlotX + 6.0f, SlotY + 9.0f, nullptr, 0.58f, false);

		if (bHovered)
		{
			DrawRect(FLinearColor(0.95f, 0.82f, 0.35f, 0.28f), SlotX, SlotY, SlotSize, SlotSize);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY - 2.0f, SlotSize + 4.0f, 2.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY + SlotSize, SlotSize + 4.0f, 2.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX - 2.0f, SlotY - 2.0f, 2.0f, SlotSize + 4.0f);
			DrawRect(FLinearColor(1.0f, 0.95f, 0.65f, 0.85f), SlotX + SlotSize, SlotY - 2.0f, 2.0f, SlotSize + 4.0f);

			bHasHoveredItem = true;
			HoveredItem = Item;
			HoveredItemSlotIndex = Index;
			HoveredItemMousePosition = MousePosition;

			if (bRightClickPressed)
			{
				InventoryComponent->TryEquipItemFromSlot(Index);
			}

			if (bLeftClickPressed && !Item.bLocked)
			{
				bIsDraggingItem = true;
				DraggedItemSource = ERogue10mDraggedItemSource::ItemSlot;
				DraggedItem = Item;
				DraggedItemSlotIndex = Index;
				DraggedEquipmentSlotType = ERogue10mInventorySlotType::Material;
				DraggedItemMousePosition = MousePosition;
				bHasHoveredItem = false;
			}
		}

		if (Item.Quantity > 1)
		{
			DrawText(FString::FromInt(Item.Quantity), InventoryTextColor, SlotX + SlotSize - 18.0f, SlotY + 4.0f, nullptr, 0.82f, false);
		}

		if (Item.bLocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), SlotX, SlotY, SlotSize, SlotSize);
			DrawText(TEXT("잠김"), FLinearColor(0.95f, 0.95f, 0.95f, 1.0f), SlotX + 7.0f, SlotY + SlotSize - 20.0f, nullptr, 0.65f, false);
		}
	}
}

void ARogue10mHUD::DrawHoveredItemTooltip()
{
	if (!Canvas || bIsDraggingItem || !bHasHoveredItem || !HoveredItem.bOccupied)
	{
		return;
	}

	const float TooltipWidth = 300.0f;
	const bool bHoveredWeapon = HoveredItem.EquipSlotType == ERogue10mInventorySlotType::MainWeapon || HoveredItem.EquipSlotType == ERogue10mInventorySlotType::SecondaryWeapon;
	const float TooltipHeight = bHoveredWeapon ? 192.0f : (HoveredItem.Category == ERogue10mItemCategory::Equipment ? 170.0f : 145.0f);
	const FVector2D TooltipSize(TooltipWidth, TooltipHeight);
	FVector2D TooltipPosition = HoveredItemMousePosition + FVector2D(18.0f, 18.0f);
	TooltipPosition = ClampWindowPosition(TooltipPosition, TooltipSize);

	DrawRect(FLinearColor(0.015f, 0.018f, 0.022f, 0.97f), TooltipPosition.X, TooltipPosition.Y, TooltipWidth, TooltipHeight);
	DrawRect(HoveredItem.ItemColor, TooltipPosition.X, TooltipPosition.Y, TooltipWidth, 4.0f);

	const float TextX = TooltipPosition.X + 14.0f;
	float TextY = TooltipPosition.Y + 16.0f;
	DrawText(FString::Printf(TEXT("아이템명: %s"), *HoveredItem.DisplayName.ToString()), InventoryTextColor, TextX, TextY, nullptr, 1.05f, false);

	TextY += 28.0f;
	DrawText(FString::Printf(TEXT("분류: %s  |  수량: %d"), *GetItemCategoryText(HoveredItem.Category), HoveredItem.Quantity), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.78f, false);

	if (HoveredItem.Category == ERogue10mItemCategory::Equipment)
	{
		TextY += 22.0f;
		DrawText(FString::Printf(TEXT("장착 위치: %s"), *GetEquipmentSlotText(HoveredItem.EquipSlotType)), FLinearColor(0.85f, 0.76f, 0.48f, 1.0f), TextX, TextY, nullptr, 0.8f, false);
		if (bHoveredWeapon)
		{
			// 무기 아이템은 숙련도/스킬 분기를 위해 세부 무기 타입도 함께 보여줍니다.
			TextY += 22.0f;
			DrawText(FString::Printf(TEXT("무기 타입: %s"), *GetWeaponTypeText(HoveredItem.WeaponType)), FLinearColor(0.9f, 0.66f, 0.42f, 1.0f), TextX, TextY, nullptr, 0.8f, false);
		}
	}

	TextY += 28.0f;
	DrawText(FString::Printf(TEXT("아이템 세부 설명: %s"), *HoveredItem.Description.ToString()), FLinearColor(0.82f, 0.86f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.72f, false);

	TextY += 38.0f;
	FString ActionText = TEXT("우클릭: 장착");
	FLinearColor ActionColor = FLinearColor(0.35f, 1.0f, 0.72f, 1.0f);
	if (HoveredItem.bLocked)
	{
		ActionText = TEXT("잠긴 아이템");
		ActionColor = WarningColor;
	}
	else if (HoveredItem.Category != ERogue10mItemCategory::Equipment)
	{
		ActionText = TEXT("장착 불가");
		ActionColor = FLinearColor(0.68f, 0.72f, 0.78f, 1.0f);
	}

	DrawText(ActionText, ActionColor, TextX, TextY, nullptr, 0.82f, false);
}

void ARogue10mHUD::DrawEquipmentCharacterStats(const ARogue10mCharacter* RogueCharacter, float X, float Y, float Width, float Height)
{
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!Canvas || !RogueCharacter || !VitalsComponent)
	{
		return;
	}

	DrawRect(FLinearColor(0.015f, 0.018f, 0.022f, 0.78f), X, Y, Width, Height);
	DrawRect(FLinearColor(0.18f, 0.24f, 0.32f, 0.92f), X, Y, Width, 3.0f);

	const float TextX = X + 12.0f;
	float TextY = Y + 12.0f;
	const float TextScale = 0.66f;
	DrawText(TEXT("현재 캐릭터 스탯"), InventoryTextColor, TextX, TextY, nullptr, 0.74f, false);

	TextY += 20.0f;
	DrawText(FString::Printf(TEXT("체력 %.0f / %.0f"), VitalsComponent->GetHealth().Current, VitalsComponent->GetHealth().Max), FLinearColor(1.0f, 0.72f, 0.72f, 1.0f), TextX, TextY, nullptr, TextScale, false);
	DrawText(FString::Printf(TEXT("스테미나 %.0f / %.0f"), VitalsComponent->GetStamina().Current, VitalsComponent->GetStamina().Max), FLinearColor(0.7f, 1.0f, 0.72f, 1.0f), X + Width * 0.5f, TextY, nullptr, TextScale, false);

	TextY += 18.0f;
	if (VitalsComponent->ShouldShowMana())
	{
		DrawText(FString::Printf(TEXT("마나 %.0f / %.0f"), VitalsComponent->GetMana().Current, VitalsComponent->GetMana().Max), FLinearColor(0.68f, 0.8f, 1.0f, 1.0f), TextX, TextY, nullptr, TextScale, false);
	}
	else
	{
		DrawText(TEXT("마나 비활성"), FLinearColor(0.58f, 0.64f, 0.72f, 1.0f), TextX, TextY, nullptr, TextScale, false);
	}
	DrawText(FString::Printf(TEXT("무기 %s"), *GetWeaponTypeText(RogueCharacter->GetEquippedWeaponType())), FLinearColor(0.9f, 0.76f, 0.48f, 1.0f), X + Width * 0.5f, TextY, nullptr, TextScale, false);
}

void ARogue10mHUD::DrawDraggedItem()
{
	if (!Canvas || !bIsDraggingItem || !DraggedItem.bOccupied)
	{
		return;
	}

	const float DragSlotSize = 58.0f;
	const FVector2D DrawPosition = DraggedItemMousePosition - FVector2D(DragSlotSize * 0.5f, DragSlotSize * 0.5f);
	const FRogue10mEquipmentSlotHitArea* DropTarget = FindEquipmentSlotHitArea(DraggedItemMousePosition);
	const FRogue10mItemSlotHitArea* ItemDropTarget = FindItemSlotHitArea(DraggedItemMousePosition);
	const bool bCanEquipDrop = DraggedItemSource == ERogue10mDraggedItemSource::ItemSlot && DropTarget && !DropTarget->bLocked && IsItemCompatibleWithSlot(DraggedItem, DropTarget->SlotType);
	const bool bCanMoveDrop = DraggedItemSource == ERogue10mDraggedItemSource::ItemSlot && ItemDropTarget && !ItemDropTarget->bLocked && ItemDropTarget->SlotIndex != DraggedItemSlotIndex;
	const bool bCanUnequipDrop = DraggedItemSource == ERogue10mDraggedItemSource::EquipmentSlot && ItemDropTarget && !ItemDropTarget->bLocked;
	const bool bCanDrop = bCanEquipDrop || bCanMoveDrop || bCanUnequipDrop;
	const FLinearColor BorderColor = bCanDrop ? FLinearColor(0.2f, 1.0f, 0.55f, 0.95f) : FLinearColor(1.0f, 0.35f, 0.25f, 0.85f);

	DrawRect(BorderColor, DrawPosition.X - 3.0f, DrawPosition.Y - 3.0f, DragSlotSize + 6.0f, DragSlotSize + 6.0f);
	DrawRect(FLinearColor(0.02f, 0.025f, 0.03f, 0.92f), DrawPosition.X, DrawPosition.Y, DragSlotSize, DragSlotSize);
	DrawRect(DraggedItem.ItemColor, DrawPosition.X + 6.0f, DrawPosition.Y + 6.0f, DragSlotSize - 12.0f, DragSlotSize - 12.0f);
	DrawText(DraggedItem.DisplayName.ToString(), InventoryTextColor, DrawPosition.X + 7.0f, DrawPosition.Y + 11.0f, nullptr, 0.58f, false);

	const FString DropText = bCanEquipDrop ? TEXT("놓기: 장착") : (bCanUnequipDrop ? TEXT("놓기: 해제") : (bCanMoveDrop ? TEXT("놓기: 이동") : TEXT("슬롯에 놓기")));
	DrawText(DropText, BorderColor, DrawPosition.X + DragSlotSize + 10.0f, DrawPosition.Y + 18.0f, nullptr, 0.8f, false);
}

void ARogue10mHUD::DrawCharacterPreview(const ARogue10mCharacter* RogueCharacter, float X, float Y, float Width, float Height)
{
	CharacterPreviewPosition = FVector2D(X, Y);
	CharacterPreviewSize = FVector2D(Width, Height);

	const int32 RenderTargetWidth = FMath::Max(256, FMath::RoundToInt(Width * 2.0f));
	const int32 RenderTargetHeight = FMath::Max(384, FMath::RoundToInt(Height * 2.0f));
	if (!EnsureCharacterPreview(RogueCharacter, RenderTargetWidth, RenderTargetHeight))
	{
		DrawText(TEXT("캐릭터 미리보기를 사용할 수 없습니다."), FLinearColor(0.75f, 0.82f, 0.9f, 1.0f), X + 20.0f, Y + Height * 0.5f, nullptr, 0.9f, false);
		return;
	}

	UpdateCharacterPreview(RogueCharacter);
	DrawTexture(CharacterPreviewRenderTarget, X, Y, Width, Height, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::White, BLEND_Translucent);
}

bool ARogue10mHUD::EnsureCharacterPreview(const ARogue10mCharacter* RogueCharacter, int32 RenderTargetWidth, int32 RenderTargetHeight)
{
	if (!RogueCharacter || !GetWorld())
	{
		return false;
	}

	if (!CharacterPreviewActor)
	{
		CharacterPreviewActor = GetWorld()->SpawnActor<AActor>();
		if (!CharacterPreviewActor)
		{
			return false;
		}

		CharacterPreviewActor->SetActorHiddenInGame(false);
		CharacterPreviewActor->SetActorEnableCollision(false);
	}

	if (!CharacterPreviewMesh)
	{
		CharacterPreviewMesh = NewObject<USkeletalMeshComponent>(CharacterPreviewActor);
		CharacterPreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterPreviewMesh->SetCastShadow(false);
		CharacterPreviewMesh->SetVisibility(true);
		CharacterPreviewMesh->SetHiddenInGame(false);
		CharacterPreviewMesh->RegisterComponentWithWorld(GetWorld());
		CharacterPreviewActor->SetRootComponent(CharacterPreviewMesh);
	}

	if (!CharacterPreviewCapture)
	{
		CharacterPreviewCapture = NewObject<USceneCaptureComponent2D>(CharacterPreviewActor);
		CharacterPreviewCapture->ProjectionType = ECameraProjectionMode::Perspective;
		CharacterPreviewCapture->FOVAngle = 22.0f;
		CharacterPreviewCapture->bCaptureEveryFrame = false;
		CharacterPreviewCapture->bCaptureOnMovement = false;
		CharacterPreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		CharacterPreviewCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		CharacterPreviewCapture->ShowOnlyComponents.Add(CharacterPreviewMesh);
		CharacterPreviewCapture->RegisterComponentWithWorld(GetWorld());
	}

	if (!CharacterPreviewRenderTarget)
	{
		CharacterPreviewRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		CharacterPreviewRenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (CharacterPreviewRenderTarget->SizeX != RenderTargetWidth || CharacterPreviewRenderTarget->SizeY != RenderTargetHeight)
	{
		CharacterPreviewRenderTarget->InitAutoFormat(RenderTargetWidth, RenderTargetHeight);
		CharacterPreviewCapture->TextureTarget = CharacterPreviewRenderTarget;
	}

	return CharacterPreviewRenderTarget && CharacterPreviewCapture && CharacterPreviewMesh;
}

void ARogue10mHUD::UpdateCharacterPreview(const ARogue10mCharacter* RogueCharacter)
{
	if (!RogueCharacter || !CharacterPreviewMesh || !CharacterPreviewCapture)
	{
		return;
	}

	const USkeletalMeshComponent* SourceMesh = RogueCharacter->GetMesh();
	if (!SourceMesh || !SourceMesh->GetSkeletalMeshAsset())
	{
		SourceMesh = RogueCharacter->GetFirstPersonMesh();
	}

	if (!SourceMesh || !SourceMesh->GetSkeletalMeshAsset())
	{
		return;
	}

	if (CharacterPreviewMesh->GetSkeletalMeshAsset() != SourceMesh->GetSkeletalMeshAsset())
	{
		CharacterPreviewMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
		CharacterPreviewMesh->SetLeaderPoseComponent(const_cast<USkeletalMeshComponent*>(SourceMesh));
	}

	const int32 MaterialCount = SourceMesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		CharacterPreviewMesh->SetMaterial(MaterialIndex, SourceMesh->GetMaterial(MaterialIndex));
	}

	const FVector PreviewLocation = RogueCharacter->GetActorLocation() + FVector(0.0f, 0.0f, -5000.0f);
	CharacterPreviewActor->SetActorLocation(PreviewLocation);
	CharacterPreviewMesh->SetWorldLocation(PreviewLocation);
	CharacterPreviewMesh->SetWorldRotation(FRotator(0.0f, CharacterPreviewYaw, 0.0f));

	const FVector FocusLocation = PreviewLocation + FVector(0.0f, 0.0f, 88.0f);
	const FVector CameraLocation = FocusLocation + FVector(-650.0f, 0.0f, 0.0f);
	CharacterPreviewCapture->SetWorldLocation(CameraLocation);
	CharacterPreviewCapture->SetWorldRotation((FocusLocation - CameraLocation).Rotation());
	CharacterPreviewCapture->TextureTarget = CharacterPreviewRenderTarget;
	CharacterPreviewCapture->CaptureScene();
}

ARogue10mBasicMonster* ARogue10mHUD::FindLookedAtMonster() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const UCameraComponent* CameraComponent = RogueCharacter ? RogueCharacter->GetFirstPersonCameraComponent() : nullptr;
	const UWorld* World = GetWorld();
	if (!RogueCharacter || !CameraComponent || !World)
	{
		return nullptr;
	}

	const FVector TraceStart = CameraComponent->GetComponentLocation();
	const FVector TraceEnd = TraceStart + CameraComponent->GetForwardVector() * 2600.0f;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LookedAtMonsterInfo), false, RogueCharacter);
	FHitResult HitResult;
	const bool bHit = World->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(28.0f),
		QueryParams);

	return bHit ? Cast<ARogue10mBasicMonster>(HitResult.GetActor()) : nullptr;
}

void ARogue10mHUD::UpdateWindowDrag()
{
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!Canvas || !OwningPlayerController || !IsAnyBlockingWindowVisible())
	{
		DraggedWindow = ERogue10mDraggedWindow::None;
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!OwningPlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	const FVector2D MousePosition(MouseX, MouseY);
	const bool bPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	const bool bHeld = OwningPlayerController->IsInputKeyDown(EKeys::LeftMouseButton);

	const FVector2D InventorySize(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
	const FVector2D ItemSize(Canvas->SizeX * 0.3f, Canvas->SizeY * 0.5f);
	const FVector2D SkillTreeSize(Canvas->SizeX * 0.56f, Canvas->SizeY * 0.62f);
	const FVector2D SettingsSize(Canvas->SizeX * 0.42f, Canvas->SizeY * 0.44f);
	const bool bMouseOverItemWindow = bItemWindowVisible && IsPointInRect(MousePosition, ItemWindowPosition, ItemSize);

	if (bPressed && DraggedSettingsSlider == ERogue10mSettingsSlider::None)
	{
		if (bSettingsVisible && IsPointInRect(MousePosition, SettingsWindowPosition, FVector2D(SettingsSize.X, 56.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::Settings;
			DragOffset = MousePosition - SettingsWindowPosition;
		}
		else if (bSkillTreeVisible && IsPointInRect(MousePosition, SkillTreeWindowPosition, FVector2D(SkillTreeSize.X, 58.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::SkillTree;
			DragOffset = MousePosition - SkillTreeWindowPosition;
		}
		else if (bItemWindowVisible && IsPointInRect(MousePosition, ItemWindowPosition, FVector2D(ItemSize.X, 58.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::Items;
			DragOffset = MousePosition - ItemWindowPosition;
		}
		else if (bInventoryVisible && IsPointInRect(MousePosition, InventoryWindowPosition, FVector2D(InventorySize.X, 56.0f)))
		{
			DraggedWindow = ERogue10mDraggedWindow::Equipment;
			DragOffset = MousePosition - InventoryWindowPosition;
		}
		else if (bInventoryVisible && !bMouseOverItemWindow && IsPointInRect(MousePosition, CharacterPreviewPosition, CharacterPreviewSize))
		{
			DraggedWindow = ERogue10mDraggedWindow::CharacterPreview;
			LastCharacterPreviewDragX = MousePosition.X;
		}
	}

	if (!bHeld)
	{
		DraggedWindow = ERogue10mDraggedWindow::None;
		return;
	}

	switch (DraggedWindow)
	{
	case ERogue10mDraggedWindow::Equipment:
		InventoryWindowPosition = ClampWindowPosition(MousePosition - DragOffset, InventorySize);
		break;
	case ERogue10mDraggedWindow::Items:
		ItemWindowPosition = ClampWindowPosition(MousePosition - DragOffset, ItemSize);
		break;
	case ERogue10mDraggedWindow::SkillTree:
		SkillTreeWindowPosition = ClampWindowPosition(MousePosition - DragOffset, SkillTreeSize);
		break;
	case ERogue10mDraggedWindow::Settings:
		SettingsWindowPosition = ClampWindowPosition(MousePosition - DragOffset, SettingsSize);
		break;
	case ERogue10mDraggedWindow::CharacterPreview:
		CharacterPreviewYaw = FMath::Fmod(CharacterPreviewYaw + (MousePosition.X - LastCharacterPreviewDragX) * 0.45f, 360.0f);
		LastCharacterPreviewDragX = MousePosition.X;
		break;
	default:
		break;
	}
}

void ARogue10mHUD::UpdateInventoryCursor() const
{
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		const bool bShouldUseUiCursor = IsAnyBlockingWindowVisible();
		OwningPlayerController->SetShowMouseCursor(bShouldUseUiCursor);
		OwningPlayerController->bEnableClickEvents = bShouldUseUiCursor;
		OwningPlayerController->bEnableMouseOverEvents = bShouldUseUiCursor;

		if (bShouldUseUiCursor)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			OwningPlayerController->SetInputMode(InputMode);
		}
		else
		{
			FInputModeGameOnly InputMode;
			OwningPlayerController->SetInputMode(InputMode);
		}
	}
}

FString ARogue10mHUD::GetItemCategoryText(ERogue10mItemCategory Category) const
{
	switch (Category)
	{
	case ERogue10mItemCategory::Equipment:
		return TEXT("장비");
	case ERogue10mItemCategory::Consumable:
		return TEXT("소모품");
	case ERogue10mItemCategory::Material:
		return TEXT("재료");
	case ERogue10mItemCategory::Currency:
		return TEXT("재화");
	case ERogue10mItemCategory::Quest:
		return TEXT("퀘스트");
	default:
		return TEXT("알 수 없음");
	}
}

FString ARogue10mHUD::GetEquipmentSlotText(ERogue10mInventorySlotType SlotType) const
{
	switch (SlotType)
	{
	case ERogue10mInventorySlotType::MainWeapon:
		return TEXT("주무기");
	case ERogue10mInventorySlotType::SecondaryWeapon:
		return TEXT("보조무기");
	case ERogue10mInventorySlotType::Head:
		return TEXT("머리");
	case ERogue10mInventorySlotType::Armor:
		return TEXT("갑옷");
	case ERogue10mInventorySlotType::Shoes:
		return TEXT("신발");
	case ERogue10mInventorySlotType::Chest:
		return TEXT("가슴");
	case ERogue10mInventorySlotType::Legs:
		return TEXT("다리");
	case ERogue10mInventorySlotType::Hands:
		return TEXT("손");
	case ERogue10mInventorySlotType::Weapon:
		return TEXT("무기");
	case ERogue10mInventorySlotType::Charm:
		return TEXT("부적");
	case ERogue10mInventorySlotType::Ring:
		return TEXT("반지");
	case ERogue10mInventorySlotType::Earring:
		return TEXT("귀걸이");
	case ERogue10mInventorySlotType::Relic:
		return TEXT("유물");
	case ERogue10mInventorySlotType::Consumable:
		return TEXT("소모품");
	case ERogue10mInventorySlotType::Material:
		return TEXT("재료");
	default:
		return TEXT("알 수 없음");
	}
}

FString ARogue10mHUD::GetWeaponTypeText(ERogue10mWeaponType WeaponType) const
{
	switch (WeaponType)
	{
	case ERogue10mWeaponType::Unarmed:
		return TEXT("맨손");
	case ERogue10mWeaponType::Dagger:
		return TEXT("단검");
	case ERogue10mWeaponType::GreatSword:
		return TEXT("대검");
	case ERogue10mWeaponType::DualBlades:
		return TEXT("쌍검");
	case ERogue10mWeaponType::Bow:
		return TEXT("활");
	case ERogue10mWeaponType::Staff:
		return TEXT("지팡이");
	case ERogue10mWeaponType::Knuckle:
		return TEXT("권");
	default:
		return TEXT("알 수 없음");
	}
}

FLinearColor ARogue10mHUD::GetWeaponTypeColor(ERogue10mWeaponType WeaponType) const
{
	switch (WeaponType)
	{
	case ERogue10mWeaponType::Dagger:
		return FLinearColor(0.82f, 0.74f, 0.56f, 1.0f);
	case ERogue10mWeaponType::GreatSword:
		return FLinearColor(0.75f, 0.52f, 0.36f, 1.0f);
	case ERogue10mWeaponType::DualBlades:
		return FLinearColor(0.62f, 0.8f, 1.0f, 1.0f);
	case ERogue10mWeaponType::Bow:
		return FLinearColor(0.32f, 0.76f, 0.42f, 1.0f);
	case ERogue10mWeaponType::Staff:
		return FLinearColor(0.58f, 0.42f, 1.0f, 1.0f);
	case ERogue10mWeaponType::Knuckle:
		return FLinearColor(0.95f, 0.46f, 0.24f, 1.0f);
	default:
		return FLinearColor(0.55f, 0.58f, 0.64f, 1.0f);
	}
}

bool ARogue10mHUD::IsWeaponSkillTreeUnlocked(ERogue10mWeaponType WeaponType) const
{
	return WeaponType == ERogue10mWeaponType::Knuckle;
}

int32 ARogue10mHUD::GetWeaponProficiencyLevel(ERogue10mWeaponType WeaponType) const
{
	return IsWeaponSkillTreeUnlocked(WeaponType) ? 1 : 0;
}

int32 ARogue10mHUD::GetVisibleSkillSlotCount() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const ERogue10mWeaponType WeaponType = RogueCharacter ? RogueCharacter->GetEquippedWeaponType() : ERogue10mWeaponType::Knuckle;
	const int32 ProficiencyLevel = GetWeaponProficiencyLevel(WeaponType);
	const int32 BaseSlotCount = FMath::Max(3, QuickSlots.Num() - 1);
	return FMath::Clamp(BaseSlotCount + FMath::Max(0, ProficiencyLevel - 1), 3, 8);
}

bool ARogue10mHUD::IsAttackSkillUnlockedForUse(const URogue10mAttackSkillData* SkillData) const
{
	if (!SkillData)
	{
		return false;
	}

	const FName SkillAssetName = SkillData->GetFName();
	if (SkillAssetName == FName(TEXT("DA_Attack_Unarmed_Primary")))
	{
		return IsKnuckleSkillUnlocked(0);
	}

	if (SkillAssetName == FName(TEXT("DA_Attack_Unarmed_JumpPrimary")))
	{
		return IsKnuckleSkillUnlocked(1);
	}

	if (SkillAssetName == FName(TEXT("DA_Attack_Unarmed_Primary_Combo2")))
	{
		return IsKnuckleSkillUnlocked(2);
	}

	if (SkillAssetName == FName(TEXT("DA_Attack_Unarmed_Primary_Combo3")))
	{
		return IsKnuckleSkillUnlocked(3);
	}

	return false;
}

bool ARogue10mHUD::IsKnuckleSkillUnlocked(int32 SkillIndex) const
{
	if (SkillIndex == 0 || SkillIndex == 1)
	{
		return true;
	}

	if (SkillIndex == 2)
	{
		return bKnuckleCombo2Unlocked;
	}

	if (SkillIndex == 3)
	{
		return bKnuckleCombo3Unlocked;
	}

	if (SkillIndex == 4)
	{
		return bKnuckleFinisherUnlocked;
	}

	if (SkillIndex == 5)
	{
		return bKnuckleAirComboUnlocked;
	}

	if (SkillIndex == 6)
	{
		return bKnuckleAirFinisherUnlocked;
	}

	return false;
}

bool ARogue10mHUD::CanUnlockKnuckleSkill(int32 SkillIndex) const
{
	if (IsKnuckleSkillUnlocked(SkillIndex))
	{
		return false;
	}

	if (SkillIndex == 2)
	{
		return IsKnuckleSkillUnlocked(0) && IsKnuckleSkillUnlocked(1);
	}

	if (SkillIndex == 3)
	{
		return IsKnuckleSkillUnlocked(2);
	}

	if (SkillIndex == 4)
	{
		return IsKnuckleSkillUnlocked(3);
	}

	if (SkillIndex == 5)
	{
		return IsKnuckleSkillUnlocked(1);
	}

	if (SkillIndex == 6)
	{
		return IsKnuckleSkillUnlocked(5);
	}

	return false;
}

void ARogue10mHUD::UnlockKnuckleSkillForTest(int32 SkillIndex)
{
	if (!CanUnlockKnuckleSkill(SkillIndex))
	{
		return;
	}

	if (SkillIndex == 2)
	{
		bKnuckleCombo2Unlocked = true;
	}
	else if (SkillIndex == 3)
	{
		bKnuckleCombo3Unlocked = true;
	}
	else if (SkillIndex == 4)
	{
		bKnuckleFinisherUnlocked = true;
	}
	else if (SkillIndex == 5)
	{
		bKnuckleAirComboUnlocked = true;
	}
	else if (SkillIndex == 6)
	{
		bKnuckleAirFinisherUnlocked = true;
	}
}

const FRogue10mEquipmentSlotHitArea* ARogue10mHUD::FindEquipmentSlotHitArea(const FVector2D& MousePosition) const
{
	for (const FRogue10mEquipmentSlotHitArea& HitArea : EquipmentSlotHitAreas)
	{
		if (IsPointInRect(MousePosition, HitArea.Position, HitArea.Size))
		{
			return &HitArea;
		}
	}

	return nullptr;
}

const FRogue10mItemSlotHitArea* ARogue10mHUD::FindItemSlotHitArea(const FVector2D& MousePosition) const
{
	for (const FRogue10mItemSlotHitArea& HitArea : ItemSlotHitAreas)
	{
		if (IsPointInRect(MousePosition, HitArea.Position, HitArea.Size))
		{
			return &HitArea;
		}
	}

	return nullptr;
}

const FRogue10mSkillWeaponHitArea* ARogue10mHUD::FindSkillWeaponHitArea(const FVector2D& MousePosition) const
{
	for (const FRogue10mSkillWeaponHitArea& HitArea : SkillWeaponHitAreas)
	{
		if (IsPointInRect(MousePosition, HitArea.Position, HitArea.Size))
		{
			return &HitArea;
		}
	}

	return nullptr;
}

bool ARogue10mHUD::IsItemCompatibleWithSlot(const FRogue10mItemStack& Item, ERogue10mInventorySlotType SlotType) const
{
	return Item.bOccupied
		&& !Item.bLocked
		&& Item.Category == ERogue10mItemCategory::Equipment
		&& Item.EquipSlotType == SlotType;
}

bool ARogue10mHUD::TryDropDraggedItemToWorld(URogue10mInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent || !DraggedItem.bOccupied)
	{
		return false;
	}

	const FRogue10mItemStack ItemToDrop = DraggedItem;
	if (!SpawnDroppedItemInWorld(ItemToDrop))
	{
		return false;
	}

	FRogue10mItemStack RemovedItem;
	bool bRemoved = false;
	if (DraggedItemSource == ERogue10mDraggedItemSource::ItemSlot)
	{
		bRemoved = InventoryComponent->RemoveItemFromSlot(DraggedItemSlotIndex, RemovedItem);
	}
	else if (DraggedItemSource == ERogue10mDraggedItemSource::EquipmentSlot)
	{
		bRemoved = InventoryComponent->RemoveEquippedItemFromSlot(DraggedEquipmentSlotType, RemovedItem);
	}

	if (!bRemoved || !RemovedItem.bOccupied)
	{
		return false;
	}

	AddCombatLogMessage(FString::Printf(TEXT("아이템 버림: %s"), *RemovedItem.DisplayName.ToString()), FLinearColor(0.95f, 0.82f, 0.42f, 1.0f), 1.8f);
	return true;
}

bool ARogue10mHUD::SpawnDroppedItemInWorld(const FRogue10mItemStack& ItemStack)
{
	UWorld* World = GetWorld();
	const APawn* OwningPawn = GetOwningPawn();
	if (!World || !OwningPawn || !ItemStack.bOccupied)
	{
		return false;
	}

	const FVector SpawnLocation = OwningPawn->GetActorLocation() + OwningPawn->GetActorForwardVector() * 150.0f + FVector(0.0f, 0.0f, 28.0f);
	const FRotator SpawnRotation(0.0f, OwningPawn->GetActorRotation().Yaw, 0.0f);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = const_cast<APawn*>(OwningPawn);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ARogue10mDroppedItem* DroppedItem = World->SpawnActor<ARogue10mDroppedItem>(ARogue10mDroppedItem::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);
	if (!DroppedItem)
	{
		return false;
	}

	DroppedItem->InitializeDroppedItem(ItemStack);
	return true;
}

void ARogue10mHUD::ClearDraggedItem()
{
	bIsDraggingItem = false;
	DraggedItemSource = ERogue10mDraggedItemSource::None;
	DraggedItem = FRogue10mItemStack();
	DraggedItemMousePosition = FVector2D::ZeroVector;
	DraggedItemSlotIndex = INDEX_NONE;
	DraggedEquipmentSlotType = ERogue10mInventorySlotType::Material;
}

bool ARogue10mHUD::IsPointInRect(const FVector2D& Point, const FVector2D& RectPosition, const FVector2D& RectSize) const
{
	return Point.X >= RectPosition.X
		&& Point.Y >= RectPosition.Y
		&& Point.X <= RectPosition.X + RectSize.X
		&& Point.Y <= RectPosition.Y + RectSize.Y;
}

bool ARogue10mHUD::IsMouseOverInventoryOrItemWindow(const FVector2D& MousePosition) const
{
	if (!Canvas)
	{
		return false;
	}

	const FVector2D InventorySize(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
	const FVector2D ItemSize(Canvas->SizeX * 0.3f, Canvas->SizeY * 0.5f);
	return (bInventoryVisible && IsPointInRect(MousePosition, InventoryWindowPosition, InventorySize))
		|| (bItemWindowVisible && IsPointInRect(MousePosition, ItemWindowPosition, ItemSize));
}

FVector2D ARogue10mHUD::ClampWindowPosition(const FVector2D& Position, const FVector2D& Size) const
{
	if (!Canvas)
	{
		return Position;
	}

	const float MaxX = FMath::Max(0.0f, Canvas->SizeX - Size.X);
	const float MaxY = FMath::Max(0.0f, Canvas->SizeY - Size.Y);
	return FVector2D(FMath::Clamp(Position.X, 0.0f, MaxX), FMath::Clamp(Position.Y, 0.0f, MaxY));
}

void ARogue10mHUD::UpdateSettingsInteraction()
{
	if (!Canvas || !bSettingsVisible)
	{
		return;
	}

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!OwningPlayerController)
	{
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!OwningPlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	const FVector2D MousePosition(MouseX, MouseY);
	const bool bPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	const bool bHeld = OwningPlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
	if (!bHeld)
	{
		DraggedSettingsSlider = ERogue10mSettingsSlider::None;
		return;
	}

	const float PanelX = SettingsWindowPosition.X;
	const float PanelY = SettingsWindowPosition.Y;
	const float SliderX = PanelX + 44.0f;
	const float SliderWidth = Canvas->SizeX * 0.42f - 88.0f;

	if (bPressed)
	{
		const auto TryStartSliderDrag = [this, &MousePosition, SliderX, SliderWidth](ERogue10mSettingsSlider SliderType, float TrackY)
		{
			if (IsPointInRect(MousePosition, FVector2D(SliderX - 8.0f, TrackY - 12.0f), FVector2D(SliderWidth + 16.0f, 32.0f)))
			{
				DraggedSettingsSlider = SliderType;
				DraggedWindow = ERogue10mDraggedWindow::None;
				return true;
			}
			return false;
		};

		TryStartSliderDrag(ERogue10mSettingsSlider::MasterVolume, PanelY + 120.0f)
			|| TryStartSliderDrag(ERogue10mSettingsSlider::LookSensitivityX, PanelY + 180.0f)
			|| TryStartSliderDrag(ERogue10mSettingsSlider::LookSensitivityY, PanelY + 240.0f);
	}

	if (DraggedSettingsSlider != ERogue10mSettingsSlider::None)
	{
		const float LocalAlpha = FMath::Clamp((MousePosition.X - SliderX) / SliderWidth, 0.0f, 1.0f);
		UpdateSettingsSliderValue(DraggedSettingsSlider, LocalAlpha);
	}
}

void ARogue10mHUD::UpdateSettingsSliderValue(ERogue10mSettingsSlider SliderType, float LocalAlpha)
{
	switch (SliderType)
	{
	case ERogue10mSettingsSlider::MasterVolume:
		MasterVolume = FMath::Clamp(LocalAlpha, 0.0f, 1.0f);
		ApplyAudioVolume();
		break;
	case ERogue10mSettingsSlider::LookSensitivityX:
		LookSensitivityX = FMath::Lerp(0.2f, 3.0f, LocalAlpha);
		break;
	case ERogue10mSettingsSlider::LookSensitivityY:
		LookSensitivityY = FMath::Lerp(0.2f, 3.0f, LocalAlpha);
		break;
	default:
		break;
	}
}

void ARogue10mHUD::ApplyFpsLimit(int32 FpsValue)
{
	CurrentFpsLimit = FpsValue;
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		OwningPlayerController->ConsoleCommand(FString::Printf(TEXT("t.MaxFPS %d"), FpsValue), true);
	}
}

void ARogue10mHUD::ApplyAudioVolume() const
{
	if (APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		OwningPlayerController->ConsoleCommand(FString::Printf(TEXT("au.GlobalVolumeMultiplier %.2f"), MasterVolume), true);
	}
}
