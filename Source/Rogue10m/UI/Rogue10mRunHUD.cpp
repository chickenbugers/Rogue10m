// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mRunHUD.h"

#include "Rogue10mBasicMonster.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mHUD.h"
#include "Rogue10mPlayerState.h"
#include "Rogue10mAttackSkillData.h"
#include "Rogue10mVitalsComponent.h"
#include "GameFramework/PlayerController.h"

namespace
{
	FText GetIdentityResourceLabel(ERogue10mIdentityType IdentityType)
	{
		switch (IdentityType)
		{
		case ERogue10mIdentityType::StoneFist:
			return FText::FromString(TEXT("돌주먹"));
		case ERogue10mIdentityType::Vigor:
			return FText::FromString(TEXT("기력"));
		case ERogue10mIdentityType::Mana:
			return FText::FromString(TEXT("마력"));
		case ERogue10mIdentityType::Focus:
			return FText::FromString(TEXT("집중"));
		case ERogue10mIdentityType::Rage:
			return FText::FromString(TEXT("분노"));
		case ERogue10mIdentityType::None:
		default:
			return FText::GetEmpty();
		}
	}

	TSoftObjectPtr<UTexture2D> GetIdentityIconTexture(ERogue10mIdentityType IdentityType)
	{
		switch (IdentityType)
		{
		case ERogue10mIdentityType::StoneFist:
			return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_StoneFist.T_Identity_StoneFist")));
		case ERogue10mIdentityType::Vigor:
			return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_Vigor.T_Identity_Vigor")));
		case ERogue10mIdentityType::Mana:
			return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_Mana.T_Identity_Mana")));
		case ERogue10mIdentityType::Focus:
			return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_Focus.T_Identity_Focus")));
		case ERogue10mIdentityType::Rage:
			return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_Rage.T_Identity_Rage")));
		case ERogue10mIdentityType::None:
		default:
			return TSoftObjectPtr<UTexture2D>();
		}
	}
}

void URogue10mRunHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bBroadcastHudDataUpdatedEveryTick)
	{
		BP_OnHudDataUpdated(InDeltaTime);
	}
}

ARogue10mHUD* URogue10mRunHUD::GetRogueHUD() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	return PlayerController ? PlayerController->GetHUD<ARogue10mHUD>() : nullptr;
}

FRogue10mHudVitalView URogue10mRunHUD::GetHealthView() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	return VitalsComponent ? MakeVitalView(VitalsComponent->GetHealth(), true, FLinearColor(0.92f, 0.12f, 0.12f, 1.0f)) : FRogue10mHudVitalView();
}

FRogue10mHudVitalView URogue10mRunHUD::GetStaminaView() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	return VitalsComponent ? MakeVitalView(VitalsComponent->GetStamina(), true, FLinearColor(0.15f, 0.42f, 1.0f, 1.0f)) : FRogue10mHudVitalView();
}

FRogue10mHudVitalView URogue10mRunHUD::GetManaView() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!VitalsComponent || !VitalsComponent->ShouldShowMana())
	{
		return FRogue10mHudVitalView();
	}

	return MakeVitalView(VitalsComponent->GetMana(), true, FLinearColor(0.45f, 0.72f, 1.0f, 1.0f));
}

FRogue10mHudProgressionView URogue10mRunHUD::GetProgressionView() const
{
	FRogue10mHudProgressionView View;
	const APlayerController* PlayerController = GetOwningPlayer();
	const ARogue10mPlayerState* RoguePlayerState = PlayerController ? PlayerController->GetPlayerState<ARogue10mPlayerState>() : nullptr;
	if (!RoguePlayerState)
	{
		return View;
	}

	View.Level = RoguePlayerState->GetPlayerLevel();
	View.CurrentExperience = RoguePlayerState->GetCurrentExperience();
	View.ExperienceToNextLevel = RoguePlayerState->GetExperienceToNextLevel();
	View.ExperienceNormalized = RoguePlayerState->GetExperienceNormalized();
	return View;
}

FRogue10mHudIdentityView URogue10mRunHUD::GetIdentityView() const
{
	FRogue10mHudIdentityView View;
	const APlayerController* PlayerController = GetOwningPlayer();
	const ARogue10mPlayerState* RoguePlayerState = PlayerController ? PlayerController->GetPlayerState<ARogue10mPlayerState>() : nullptr;
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	if (!RoguePlayerState)
	{
		return View;
	}

	const ERogue10mWeaponType WeaponType = RogueCharacter ? RogueCharacter->GetEquippedWeaponType() : ERogue10mWeaponType::Knuckle;
	const ERogue10mIdentityType IdentityType = RoguePlayerState->GetIdentityType();
	const int32 MasteryLevel = RoguePlayerState->GetWeaponMasteryLevel(WeaponType);
	constexpr int32 MaxMasteryLevel = 10;
	const float MasteryNormalized = FMath::Clamp(static_cast<float>(MasteryLevel) / static_cast<float>(MaxMasteryLevel), 0.0f, 1.0f);
	const FString IdentityWeaponText = RogueHUD ? RogueHUD->GetWeaponTypeTextForWidget(WeaponType) : TEXT("무기");
	View.Label = FText::FromString(FString::Printf(TEXT("%s 숙련도 %d"), *IdentityWeaponText, MasteryLevel));
	View.WeaponType = WeaponType;
	View.IdentityType = IdentityType;
	View.MasteryLevel = MasteryLevel;
	View.MasteryNormalized = MasteryNormalized;
	View.OutlineColor = FLinearColor::LerpUsingHSV(FLinearColor(0.36f, 0.36f, 0.36f, 1.0f), FLinearColor(0.92f, 0.78f, 0.34f, 1.0f), MasteryNormalized);
	View.OutlineThickness = FMath::Lerp(1.0f, 6.0f, MasteryNormalized);
	View.ResourceLabel = FText::FromString(TEXT("특수 자원"));
	if (WeaponType == ERogue10mWeaponType::Unarmed || WeaponType == ERogue10mWeaponType::Knuckle)
	{
		View.IconTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/UI/Icons/T_Identity_StoneFist.T_Identity_StoneFist")));
	}
	View.ResourceLabel = GetIdentityResourceLabel(IdentityType);
	View.IconTexture = GetIdentityIconTexture(IdentityType);
	View.Current = RoguePlayerState->GetIdentityCurrent();
	View.Max = RoguePlayerState->GetIdentityMax();
	View.Normalized = RoguePlayerState->GetIdentityNormalized();
	View.bHasIdentityResource = RoguePlayerState->HasIdentityResource();
	return View;
}

FRogue10mHudMonsterInfoView URogue10mRunHUD::GetLookedAtMonsterInfoView() const
{
	FRogue10mHudMonsterInfoView View;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	const ARogue10mBasicMonster* Monster = RogueHUD ? RogueHUD->FindLookedAtMonsterForWidget() : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = Monster ? Monster->GetVitalsComponent() : nullptr;
	if (!Monster || !VitalsComponent)
	{
		return View;
	}

	View.bHasMonster = true;
	View.Name = Monster->GetMonsterDisplayName();
	View.Level = Monster->GetMonsterLevel();
	View.AttributeText = Monster->GetMonsterAttributeText();
	View.StateText = Monster->IsDead() ? FText::FromString(TEXT("사망")) : FText::FromString(TEXT("적대"));
	View.Health = MakeVitalView(VitalsComponent->GetHealth(), true, FLinearColor(0.92f, 0.12f, 0.12f, 1.0f));
	return View;
}

TArray<FRogue10mHudQuickSlotView> URogue10mRunHUD::GetSkillQuickSlotViews() const
{
	TArray<FRogue10mHudQuickSlotView> Views;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	if (!RogueHUD)
	{
		return Views;
	}

	const TArray<FRogue10mQuickSlotView> SkillSlots = RogueHUD->GetWeaponSkillQuickSlotsForWidget();
	Views.Reserve(SkillSlots.Num());
	for (int32 Index = 0; Index < SkillSlots.Num(); ++Index)
	{
		const FRogue10mQuickSlotView& QuickSlot = SkillSlots[Index];

		FRogue10mHudQuickSlotView View;
		View.SlotNumber = QuickSlot.SlotNumber;
		View.bUnlocked = true;
		View.DisplayName = QuickSlot.DisplayName;
		View.IconColor = QuickSlot.IconColor;
		View.SkillIcon = QuickSlot.SkillIcon;
		View.CooldownDuration = QuickSlot.CooldownDuration;
		View.CooldownRemaining = RogueHUD->GetQuickSlotCooldownRemainingForWidget(QuickSlot);
		View.CooldownNormalized = View.CooldownDuration > 0.0f ? FMath::Clamp(View.CooldownRemaining / View.CooldownDuration, 0.0f, 1.0f) : 0.0f;
		Views.Add(View);
	}

	return Views;
}

TArray<FRogue10mHudQuickSlotView> URogue10mRunHUD::GetItemQuickSlotViews() const
{
	TArray<FRogue10mHudQuickSlotView> Views;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	if (!RogueHUD)
	{
		return Views;
	}

	const TArray<FRogue10mQuickSlotView>& QuickSlots = RogueHUD->GetQuickSlotsForWidget();
	constexpr int32 SkillSlotCount = 4;
	constexpr int32 ItemSlotCount = 4;
	Views.Reserve(ItemSlotCount);
	for (int32 Index = 0; Index < ItemSlotCount; ++Index)
	{
		const int32 QuickSlotIndex = SkillSlotCount + Index;
		FRogue10mHudQuickSlotView View;
		View.SlotNumber = QuickSlotIndex + 1;
		View.bUnlocked = QuickSlots.IsValidIndex(QuickSlotIndex);
		if (QuickSlots.IsValidIndex(QuickSlotIndex))
		{
			const FRogue10mQuickSlotView& QuickSlot = QuickSlots[QuickSlotIndex];
			View.SlotNumber = QuickSlot.SlotNumber;
			View.DisplayName = QuickSlot.DisplayName;
			View.IconColor = QuickSlot.IconColor;
			View.SkillIcon = QuickSlot.SkillIcon;
			View.CooldownDuration = QuickSlot.CooldownDuration;
			View.CooldownRemaining = RogueHUD->GetQuickSlotCooldownRemainingForWidget(QuickSlot);
			View.CooldownNormalized = View.CooldownDuration > 0.0f ? FMath::Clamp(View.CooldownRemaining / View.CooldownDuration, 0.0f, 1.0f) : 0.0f;
		}
		else
		{
			View.DisplayName = FText::GetEmpty();
			View.IconColor = FLinearColor(0.2f, 0.22f, 0.26f, 1.0f);
		}
		Views.Add(View);
	}

	return Views;
}

TArray<FRogue10mHudLogEntryView> URogue10mRunHUD::GetSystemLogEntries() const
{
	TArray<FRogue10mHudLogEntryView> Views;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	const UWorld* World = GetWorld();
	if (!RogueHUD || !World)
	{
		return Views;
	}

	const float CurrentTime = World->GetTimeSeconds();
	for (const FRogue10mCombatLogEntry& Entry : RogueHUD->GetCombatLogEntriesForWidget())
	{
		FRogue10mHudLogEntryView View;
		View.Message = FText::FromString(Entry.Message);
		View.Color = Entry.Color;
		View.RemainingSeconds = FMath::Max(0.0f, Entry.ExpireTime - CurrentTime);
		Views.Add(View);
	}

	return Views;
}

TArray<FRogue10mHudLogEntryView> URogue10mRunHUD::GetItemAcquisitionEntries() const
{
	TArray<FRogue10mHudLogEntryView> Views;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	const UWorld* World = GetWorld();
	if (!RogueHUD || !World)
	{
		return Views;
	}

	const float CurrentTime = World->GetTimeSeconds();
	for (const FRogue10mItemAcquisitionEntry& Entry : RogueHUD->GetItemAcquisitionEntriesForWidget())
	{
		FRogue10mHudLogEntryView View;
		View.Message = FText::FromString(Entry.Message);
		View.Color = Entry.Color;
		View.RemainingSeconds = FMath::Max(0.0f, Entry.ExpireTime - CurrentTime);
		Views.Add(View);
	}

	return Views;
}

FRogue10mHudPanelStateView URogue10mRunHUD::GetPanelStateView() const
{
	FRogue10mHudPanelStateView View;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	if (!RogueHUD)
	{
		return View;
	}

	View.bEquipmentVisible = RogueHUD->IsInventoryVisible();
	View.bItemWindowVisible = RogueHUD->IsItemWindowVisible();
	View.bSkillTreeVisible = RogueHUD->IsSkillTreeVisible();
	View.bSettingsVisible = RogueHUD->IsSettingsVisible();
	return View;
}

TArray<FRogue10mHudMinimapMarkerView> URogue10mRunHUD::GetPrototypeMinimapMarkers() const
{
	TArray<FRogue10mHudMinimapMarkerView> Views;
	Views.Reserve(4);

	FRogue10mHudMinimapMarkerView PlayerMarker;
	PlayerMarker.NormalizedPosition = FVector2D(0.5f, 0.5f);
	PlayerMarker.Color = FLinearColor(0.28f, 0.86f, 1.0f, 1.0f);
	PlayerMarker.Label = FText::FromString(TEXT("플레이어"));
	Views.Add(PlayerMarker);

	FRogue10mHudMinimapMarkerView MonsterMarker;
	MonsterMarker.NormalizedPosition = FVector2D(0.23f, 0.32f);
	MonsterMarker.Color = FLinearColor(0.92f, 0.18f, 0.12f, 1.0f);
	MonsterMarker.Label = FText::FromString(TEXT("적"));
	Views.Add(MonsterMarker);

	FRogue10mHudMinimapMarkerView NpcMarker;
	NpcMarker.NormalizedPosition = FVector2D(0.81f, 0.44f);
	NpcMarker.Color = FLinearColor(0.35f, 0.9f, 0.42f, 1.0f);
	NpcMarker.Label = FText::FromString(TEXT("NPC"));
	Views.Add(NpcMarker);

	FRogue10mHudMinimapMarkerView ObjectiveMarker;
	ObjectiveMarker.NormalizedPosition = FVector2D(0.38f, 0.78f);
	ObjectiveMarker.Color = FLinearColor(0.96f, 0.78f, 0.28f, 1.0f);
	ObjectiveMarker.Label = FText::FromString(TEXT("목표"));
	Views.Add(ObjectiveMarker);

	return Views;
}

FRogue10mHudRunTimerView URogue10mRunHUD::GetRunTimerView() const
{
	FRogue10mHudRunTimerView View;
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!RogueGameState || !RogueGameState->IsRunActive())
	{
		return View;
	}

	View.bVisible = true;
	View.RemainingSeconds = RogueGameState->GetRemainingRunSeconds();
	View.TotalSeconds = RogueGameState->GetRunDurationSeconds();
	View.ProgressAlpha = RogueGameState->GetRunProgressAlpha();
	View.RemainingText = RogueGameState->GetRemainingTimeText();
	return View;
}

FRogue10mHudRunResultView URogue10mRunHUD::GetRunResultView() const
{
	FRogue10mHudRunResultView View;
	const ARogue10mGameState* RogueGameState = GetWorld() ? GetWorld()->GetGameState<ARogue10mGameState>() : nullptr;
	if (!RogueGameState || RogueGameState->IsRunActive())
	{
		return View;
	}

	View.RunPhase = RogueGameState->GetRunPhase();
	if (View.RunPhase == ERogue10mRunPhase::Victory)
	{
		View.bVisible = true;
		View.ResultText = FText::FromString(TEXT("승리"));
		View.DescriptionText = FText::FromString(TEXT("보스를 처치하고 생존에 성공했습니다."));
		View.ResultColor = FLinearColor(0.2f, 1.0f, 0.35f, 1.0f);
	}
	else if (View.RunPhase == ERogue10mRunPhase::Defeat)
	{
		View.bVisible = true;
		View.ResultText = FText::FromString(TEXT("게임 오버"));
		View.DescriptionText = FText::FromString(TEXT("제한 시간이 끝나 캐릭터가 사망했습니다."));
		View.ResultColor = FLinearColor(1.0f, 0.2f, 0.15f, 1.0f);
	}

	return View;
}

FRogue10mHudAimCrossLineView URogue10mRunHUD::GetAimCrossLineView() const
{
	FRogue10mHudAimCrossLineView View;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	View.bVisible = RogueCharacter && !RogueCharacter->IsDead() && (!RogueHUD || !RogueHUD->IsAnyBlockingWindowVisible());
	if (RogueHUD)
	{
		View.Color = RogueHUD->GetAimCrossLineColorForWidget();
		View.LineLength = RogueHUD->GetAimCrossLineLengthForWidget();
		View.Gap = RogueHUD->GetAimCrossLineGapForWidget();
		View.Thickness = RogueHUD->GetAimCrossLineThicknessForWidget();
	}
	return View;
}

FRogue10mHudDamageFeedbackView URogue10mRunHUD::GetDamageFeedbackView() const
{
	FRogue10mHudDamageFeedbackView View;
	const ARogue10mHUD* RogueHUD = GetRogueHUD();
	if (!RogueHUD)
	{
		return View;
	}

	View.Alpha = RogueHUD->GetPlayerDamageFeedbackAlphaForWidget();
	View.Strength = RogueHUD->GetPlayerDamageFeedbackStrengthForWidget();
	View.bVisible = View.Alpha > 0.0f;
	return View;
}

FRogue10mHudAttackCooldownView URogue10mRunHUD::GetAttackCooldownView() const
{
	FRogue10mHudAttackCooldownView View;
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	if (!RogueCharacter)
	{
		return View;
	}

	const URogue10mAttackSkillData* DisplaySkill = RogueCharacter->GetDisplayedAttackSkillForHUD();
	if (!DisplaySkill)
	{
		return View;
	}

	View.bVisible = true;
	View.SkillName = DisplaySkill->SkillName;
	View.IconLabel = DisplaySkill->IconLabel;
	View.IconTint = DisplaySkill->IconTint;
	View.SkillIcon = DisplaySkill->SkillIcon;
	View.CooldownRemaining = RogueCharacter->GetAttackCooldownRemaining();
	View.CooldownDuration = RogueCharacter->GetAttackCooldownDuration();
	View.CooldownNormalized = View.CooldownDuration > 0.0f ? FMath::Clamp(View.CooldownRemaining / View.CooldownDuration, 0.0f, 1.0f) : 0.0f;
	View.bOnCooldown = View.CooldownRemaining > 0.0f && View.CooldownDuration > 0.0f;
	return View;
}

FRogue10mHudVitalView URogue10mRunHUD::MakeVitalView(const FRogue10mVitalValue& Vital, bool bVisible, const FLinearColor& FillColor) const
{
	FRogue10mHudVitalView View;
	View.Current = Vital.Current;
	View.Max = Vital.Max;
	View.Normalized = Vital.GetNormalized();
	View.Percent = View.Normalized * 100.0f;
	View.ValueText = FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), View.Current, View.Max));
	View.PercentText = FText::FromString(FString::Printf(TEXT("%.0f%%"), View.Percent));
	View.FillColor = FillColor;
	View.bVisible = bVisible;
	return View;
}
