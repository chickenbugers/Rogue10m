"""Build Rogue10m menu layouts directly in Widget Blueprint Designer trees."""

from __future__ import annotations

import unreal


ASSETS = {
    "inventory_cell": "/Game/Widget/Menu/Inventory/WBP_InventoryCell",
    "inventory_item": "/Game/Widget/Menu/Inventory/WBP_InventoryItem",
    "bag_tab": "/Game/Widget/Menu/Inventory/WBP_BagTab",
    "inventory": "/Game/Widget/Menu/Inventory/WBP_InventoryWindow",
    "equipment": "/Game/Widget/Menu/Equipment/WBP_EquipmentWindow",
    "entry": "/Game/Widget/Menu/SkillTree/WBP_SkillTreeEntry",
    "skill_tree": "/Game/Widget/Menu/SkillTree/WBP_SkillTreeWindow",
}
REQUIRED_WIDGETS = {
    "inventory_cell": {"UI_InventoryCellFrame"},
    "inventory_item": {
        "UI_InventoryItemSize",
        "UI_InventoryItemIcon",
        "UI_InventoryItemQuantityText",
        "UI_InventoryItemPreviewBorder",
    },
    "bag_tab": {"UI_BagTabButton", "UI_BagNameText"},
    "inventory": {
        "UI_InventoryGridFrame",
        "UI_InventoryGrid",
        "UI_InventoryItemCanvas",
        "UI_InventoryMoneyText",
        "UI_InventoryWeightText",
    },
    "equipment": {
        "UI_EquipmentSlotContainer",
        "UI_CharacterPreviewImage",
        "UI_EquipmentStatsContainer",
        "UI_WeaponSlotIcon",
        "UI_HeadSlotIcon",
        "UI_ChestSlotIcon",
        "UI_HandsSlotIcon",
        "UI_LegsSlotIcon",
        "UI_FeetSlotIcon",
        "UI_AccessorySlotIcon",
    },
    "entry": {
        "UI_SkillIconImage",
        "UI_SkillNameText",
        "UI_SkillDescriptionText",
        "UI_SkillLockText",
    },
    "skill_tree": {"UI_SkillListContainer"},
}
FORBIDDEN_WIDGETS = {
    "inventory": {
        "UI_InventoryCapacityText",
        "UI_InventoryHintText",
        "UI_BagTabContainer",
    },
}
FRAME_COLOR = unreal.LinearColor(0.02, 0.025, 0.03, 0.96)
SECTION_COLOR = unreal.LinearColor(0.055, 0.06, 0.07, 0.96)
CELL_COLOR = unreal.LinearColor(0.085, 0.09, 0.10, 0.96)
CELL_BORDER_COLOR = unreal.LinearColor(0.018, 0.02, 0.024, 1.0)
GRID_RECESS_COLOR = unreal.LinearColor(0.012, 0.014, 0.018, 0.98)
UMG_TOOL = unreal.get_default_object(unreal.UMGToolSet)

def umg(method: str, *args):
    return UMG_TOOL.call_method(method, args)


def log(message: str) -> None:
    unreal.log(f"[Rogue10mMenuDesigner] {message}")


def require_asset(path: str):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"Widget Blueprint를 찾을 수 없습니다: {path}")
    return asset


def create_or_load_inventory_cell():
    path = ASSETS["inventory_cell"]
    existing = unreal.EditorAssetLibrary.load_asset(path)
    if existing:
        return existing

    parent_class = unreal.load_class(
        None, "/Script/Rogue10m.Rogue10mInventoryCellWidget"
    )
    if not parent_class:
        raise RuntimeError("Rogue10mInventoryCellWidget 부모 클래스를 찾을 수 없습니다.")

    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "WBP_InventoryCell", path.rsplit("/", 1)[0], unreal.WidgetBlueprint, factory
    )
    if not asset:
        raise RuntimeError("WBP_InventoryCell 생성에 실패했습니다.")
    log(f"생성 완료: {path}")
    return asset


def create_widget_blueprint_if_missing(key: str, asset_name: str, parent_class_path: str):
    path = ASSETS[key]
    existing = unreal.EditorAssetLibrary.load_asset(path)
    if existing:
        return existing
    parent_class = unreal.load_class(None, parent_class_path)
    if not parent_class:
        raise RuntimeError(f"Parent class not found: {parent_class_path}")
    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, path.rsplit("/", 1)[0], unreal.WidgetBlueprint, factory
    )
    if not asset:
        raise RuntimeError(f"Failed to create {asset_name}")
    log(f"Created: {path}")
    return asset


def create_or_load_inventory_item():
    return create_widget_blueprint_if_missing(
        "inventory_item", "WBP_InventoryItem", "/Script/Rogue10m.Rogue10mInventoryItemWidget"
    )


def create_or_load_bag_tab():
    return create_widget_blueprint_if_missing(
        "bag_tab", "WBP_BagTab", "/Script/Rogue10m.Rogue10mBagTabWidget"
    )

def load_blueprint_class(path: str):
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(path)
    if not generated_class:
        raise RuntimeError(f"Blueprint GeneratedClass를 찾을 수 없습니다: {path}")
    return generated_class

def clear_tree(widget_blueprint) -> None:
    tree = umg("GetWidgets", widget_blueprint)
    roots = [
        info.widget
        for info in tree.widgets
        if info.widget and not info.parent and not info.named_slot_host
    ]
    for root in roots:
        if not umg("RemoveWidget", widget_blueprint, root):
            raise RuntimeError(f"기존 root 제거 실패: {root.get_name()}")


def add(widget_blueprint, widget_class, name: str, parent=None, variable=False):
    info = umg("AddWidget",
        widget_blueprint, widget_class, name, parent, -1
    )
    if not info.widget:
        raise RuntimeError(f"Widget 추가 실패: {name}")
    if variable:
        umg("ToggleWidgetAsVariable",
            widget_blueprint, info.widget, True
        )
    return info.widget, info.slot


def set_canvas_layout(
    slot,
    position,
    size,
    anchors=(0.5, 0.5),
    alignment=(0.5, 0.5),
    z_order=0,
):
    if not slot:
        return
    anchor = unreal.Vector2D(anchors[0], anchors[1])
    slot.set_anchors(unreal.Anchors(minimum=anchor, maximum=anchor))
    slot.set_alignment(unreal.Vector2D(alignment[0], alignment[1]))
    slot.set_position(unreal.Vector2D(position[0], position[1]))
    slot.set_size(unreal.Vector2D(size[0], size[1]))
    slot.set_z_order(z_order)


def set_fill_alignment(slot):
    if not slot:
        return
    if hasattr(slot, "set_horizontal_alignment"):
        slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_FILL)
    if hasattr(slot, "set_vertical_alignment"):
        slot.set_vertical_alignment(unreal.VerticalAlignment.V_ALIGN_FILL)


def set_padding(slot, value=4.0):
    if slot and hasattr(slot, "set_padding"):
        slot.set_padding(unreal.Margin(value, value, value, value))


def make_text(widget_blueprint, name, text, parent, variable=False, wrap=False, font_size=None):
    widget, slot = add(
        widget_blueprint, unreal.TextBlock, name, parent, variable=variable
    )
    widget.set_text(text)
    widget.set_auto_wrap_text(wrap)
    if font_size is not None:
        font = widget.get_editor_property("font")
        font.size = font_size
        widget.set_editor_property("font", font)
    set_padding(slot)
    return widget, slot


def make_border(widget_blueprint, name, parent, color=CELL_COLOR, padding=6.0):
    widget, slot = add(widget_blueprint, unreal.Border, name, parent)
    widget.set_brush_color(color)
    widget.set_padding(unreal.Margin(padding, padding, padding, padding))
    return widget, slot


def build_inventory_cell(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    size_box, _ = add(widget_blueprint, unreal.SizeBox, "UI_InventoryCellSize")
    size_box.set_width_override(44.0)
    size_box.set_height_override(44.0)
    frame, _ = make_border(
        widget_blueprint, "UI_InventoryCellFrame", size_box,
        CELL_BORDER_COLOR, 1.0,
    )
    make_border(widget_blueprint, "UI_InventoryCellFill", frame, CELL_COLOR, 0.0)


def build_inventory_item(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    size_box, _ = add(
        widget_blueprint, unreal.SizeBox, "UI_InventoryItemSize", variable=True
    )
    # Designer와 1x1 fallback 기준 크기입니다. 런타임에는 C++가 W*CellSize, H*CellSize로 덮어씁니다.
    size_box.set_width_override(44.0)
    size_box.set_height_override(44.0)
    grid, size_box_slot = add(widget_blueprint, unreal.GridPanel, "UI_InventoryItemRoot", size_box)
    set_fill_alignment(size_box_slot)
    border, border_slot = add(widget_blueprint, unreal.Border, "UI_InventoryItemPreviewBorder", grid, variable=True)
    set_fill_alignment(border_slot)
    border_slot.set_layer(0)
    border.set_padding(unreal.Margin(0.0, 0.0, 0.0, 0.0))
    border.set_brush_color(unreal.LinearColor(0.05, 0.05, 0.05, 0.35))
    # Border paints its brush first and then its Image content. C++ computes an
    # aspect-ratio-preserving desired size from the texture and NxM footprint.
    icon, icon_slot = add(
        widget_blueprint, unreal.Image, "UI_InventoryItemIcon", border, variable=True
    )
    set_padding(icon_slot, 4.0)
    icon_slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_CENTER)
    icon_slot.set_vertical_alignment(unreal.VerticalAlignment.V_ALIGN_CENTER)

    quantity, quantity_slot = make_text(
        widget_blueprint,
        "UI_InventoryItemQuantityText",
        "1",
        grid,
        variable=True,
        font_size=16,
    )
    quantity.set_shadow_offset(unreal.Vector2D(1.0, 1.0))
    quantity.set_shadow_color_and_opacity(unreal.LinearColor(0.0, 0.0, 0.0, 0.9))
    if quantity_slot and hasattr(quantity_slot, "set_padding"):
        quantity_slot.set_padding(unreal.Margin(0.0, 1.0, 3.0, 0.0))
    if quantity_slot and hasattr(quantity_slot, "set_layer"):
        quantity_slot.set_layer(1)
    if quantity_slot and hasattr(quantity_slot, "set_horizontal_alignment"):
        quantity_slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_RIGHT)
    if quantity_slot and hasattr(quantity_slot, "set_vertical_alignment"):
        quantity_slot.set_vertical_alignment(unreal.VerticalAlignment.V_ALIGN_TOP)


def build_bag_tab(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    size_box, _ = add(widget_blueprint, unreal.SizeBox, "UI_BagTabSize")
    size_box.set_width_override(136.0)
    size_box.set_height_override(36.0)
    button, _ = add(widget_blueprint, unreal.Button, "UI_BagTabButton", size_box, variable=True)
    button.set_background_color(unreal.LinearColor(0.09, 0.095, 0.105, 1.0))
    column, _ = add(widget_blueprint, unreal.VerticalBox, "UI_BagTabContent", button)
    make_text(
        widget_blueprint, "UI_BagNameText", "Base Bag", column,
        variable=True, font_size=14,
    )

def build_inventory(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    root, _ = add(widget_blueprint, unreal.CanvasPanel, "UI_InventoryCanvas")
    _, frame_slot = make_border(
        widget_blueprint, "UI_InventoryWindowFrame", root, FRAME_COLOR, 0.0
    )
    set_canvas_layout(frame_slot, (0, 0), (560, 610), z_order=0)

    _, title_slot = make_text(
        widget_blueprint, "UI_InventoryTitleText", "인벤토리", root
    )
    set_canvas_layout(title_slot, (-220, -276), (440, 32), alignment=(0, 0.5), z_order=2)


    _, grid_frame_slot = make_border(
        widget_blueprint, "UI_InventoryGridFrame", root, GRID_RECESS_COLOR, 4.0
    )
    set_canvas_layout(grid_frame_slot, (0, -12), (448, 448), z_order=1)

    grid, grid_slot = add(
        widget_blueprint, unreal.UniformGridPanel, "UI_InventoryGrid", root, variable=True
    )
    grid.set_min_desired_slot_width(44.0)
    grid.set_min_desired_slot_height(44.0)
    set_canvas_layout(grid_slot, (0, -12), (440, 440), z_order=2)

    _, item_slot = add(
        widget_blueprint, unreal.CanvasPanel, "UI_InventoryItemCanvas", root, variable=True
    )
    set_canvas_layout(item_slot, (0, -12), (440, 440), z_order=3)

    bottom, bottom_slot = add(
        widget_blueprint, unreal.HorizontalBox, "UI_InventoryBottomInfo", root
    )
    set_canvas_layout(bottom_slot, (-220, 264), (440, 34), alignment=(0, 0.5), z_order=2)
    make_text(
        widget_blueprint, "UI_InventoryMoneyText", "돈: 0", bottom, variable=True
    )
    make_text(
        widget_blueprint,
        "UI_InventoryWeightText",
        "무게: 0.0 / 100.0 kg",
        bottom,
        variable=True,
    )

def build_equipment(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    root, _ = add(widget_blueprint, unreal.CanvasPanel, "UI_EquipmentCanvas")
    _, frame_slot = make_border(
        widget_blueprint, "UI_EquipmentWindowFrame", root, FRAME_COLOR, 0.0
    )
    set_canvas_layout(frame_slot, (0, 0), (760, 620), z_order=0)
    _, title_slot = make_text(widget_blueprint, "UI_EquipmentTitleText", "장비", root)
    set_canvas_layout(title_slot, (-340, -280), (680, 32), alignment=(0, 0.5), z_order=1)

    preview_frame, preview_slot = make_border(
        widget_blueprint, "UI_CharacterPreviewFrame", root, SECTION_COLOR, 8.0
    )
    set_canvas_layout(preview_slot, (-145, -5), (330, 500), z_order=1)
    preview, _ = add(
        widget_blueprint, unreal.Image, "UI_CharacterPreviewImage", preview_frame, variable=True
    )
    preview.set_desired_size_override(unreal.Vector2D(314, 484))

    slots, slots_slot = add(
        widget_blueprint, unreal.VerticalBox, "UI_EquipmentSlotContainer", root, variable=True
    )
    set_canvas_layout(slots_slot, (235, -60), (210, 390), z_order=1)
    for frame_name, icon_name, label in (
        ("UI_WeaponSlotFrame", "UI_WeaponSlotIcon", "무기"),
        ("UI_HeadSlotFrame", "UI_HeadSlotIcon", "머리"),
        ("UI_ChestSlotFrame", "UI_ChestSlotIcon", "상의"),
        ("UI_HandsSlotFrame", "UI_HandsSlotIcon", "장갑"),
        ("UI_LegsSlotFrame", "UI_LegsSlotIcon", "하의"),
        ("UI_FeetSlotFrame", "UI_FeetSlotIcon", "신발"),
        ("UI_AccessorySlotFrame", "UI_AccessorySlotIcon", "장신구"),
    ):
        slot_frame, outer_slot = make_border(
            widget_blueprint, frame_name, slots, CELL_COLOR, 2.0
        )
        set_padding(outer_slot, 3.0)
        slot_grid, frame_content_slot = add(
            widget_blueprint, unreal.GridPanel, f"{frame_name}_Grid", slot_frame
        )
        set_fill_alignment(frame_content_slot)
        icon_scale, icon_scale_slot = add(
            widget_blueprint, unreal.ScaleBox, f"{frame_name}_IconScale", slot_grid
        )
        set_fill_alignment(icon_scale_slot)
        set_padding(icon_scale_slot, 4.0)
        icon_scale.set_stretch(unreal.Stretch.SCALE_TO_FIT)
        icon_scale.set_stretch_direction(unreal.StretchDirection.BOTH)
        _, icon_slot = add(
            widget_blueprint, unreal.Image, icon_name, icon_scale, variable=True
        )
        set_fill_alignment(icon_slot)
        label_widget, label_slot = make_text(
            widget_blueprint,
            f"{frame_name}_Text",
            label,
            slot_grid,
            font_size=12,
        )
        label_widget.set_shadow_offset(unreal.Vector2D(1.0, 1.0))
        label_widget.set_shadow_color_and_opacity(
            unreal.LinearColor(0.0, 0.0, 0.0, 0.9)
        )
        if label_slot and hasattr(label_slot, "set_horizontal_alignment"):
            label_slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_CENTER)
        if label_slot and hasattr(label_slot, "set_vertical_alignment"):
            label_slot.set_vertical_alignment(unreal.VerticalAlignment.V_ALIGN_BOTTOM)

    stats, stats_slot = add(
        widget_blueprint, unreal.VerticalBox, "UI_EquipmentStatsContainer", root, variable=True
    )
    set_canvas_layout(stats_slot, (235, 205), (210, 95), z_order=1)
    make_text(widget_blueprint, "UI_AttackStatText", "공격력 0", stats)
    make_text(widget_blueprint, "UI_DefenseStatText", "방어력 0", stats)


def build_skill_entry(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    size_box, _ = add(widget_blueprint, unreal.SizeBox, "UI_SkillTreeEntrySize")
    size_box.set_width_override(360.0)
    size_box.set_height_override(104.0)
    frame, _ = make_border(
        widget_blueprint, "UI_SkillTreeEntryFrame", size_box, SECTION_COLOR, 8.0
    )
    row, _ = add(widget_blueprint, unreal.HorizontalBox, "UI_SkillTreeEntryRoot", frame)
    icon, icon_slot = add(
        widget_blueprint, unreal.Image, "UI_SkillIconImage", row, variable=True
    )
    icon.set_desired_size_override(unreal.Vector2D(64, 64))
    set_padding(icon_slot, 4.0)
    text_column, text_slot = add(
        widget_blueprint, unreal.VerticalBox, "UI_SkillTextColumn", row
    )
    set_padding(text_slot, 4.0)
    make_text(widget_blueprint, "UI_SkillNameText", "스킬", text_column, variable=True)
    make_text(
        widget_blueprint,
        "UI_SkillDescriptionText",
        "스킬 설명",
        text_column,
        variable=True,
        wrap=True,
    )
    make_text(widget_blueprint, "UI_SkillLockText", "잠김", text_column, variable=True)


def build_skill_tree(widget_blueprint) -> None:
    clear_tree(widget_blueprint)
    root, _ = add(widget_blueprint, unreal.CanvasPanel, "UI_SkillTreeCanvas")
    _, frame_slot = make_border(
        widget_blueprint, "UI_SkillTreeWindowFrame", root, FRAME_COLOR, 0.0
    )
    set_canvas_layout(frame_slot, (0, 0), (860, 660), z_order=0)
    _, title_slot = make_text(widget_blueprint, "UI_SkillTreeTitleText", "스킬 트리", root)
    set_canvas_layout(title_slot, (-390, -295), (780, 32), alignment=(0, 0.5), z_order=1)
    _, guide_slot = make_text(
        widget_blueprint,
        "UI_SkillTreeGuideText",
        "해금된 스킬을 HUD 입력 슬롯으로 Drag & Drop 하세요.",
        root,
    )
    set_canvas_layout(guide_slot, (-390, -255), (780, 28), alignment=(0, 0.5), z_order=1)

    filters, filters_slot = add(
        widget_blueprint, unreal.HorizontalBox, "UI_SkillFilterBar", root
    )
    set_canvas_layout(filters_slot, (-390, -210), (780, 36), alignment=(0, 0.5), z_order=1)
    make_text(widget_blueprint, "UI_AllSkillsFilterText", "전체", filters)
    make_text(widget_blueprint, "UI_UnlockedSkillsFilterText", "해금", filters)
    make_text(widget_blueprint, "UI_LockedSkillsFilterText", "잠김", filters)

    scroll, scroll_slot = add(widget_blueprint, unreal.ScrollBox, "UI_SkillListScrollBox", root)
    set_canvas_layout(scroll_slot, (0, 35), (780, 430), z_order=1)
    skill_list, _ = add(
        widget_blueprint, unreal.WrapBox, "UI_SkillListContainer", scroll, variable=True
    )
    skill_list.set_inner_slot_padding(unreal.Vector2D(8, 8))
    _, points_slot = make_text(
        widget_blueprint, "UI_SkillPointText", "사용 가능 스킬 포인트: 0", root
    )
    set_canvas_layout(points_slot, (-390, 292), (780, 28), alignment=(0, 0.5), z_order=1)


def compile_and_save(widget_blueprint, path: str) -> None:
    if not umg("CompileWidgetBlueprint", widget_blueprint):
        raise RuntimeError(f"Widget Blueprint 컴파일 실패: {path}")
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        widget_blueprint, only_if_is_dirty=False
    ):
        raise RuntimeError(f"Widget Blueprint 저장 실패: {path}")
    tree = umg("GetWidgets", widget_blueprint)
    log(f"저장 완료: {path} ({len(tree.widgets)} widgets)")


def validate_tree(widget_blueprint, key: str, path: str) -> None:
    tree = umg("GetWidgets", widget_blueprint)
    widgets = [info.widget for info in tree.widgets if info.widget]
    names = {widget.get_name() for widget in widgets}
    missing = sorted(REQUIRED_WIDGETS[key] - names)
    if missing:
        raise RuntimeError(f"필수 위젯 누락: {path}: {', '.join(missing)}")

    forbidden = sorted(FORBIDDEN_WIDGETS.get(key, set()) & names)
    if forbidden:
        raise RuntimeError(
            f"제거 대상 위젯 잔존: {path}: {', '.join(forbidden)}"
        )
    overlays = [
        widget.get_name()
        for widget in widgets
        if widget.get_class().get_name() == "Overlay"
    ]
    if overlays:
        raise RuntimeError(f"Overlay 직접 배치 위반: {path}: {', '.join(overlays)}")

    log(f"Designer 검증 완료: {path} (필수 위젯 {len(REQUIRED_WIDGETS[key])}개, Overlay 0개)")

def main() -> None:
    if not hasattr(unreal, "UMGToolSet"):
        raise RuntimeError("UE5.8 UMGToolSet 플러그인이 로드되지 않았습니다.")
    create_or_load_inventory_cell()
    create_or_load_inventory_item()
    create_or_load_bag_tab()
    builders = {
        "inventory_cell": build_inventory_cell,
        "inventory_item": build_inventory_item,
        "bag_tab": build_bag_tab,
        "inventory": build_inventory,
        "equipment": build_equipment,
        "entry": build_skill_entry,
        "skill_tree": build_skill_tree,
    }
    for key, path in ASSETS.items():
        widget_blueprint = require_asset(path)
        builders[key](widget_blueprint)
        validate_tree(widget_blueprint, key, path)
        compile_and_save(widget_blueprint, path)
    inventory_cdo = unreal.get_default_object(
        load_blueprint_class(ASSETS["inventory"])
    )
    inventory_cdo.set_editor_property(
        "inventory_cell_widget_class",
        load_blueprint_class(ASSETS["inventory_cell"]),
    )
    inventory_cdo.set_editor_property(
        "inventory_item_widget_class", load_blueprint_class(ASSETS["inventory_item"])
    )

    inventory_blueprint = require_asset(ASSETS["inventory"])
    compile_and_save(inventory_blueprint, ASSETS["inventory"])
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log("메뉴 Widget Blueprint와 Inventory Cell Designer 배치를 완료했습니다.")


if __name__ == "__main__":
    main()
