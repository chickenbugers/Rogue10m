"""Normalize starter item icon scale values without rebuilding or reimporting assets."""

from __future__ import annotations

import unreal


ITEM_ASSET_PATHS = (
    "/Game/DataAsset/Item/Starter/DA_Item_HealthPotion",
    "/Game/DataAsset/Item/Starter/DA_Item_IronLongsword",
    "/Game/DataAsset/Item/Starter/DA_Item_IronHelmet",
    "/Game/DataAsset/Item/Starter/DA_Item_GuardianHelmet",
    "/Game/DataAsset/Item/Starter/DA_Item_LeatherArmor",
    "/Game/DataAsset/Item/Starter/DA_Item_LeatherBoots",
    "/Game/DataAsset/Item/Starter/DA_Item_NoviceRing",
)


def main() -> None:
    updated = 0
    for asset_path in ITEM_ASSET_PATHS:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            raise RuntimeError(f"Starter item asset not found: {asset_path}")
        current_scale = float(asset.get_editor_property("inventory_icon_scale"))
        if abs(current_scale - 1.0) > 0.0001:
            asset.set_editor_property("inventory_icon_scale", 1.0)
            updated += 1
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
        unreal.log(f"[Rogue10mIconScale] {asset_path} -> 1.0")

    unreal.log(f"[Rogue10mIconScale] Complete. Updated assets: {updated}")


if __name__ == "__main__":
    main()
