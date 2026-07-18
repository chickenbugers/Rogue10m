"""Apply only equipment stat modifiers to existing starter Item Data Assets."""

from __future__ import annotations

import os
import sys

import unreal


SCRIPT_DIRECTORY = os.path.dirname(os.path.abspath(__file__))
if SCRIPT_DIRECTORY not in sys.path:
    sys.path.insert(0, SCRIPT_DIRECTORY)

from CreateStarterItemAssets import ITEMS, ITEM_ROOT  # noqa: E402


STAT_PROPERTIES = (
    "attack_power_bonus",
    "defense_bonus",
    "max_health_bonus",
    "critical_chance_bonus",
    "attack_speed_bonus",
    "move_speed_bonus",
)


def main() -> None:
    for definition in ITEMS:
        asset_path = f"{ITEM_ROOT}/{definition['asset']}"
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            raise RuntimeError(f"Missing starter Item Data Asset: {asset_path}")

        equipment_stats = asset.get_editor_property("equipment_stats")
        stat_values = definition.get("stats", {})
        for property_name in STAT_PROPERTIES:
            equipment_stats.set_editor_property(
                property_name, stat_values.get(property_name, 0.0)
            )
        asset.set_editor_property("equipment_stats", equipment_stats)
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
        unreal.log(
            f"[StarterEquipmentStats] {asset_path}: "
            f"{', '.join(f'{name}={stat_values.get(name, 0.0):g}' for name in STAT_PROPERTIES)}"
        )

    unreal.log("[StarterEquipmentStats] Starter equipment stat modifiers applied.")


if __name__ == "__main__":
    main()
