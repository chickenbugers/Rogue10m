"""Apply only rarity values to existing starter Item Data Assets."""

from __future__ import annotations

import os
import sys

import unreal


SCRIPT_FILE = globals().get(
    "__file__",
    os.path.join(os.getcwd(), "Scripts", "Editor", "ConfigureStarterItemRarities.py"),
)
SCRIPT_DIRECTORY = os.path.dirname(os.path.abspath(SCRIPT_FILE))
if SCRIPT_DIRECTORY not in sys.path:
    sys.path.insert(0, SCRIPT_DIRECTORY)

from CreateStarterItemAssets import ITEMS, ITEM_ROOT  # noqa: E402


def main() -> None:
    for definition in ITEMS:
        asset_path = f"{ITEM_ROOT}/{definition['asset']}"
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            raise RuntimeError(f"Missing starter Item Data Asset: {asset_path}")
        rarity = definition["rarity"]
        asset.set_editor_property("rarity", rarity)
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
        unreal.log(f"[StarterItemRarity] {asset_path}: {rarity}")

    unreal.log("[StarterItemRarity] Starter item rarities applied.")


if __name__ == "__main__":
    main()
