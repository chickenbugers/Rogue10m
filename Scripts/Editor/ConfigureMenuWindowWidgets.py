"""Create or repair Rogue10m menu Widget Blueprints and assign them to the player controller.

Run this script in the open Unreal Editor after rebuilding the Rogue10mEditor target.
The operation is idempotent: existing assets are reused, compiled, and reassigned.
"""

import unreal


WIDGET_ROOT = "/Game/Widget/Menu"
PLAYER_CONTROLLER_BP_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"
WIDGETS = {
    "inventory": ("WBP_InventoryWindow", "/Script/Rogue10m.Rogue10mInventoryWindowWidget"),
    "equipment": ("WBP_EquipmentWindow", "/Script/Rogue10m.Rogue10mEquipmentWindowWidget"),
    "skill_tree_entry": ("WBP_SkillTreeEntry", "/Script/Rogue10m.Rogue10mSkillTreeEntryWidget"),
    "skill_tree": ("WBP_SkillTreeWindow", "/Script/Rogue10m.Rogue10mSkillTreeWindowWidget"),
}


def log(message):
    unreal.log(f"[Rogue10mMenuSetup] {message}")


def load_blueprint_class(asset_path):
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(asset_path)
    if not generated_class:
        raise RuntimeError(f"Blueprint generated class could not be loaded: {asset_path}")
    return generated_class


def create_or_load_widget_blueprint(asset_name, parent_class_path):
    asset_path = f"{WIDGET_ROOT}/{asset_name}"
    widget_blueprint = unreal.EditorAssetLibrary.load_asset(asset_path)
    if widget_blueprint:
        return widget_blueprint, asset_path

    parent_class = unreal.load_class(None, parent_class_path)
    if not parent_class:
        raise RuntimeError(f"Native parent class could not be loaded: {parent_class_path}")

    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    widget_blueprint = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, WIDGET_ROOT, unreal.WidgetBlueprint, factory
    )
    if not widget_blueprint:
        raise RuntimeError(f"Widget Blueprint creation failed: {asset_path}")

    log(f"Created {asset_path}")
    return widget_blueprint, asset_path


def main():
    widget_blueprints = {}
    widget_paths = {}
    for key, (asset_name, parent_class_path) in WIDGETS.items():
        widget_blueprint, asset_path = create_or_load_widget_blueprint(
            asset_name, parent_class_path
        )
        unreal.BlueprintEditorLibrary.compile_blueprint(widget_blueprint)
        widget_blueprints[key] = widget_blueprint
        widget_paths[key] = asset_path

    skill_tree_cdo = unreal.get_default_object(load_blueprint_class(widget_paths["skill_tree"]))
    skill_tree_cdo.set_editor_property(
        "skill_tree_entry_widget_class",
        load_blueprint_class(widget_paths["skill_tree_entry"]),
    )
    unreal.BlueprintEditorLibrary.compile_blueprint(widget_blueprints["skill_tree"])

    controller_blueprint = unreal.EditorAssetLibrary.load_asset(PLAYER_CONTROLLER_BP_PATH)
    if not controller_blueprint:
        raise RuntimeError(
            f"PlayerController Blueprint could not be loaded: {PLAYER_CONTROLLER_BP_PATH}"
        )

    unreal.BlueprintEditorLibrary.compile_blueprint(controller_blueprint)
    controller_cdo = unreal.get_default_object(
        load_blueprint_class(PLAYER_CONTROLLER_BP_PATH)
    )
    controller_cdo.set_editor_property(
        "inventory_window_widget_class",
        load_blueprint_class(widget_paths["inventory"]),
    )
    controller_cdo.set_editor_property(
        "equipment_window_widget_class",
        load_blueprint_class(widget_paths["equipment"]),
    )
    controller_cdo.set_editor_property(
        "skill_tree_window_widget_class",
        load_blueprint_class(widget_paths["skill_tree"]),
    )
    unreal.BlueprintEditorLibrary.compile_blueprint(controller_blueprint)

    for widget_blueprint in widget_blueprints.values():
        unreal.EditorAssetLibrary.save_loaded_asset(
            widget_blueprint, only_if_is_dirty=False
        )
    unreal.EditorAssetLibrary.save_loaded_asset(
        controller_blueprint, only_if_is_dirty=False
    )

    log("Inventory, equipment, and skill tree windows are assigned to the player controller.")


if __name__ == "__main__":
    main()
