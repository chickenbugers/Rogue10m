"""Validate Menu, Lobby, and reusable Component widget folder roles."""

import unreal


MAIN_MENU_PATH = "/Game/Widget/Menu/WBP_MainMenu"
LOBBY_PATH = "/Game/Widget/Lobby/WBP_CharacterLobby"
COMPONENT_ASSETS = (
    "/Game/Widget/Component/Inventory/WBP_InventoryWindow",
    "/Game/Widget/Component/Equipment/WBP_EquipmentWindow",
    "/Game/Widget/Component/SkillTree/WBP_SkillTreeWindow",
)
OLD_LOBBY_PATH = "/Game/Widget/Character/WBP_CharacterLobby"
MAIN_MENU_WIDGETS = {
    "UI_MainMenuRoot",
    "UI_StartGameButton",
    "UI_QuitGameButton",
}


def fail(message):
    raise RuntimeError(message)


def widget_names(path):
    widget = unreal.EditorAssetLibrary.load_asset(path)
    if not widget:
        fail(f"필수 Widget Blueprint 누락: {path}")
    tool = unreal.get_default_object(unreal.UMGToolSet)
    tree = tool.call_method("GetWidgets", (widget,))
    return {info.widget.get_name() for info in tree.widgets if info.widget}


def main():
    if unreal.EditorAssetLibrary.does_asset_exist(OLD_LOBBY_PATH):
        fail(f"이전 Lobby 경로가 남아 있습니다: {OLD_LOBBY_PATH}")
    if not unreal.EditorAssetLibrary.does_asset_exist(LOBBY_PATH):
        fail(f"새 Lobby 경로가 없습니다: {LOBBY_PATH}")

    for asset_path in COMPONENT_ASSETS:
        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            fail(f"Component 위젯 누락: {asset_path}")

    names = widget_names(MAIN_MENU_PATH)
    missing = sorted(MAIN_MENU_WIDGETS - names)
    if missing:
        fail(f"Main Menu 필수 위젯 누락: {missing}")

    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(
        MAIN_MENU_PATH
    )
    if not generated_class:
        fail("WBP_MainMenu GeneratedClass를 불러오지 못했습니다.")
    main_menu_cdo = unreal.get_default_object(generated_class)
    if not isinstance(main_menu_cdo, unreal.Rogue10mMainMenuWidget):
        fail("WBP_MainMenu가 Rogue10mMainMenuWidget을 상속하지 않습니다.")

    unreal.log(
        "[Rogue10mWidgetFlowValidation] PASSED: "
        "Menu -> Lobby -> StartMap flow assets + Component paths"
    )


if __name__ == "__main__":
    main()