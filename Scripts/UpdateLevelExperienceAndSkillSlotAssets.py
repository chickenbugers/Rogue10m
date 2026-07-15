import unreal


SKILL_PANEL_PATH = "/Game/Widget/Parts/WBP_SkillSlotPanel"
OLD_LEVEL_PANEL_PATH = "/Game/Widget/Parts/WBP_Progression"
NEW_LEVEL_PANEL_PATH = "/Game/Widget/Parts/WBP_LevelExperiencePanel"
MAIN_HUD_PATH = "/Game/Widget/WBP_Rogue10mMainHUD"


def save_asset(asset_path: str) -> None:
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset is None:
        raise RuntimeError(f"Asset not found: {asset_path}")
    if not unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
        raise RuntimeError(f"Failed to save asset: {asset_path}")


skill_panel = unreal.EditorAssetLibrary.load_asset(SKILL_PANEL_PATH)
panel_parent = unreal.load_class(None, "/Script/Rogue10m.Rogue10mSkillSlotPanelWidget")
if skill_panel is None or panel_parent is None:
    raise RuntimeError("Skill slot panel or native parent class could not be loaded")

unreal.BlueprintEditorLibrary.reparent_blueprint(skill_panel, panel_parent)
if not unreal.EditorAssetLibrary.save_loaded_asset(skill_panel, only_if_is_dirty=False):
    raise RuntimeError("Failed to save reparented skill slot panel")

if unreal.EditorAssetLibrary.does_asset_exist(OLD_LEVEL_PANEL_PATH):
    if unreal.EditorAssetLibrary.does_asset_exist(NEW_LEVEL_PANEL_PATH):
        raise RuntimeError(f"Rename target already exists: {NEW_LEVEL_PANEL_PATH}")
    if not unreal.EditorAssetLibrary.rename_asset(OLD_LEVEL_PANEL_PATH, NEW_LEVEL_PANEL_PATH):
        raise RuntimeError("Failed to rename the level and experience panel")

save_asset(NEW_LEVEL_PANEL_PATH)
save_asset(MAIN_HUD_PATH)
unreal.log("Level/experience and skill slot assets updated successfully")
