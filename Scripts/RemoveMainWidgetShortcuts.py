import unreal


ASSET_PATH = "/Game/Widget/WBP_Rogue10mMainHUD"
WIDGET_NAMES = [
    "EquipmentShortcutWidget",
    "ItemWindowShortcutWidget",
    "SkillTreeShortcutWidget",
    "SettingsShortcutWidget",
    "Box_ShortcutHints",
    "Box_ShortcutHintItems",
]


def remove_widget_by_name(widget_tree, widget_name):
    widget = widget_tree.find_widget(widget_name)
    if not widget:
        unreal.log_warning(f"[Rogue10m] {widget_name} 위젯을 찾지 못했습니다.")
        return False

    widget_tree.remove_widget(widget)
    unreal.log(f"[Rogue10m] {widget_name} 위젯을 캐릭터 메인 위젯에서 제거했습니다.")
    return True


def main():
    asset = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    if not asset:
        unreal.log_error(f"[Rogue10m] 에셋을 찾지 못했습니다: {ASSET_PATH}")
        return

    widget_tree = None
    try:
        widget_tree = asset.get_editor_property("widget_tree")
    except Exception as error:
        unreal.log_warning(f"[Rogue10m] get_editor_property('widget_tree') 실패: {error}")

    if not widget_tree:
        widget_tree = getattr(asset, "widget_tree", None)

    if not widget_tree:
        unreal.log_error(f"[Rogue10m] WidgetTree를 찾지 못했습니다: {ASSET_PATH} / class={asset.get_class().get_name()}")
        return

    removed_count = 0
    for widget_name in WIDGET_NAMES:
        if remove_widget_by_name(widget_tree, widget_name):
            removed_count += 1

    if removed_count > 0:
        unreal.KismetEditorUtilities.compile_blueprint(asset)
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
        unreal.log(f"[Rogue10m] 캐릭터 메인 위젯 숏컷 정리 완료: {removed_count}개 제거")
    else:
        unreal.log_warning("[Rogue10m] 제거된 숏컷 위젯이 없습니다.")


main()
