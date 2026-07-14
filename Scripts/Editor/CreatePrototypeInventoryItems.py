"""Create or update prototype 1x1, 2x3, and 4x3 inventory item Data Assets."""

import unreal


ASSET_ROOT = "/Game/DataAsset/Item/Prototype"
ITEMS = (
    {
        "name": "DA_Item_Prototype_1x1",
        "id": "PrototypeItem_1x1",
        "display_name": "소형 테스트 아이템",
        "description": "1x1 인벤토리 배치와 드래그 앤 드롭을 확인하는 프로토타입 아이템입니다.",
        "size": (1, 1),
        "weight": 0.1,
        "tint": (0.10, 0.58, 0.78, 0.88),
    },
    {
        "name": "DA_Item_Prototype_2x3",
        "id": "PrototypeItem_2x3",
        "display_name": "중형 테스트 아이템",
        "description": "2x3 인벤토리 배치, 충돌 프리뷰와 회전을 확인하는 프로토타입 아이템입니다.",
        "size": (2, 3),
        "weight": 1.0,
        "tint": (0.22, 0.42, 0.86, 0.88),
    },
    {
        "name": "DA_Item_Prototype_4x3",
        "id": "PrototypeItem_4x3",
        "display_name": "대형 테스트 아이템",
        "description": "4x3 대형 아이템의 공간 점유와 회전을 확인하는 프로토타입 아이템입니다.",
        "size": (4, 3),
        "weight": 2.5,
        "tint": (0.56, 0.24, 0.78, 0.88),
    },
)


def create_or_load(path, data_asset_class):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        return asset
    package_path, asset_name = path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", data_asset_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, data_asset_class, factory
    )
    if not asset:
        raise RuntimeError(f"Data Asset creation failed: {path}")
    return asset


def main():
    item_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mItemDataAsset")
    if not item_class:
        raise RuntimeError("Rogue10mItemDataAsset class is unavailable. Build the Editor target first.")

    created_paths = []
    for definition in ITEMS:
        path = f"{ASSET_ROOT}/{definition['name']}"
        asset = create_or_load(path, item_class)
        asset.set_editor_property("item_id", definition["id"])
        asset.set_editor_property("display_name", definition["display_name"])
        asset.set_editor_property("description", definition["description"])
        asset.set_editor_property("inventory_size", unreal.IntPoint(*definition["size"]))
        asset.set_editor_property("max_stack_size", 1)
        asset.set_editor_property("unit_weight", definition["weight"])
        asset.set_editor_property("inventory_tint", unreal.LinearColor(*definition["tint"]))
        asset.set_editor_property("can_drop", True)
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
        created_paths.append(path)
        unreal.log(
            f"[PrototypeInventoryItems] Ready: {path} "
            f"({definition['size'][0]}x{definition['size'][1]})"
        )

    for path, definition in zip(created_paths, ITEMS):
        asset = unreal.EditorAssetLibrary.load_asset(path)
        size = asset.get_editor_property("inventory_size")
        if (size.x, size.y) != definition["size"]:
            raise RuntimeError(f"Inventory size validation failed: {path} -> {size}")

    component_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mInventoryComponent")
    component_cdo = unreal.get_default_object(component_class)
    starting_items = component_cdo.get_editor_property("prototype_starting_items")
    if len(starting_items) != len(ITEMS):
        raise RuntimeError(
            f"Prototype starting item reference validation failed: {len(starting_items)}"
        )

    unreal.log(
        "[PrototypeInventoryItems] Validation passed for assets and "
        "inventory component starter references."
    )


if __name__ == "__main__":
    main()