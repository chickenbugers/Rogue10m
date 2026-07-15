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
	StartMap.DisplayName = FText::FromString(TEXT("시작맵"));
	StartMap.LevelName = FName(TEXT("StartHub"));
	StartMap.LevelAssetPath = FSoftObjectPath(TEXT("/Game/Rogue10m/Maps/StartHub.StartHub"));
	StartMap.Role = ERogue10mMapRole::StartHub;
	StartMap.TargetWorldSizeMeters = FVector2D(120.0f, 120.0f);
	StartMap.Npcs = {
		Rogue10mMapData::MakeNpc(TEXT("ItemNpc"), TEXT("아이템 관리 NPC"), TEXT("시작 장비와 아이템을 제공한다."), FVector(300.0f, -200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("TraitNpc"), TEXT("특성 강화 NPC"), TEXT("획득 포인트로 간단한 특성을 강화한다."), FVector(300.0f, 200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("DifficultyNpc"), TEXT("난이도 조절 NPC"), TEXT("플레이맵 진입 전 난이도를 조절한다."), FVector(-300.0f, -200.0f, 0.0f)),
		Rogue10mMapData::MakeNpc(TEXT("StoryNpc"), TEXT("스토리 안내 NPC"), TEXT("시작 스토리와 런 목표를 설명한다."), FVector(-300.0f, 200.0f, 0.0f))
	};

	FRogue10mMapDefinition PlayMap;
	PlayMap.Id = FName(TEXT("OpenRunMap"));
	PlayMap.DisplayName = FText::FromString(TEXT("플레이맵"));
	PlayMap.LevelName = FName(TEXT("OpenRunMap"));
	PlayMap.LevelAssetPath = FSoftObjectPath(TEXT("/Game/Rogue10m/Maps/OpenRunMap.OpenRunMap"));
	PlayMap.Role = ERogue10mMapRole::CombatRun;
	PlayMap.TargetWorldSizeMeters = FVector2D(1800.0f, 1800.0f);

	return { StartMap, PlayMap };
}
