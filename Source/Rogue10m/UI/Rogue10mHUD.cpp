// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mHUD.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Canvas.h"
#include "Engine/EngineTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Rogue10mBasicMonster.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mGameState.h"
#include "Rogue10mInventoryComponent.h"
#include "Rogue10mVitalsComponent.h"

ARogue10mHUD::ARogue10mHUD()
{
	CharacterPreviewYaw = 90.0f;

	// 임시 퀵 슬롯 데이터입니다. 실제 스킬/아이템 시스템이 생기면 데이터 기반으로 교체합니다.
	QuickSlots =
	{
		{ 1, FText::FromString(TEXT("Fist")), FLinearColor(0.9f, 0.36f, 0.22f, 1.0f), 1.8f, 0.0f },
		{ 2, FText::FromString(TEXT("Step")), FLinearColor(0.28f, 0.65f, 1.0f, 1.0f), 4.0f, 0.0f },
		{ 3, FText::FromString(TEXT("Guard")), FLinearColor(0.32f, 0.86f, 0.46f, 1.0f), 3.0f, 0.0f },
		{ 4, FText::FromString(TEXT("Skill")), FLinearColor(0.74f, 0.48f, 1.0f, 1.0f), 6.0f, 0.0f },
		{ 5, FText::FromString(TEXT("Item")), FLinearColor(0.95f, 0.75f, 0.26f, 1.0f), 8.0f, 0.0f }
	};
}

void ARogue10mHUD::DrawHUD()
{
	Super::DrawHUD();

	bHasHoveredItem = false;
	HoveredItemSlotIndex = INDEX_NONE;
	EquipmentSlotHitAreas.Reset();
	ItemSlotHitAreas.Reset();
	SkillWeaponHitAreas.Reset();

	UpdateWindowDrag();
	DrawRunTimer();
	DrawVitals();
	DrawCharacterInfo();
	DrawLookedAtMonsterInfo();
	DrawCombatLog();
	DrawQuickSlots();
	DrawPanelShortcutHints();
	DrawRunResult();
	DrawInventory();
	DrawItemWindow();
	DrawSkillTree();
	DrawSettingsPanel();
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
	if (!GetWorld())
	{
		return;
	}

	FRogue10mCombatLogEntry Entry;
	Entry.Message = Message;
	Entry.Color = Color;
	Entry.ExpireTime = GetWorld()->GetTimeSeconds() + FMath::Max(0.2f, Duration);
	CombatLogEntries.Insert(Entry, 0);

	constexpr int32 MaxCombatLogCount = 6;
	if (CombatLogEntries.Num() > MaxCombatLogCount)
	{
		CombatLogEntries.SetNum(MaxCombatLogCount);
	}
}

void ARogue10mHUD::DrawRunTimer()
{
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!Canvas || !RogueGameState)
	{
		return;
	}

	const FString TimerText = FString::Printf(TEXT("Run Timer  %s"), *RogueGameState->GetRemainingTimeText().ToString());
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
		ResultText = TEXT("RUN CLEARED");
		ResultColor = FLinearColor(0.2f, 1.0f, 0.35f, 1.0f);
		break;
	case ERogue10mRunPhase::Defeat:
		ResultText = TEXT("GAME OVER");
		ResultColor = WarningColor;
		break;
	default:
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f - 150.0f;
	const float CenterY = Canvas->SizeY * 0.5f - 40.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), CenterX - 48.0f, CenterY - 28.0f, 396.0f, 116.0f);
	DrawText(ResultText, ResultColor, CenterX, CenterY, nullptr, ResultScale, false);
	DrawText(TEXT("The character died when the run timer expired."), TimerColor, CenterX - 20.0f, CenterY + 52.0f, nullptr, 1.1f, false);
}

void ARogue10mHUD::DrawVitals()
{
	if (!Canvas)
	{
		return;
	}

	const ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!VitalsComponent)
	{
		return;
	}

	const FVector2D BarSize(260.0f, 20.0f);
	const int32 VisibleBarCount = VitalsComponent->ShouldShowMana() ? 3 : 2;
	const FVector2D StartPosition(32.0f, Canvas->SizeY - (VisibleBarCount * 34.0f + 16.0f));
	DrawVitalBar(TEXT("HP"), VitalsComponent->GetHealth(), StartPosition, BarSize, FLinearColor(0.92f, 0.12f, 0.12f, 1.0f));

	float NextBarOffsetY = 34.0f;
	if (VitalsComponent->ShouldShowMana())
	{
		DrawVitalBar(TEXT("MP"), VitalsComponent->GetMana(), StartPosition + FVector2D(0.0f, NextBarOffsetY), BarSize, FLinearColor(0.15f, 0.42f, 1.0f, 1.0f));
		NextBarOffsetY += 34.0f;
	}

	DrawVitalBar(TEXT("SP"), VitalsComponent->GetStamina(), StartPosition + FVector2D(0.0f, NextBarOffsetY), BarSize, FLinearColor(0.16f, 0.86f, 0.32f, 1.0f));
}

void ARogue10mHUD::DrawVitalBar(const FString& Label, const FRogue10mVitalValue& Vital, const FVector2D& Position, const FVector2D& Size, const FLinearColor& FillColor)
{
	const float FillWidth = Size.X * Vital.GetNormalized();
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.58f), Position.X - 4.0f, Position.Y - 4.0f, Size.X + 8.0f, Size.Y + 8.0f);
	DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), Position.X, Position.Y, Size.X, Size.Y);
	DrawRect(FillColor, Position.X, Position.Y, FillWidth, Size.Y);
	DrawRect(FLinearColor(1.0f, 1.0f, 1.0f, 0.18f), Position.X, Position.Y, Size.X, 2.0f);

	DrawText(Label, InventoryTextColor, Position.X + 8.0f, Position.Y + 3.0f, nullptr, 0.82f, false);
	DrawText(FString::Printf(TEXT("%.0f / %.0f"), Vital.Current, Vital.Max), InventoryTextColor, Position.X + Size.X - 86.0f, Position.Y + 3.0f, nullptr, 0.82f, false);
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

	const FVector2D PanelPosition(32.0f, Canvas->SizeY * 0.34f);
	const float LineHeight = 22.0f;
	const float PanelWidth = 420.0f;
	const float PanelHeight = CombatLogEntries.Num() * LineHeight + 16.0f;
	if (CombatLogEntries.Num() <= 0)
	{
		return;
	}

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.46f), PanelPosition.X - 8.0f, PanelPosition.Y - 8.0f, PanelWidth, PanelHeight);
	for (int32 Index = 0; Index < CombatLogEntries.Num(); ++Index)
	{
		const FRogue10mCombatLogEntry& Entry = CombatLogEntries[Index];
		DrawText(Entry.Message, Entry.Color, PanelPosition.X, PanelPosition.Y + Index * LineHeight, nullptr, 0.82f, false);
	}
}

void ARogue10mHUD::DrawPanelShortcutHints()
{
	if (!Canvas)
	{
		return;
	}

	const float RowWidth = 188.0f;
	const float RowHeight = 32.0f;
	const float Gap = 8.0f;
	const float PanelX = Canvas->SizeX - RowWidth - 28.0f;
	const float PanelY = Canvas->SizeY - (RowHeight * 3.0f + Gap * 2.0f) - 28.0f;

	struct FShortcutHint
	{
		const TCHAR* Key;
		const TCHAR* Label;
		FLinearColor Color;
		bool bActive;
	};

	const FShortcutHint Hints[] =
	{
		{ TEXT("B"), TEXT("Inventory"), FLinearColor(0.35f, 0.92f, 0.72f, 1.0f), bItemWindowVisible },
		{ TEXT("I"), TEXT("Equipment"), FLinearColor(0.92f, 0.72f, 0.35f, 1.0f), bInventoryVisible },
		{ TEXT("K"), TEXT("Skill Tree"), FLinearColor(0.58f, 0.72f, 1.0f, 1.0f), bSkillTreeVisible }
	};

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.38f), PanelX - 8.0f, PanelY - 8.0f, RowWidth + 16.0f, RowHeight * 3.0f + Gap * 2.0f + 16.0f);

	for (int32 Index = 0; Index < 3; ++Index)
	{
		const FShortcutHint& Hint = Hints[Index];
		const float RowY = PanelY + Index * (RowHeight + Gap);
		const FLinearColor RowColor = Hint.bActive ? FLinearColor(0.12f, 0.18f, 0.26f, 0.94f) : FLinearColor(0.025f, 0.03f, 0.04f, 0.86f);

		DrawRect(RowColor, PanelX, RowY, RowWidth, RowHeight);
		DrawRect(Hint.Color, PanelX + 6.0f, RowY + 6.0f, 20.0f, 20.0f);
		DrawText(Hint.Key, FLinearColor::Black, PanelX + 12.0f, RowY + 8.0f, nullptr, 0.62f, false);
		DrawText(Hint.Label, InventoryTextColor, PanelX + 38.0f, RowY + 8.0f, nullptr, 0.74f, false);
		DrawText(Hint.bActive ? TEXT("Open") : TEXT("Toggle"), Hint.bActive ? Hint.Color : FLinearColor(0.62f, 0.68f, 0.76f, 1.0f), PanelX + RowWidth - 58.0f, RowY + 8.0f, nullptr, 0.62f, false);
	}
}

void ARogue10mHUD::DrawQuickSlots()
{
	if (!Canvas || QuickSlots.Num() <= 0)
	{
		return;
	}

	const float SlotSize = FMath::Clamp(Canvas->SizeY * 0.058f, 44.0f, 56.0f);
	const float SlotGap = 8.0f;
	const float GroupWidth = QuickSlots.Num() * SlotSize + (QuickSlots.Num() - 1) * SlotGap;
	const float StartX = Canvas->SizeX * 0.5f - GroupWidth * 0.5f;
	const float StartY = Canvas->SizeY - SlotSize - 24.0f;

	// 너무 크지 않은 하단 중앙 묶음으로 배치해 전투 시야를 가리지 않게 합니다.
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.42f), StartX - 10.0f, StartY - 10.0f, GroupWidth + 20.0f, SlotSize + 20.0f);

	for (int32 Index = 0; Index < QuickSlots.Num(); ++Index)
	{
		DrawQuickSlot(QuickSlots[Index], FVector2D(StartX + Index * (SlotSize + SlotGap), StartY), SlotSize);
	}
}

void ARogue10mHUD::DrawQuickSlot(const FRogue10mQuickSlotView& QuickSlot, const FVector2D& Position, float SlotSize)
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
	DrawText(FString::FromInt(QuickSlot.SlotNumber), InventoryTextColor, Position.X + 7.0f, Position.Y + 4.0f, nullptr, 0.62f, false);

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
	const float PanelY = 28.0f;
	const float TextScale = 0.72f;
	const float LineHeight = 18.0f;
	float TextY = PanelY + 14.0f;

	const FString StateText = RogueCharacter->IsDead()
		? TEXT("Dead")
		: (IsAnyBlockingWindowVisible() ? TEXT("UI Locked") : TEXT("Ready"));

	DrawRect(FLinearColor(0.01f, 0.014f, 0.02f, 0.72f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.55f, 0.72f, 0.95f, 0.85f), PanelX, PanelY, PanelWidth, 2.0f);

	DrawText(TEXT("CHARACTER INFO"), FLinearColor(0.78f, 0.86f, 1.0f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight + 4.0f;
	DrawText(FString::Printf(TEXT("Name : %s"), *RogueCharacter->GetCharacterDisplayName().ToString()), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("Job : %s"), *RogueCharacter->GetCharacterJobName().ToString()), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("State : %s"), *StateText), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("Weapon : %s"), *GetWeaponTypeText(RogueCharacter->GetEquippedWeaponType())), InventoryTextColor, PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	DrawText(FString::Printf(TEXT("HP : %.0f / %.0f"), VitalsComponent->GetHealth().Current, VitalsComponent->GetHealth().Max), FLinearColor(1.0f, 0.72f, 0.72f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
	TextY += LineHeight;
	if (VitalsComponent->ShouldShowMana())
	{
		DrawText(FString::Printf(TEXT("MP : %.0f / %.0f"), VitalsComponent->GetMana().Current, VitalsComponent->GetMana().Max), FLinearColor(0.68f, 0.8f, 1.0f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
		TextY += LineHeight;
	}
	DrawText(FString::Printf(TEXT("SP : %.0f / %.0f"), VitalsComponent->GetStamina().Current, VitalsComponent->GetStamina().Max), FLinearColor(0.7f, 1.0f, 0.72f, 1.0f), PanelX + 14.0f, TextY, nullptr, TextScale, false);
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
	const float PanelWidth = 360.0f;
	const float PanelHeight = 74.0f;
	const float PanelX = Canvas->SizeX * 0.5f - PanelWidth * 0.5f;
	const float PanelY = 28.0f;
	const float BarX = PanelX + 18.0f;
	const float BarY = PanelY + 44.0f;
	const float BarWidth = PanelWidth - 36.0f;
	const float BarHeight = 12.0f;
	const FString StateText = Monster->IsDead() ? TEXT("Dead") : TEXT("Hostile");

	DrawRect(FLinearColor(0.01f, 0.012f, 0.016f, 0.78f), PanelX, PanelY, PanelWidth, PanelHeight);
	DrawRect(FLinearColor(0.84f, 0.22f, 0.18f, 0.95f), PanelX, PanelY, PanelWidth, 3.0f);
	DrawText(Monster->GetMonsterDisplayName().ToString(), InventoryTextColor, PanelX + 18.0f, PanelY + 14.0f, nullptr, 0.9f, false);
	DrawText(StateText, FLinearColor(1.0f, 0.62f, 0.5f, 1.0f), PanelX + PanelWidth - 76.0f, PanelY + 15.0f, nullptr, 0.72f, false);

	DrawRect(FLinearColor(0.05f, 0.055f, 0.065f, 0.95f), BarX, BarY, BarWidth, BarHeight);
	DrawRect(FLinearColor(0.92f, 0.1f, 0.08f, 1.0f), BarX, BarY, BarWidth * Health.GetNormalized(), BarHeight);
	DrawText(FString::Printf(TEXT("HP %.0f / %.0f"), Health.Current, Health.Max), InventoryTextColor, BarX + BarWidth - 92.0f, BarY - 2.0f, nullptr, 0.62f, false);
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
	DrawText(TEXT("INVENTORY"), InventoryTextColor, PanelX + 28.0f, PanelY + 22.0f, nullptr, 1.35f, false);
	DrawText(TEXT("I : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 250.0f, PanelY + 26.0f, nullptr, 0.85f, false);

	DrawCharacterPreview(RogueCharacter, PanelX + PanelWidth * 0.27f, PanelY + 62.0f, PanelWidth * 0.46f, PanelHeight - 78.0f);
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
	DrawText(TEXT("ITEMS"), InventoryTextColor, PanelX + PanelWidth * 0.5f - 36.0f, PanelY + 18.0f, nullptr, 1.35f, false);
	DrawText(TEXT("B : Close"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 112.0f, PanelY + 22.0f, nullptr, 0.85f, false);

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
	DrawText(FString::Printf(TEXT("Gold  %d"), InventoryComponent->GetGold()), FLinearColor(0.35f, 1.0f, 0.72f, 1.0f), PanelX + PanelWidth * 0.44f, FooterY + 12.0f, nullptr, 1.0f, false);
	DrawText(FString::Printf(TEXT("Crystals  %d"), InventoryComponent->GetCrystals()), FLinearColor(0.62f, 0.8f, 1.0f, 1.0f), PanelX + PanelWidth * 0.72f, FooterY + 12.0f, nullptr, 1.0f, false);
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
	DrawText(TEXT("SKILL TREE"), InventoryTextColor, PanelX + 24.0f, PanelY + 19.0f, nullptr, 1.35f, false);
	DrawText(TEXT("K : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 260.0f, PanelY + 23.0f, nullptr, 0.84f, false);

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
		ERogue10mWeaponType::Dagger,
		ERogue10mWeaponType::GreatSword,
		ERogue10mWeaponType::DualBlades,
		ERogue10mWeaponType::Bow,
		ERogue10mWeaponType::Staff,
		ERogue10mWeaponType::Knuckle
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

	DrawText(TEXT("Select a weapon"), FLinearColor(0.82f, 0.88f, 0.96f, 1.0f), PanelX + 28.0f, PanelY + 74.0f, nullptr, 0.95f, false);

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
		const bool bHovered = bHasMousePosition && IsPointInRect(MousePosition, CardPosition, CardSize);

		SkillWeaponHitAreas.Add({ WeaponType, CardPosition, CardSize });

		DrawRect(bHovered ? FLinearColor(0.95f, 0.82f, 0.35f, 0.9f) : FLinearColor(0.34f, 0.38f, 0.45f, 0.9f), CardPosition.X - 2.0f, CardPosition.Y - 2.0f, CardSize.X + 4.0f, CardSize.Y + 4.0f);
		DrawRect(FLinearColor(0.025f, 0.03f, 0.04f, 0.96f), CardPosition.X, CardPosition.Y, CardSize.X, CardSize.Y);
		DrawRect(GetWeaponTypeColor(WeaponType), CardPosition.X + 12.0f, CardPosition.Y + 12.0f, CardSize.X - 24.0f, CardSize.Y - 46.0f);
		DrawText(GetWeaponTypeText(WeaponType), InventoryTextColor, CardPosition.X + 14.0f, CardPosition.Y + CardSize.Y - 28.0f, nullptr, 0.9f, false);

		if (bHovered && bLeftClickPressed)
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
	DrawText(FString::Printf(TEXT("%s Skill Tree"), *WeaponName), InventoryTextColor, PanelX + 130.0f, PanelY + 80.0f, nullptr, 1.15f, false);

	const float CenterX = PanelX + PanelWidth * 0.5f;
	const float StartY = PanelY + 150.0f;
	const FLinearColor WeaponColor = GetWeaponTypeColor(SelectedSkillTreeWeapon);

	// 무기별 상세 설계가 달라질 예정이므로 현재는 공통 노드 자리만 표시합니다.
	for (int32 Index = 0; Index < 6; ++Index)
	{
		const float NodeX = CenterX + ((Index % 2 == 0) ? -90.0f : 90.0f);
		const float NodeY = StartY + (Index / 2) * 82.0f;
		DrawRect(FLinearColor(0.34f, 0.38f, 0.45f, 0.92f), NodeX - 38.0f, NodeY - 22.0f, 76.0f, 44.0f);
		DrawRect(WeaponColor, NodeX - 32.0f, NodeY - 16.0f, 64.0f, 32.0f);
		DrawText(FString::Printf(TEXT("Skill %d"), Index + 1), InventoryTextColor, NodeX - 26.0f, NodeY - 7.0f, nullptr, 0.65f, false);
	}

	DrawText(TEXT("Prototype layout. Each weapon can receive a custom tree later."), FLinearColor(0.68f, 0.74f, 0.82f, 1.0f), PanelX + 28.0f, PanelY + PanelHeight - 44.0f, nullptr, 0.78f, false);
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
	DrawText(TEXT("SETTINGS"), InventoryTextColor, PanelX + 24.0f, PanelY + 18.0f, nullptr, 1.25f, false);
	DrawText(TEXT("O : Close  |  Drag title bar"), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), PanelX + PanelWidth - 236.0f, PanelY + 22.0f, nullptr, 0.78f, false);

	const float SliderX = PanelX + 44.0f;
	const float SliderWidth = PanelWidth - 88.0f;
	DrawSettingsSlider(TEXT("Voice Volume"), ERogue10mSettingsSlider::MasterVolume, MasterVolume, 0.0f, 1.0f, FVector2D(SliderX, PanelY + 94.0f), SliderWidth);
	DrawSettingsSlider(TEXT("Look Sensitivity X"), ERogue10mSettingsSlider::LookSensitivityX, LookSensitivityX, 0.2f, 3.0f, FVector2D(SliderX, PanelY + 154.0f), SliderWidth);
	DrawSettingsSlider(TEXT("Look Sensitivity Y"), ERogue10mSettingsSlider::LookSensitivityY, LookSensitivityY, 0.2f, 3.0f, FVector2D(SliderX, PanelY + 214.0f), SliderWidth);

	DrawText(TEXT("FPS Limit"), InventoryTextColor, SliderX, PanelY + 274.0f, nullptr, 0.86f, false);
	const FVector2D ButtonSize(78.0f, 34.0f);
	DrawSettingsFpsButton(60, FVector2D(SliderX, PanelY + 304.0f), ButtonSize);
	DrawSettingsFpsButton(120, FVector2D(SliderX + 94.0f, PanelY + 304.0f), ButtonSize);
	DrawSettingsFpsButton(140, FVector2D(SliderX + 188.0f, PanelY + 304.0f), ButtonSize);
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

void ARogue10mHUD::DrawSettingsFpsButton(int32 FpsValue, const FVector2D& Position, const FVector2D& Size)
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
		ApplyFpsLimit(FpsValue);
	}

	const bool bSelected = CurrentFpsLimit == FpsValue;
	const FLinearColor ButtonColor = bSelected ? FLinearColor(0.24f, 0.64f, 0.36f, 0.96f) : (bHovered ? FLinearColor(0.26f, 0.38f, 0.56f, 0.96f) : FLinearColor(0.14f, 0.18f, 0.25f, 0.96f));
	DrawRect(ButtonColor, Position.X, Position.Y, Size.X, Size.Y);
	DrawText(FString::Printf(TEXT("%d"), FpsValue), InventoryTextColor, Position.X + 22.0f, Position.Y + 9.0f, nullptr, 0.82f, false);
}

void ARogue10mHUD::DrawInventorySlots(const TArray<FRogue10mInventorySlot>& Slots, float X, float Y, float SlotSize, float Gap, bool bRightSide)
{
	ARogue10mCharacter* RogueCharacter = GetOwningPawn() ? Cast<ARogue10mCharacter>(GetOwningPawn()) : nullptr;
	URogue10mInventoryComponent* InventoryComponent = RogueCharacter ? RogueCharacter->GetInventoryComponent() : nullptr;
	FVector2D MousePosition = FVector2D::ZeroVector;
	bool bHasMousePosition = false;
	bool bRightClickPressed = false;
	if (const APlayerController* OwningPlayerController = GetOwningPlayerController())
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		bHasMousePosition = OwningPlayerController->GetMousePosition(MouseX, MouseY);
		MousePosition = FVector2D(MouseX, MouseY);
		bRightClickPressed = OwningPlayerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
	}

	const float PulseAlpha = GetWorld() ? (0.35f + 0.35f * FMath::Abs(FMath::Sin(GetWorld()->GetTimeSeconds() * 7.5f))) : 0.45f;

	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FRogue10mInventorySlot& Slot = Slots[Index];
		const float SlotY = Y + Index * (SlotSize + Gap);
		EquipmentSlotHitAreas.Add({ Slot.SlotType, FVector2D(X, SlotY), FVector2D(SlotSize, SlotSize), Slot.bLocked });
		const bool bDragCompatible = bIsDraggingItem && IsItemCompatibleWithSlot(DraggedItem, Slot.SlotType) && !Slot.bLocked;
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

		const FString SlotName = Slot.DisplayName.ToString();
		DrawText(SlotName, InventoryTextColor, X + 8.0f, SlotY + 10.0f, nullptr, 0.85f, false);

		if (Slot.bLocked)
		{
			DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), X, SlotY, SlotSize, SlotSize);
			DrawText(TEXT("LOCK"), FLinearColor(0.9f, 0.9f, 0.9f, 1.0f), X + 12.0f, SlotY + 38.0f, nullptr, 0.75f, false);
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
			if (const FRogue10mEquipmentSlotHitArea* DropTarget = FindEquipmentSlotHitArea(MousePosition))
			{
				InventoryComponent->TryEquipItemToSlot(DraggedItemSlotIndex, DropTarget->SlotType);
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
					InventoryComponent->TryMoveItemSlot(DraggedItemSlotIndex, TargetItemSlotIndex);
				}
			}

			bIsDraggingItem = false;
			DraggedItemSlotIndex = INDEX_NONE;
		}
	}
	else if (!bLeftClickHeld)
	{
		bIsDraggingItem = false;
		DraggedItemSlotIndex = INDEX_NONE;
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
				DraggedItem = Item;
				DraggedItemSlotIndex = Index;
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
			DrawText(TEXT("LOCK"), FLinearColor(0.95f, 0.95f, 0.95f, 1.0f), SlotX + 7.0f, SlotY + SlotSize - 20.0f, nullptr, 0.65f, false);
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
	const float TooltipHeight = HoveredItem.EquipSlotType == ERogue10mInventorySlotType::Weapon ? 192.0f : (HoveredItem.Category == ERogue10mItemCategory::Equipment ? 170.0f : 145.0f);
	const FVector2D TooltipSize(TooltipWidth, TooltipHeight);
	FVector2D TooltipPosition = HoveredItemMousePosition + FVector2D(18.0f, 18.0f);
	TooltipPosition = ClampWindowPosition(TooltipPosition, TooltipSize);

	DrawRect(FLinearColor(0.015f, 0.018f, 0.022f, 0.97f), TooltipPosition.X, TooltipPosition.Y, TooltipWidth, TooltipHeight);
	DrawRect(HoveredItem.ItemColor, TooltipPosition.X, TooltipPosition.Y, TooltipWidth, 4.0f);

	const float TextX = TooltipPosition.X + 14.0f;
	float TextY = TooltipPosition.Y + 16.0f;
	DrawText(HoveredItem.DisplayName.ToString(), InventoryTextColor, TextX, TextY, nullptr, 1.05f, false);

	TextY += 28.0f;
	DrawText(FString::Printf(TEXT("%s  |  Qty %d"), *GetItemCategoryText(HoveredItem.Category), HoveredItem.Quantity), FLinearColor(0.65f, 0.78f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.78f, false);

	if (HoveredItem.Category == ERogue10mItemCategory::Equipment)
	{
		TextY += 22.0f;
		DrawText(FString::Printf(TEXT("Slot : %s"), *GetEquipmentSlotText(HoveredItem.EquipSlotType)), FLinearColor(0.85f, 0.76f, 0.48f, 1.0f), TextX, TextY, nullptr, 0.8f, false);
		if (HoveredItem.EquipSlotType == ERogue10mInventorySlotType::Weapon)
		{
			// 무기 아이템은 숙련도/스킬 분기를 위해 세부 무기 타입도 함께 보여줍니다.
			TextY += 22.0f;
			DrawText(FString::Printf(TEXT("Type : %s"), *GetWeaponTypeText(HoveredItem.WeaponType)), FLinearColor(0.9f, 0.66f, 0.42f, 1.0f), TextX, TextY, nullptr, 0.8f, false);
		}
	}

	TextY += 28.0f;
	DrawText(HoveredItem.Description.ToString(), FLinearColor(0.82f, 0.86f, 0.9f, 1.0f), TextX, TextY, nullptr, 0.72f, false);

	TextY += 38.0f;
	FString ActionText = TEXT("Right-click : Equip");
	FLinearColor ActionColor = FLinearColor(0.35f, 1.0f, 0.72f, 1.0f);
	if (HoveredItem.bLocked)
	{
		ActionText = TEXT("Locked item");
		ActionColor = WarningColor;
	}
	else if (HoveredItem.Category != ERogue10mItemCategory::Equipment)
	{
		ActionText = TEXT("Not equipable");
		ActionColor = FLinearColor(0.68f, 0.72f, 0.78f, 1.0f);
	}

	DrawText(ActionText, ActionColor, TextX, TextY, nullptr, 0.82f, false);
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
	const bool bCanEquipDrop = DropTarget && !DropTarget->bLocked && IsItemCompatibleWithSlot(DraggedItem, DropTarget->SlotType);
	const bool bCanMoveDrop = ItemDropTarget && !ItemDropTarget->bLocked && ItemDropTarget->SlotIndex != DraggedItemSlotIndex;
	const bool bCanDrop = bCanEquipDrop || bCanMoveDrop;
	const FLinearColor BorderColor = bCanDrop ? FLinearColor(0.2f, 1.0f, 0.55f, 0.95f) : FLinearColor(1.0f, 0.35f, 0.25f, 0.85f);

	DrawRect(BorderColor, DrawPosition.X - 3.0f, DrawPosition.Y - 3.0f, DragSlotSize + 6.0f, DragSlotSize + 6.0f);
	DrawRect(FLinearColor(0.02f, 0.025f, 0.03f, 0.92f), DrawPosition.X, DrawPosition.Y, DragSlotSize, DragSlotSize);
	DrawRect(DraggedItem.ItemColor, DrawPosition.X + 6.0f, DrawPosition.Y + 6.0f, DragSlotSize - 12.0f, DragSlotSize - 12.0f);
	DrawText(DraggedItem.DisplayName.ToString(), InventoryTextColor, DrawPosition.X + 7.0f, DrawPosition.Y + 11.0f, nullptr, 0.58f, false);

	const FString DropText = bCanEquipDrop ? TEXT("Release : Equip") : (bCanMoveDrop ? TEXT("Release : Move") : TEXT("Drop on slot"));
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
		DrawText(TEXT("Character preview unavailable"), FLinearColor(0.75f, 0.82f, 0.9f, 1.0f), X + 20.0f, Y + Height * 0.5f, nullptr, 0.9f, false);
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
		OwningPlayerController->SetShowMouseCursor(IsAnyBlockingWindowVisible());
	}
}

FString ARogue10mHUD::GetItemCategoryText(ERogue10mItemCategory Category) const
{
	switch (Category)
	{
	case ERogue10mItemCategory::Equipment:
		return TEXT("Equipment");
	case ERogue10mItemCategory::Consumable:
		return TEXT("Consumable");
	case ERogue10mItemCategory::Material:
		return TEXT("Material");
	case ERogue10mItemCategory::Currency:
		return TEXT("Currency");
	case ERogue10mItemCategory::Quest:
		return TEXT("Quest");
	default:
		return TEXT("Unknown");
	}
}

FString ARogue10mHUD::GetEquipmentSlotText(ERogue10mInventorySlotType SlotType) const
{
	switch (SlotType)
	{
	case ERogue10mInventorySlotType::Head:
		return TEXT("Head");
	case ERogue10mInventorySlotType::Chest:
		return TEXT("Chest");
	case ERogue10mInventorySlotType::Legs:
		return TEXT("Legs");
	case ERogue10mInventorySlotType::Hands:
		return TEXT("Hands");
	case ERogue10mInventorySlotType::Weapon:
		return TEXT("Weapon");
	case ERogue10mInventorySlotType::Charm:
		return TEXT("Charm");
	case ERogue10mInventorySlotType::Ring:
		return TEXT("Ring");
	case ERogue10mInventorySlotType::Relic:
		return TEXT("Relic");
	case ERogue10mInventorySlotType::Consumable:
		return TEXT("Consumable");
	case ERogue10mInventorySlotType::Material:
		return TEXT("Material");
	default:
		return TEXT("Unknown");
	}
}

FString ARogue10mHUD::GetWeaponTypeText(ERogue10mWeaponType WeaponType) const
{
	switch (WeaponType)
	{
	case ERogue10mWeaponType::Unarmed:
		return TEXT("Unarmed");
	case ERogue10mWeaponType::Dagger:
		return TEXT("Dagger");
	case ERogue10mWeaponType::GreatSword:
		return TEXT("Great Sword");
	case ERogue10mWeaponType::DualBlades:
		return TEXT("Dual Blades");
	case ERogue10mWeaponType::Bow:
		return TEXT("Bow");
	case ERogue10mWeaponType::Staff:
		return TEXT("Staff");
	case ERogue10mWeaponType::Knuckle:
		return TEXT("Knuckle");
	default:
		return TEXT("Unknown");
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

bool ARogue10mHUD::IsPointInRect(const FVector2D& Point, const FVector2D& RectPosition, const FVector2D& RectSize) const
{
	return Point.X >= RectPosition.X
		&& Point.Y >= RectPosition.Y
		&& Point.X <= RectPosition.X + RectSize.X
		&& Point.Y <= RectPosition.Y + RectSize.Y;
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
