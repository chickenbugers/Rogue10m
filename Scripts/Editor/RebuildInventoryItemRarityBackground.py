"""Rebuild only WBP_InventoryItem with the rarity background layer."""

from __future__ import annotations

import os
import sys

import unreal


SCRIPT_FILE = globals().get(
    "__file__",
    os.path.join(os.getcwd(), "Scripts", "Editor", "RebuildInventoryItemRarityBackground.py"),
)
SCRIPT_DIRECTORY = os.path.dirname(os.path.abspath(SCRIPT_FILE))
if SCRIPT_DIRECTORY not in sys.path:
    sys.path.insert(0, SCRIPT_DIRECTORY)

import BuildMenuDesignerLayouts as menu  # noqa: E402


def main() -> None:
    if not hasattr(unreal, "UMGToolSet"):
        raise RuntimeError("UE5.8 UMGToolSet 플러그인이 로드되지 않았습니다.")

    item_blueprint = menu.create_or_load_inventory_item()
    menu.build_inventory_item(item_blueprint)
    menu.validate_tree(
        item_blueprint,
        "inventory_item",
        menu.ASSETS["inventory_item"],
    )
    menu.compile_and_save(item_blueprint, menu.ASSETS["inventory_item"])

    item_cdo = unreal.get_default_object(
        menu.load_blueprint_class(menu.ASSETS["inventory_item"])
    )
    item_cdo.set_editor_property(
        "inventory_item_tooltip_widget_class",
        menu.load_blueprint_class(menu.ASSETS["inventory_item_tooltip"]),
    )
    menu.compile_and_save(item_blueprint, menu.ASSETS["inventory_item"])
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    menu.log("Inventory Item Rarity Background WBP 재구성 완료")


if __name__ == "__main__":
    main()
