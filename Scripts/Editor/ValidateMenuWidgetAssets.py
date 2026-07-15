"""Compile and validate organized Rogue10m menu Widget Blueprints."""
import unreal

ASSETS = {
    "/Game/Widget/Menu/Inventory/WBP_InventoryCell": {"UI_InventoryCellFrame", "UI_InventoryCellFill"},
    "/Game/Widget/Menu/Inventory/WBP_InventoryItem": {"UI_InventoryItemSize", "UI_InventoryItemIcon", "UI_InventoryItemQuantityText", "UI_InventoryItemPreviewBorder"},
    "/Game/Widget/Menu/Inventory/WBP_BagTab": {"UI_BagTabButton", "UI_BagNameText"},
    "/Game/Widget/Menu/Inventory/WBP_InventoryWindow": {"UI_InventoryGridFrame", "UI_InventoryGrid", "UI_InventoryItemCanvas"},
    "/Game/Widget/Menu/Equipment/WBP_EquipmentWindow": {
        "UI_EquipmentSlotContainer",
        "UI_CharacterPreviewImage",
        "UI_WeaponSlotIcon",
        "UI_HeadSlotIcon",
        "UI_ChestSlotIcon",
        "UI_HandsSlotIcon",
        "UI_LegsSlotIcon",
        "UI_FeetSlotIcon",
        "UI_AccessorySlotIcon",
    },
    "/Game/Widget/Menu/SkillTree/WBP_SkillTreeEntry": {"UI_SkillIconImage", "UI_SkillNameText"},
    "/Game/Widget/Menu/SkillTree/WBP_SkillTreeWindow": {"UI_SkillListContainer"},
}

CLASS_REFERENCES = {
    "/Game/Widget/Menu/Inventory/WBP_InventoryWindow": {
        "inventory_cell_widget_class": "/Game/Widget/Menu/Inventory/WBP_InventoryCell",
        "inventory_item_widget_class": "/Game/Widget/Menu/Inventory/WBP_InventoryItem",
    },
    "/Game/Widget/Menu/SkillTree/WBP_SkillTreeWindow": {
        "skill_tree_entry_widget_class": "/Game/Widget/Menu/SkillTree/WBP_SkillTreeEntry",
    },
}


def load_blueprint_class(path):
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(path)
    if not generated_class:
        raise RuntimeError(f"Missing generated class: {path}")
    return generated_class


def main():
    tool = unreal.get_default_object(unreal.UMGToolSet)
    for path, required in ASSETS.items():
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            raise RuntimeError(f"Missing asset: {path}")
        tree = tool.call_method("GetWidgets", (asset,))
        names = {info.widget.get_name() for info in tree.widgets if info.widget}
        missing = required - names
        if missing:
            raise RuntimeError(f"Missing widgets in {path}: {sorted(missing)}")
        if path.endswith("/WBP_InventoryItem"):
            slots = {
                info.widget.get_name(): info.slot
                for info in tree.widgets
                if info.widget and info.slot
            }
            expected_layers = {
                "UI_InventoryItemPreviewBorder": 0,
                "UI_InventoryItemQuantityText": 1,
            }
            for widget_name, expected_layer in expected_layers.items():
                actual_layer = slots[widget_name].get_editor_property("layer")
                if actual_layer != expected_layer:
                    raise RuntimeError(
                        f"Invalid layer {widget_name}: {actual_layer}, expected {expected_layer}"
                    )
        if "UI_BagSizeText" in names:
            raise RuntimeError("UI_BagSizeText still exists")
        if path.endswith("/WBP_InventoryWindow") and "UI_BagTabContainer" in names:
            raise RuntimeError("UI_BagTabContainer still exists in the single-bag window")
        if not tool.call_method("CompileWidgetBlueprint", (asset,)):
            raise RuntimeError(f"Compile failed: {path}")
        unreal.log(f"[Rogue10mMenuValidation] OK {path}: {sorted(names)}")

    for owner_path, properties in CLASS_REFERENCES.items():
        owner_cdo = unreal.get_default_object(load_blueprint_class(owner_path))
        for property_name, expected_path in properties.items():
            actual_class = owner_cdo.get_editor_property(property_name)
            expected_class = load_blueprint_class(expected_path)
            if actual_class != expected_class:
                raise RuntimeError(
                    f"Invalid class reference {owner_path}.{property_name}: "
                    f"expected {expected_path}, got {actual_class}"
                )
            unreal.log(
                f"[Rogue10mMenuValidation] REF {owner_path}.{property_name} -> {expected_path}"
            )
    unreal.log("[Rogue10mMenuValidation] All organized menu widgets passed.")


if __name__ == "__main__":
    main()