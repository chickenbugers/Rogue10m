"""Rebuild and validate only the equipment window Designer tree."""

from __future__ import annotations

import importlib.util
from pathlib import Path

import unreal


LAYOUT_SCRIPT = Path(r"D:\Project\Rogue10m\Scripts\Editor\BuildMenuDesignerLayouts.py")
EXPECTED_SLOT_LAYOUT = {
    "UI_HeadSlotFrame": ((-115, -180), (100, 100)),
    "UI_NecklaceSlotFrame": ((0, -180), (100, 100)),
    "UI_ChestSlotFrame": ((-115, 0), (100, 150)),
    "UI_HandsSlotFrame": ((0, 0), (100, 100)),
    "UI_WeaponSlotFrame": ((120, 0), (100, 200)),
    "UI_FeetSlotFrame": ((-115, 180), (100, 100)),
    "UI_RingSlotFrame": ((0, 180), (100, 100)),
}
EXPECTED_PANEL_LAYOUT = {
    "UI_EquipmentStatsFrame": ((-375, -5), (200, 500)),
    "UI_CharacterPreviewFrame": ((-75, -5), (340, 500)),
    "UI_EquipmentSlotContainer": ((290, -5), (350, 500)),
}


def load_layout_module():
    spec = importlib.util.spec_from_file_location("rogue10m_menu_layouts", LAYOUT_SCRIPT)
    if not spec or not spec.loader:
        raise RuntimeError(f"레이아웃 모듈을 불러올 수 없습니다: {LAYOUT_SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def validate_slot_layout(layouts, widget_blueprint) -> None:
    tree = layouts.umg("GetWidgets", widget_blueprint)
    slots = {
        info.widget.get_name(): info.slot
        for info in tree.widgets
        if info.widget and info.slot
    }
    for panel_name, expected_layout in EXPECTED_PANEL_LAYOUT.items():
        panel_slot = slots.get(panel_name)
        if not panel_slot:
            raise RuntimeError(f"패널 CanvasSlot을 찾을 수 없습니다: {panel_name}")
        panel_position = panel_slot.get_position()
        panel_size = panel_slot.get_size()
        actual_layout = (
            (round(panel_position.x), round(panel_position.y)),
            (round(panel_size.x), round(panel_size.y)),
        )
        if actual_layout != expected_layout:
            raise RuntimeError(
                f"장비창 패널 배치 불일치: {panel_name}: {actual_layout}"
            )
    for widget_name, (expected_position, expected_size) in EXPECTED_SLOT_LAYOUT.items():
        slot = slots.get(widget_name)
        if not slot or not hasattr(slot, "get_position") or not hasattr(slot, "get_size"):
            raise RuntimeError(f"CanvasSlot을 찾을 수 없습니다: {widget_name}")
        position = slot.get_position()
        size = slot.get_size()
        actual_position = (round(position.x), round(position.y))
        actual_size = (round(size.x), round(size.y))
        if actual_position != expected_position or actual_size != expected_size:
            raise RuntimeError(
                f"장비 슬롯 배치 불일치: {widget_name}: "
                f"position={actual_position}, size={actual_size}"
            )
    unreal.log(
        f"[Rogue10mEquipmentLayout] 7개 슬롯 위치·크기 검증 완료: {EXPECTED_SLOT_LAYOUT}"
    )


def main() -> None:
    layouts = load_layout_module()
    path = layouts.ASSETS["equipment"]
    widget_blueprint = layouts.require_asset(path)
    layouts.build_equipment(widget_blueprint)
    layouts.validate_tree(widget_blueprint, "equipment", path)
    validate_slot_layout(layouts, widget_blueprint)
    layouts.compile_and_save(widget_blueprint, path)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("[Rogue10mEquipmentLayout] 장비창 7부위 레이아웃 재생성 완료")


if __name__ == "__main__":
    main()
