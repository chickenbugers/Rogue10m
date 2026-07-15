"""Rebuild the single-bag inventory window and aspect-safe NxM item widget."""

import os
import sys
import unreal

SCRIPT_DIR = (os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() else r"D:\Project\Rogue10m\Scripts\Editor")
if SCRIPT_DIR not in sys.path:
    sys.path.insert(0, SCRIPT_DIR)

import BuildMenuDesignerLayouts as layouts


def main():
    if not hasattr(unreal, "UMGToolSet"):
        raise RuntimeError("UE5.8 UMGToolSet plugin is not loaded.")

    builders = {
        "inventory_item": layouts.build_inventory_item,
        "inventory": layouts.build_inventory,
    }
    for key, builder in builders.items():
        path = layouts.ASSETS[key]
        widget_blueprint = layouts.require_asset(path)
        builder(widget_blueprint)
        layouts.validate_tree(widget_blueprint, key, path)
        layouts.compile_and_save(widget_blueprint, path)

    inventory_cdo = unreal.get_default_object(
        layouts.load_blueprint_class(layouts.ASSETS["inventory"])
    )
    inventory_cdo.set_editor_property(
        "inventory_cell_widget_class",
        layouts.load_blueprint_class(layouts.ASSETS["inventory_cell"]),
    )
    inventory_cdo.set_editor_property(
        "inventory_item_widget_class",
        layouts.load_blueprint_class(layouts.ASSETS["inventory_item"]),
    )
    layouts.compile_and_save(
        layouts.require_asset(layouts.ASSETS["inventory"]),
        layouts.ASSETS["inventory"],
    )
    unreal.log(
        "[InventoryItemFootprint] Single-bag inventory and aspect-safe item widgets rebuilt."
    )


if __name__ == "__main__":
    main()