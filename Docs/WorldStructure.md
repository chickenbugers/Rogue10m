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
  - Sword
  - Staff
  - Bow

## Customization

- Character customization is cosmetic only.
- Customization should not affect stats, mastery, damage, or difficulty.

## Open Decisions

- Exact start map layout and NPC placement.
- Whether NPCs use dialogue widgets, proximity prompts, or direct interaction menus.
- Whether the play map starts at center, edge, or a safe spawn camp.
- Exact player walk/sprint speed.
- Boss spawn location and encounter boundary.
