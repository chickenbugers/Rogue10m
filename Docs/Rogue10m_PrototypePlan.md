# Rogue10m Prototype Plan

## Current Base Setup

- Engine association: UE 5.7 project installed under `D:\Program Files\UE_5.7`.
- Default map: `/Game/FirstPerson/Lvl_FirstPerson`.
- Default game mode: `/Script/Rogue10m.Rogue10mGameMode`.
- C++ run state: `ARogue10mGameState`.
- C++ HUD base: `URogue10mRunHUD`.

## First Playable Assumptions

- Single-player first-person run.
- Each run starts automatically when play begins.
- Default run duration is 20 minutes.
- Timer expiry currently ends the run as defeat.
- Victory condition: defeat the boss within 20 minutes.
- Run seed, current floor, and defeated enemy count are tracked in C++ for later systems.
- The character starts unarmed and can attack with fists.
- Weapon mastery progresses by weapon type: sword, staff, bow, and future types.
- Character customization is cosmetic only and does not affect stats.

## Map Structure

### Start Map

The start map is a hub with NPC-driven systems.

- Item NPC: lets the player receive or manage items.
- Trait NPC: lets the player spend previously earned points on simple trait upgrades.
- Difficulty NPC: lets the player adjust run difficulty.
- Story NPC: introduces the game's starting story and run premise.

### Play Map

- First-person open-world combat map.
- Target size: from one edge to the opposite edge should take about 5 minutes on foot.
- Exact world dimensions are still undecided and should be tuned after movement speed is finalized.
- The boss must be defeated within 20 minutes or the run fails.

## Blueprint Hookup

1. Create a Widget Blueprint based on `URogue10mRunHUD`.
2. Implement `Update Run Timer` to display remaining time and progress.
3. Implement `Run Phase Changed` for win/lose screens.
4. Assign that widget class to `Run HUD Class` on the active player controller Blueprint.

## Design Questions

1. Exact start map NPC layout and interaction range.
2. Play map traversal size after finalizing movement speed.
3. Boss spawn timing and boss arena rules.
4. Item reward table after boss clear.
5. Visual tone: sci-fi, dark fantasy, modern horror, abstract arena, or another direction?
