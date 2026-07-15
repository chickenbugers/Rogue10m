# Prototype Run Flow - Development Summary

## Summary

Implemented the first playable prototype run flow for Rogue10m.
The player starts in first person with no explicit weapon equipped, can use the existing First Person WASD movement setup, has a basic fist attack, and sees a C++ HUD timer.

For prototype validation, the run duration is temporarily overridden to 30 seconds through `ARogue10mGameMode`.
When the timer expires, the run enters `Defeat` and the HUD displays an end message.

## Changed Files

- `Source/Rogue10m/Rogue10mGameMode.h`
- `Source/Rogue10m/Rogue10mGameMode.cpp`
- `Source/Rogue10m/Rogue10mGameState.h`
- `Source/Rogue10m/Rogue10mGameState.cpp`
- `Source/Rogue10m/Rogue10mCharacter.h`
- `Source/Rogue10m/Rogue10mCharacter.cpp`
- `Source/Rogue10m/Rogue10mInventoryComponent.h`
- `Source/Rogue10m/Rogue10mInventoryComponent.cpp`
- `Source/Rogue10m/Rogue10mMapDataLibrary.h`
- `Source/Rogue10m/Rogue10mMapDataLibrary.cpp`
- `Source/Rogue10m/Rogue10mHUD.h`
- `Source/Rogue10m/Rogue10mHUD.cpp`
- `Config/DefaultEngine.ini`

## Behavior

- `ARogue10mGameMode` is now the global default game mode.
- The game mode assigns the existing First Person character and player controller Blueprints.
- `ARogue10mGameState` supports a temporary run duration override.
- The prototype run currently uses 30 seconds for fast validation.
- `ARogue10mHUD` draws:
  - remaining run timer
  - timer progress bar
  - `RUN FAILED` message after the prototype timer expires
- `ARogue10mCharacter` starts with `Unarmed` as the equipped weapon type.
- Left mouse performs a basic unarmed sweep attack.
- `I` toggles the prototype inventory panel.
- `B` toggles the prototype item window.
- Equipment and item windows can be opened independently.
- Equipment and item windows can be repositioned by dragging their title bars.
- `URogue10mInventoryComponent` stores prototype equipment slot data.
- `URogue10mInventoryComponent` also stores prototype item stacks and currencies.
- `URogue10mMapDataLibrary` exposes prototype start-map and play-map definitions.
- The C++ HUD draws an inventory panel inspired by the provided reference image:
  - left equipment slots
  - center character preview
  - right accessory/material slots
- The C++ HUD draws an item window inspired by the provided item-bag reference:
  - tab strip
  - item grid
  - stack counts
  - lock markers
  - gold and crystal totals

## Build Result

Succeeded with:

```powershell
.\Scripts\BuildEditor.ps1 -NoHotReload
```

## Editor Or Blueprint Setup

- No new Blueprint setup is required for the C++ HUD.
- Existing First Person character/controller assets are loaded by the C++ GameMode.
- Later, a production UMG HUD can replace or extend the C++ debug HUD.

## Design Notes

- The final game should use a 20-minute run duration.
- The current 30-second duration is a prototype setting for quick end-state validation.
- Weapon equipment, mastery, skill growth, boss rewards, and character customization are not fully implemented yet.
- Weapon mastery will progress by weapon type.
- Character customization is cosmetic only.
- Sword, staff, and bow will use weapon-specific action sets.
- The final clear condition is defeating the boss within 20 minutes.

## Map Direction

- Start map: NPC hub for items, trait upgrades, difficulty control, and story setup.
- Play map: open-world combat space.
- Play map traversal target: about 5 minutes from edge to edge on foot.
- Prototype map data is available through `URogue10mMapDataLibrary::GetPrototypeMapDefinitions()`.

## Open Questions

- Exact first weapon acquisition path.
- Exact movement speed and world size for the 5-minute traversal target.
- Boss spawn and boss arena rules.
- NPC interaction implementation details.
