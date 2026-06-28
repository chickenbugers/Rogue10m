// Copyright Epic Games, Inc. All Rights Reserved.

#include "Rogue10mMapDataLibrary.h"

namespace Rogue10mMapData
{
	FRogue10mNpcMapEntry MakeNpc(const TCHAR* Id, const TCHAR* DisplayName, const TCHAR* Purpose, const FVector& SuggestedLocation)
	{
		FRogue10mNpcMapEntry Npc;
		Npc.Id = FName(Id);
		Npc.DisplayName = FText::FromString(FString(DisplayName));
		Npc.Purpose = FText::FromString(FString(Purpose));
		Npc.SuggestedLocation = SuggestedLocation;
		return Npc;
	}
}

TArray<FRogue10mMapDefinition> URogue10mMapDataLibrary::GetPrototypeMapDefinitions()
{
	FRogue10mMapDefinition StartMap;
	StartMap.Id = FName(TEXT("StartHub"));
	StartMap.DisplayName = FText::FromString(TEXT("Start Map"));
	StartMap.Role = ERogue10mMapRole::StartHub;
	StartMap.TargetWorldSizeMeters = FVector2D(120.0f, 120.0f);
	StartMap.Npcs = {
		Rogue10mMapData::MakeNpc(TEXT("ItemNpc"), TEXT("Item Keeper"), TEXT("Items and starting equipment."), FVector(300.0f, -200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("TraitNpc"), TEXT("Trait Mentor"), TEXT("Spend earned points on simple trait upgrades."), FVector(300.0f, 200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("DifficultyNpc"), TEXT("Trial Guide"), TEXT("Adjust run difficulty before entering the play map."), FVector(-300.0f, -200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("StoryNpc"), TEXT("Storyteller"), TEXT("Introduce the opening story and run premise."), FVector(-300.0f, 200.0f, 0.0f))
	};

	FRogue10mMapDefinition PlayMap;
	PlayMap.Id = FName(TEXT("OpenRunMap"));
	PlayMap.DisplayName = FText::FromString(TEXT("Play Map"));
	PlayMap.Role = ERogue10mMapRole::CombatRun;
	PlayMap.TargetWorldSizeMeters = FVector2D(1800.0f, 1800.0f);

	return { StartMap, PlayMap };
}
