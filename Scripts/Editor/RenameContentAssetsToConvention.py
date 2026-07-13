"""Rename selected Rogue10m assets to the UE 5.8 recommended naming convention.

Run inside Unreal Editor. Existing destination assets cause a hard failure to avoid
overwriting content. The script is idempotent after a successful rename.
"""

import unreal


RENAMES = {
    "/Game/Widget/UW_Rogue10mMainWidget": "/Game/Widget/WBP_Rogue10mMainHUD",
    "/Game/FirstPerson/Anims/ABP_FP_Copy": "/Game/FirstPerson/Anims/ABP_FirstPerson",
    "/Game/FirstPerson/Anims/CtrlRig_FPWarp": "/Game/FirstPerson/Anims/Rig_FirstPersonWarp",
}

MAIN_HUD_SOURCE = "/Game/Widget/UW_Rogue10mMainWidget"
MAIN_HUD_DESTINATION = "/Game/Widget/WBP_Rogue10mMainHUD"
PLAYER_CONTROLLER_BP = "/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"


def log(message):
    unreal.log(f"[Rogue10mAssetNaming] {message}")


def rename_assets():
    rename_data = []
    for source_path, destination_path in RENAMES.items():
        source_exists = unreal.EditorAssetLibrary.does_asset_exist(source_path)
        destination_exists = unreal.EditorAssetLibrary.does_asset_exist(destination_path)

        if destination_exists:
            if source_exists:
                raise RuntimeError(
                    f"Both source and destination exist; rename aborted: "
                    f"{source_path} -> {destination_path}"
                )
            log(f"Already renamed: {destination_path}")
            continue

        if not source_exists:
            raise RuntimeError(f"Source asset is missing: {source_path}")

        asset = unreal.EditorAssetLibrary.load_asset(source_path)
        if not asset:
            raise RuntimeError(f"Source asset could not be loaded: {source_path}")

        package_path, new_name = destination_path.rsplit("/", 1)
        entry = unreal.AssetRenameData()
        entry.set_editor_property("asset", asset)
        entry.set_editor_property("new_package_path", package_path)
        entry.set_editor_property("new_name", new_name)
        rename_data.append(entry)

    if rename_data and not unreal.AssetToolsHelpers.get_asset_tools().rename_assets(rename_data):
        raise RuntimeError("AssetTools rename failed; see the Unreal Output Log for details.")

    for source_path, destination_path in RENAMES.items():
        if unreal.EditorAssetLibrary.does_asset_exist(destination_path):
            log(f"Ready: {source_path} -> {destination_path}")


def get_player_controller_cdos():
    native_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mPlayerController")
    blueprint_class = unreal.EditorAssetLibrary.load_blueprint_class(PLAYER_CONTROLLER_BP)
    classes = [value for value in (native_class, blueprint_class) if value]
    return [unreal.get_default_object(value) for value in classes]


def set_default_run_hud_class(controller_cdos, widget_class):
    for controller_cdo in controller_cdos:
        controller_cdo.set_editor_property("default_run_hud_class", widget_class)


def restore_default_run_hud_class(controller_cdos):
    resolved_path = (
        MAIN_HUD_DESTINATION
        if unreal.EditorAssetLibrary.does_asset_exist(MAIN_HUD_DESTINATION)
        else MAIN_HUD_SOURCE
    )
    widget_class = unreal.EditorAssetLibrary.load_blueprint_class(resolved_path)
    if not widget_class:
        raise RuntimeError(f"Main HUD class could not be restored: {resolved_path}")
    set_default_run_hud_class(controller_cdos, widget_class)

    controller_blueprint = unreal.EditorAssetLibrary.load_asset(PLAYER_CONTROLLER_BP)
    if controller_blueprint:
        unreal.BlueprintEditorLibrary.compile_blueprint(controller_blueprint)
        unreal.EditorAssetLibrary.save_loaded_asset(
            controller_blueprint, only_if_is_dirty=False
        )


def remove_legacy_first_person_sources():
    legacy_paths = [
        "/Game/FirstPerson/Anims/ABP_FP_Copy",
        "/Game/FirstPerson/Anims/CtrlRig_FPWarp",
    ]
    legacy_assets = [
        unreal.EditorAssetLibrary.load_asset(path)
        for path in legacy_paths
        if unreal.EditorAssetLibrary.does_asset_exist(path)
    ]
    if not legacy_assets:
        return

    asset_editor_subsystem = unreal.get_editor_subsystem(
        unreal.AssetEditorSubsystem
    )
    for asset in legacy_assets:
        asset_editor_subsystem.close_all_editors_for_asset(asset)

    editor_asset_subsystem = unreal.get_editor_subsystem(
        unreal.EditorAssetSubsystem
    )
    if not editor_asset_subsystem.delete_loaded_assets(legacy_assets):
        raise RuntimeError("Failed to remove legacy FirstPerson source assets.")
    log(f"Removed {len(legacy_assets)} legacy FirstPerson source asset(s).")


def fix_redirectors():
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    redirectors = []
    for root in ("/Game/Widget", "/Game/FirstPerson"):
        for asset_data in registry.get_assets_by_path(unreal.Name(root), recursive=True):
            if str(asset_data.asset_class_path.asset_name) == "ObjectRedirector":
                redirector = asset_data.get_asset()
                if redirector:
                    redirectors.append(redirector)

    if redirectors:
        unreal.AssetToolsHelpers.get_asset_tools().fixup_referencers(redirectors)
        log(f"Fixed {len(redirectors)} redirector(s).")


def main():
    controller_cdos = get_player_controller_cdos()
    set_default_run_hud_class(controller_cdos, None)
    try:
        rename_assets()
    finally:
        restore_default_run_hud_class(controller_cdos)

    remove_legacy_first_person_sources()
    unreal.EditorAssetLibrary.save_directory("/Game/Widget", only_if_is_dirty=False, recursive=True)
    unreal.EditorAssetLibrary.save_directory("/Game/FirstPerson", only_if_is_dirty=False, recursive=True)
    fix_redirectors()
    unreal.EditorAssetLibrary.save_directory("/Game/Widget", only_if_is_dirty=False, recursive=True)
    unreal.EditorAssetLibrary.save_directory("/Game/FirstPerson", only_if_is_dirty=False, recursive=True)
    log("Asset naming cleanup completed.")


if __name__ == "__main__":
    main()
