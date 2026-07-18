from __future__ import annotations

import importlib.util
from pathlib import Path

import unreal


LAYOUT_SCRIPT = Path(r"D:\Project\Rogue10m\Scripts\Editor\BuildMenuDesignerLayouts.py")
WINDOW_LAYOUT = {
    "inventory": ((-360, 0), (560, 610)),
    "equipment": ((200, 0), (980, 620)),
}
EQUIPMENT_SLOT_LAYOUT = {
    "Head": ("투구", (-115, -180), (100, 100)),
    "Necklace": ("목걸이", (0, -180), (100, 100)),
    "Chest": ("갑옷", (-115, 0), (100, 150)),
    "Hands": ("장갑", (0, 0), (100, 100)),
    "Weapon": ("무기", (120, 0), (100, 200)),
    "Feet": ("신발", (-115, 180), (100, 100)),
    "Ring": ("반지", (0, 180), (100, 100)),
}
EQUIPMENT_STAT_TEXTS = {
    "UI_StatsTitleText": ("캐릭터 스탯", 16),
    "UI_AttackStatText": ("공격력 0", 13),
    "UI_DefenseStatText": ("방어력 0", 13),
    "UI_MaxHealthStatText": ("최대 체력 0", 13),
    "UI_CriticalChanceStatText": ("치명타 확률 0%", 13),
    "UI_AttackSpeedStatText": ("공격 속도 0%", 13),
    "UI_MoveSpeedStatText": ("이동 속도 0", 13),
}


def load_layout_module():
    spec = importlib.util.spec_from_file_location("rogue10m_menu_layouts", LAYOUT_SCRIPT)
    if not spec or not spec.loader:
        raise RuntimeError(f"레이아웃 모듈을 불러올 수 없습니다: {LAYOUT_SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def rounded_vector(value):
    return round(value.x), round(value.y)


def require_parent(infos, child_name, parent_name):
    info = infos.get(child_name)
    actual_parent = info.parent.get_name() if info and info.parent else None
    if actual_parent != parent_name:
        raise RuntimeError(
            f"잘못된 Widget 계층: {child_name} parent={actual_parent}, expected={parent_name}"
        )


def validate_window(layouts, key, widget_blueprint):
    tree = layouts.umg("GetWidgets", widget_blueprint)
    infos = {info.widget.get_name(): info for info in tree.widgets if info.widget}
    widgets = {name: info.widget for name, info in infos.items()}
    slots = {name: info.slot for name, info in infos.items() if info.slot}

    layouts.validate_tree(widget_blueprint, key, layouts.ASSETS[key])
    screen_root_name = "UI_InventoryCanvas" if key == "inventory" else "UI_EquipmentCanvas"
    require_parent(infos, "UI_WindowRoot", screen_root_name)
    require_parent(infos, "UI_WindowDragHandle", "UI_WindowRoot")
    if widgets[screen_root_name].get_visibility() != unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE:
        raise RuntimeError(f"{screen_root_name}는 다른 창 입력을 위한 SelfHitTestInvisible이어야 합니다.")
    expected_position, expected_size = WINDOW_LAYOUT[key]
    window_slot = slots["UI_WindowRoot"]
    if rounded_vector(window_slot.get_position()) != expected_position:
        raise RuntimeError(f"{key} 초기 창 위치 불일치: {window_slot.get_position()}")
    if rounded_vector(window_slot.get_size()) != expected_size:
        raise RuntimeError(f"{key} 창 크기 불일치: {window_slot.get_size()}")
    handle_size = rounded_vector(slots["UI_WindowDragHandle"].get_size())
    if handle_size[1] != 44:
        raise RuntimeError(f"{key} DragHandle 높이는 44px여야 합니다: {handle_size}")

    if key != "equipment":
        return

    require_parent(infos, "UI_CharacterPreviewFrame", "UI_WindowRoot")
    require_parent(infos, "UI_EquipmentSlotContainer", "UI_WindowRoot")
    require_parent(infos, "UI_EquipmentStatsFrame", "UI_WindowRoot")
    require_parent(infos, "UI_EquipmentStatsContainer", "UI_EquipmentStatsFrame")
    for stat_name, (expected_text, expected_font_size) in EQUIPMENT_STAT_TEXTS.items():
        require_parent(infos, stat_name, "UI_EquipmentStatsContainer")
        stat_widget = widgets[stat_name]
        if str(stat_widget.get_text()) != expected_text:
            raise RuntimeError(f"{stat_name} 텍스트 불일치: {stat_widget.get_text()}")
        if stat_widget.get_editor_property("font").size != expected_font_size:
            raise RuntimeError(f"{stat_name} 폰트 크기 불일치")

    stats_frame_slot = slots["UI_EquipmentStatsFrame"]
    if rounded_vector(stats_frame_slot.get_position()) != (-375, -5):
        raise RuntimeError("스탯 패널이 프리뷰 왼쪽 위치에 있지 않습니다.")
    if rounded_vector(stats_frame_slot.get_size()) != (200, 500):
        raise RuntimeError("스탯 패널 크기는 200x500이어야 합니다.")

    preview_slot = slots["UI_CharacterPreviewFrame"]
    if rounded_vector(preview_slot.get_position()) != (-75, -5):
        raise RuntimeError("캐릭터 프리뷰가 장비창 중앙 왼쪽 위치에 있지 않습니다.")
    if rounded_vector(preview_slot.get_size()) != (340, 500):
        raise RuntimeError("캐릭터 프리뷰 크기는 340x500이어야 합니다.")

    equipment_container_slot = slots["UI_EquipmentSlotContainer"]
    if rounded_vector(equipment_container_slot.get_position()) != (290, -5):
        raise RuntimeError("장비 슬롯 컨테이너가 장비창 우측 위치에 있지 않습니다.")
    if rounded_vector(equipment_container_slot.get_size()) != (350, 500):
        raise RuntimeError("장비 슬롯 컨테이너 크기는 350x500이어야 합니다.")

    for slot_key, (expected_text, expected_position, expected_size) in EQUIPMENT_SLOT_LAYOUT.items():
        frame_name = f"UI_{slot_key}SlotFrame"
        layer_name = f"{frame_name}_Layer"
        scale_name = f"{frame_name}_IconScale"
        icon_name = f"UI_{slot_key}SlotIcon"
        text_name = f"UI_{slot_key}SlotLocationText"
        require_parent(infos, frame_name, "UI_EquipmentSlotContainer")
        require_parent(infos, layer_name, frame_name)
        require_parent(infos, scale_name, layer_name)
        require_parent(infos, icon_name, scale_name)
        require_parent(infos, text_name, layer_name)

        if widgets[layer_name].get_class().get_name() != "CanvasPanel":
            raise RuntimeError(f"{layer_name}는 고정 슬롯 경계를 위한 CanvasPanel이어야 합니다.")

        frame_slot = slots[frame_name]
        if rounded_vector(frame_slot.get_position()) != expected_position:
            raise RuntimeError(f"{frame_name} 위치 불일치: {frame_slot.get_position()}")
        if rounded_vector(frame_slot.get_size()) != expected_size:
            raise RuntimeError(f"{frame_name} 크기 불일치: {frame_slot.get_size()}")

        label = widgets[text_name]
        if str(label.get_text()) != expected_text:
            raise RuntimeError(f"{text_name} 텍스트 불일치: {label.get_text()}")
        if label.get_editor_property("font").size != 8:
            raise RuntimeError(f"{text_name} 폰트는 8px여야 합니다.")
        icon_scale_slot = slots[scale_name]
        icon_anchors = icon_scale_slot.get_anchors()
        if rounded_vector(icon_anchors.minimum) != (0, 0) or rounded_vector(icon_anchors.maximum) != (1, 1):
            raise RuntimeError(f"{scale_name}는 슬롯 전체 Stretch Anchor여야 합니다.")
        if icon_scale_slot.get_z_order() != 0:
            raise RuntimeError(f"{scale_name}는 ZOrder 0이어야 합니다.")
        icon_offsets = icon_scale_slot.get_offsets()
        actual_icon_offsets = tuple(round(value) for value in (icon_offsets.left, icon_offsets.top, icon_offsets.right, icon_offsets.bottom))
        if actual_icon_offsets != (7, 7, 7, 7):
            raise RuntimeError(f"{scale_name} 사방 여백은 7px이어야 합니다: {actual_icon_offsets}")

        label_slot = slots[text_name]
        label_anchors = label_slot.get_anchors()
        if rounded_vector(label_anchors.minimum) != (1, 0) or rounded_vector(label_anchors.maximum) != (1, 0):
            raise RuntimeError(f"{text_name}는 우측 상단 Anchor여야 합니다.")
        if rounded_vector(label_slot.get_alignment()) != (1, 0):
            raise RuntimeError(f"{text_name}는 우측 상단 Alignment여야 합니다.")
        if rounded_vector(label_slot.get_position()) != (-4, 3):
            raise RuntimeError(f"{text_name} 우측 상단 오프셋은 (-4, 3)이어야 합니다.")
        if not label_slot.get_auto_size():
            raise RuntimeError(f"{text_name}는 AutoSize여야 합니다.")
        if label_slot.get_z_order() != 1:
            raise RuntimeError(f"{text_name}는 장비 이미지 위 ZOrder 1이어야 합니다.")


def main():
    layouts = load_layout_module()
    assets = {
        "inventory": layouts.require_asset(layouts.ASSETS["inventory"]),
        "equipment": layouts.require_asset(layouts.ASSETS["equipment"]),
    }
    layouts.build_inventory(assets["inventory"])
    layouts.build_equipment(assets["equipment"])

    for key, asset in assets.items():
        validate_window(layouts, key, asset)
        layouts.compile_and_save(asset, layouts.ASSETS[key])

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log(
        "[Rogue10mDraggableWindows] 두 창 DragHandle, 좌우 장비창, 7개 슬롯 계층·8px 라벨 검증 완료"
    )


if __name__ == "__main__":
    main()
