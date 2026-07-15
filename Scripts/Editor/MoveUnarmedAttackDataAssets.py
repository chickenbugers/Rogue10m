import unreal


SOURCE_FOLDER = "/Game/DataAsset"
TARGET_FOLDER = "/Game/DataAsset/AttackSkill/Unarmed"

UNARMED_ATTACK_ASSETS = [
    "DA_Attack_Unarmed_ChargedPrimary",
    "DA_Attack_Unarmed_ChargedSpecial",
    "DA_Attack_Unarmed_JumpPrimary",
    "DA_Attack_Unarmed_JumpSpecial",
    "DA_Attack_Unarmed_Primary",
    "DA_Attack_Unarmed_Primary_Combo2",
    "DA_Attack_Unarmed_Primary_Combo3",
    "DA_Attack_Unarmed_Special",
]


def ensure_folder(folder_path):
    if not unreal.EditorAssetLibrary.does_directory_exist(folder_path):
        unreal.EditorAssetLibrary.make_directory(folder_path)


def move_asset(asset_tools, asset_name):
    source_path = f"{SOURCE_FOLDER}/{asset_name}"
    target_path = f"{TARGET_FOLDER}/{asset_name}"

    if unreal.EditorAssetLibrary.does_asset_exist(target_path):
        if unreal.EditorAssetLibrary.does_asset_exist(source_path):
            if unreal.EditorAssetLibrary.delete_asset(source_path):
                unreal.log(f"Removed leftover source after move: {source_path}")
            else:
                unreal.log_warning(f"Target exists but leftover source could not be removed: {source_path}")
        unreal.log(f"Already moved: {target_path}")
        return

    if not unreal.EditorAssetLibrary.does_asset_exist(source_path):
        unreal.log_warning(f"Missing source asset: {source_path}")
        return

    rename_data = unreal.AssetRenameData(
        unreal.EditorAssetLibrary.load_asset(source_path),
        TARGET_FOLDER,
        asset_name,
    )
    b_success = asset_tools.rename_assets([rename_data])
    if not b_success:
        b_success = unreal.EditorAssetLibrary.rename_asset(source_path, target_path)

    if not b_success:
        raise RuntimeError(f"Failed to move {source_path} to {target_path}")

    unreal.log(f"Moved: {source_path} -> {target_path}")


def main():
    ensure_folder(TARGET_FOLDER)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    for asset_name in UNARMED_ATTACK_ASSETS:
        move_asset(asset_tools, asset_name)

    unreal.EditorAssetLibrary.fixup_redirectors(SOURCE_FOLDER)
    unreal.EditorAssetLibrary.save_directory(TARGET_FOLDER, only_if_is_dirty=False, recursive=True)
    unreal.EditorAssetLibrary.save_directory(SOURCE_FOLDER, only_if_is_dirty=True, recursive=False)
    unreal.log("Unarmed attack Data Assets moved to AttackSkill/Unarmed.")


main()
