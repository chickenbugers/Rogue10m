"""Move menu Widget Blueprints into feature folders through Unreal Editor APIs."""

import unreal

MOVES = {
    "/Game/Widget/Component/WBP_InventoryWindow": "/Game/Widget/Component/Inventory/WBP_InventoryWindow",
    "/Game/Widget/Component/WBP_InventoryCell": "/Game/Widget/Component/Inventory/WBP_InventoryCell",
    "/Game/Widget/Component/WBP_InventoryItem": "/Game/Widget/Component/Inventory/WBP_InventoryItem",
    "/Game/Widget/Component/WBP_BagTab": "/Game/Widget/Component/Inventory/WBP_BagTab",
    "/Game/Widget/Component/WBP_EquipmentWindow": "/Game/Widget/Component/Equipment/WBP_EquipmentWindow",
    "/Game/Widget/Component/WBP_SkillTreeWindow": "/Game/Widget/Component/SkillTree/WBP_SkillTreeWindow",
    "/Game/Widget/Component/WBP_SkillTreeEntry": "/Game/Widget/Component/SkillTree/WBP_SkillTreeEntry",
}


def main():
    for target in MOVES.values():
        unreal.EditorAssetLibrary.make_directory(target.rsplit("/", 1)[0])

    for source, target in MOVES.items():
        if unreal.EditorAssetLibrary.does_asset_exist(target):
            unreal.log(f"[Rogue10mMenuOrganizer] Already moved: {target}")
            continue
        if not unreal.EditorAssetLibrary.does_asset_exist(source):
            raise RuntimeError(f"Source asset not found: {source}")
        if not unreal.EditorAssetLibrary.rename_asset(source, target):
            raise RuntimeError(f"Failed to move {source} -> {target}")
        unreal.log(f"[Rogue10mMenuOrganizer] Moved {source} -> {target}")

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("[Rogue10mMenuOrganizer] Menu Widget asset organization complete.")


if __name__ == "__main__":
    main()