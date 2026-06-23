# Rogue10m Prototype Plan

## Current Base Setup

- Engine association: UE 5.7 project installed under `D:\Program Files\UE_5.7`.
- Default map: `/Game/FirstPerson/Lvl_FirstPerson`.
- Default game mode: `/Game/FirstPerson/Blueprints/BP_FirstPersonGameMode`.
- C++ run state: `ARogue10mGameState`.
- C++ HUD base: `URogue10mRunHUD`.

## First Playable Assumptions

- Single-player first-person run.
- Each run starts automatically when play begins.
- Default run duration is 20 minutes.
- Timer expiry currently ends the run as defeat.
- Victory condition is left open for the first prototype.
- Run seed, current floor, and defeated enemy count are tracked in C++ for later systems.

## Blueprint Hookup

1. Create a Widget Blueprint based on `URogue10mRunHUD`.
2. Implement `Update Run Timer` to display remaining time and progress.
3. Implement `Run Phase Changed` for win/lose screens.
4. Assign that widget class to `Run HUD Class` on the active player controller Blueprint.

## Design Questions

1. Combat direction: gun-focused shooter, melee-first, magic/ability shooter, or mixed?
2. Run structure: one continuous arena for 20 minutes, floor-by-floor rooms, or extraction after objectives?
3. Player growth: level-up choices during the run, loot drops, permanent upgrades, or all three?
4. Failure condition: timer reaches zero, player death, or both?
5. Visual tone: sci-fi, dark fantasy, modern horror, abstract arena, or another direction?
