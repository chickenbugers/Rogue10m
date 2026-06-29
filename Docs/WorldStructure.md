# Rogue10m World Structure

## Overview

Rogue10m uses two main map types:

- Start Map: NPC hub before a run.
- Play Map: first-person open-world run space.

## Start Map

The start map is a hub with NPC-driven systems.

### NPC Roles

- Item NPC
  - Lets the player receive, manage, or prepare items before a run.
  - Future link: inventory, starting item choice, reward claim.

- Trait NPC
  - Lets the player spend previously earned points on simple trait upgrades.
  - Future link: permanent progression, account-level or character-level traits.

- Difficulty NPC
  - Lets the player adjust run difficulty.
  - Future link: enemy scaling, boss modifiers, reward multiplier.

- Story NPC
  - Explains the starting story and run premise.
  - Future link: tutorial, quest intro, world lore.

## Play Map

The play map is an appropriately sized first-person open-world combat area.

### Size Target

The current design target is that walking from one edge of the map to the opposite edge takes about 5 minutes.

Unreal default character movement often uses about `600 cm/s` walking speed.
At that speed:

- 5 minutes = 300 seconds.
- 600 cm/s * 300 seconds = 180,000 cm.
- 180,000 cm = 1,800 meters.

Initial play map target:

- Edge-to-edge traversal: about 1.8 km at 600 cm/s.
- Center-to-edge traversal: about 900 m if the player starts near the center.

This should be tuned after the final player movement speed is decided.

## Run Rule

- The player must defeat the boss within 20 minutes.
- If the boss is not defeated within 20 minutes, the run fails.
- The current prototype still uses a 30-second timer override for quick UI validation.

## Combat Starting State

- The player starts unarmed.
- The player can attack with fists in the default state.
- Weapon mastery progresses by weapon type.
- Planned weapon types:
  - Dagger
  - Great Sword
  - Dual Blades
  - Bow
  - Staff
  - Knuckle

## Customization

- Character customization is cosmetic only.
- Customization should not affect stats, mastery, damage, or difficulty.

## Open Decisions

- Exact start map layout and NPC placement.
- Whether NPCs use dialogue widgets, proximity prompts, or direct interaction menus.
- Whether the play map starts at center, edge, or a safe spawn camp.
- Exact player walk/sprint speed.
- Boss spawn location and encounter boundary.

## 포탈 및 플레이맵 이동 규칙

- 시작맵에서는 20분 런 타이머를 자동으로 시작하지 않는다.
- 시작맵에 `ARogue10mRunPortal` 또는 그 Blueprint 자식을 배치한다.
- 포탈의 `Target Level Name`은 기본적으로 `OpenRunMap`을 사용한다.
- 포탈의 `Start Run Timer On Travel`을 켜면 다음 맵 이동 옵션에 `StartRun=1`이 붙는다.
- 다음 맵의 GameMode는 `StartRun=1` 옵션을 확인한 뒤 20분 런 타이머를 시작한다.

## 다음 맵 데이터 추가 위치

- 실제 플레이맵 에셋 권장 위치: `Content/Rogue10m/Maps/OpenRunMap.umap`
- 시작맵 에셋 권장 위치: `Content/Rogue10m/Maps/StartHub.umap`
- 맵 메타데이터 추가 위치: `Source/Rogue10m/World/Rogue10mMapDataLibrary.cpp`
- 포탈 C++ 클래스 위치: `Source/Rogue10m/World/Rogue10mRunPortal.*`
- 에디터에서 새 플레이맵을 만들면 `Rogue10mMapDataLibrary.cpp`의 `LevelName`, `LevelAssetPath`, `TargetWorldSizeMeters` 값을 함께 갱신한다.
