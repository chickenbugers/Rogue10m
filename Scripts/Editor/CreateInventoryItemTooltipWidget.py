"""Create and wire only the inventory item tooltip Widget Blueprint."""

from __future__ import annotations

import os
import sys

import unreal


SCRIPT_DIRECTORY = os.path.dirname(os.path.abspath(__file__))
if SCRIPT_DIRECTORY not in sys.path:
    sys.path.insert(0, SCRIPT_DIRECTORY)

import BuildMenuDesignerLayouts as menu  # noqa: E402


def main() -> None:
    if not hasattr(unreal, "UMGToolSet"):
        raise RuntimeError("UE5.8 UMGToolSet 플러그인이 로드되지 않았습니다.")

    tooltip_blueprint = menu.create_or_load_inventory_item_tooltip()
    menu.build_inventory_item_tooltip(tooltip_blueprint)
    menu.validate_tree(
        tooltip_blueprint,
        "inventory_item_tooltip",
        menu.ASSETS["inventory_item_tooltip"],
    )
    menu.compile_and_save(
        tooltip_blueprint,
        menu.ASSETS["inventory_item_tooltip"],
    )

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    menu.log("Inventory Item Tooltip WBP 생성 완료")


if __name__ == "__main__":
    main()
