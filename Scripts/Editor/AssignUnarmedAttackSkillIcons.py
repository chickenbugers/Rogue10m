import unreal


DATA_ASSET_FOLDER = "/Game/DataAsset/AttackSkill/Unarmed"
STONE_FIST_ICON = "/Game/UI/Icons/T_Identity_StoneFist.T_Identity_StoneFist"
UNARMED_ATTACK_ASSETS = [
    "DA_Attack_Unarmed_Primary",
    "DA_Attack_Unarmed_Primary_Combo2",
    "DA_Attack_Unarmed_Primary_Combo3",
    "DA_Attack_Unarmed_Special",
    "DA_Attack_Unarmed_JumpPrimary",
    "DA_Attack_Unarmed_JumpSpecial",
    "DA_Attack_Unarmed_ChargedPrimary",
    "DA_Attack_Unarmed_ChargedSpecial",
]


asset_library = unreal.EditorAssetLibrary
icon_texture = asset_library.load_asset(STONE_FIST_ICON)
if icon_texture is None:
    unreal.log_error(f"Missing icon asset: {STONE_FIST_ICON}")
else:
    changed_count = 0
    for asset_name in UNARMED_ATTACK_ASSETS:
        asset_path = f"{DATA_ASSET_FOLDER}/{asset_name}.{asset_name}"
        asset = asset_library.load_asset(asset_path)
        if asset is None:
            unreal.log_warning(f"Missing attack skill asset: {asset_path}")
            continue

        try:
            asset.set_editor_property("skill_icon", icon_texture)
            asset_library.save_loaded_asset(asset)
            changed_count += 1
            unreal.log(f"Assigned unarmed skill icon: {asset_path}")
        except Exception as error:
            unreal.log_warning(f"Skipped unarmed skill icon: {asset_path} / {error}")

    unreal.log(f"Assigned unarmed attack skill icons: {changed_count}")
