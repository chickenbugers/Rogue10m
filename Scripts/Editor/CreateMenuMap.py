"""Create the UI-only menu map and assign the native menu GameMode."""

import unreal


MENU_MAP_PACKAGE = "/Game/Rogue10m/Maps/L_Menu"
MENU_GAME_MODE_CLASS = "/Script/Rogue10m.Rogue10mMenuGameMode"


def log(message):
    unreal.log(f"[Rogue10mMenuMap] {message}")


def main():
    menu_game_mode = unreal.load_class(None, MENU_GAME_MODE_CLASS)
    if not menu_game_mode:
        raise RuntimeError(
            "Rogue10mMenuGameMode is unavailable. Build and restart the editor first."
        )

    if unreal.EditorAssetLibrary.does_asset_exist(MENU_MAP_PACKAGE):
        if not unreal.EditorLevelLibrary.load_level(MENU_MAP_PACKAGE):
            raise RuntimeError(f"Failed to load existing map: {MENU_MAP_PACKAGE}")
    else:
        if not unreal.EditorLevelLibrary.new_level(MENU_MAP_PACKAGE):
            raise RuntimeError(f"Failed to create menu map: {MENU_MAP_PACKAGE}")

    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        raise RuntimeError("Editor world is unavailable.")

    world_settings = world.get_world_settings()
    world_settings.set_editor_property("default_game_mode", menu_game_mode)

    if not unreal.EditorLoadingAndSavingUtils.save_map(
        world, MENU_MAP_PACKAGE
    ):
        raise RuntimeError(f"Failed to save menu map: {MENU_MAP_PACKAGE}")

    log(
        f"Created menu map: {MENU_MAP_PACKAGE}, "
        f"GameMode={menu_game_mode.get_name()}"
    )


if __name__ == "__main__":
    main()
