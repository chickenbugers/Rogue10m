// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mRunHUD.h"

#include "Rogue10mBasicMonster.h"
#include "Rogue10mCharacter.h"
#include "Rogue10mHUD.h"
#include "Rogue10mPlayerState.h"
#include "Rogue10mVitalsComponent.h"
#include "GameFramework/PlayerController.h"

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
	return VitalsComponent ? MakeVitalView(VitalsComponent->GetHealth(), true) : FRogue10mHudVitalView();
}

FRogue10mHudVitalView URogue10mRunHUD::GetStaminaView() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	return VitalsComponent ? MakeVitalView(VitalsComponent->GetStamina(), true) : FRogue10mHudVitalView();
}

FRogue10mHudVitalView URogue10mRunHUD::GetManaView() const
{
	const ARogue10mCharacter* RogueCharacter = GetOwningPlayerPawn() ? Cast<ARogue10mCharacter>(GetOwningPlayerPawn()) : nullptr;
	const URogue10mVitalsComponent* VitalsComponent = RogueCharacter ? RogueCharacter->GetVitalsComponent() : nullptr;
	if (!VitalsComponent || !VitalsComponent->ShouldShowMana())
	{
		return FRogue10mHudVitalView();
	}

	return MakeVitalView(VitalsComponent->GetMana(), true);
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
	const FString WeaponText = RogueHUD ? RogueHUD->GetWeaponTypeTextForWidget(WeaponType) : TEXT("무기");
	View.Label = FText::FromString(FString::Printf(TEXT("%s 숙련 %d"), *WeaponText, RoguePlayerState->GetWeaponMasteryLevel(WeaponType)));
	View.Current = RoguePlayerState->GetIdentityCurrent();
	View.Max = RoguePlayerState->GetIdentityMax();
	View.Normalized = RoguePlayerState->GetIdentityNormalized();
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
	View.Health = MakeVitalView(VitalsComponent->GetHealth(), true);
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

	const TArray<FRogue10mQuickSlotView>& QuickSlots = RogueHUD->GetQuickSlotsForWidget();
	const int32 SlotCount = RogueHUD->GetVisibleSkillSlotCountForWidget();
	Views.Reserve(SlotCount);
	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		FRogue10mHudQuickSlotView View;
		View.SlotNumber = Index + 1;
		View.bUnlocked = QuickSlots.IsValidIndex(Index);
		if (QuickSlots.IsValidIndex(Index))
		{
			const FRogue10mQuickSlotView& QuickSlot = QuickSlots[Index];
			View.SlotNumber = QuickSlot.SlotNumber;
			View.DisplayName = QuickSlot.DisplayName;
			View.IconColor = QuickSlot.IconColor;
			View.CooldownDuration = QuickSlot.CooldownDuration;
			View.CooldownRemaining = RogueHUD->GetQuickSlotCooldownRemainingForWidget(QuickSlot);
			View.CooldownNormalized = View.CooldownDuration > 0.0f ? FMath::Clamp(View.CooldownRemaining / View.CooldownDuration, 0.0f, 1.0f) : 0.0f;
		}
		else
		{
			View.DisplayName = FText::FromString(TEXT("잠김"));
			View.IconColor = FLinearColor(0.2f, 0.22f, 0.26f, 1.0f);
		}
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

FRogue10mHudVitalView URogue10mRunHUD::MakeVitalView(const FRogue10mVitalValue& Vital, bool bVisible) const
{
	FRogue10mHudVitalView View;
	View.Current = Vital.Current;
	View.Max = Vital.Max;
	View.Normalized = Vital.GetNormalized();
	View.bVisible = bVisible;
	return View;
}
