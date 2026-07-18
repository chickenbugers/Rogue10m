"""Create the equipment slot action WBP and configure equipment widget class references."""

from __future__ import annotations

import os
import sys

import unreal


SCRIPT_FILE = globals().get(
    "__file__",
    os.path.join(os.getcwd(), "Scripts", "Editor", "CreateEquipmentSlotActionWidget.py"),
)
SCRIPT_DIRECTORY = os.path.dirname(os.path.abspath(SCRIPT_FILE))
if SCRIPT_DIRECTORY not in sys.path:
    sys.path.insert(0, SCRIPT_DIRECTORY)

import BuildMenuDesignerLayouts as menu  # noqa: E402


def main() -> None:
    if not hasattr(unreal, "UMGToolSet"):
        raise RuntimeError("UE5.8 UMGToolSet is not loaded.")

    action_blueprint = menu.create_or_load_equipment_slot_action()
    menu.build_equipment_slot_action(action_blueprint)
    menu.validate_tree(
        action_blueprint,
        "equipment_slot_action",
        menu.ASSETS["equipment_slot_action"],
    )
    menu.compile_and_save(action_blueprint, menu.ASSETS["equipment_slot_action"])

    equipment_blueprint = menu.require_asset(menu.ASSETS["equipment"])
    equipment_cdo = unreal.get_default_object(
        menu.load_blueprint_class(menu.ASSETS["equipment"])
    )
    equipment_cdo.set_editor_property(
        "equipment_item_tooltip_widget_class",
        menu.load_blueprint_class(menu.ASSETS["inventory_item_tooltip"]),
    )
    equipment_cdo.set_editor_property(
        "equipment_slot_action_widget_class",
        menu.load_blueprint_class(menu.ASSETS["equipment_slot_action"]),
    )
    menu.compile_and_save(equipment_blueprint, menu.ASSETS["equipment"])
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    menu.log("Equipment slot action widget created and configured.")


if __name__ == "__main__":
    main()
