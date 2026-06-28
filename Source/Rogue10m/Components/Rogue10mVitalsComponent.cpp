// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mVitalsComponent.h"

URogue10mVitalsComponent::URogue10mVitalsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Health.Current = 100.0f;
	Health.Max = 100.0f;
	Mana.Current = 50.0f;
	Mana.Max = 50.0f;
	Stamina.Current = 100.0f;
	Stamina.Max = 100.0f;
	bShowMana = false;
}

void URogue10mVitalsComponent::SetShowMana(bool bNewShowMana)
{
	bShowMana = bNewShowMana;
}

void URogue10mVitalsComponent::SetHealth(float NewCurrent)
{
	SetVitalCurrent(Health, NewCurrent);
}

void URogue10mVitalsComponent::SetMana(float NewCurrent)
{
	SetVitalCurrent(Mana, NewCurrent);
}

void URogue10mVitalsComponent::SetStamina(float NewCurrent)
{
	SetVitalCurrent(Stamina, NewCurrent);
}

void URogue10mVitalsComponent::RestoreAll()
{
	Health.Current = Health.Max;
	Mana.Current = Mana.Max;
	Stamina.Current = Stamina.Max;
}

void URogue10mVitalsComponent::SetVitalCurrent(FRogue10mVitalValue& Vital, float NewCurrent)
{
	Vital.Current = FMath::Clamp(NewCurrent, 0.0f, Vital.Max);
}
