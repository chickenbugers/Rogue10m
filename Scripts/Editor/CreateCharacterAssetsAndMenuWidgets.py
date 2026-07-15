"""Create the starter character loadout assets and menu Widget Blueprints in the open UE editor.

Run through Unreal Editor Python after the Rogue10m Editor target has been rebuilt.
The script is idempotent: existing assets are updated instead of duplicated.
"""

import unreal


DODGE_PATH = "/Game/DataAsset/DodgeSkill/DA_Dodge_Unarmed"
PROFILE_PATH = "/Game/DataAsset/SkillProfile/DA_SkillProfile_Unarmed"
CHARACTER_DATA_PATH = "/Game/DataAsset/Character/DA_Character_Default"
CHARACTER_BP_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"
PLAYER_CONTROLLER_BP_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"
WIDGET_ROOT = "/Game/Widget/Menu"

ATTACK_ASSETS = {
    "PRIMARY": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_Primary",
    "SPECIAL": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_Special",
    "JUMP_PRIMARY": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_JumpPrimary",
    "JUMP_SPECIAL": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_JumpSpecial",
    "CHARGED_PRIMARY": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_ChargedPrimary",
    "CHARGED_SPECIAL": "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_ChargedSpecial",
}

WIDGETS = {
    "Inventory": ("/Game/Widget/Menu/Inventory/WBP_InventoryWindow", "/Script/Rogue10m.Rogue10mInventoryWindowWidget"),
    "Equipment": ("/Game/Widget/Menu/Equipment/WBP_EquipmentWindow", "/Script/Rogue10m.Rogue10mEquipmentWindowWidget"),
    "SkillTreeEntry": ("/Game/Widget/Menu/SkillTree/WBP_SkillTreeEntry", "/Script/Rogue10m.Rogue10mSkillTreeEntryWidget"),
    "SkillTree": ("/Game/Widget/Menu/SkillTree/WBP_SkillTreeWindow", "/Script/Rogue10m.Rogue10mSkillTreeWindowWidget"),
}


def log(message):
    unreal.log(f"[Rogue10mSetup] {message}")


def require_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"필수 에셋을 찾을 수 없습니다: {path}")
    return asset


def enum_value(enum_type, name):
    for candidate in (name, name.upper(), name.title().replace("_", "")):
        if hasattr(enum_type, candidate):
            return getattr(enum_type, candidate)
    raise RuntimeError(f"enum 값을 찾을 수 없습니다: {enum_type}.{name}")


def create_or_load_data_asset(asset_path, data_asset_class):
    existing = unreal.EditorAssetLibrary.load_asset(asset_path)
    if existing:
        return existing

    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", data_asset_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, data_asset_class, factory
    )
    if not asset:
        raise RuntimeError(f"Data Asset 생성 실패: {asset_path}")
    log(f"생성: {asset_path}")
    return asset


def create_or_load_widget_blueprint(asset_name, parent_class_path):
    asset_path = asset_name
    package_path, short_name = asset_path.rsplit("/", 1)
    existing = unreal.EditorAssetLibrary.load_asset(asset_path)
    if existing:
        return existing

    parent_class = unreal.load_class(None, parent_class_path)
    if not parent_class:
        raise RuntimeError(f"부모 클래스를 찾을 수 없습니다: {parent_class_path}")

    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    widget_blueprint = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        short_name, package_path, unreal.WidgetBlueprint, factory
    )
    if not widget_blueprint:
        raise RuntimeError(f"Widget Blueprint 생성 실패: {asset_path}")
    unreal.BlueprintEditorLibrary.compile_blueprint(widget_blueprint)
    log(f"생성: {asset_path}")
    return widget_blueprint


def generated_class(asset_path):
    cls = unreal.EditorAssetLibrary.load_blueprint_class(asset_path)
    if not cls:
        raise RuntimeError(f"Blueprint GeneratedClass 로드 실패: {asset_path}")
    return cls


def configure_character_assets():
    dodge_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mDodgeSkillDataAsset")
    profile_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mWeaponSkillProfileDataAsset")
    character_data_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mCharacterDataAsset")
    if not dodge_class or not profile_class or not character_data_class:
        raise RuntimeError("Rogue10m Data Asset C++ 클래스가 로드되지 않았습니다. Editor 빌드를 먼저 완료하세요.")

    dodge = create_or_load_data_asset(DODGE_PATH, dodge_class)
    dodge.set_editor_property("dodge_id", "UnarmedDodge")
    dodge.set_editor_property("display_name", "기본 회피")
    dodge.set_editor_property("distance", 100.0)
    dodge.set_editor_property("duration", 0.16)
    dodge.set_editor_property("cooldown", 0.45)
    dodge.set_editor_property("stamina_cost", 10.0)

    skills = {name: require_asset(path) for name, path in ATTACK_ASSETS.items()}
    input_enum = unreal.Rogue10mAttackInputSlot
    bindings = {enum_value(input_enum, name): skill for name, skill in skills.items()}

    profile = create_or_load_data_asset(PROFILE_PATH, profile_class)
    profile.set_editor_property("profile_id", "Unarmed")
    profile.set_editor_property("weapon_type", enum_value(unreal.Rogue10mWeaponType, "UNARMED"))
    profile.set_editor_property("default_skill_bindings", bindings)
    profile.set_editor_property("skill_tree_skills", list(skills.values()))
    profile.set_editor_property("initially_unlocked_skills", list(skills.values()))
    profile.set_editor_property("default_dodge_skill", dodge)

    character_data = create_or_load_data_asset(CHARACTER_DATA_PATH, character_data_class)
    character_data.set_editor_property("character_id", "DefaultCharacter")
    character_data.set_editor_property("display_name", "기본 캐릭터")
    character_data.set_editor_property("max_health", 100.0)
    character_data.set_editor_property("max_stamina", 100.0)
    character_data.set_editor_property("max_mana", 100.0)
    character_data.set_editor_property("walk_speed", 600.0)
    character_data.set_editor_property("sprint_speed", 900.0)
    character_data.set_editor_property("weapon_skill_profiles", [profile])

    character_bp = require_asset(CHARACTER_BP_PATH)
    character_cdo = unreal.get_default_object(generated_class(CHARACTER_BP_PATH))
    combat_component = character_cdo.get_editor_property("combat_component")
    if not combat_component:
        raise RuntimeError("BP_FirstPersonCharacter CDO에서 CombatComponent를 찾을 수 없습니다.")
    combat_component.set_editor_property("character_data", character_data)
    combat_component.set_editor_property("weapon_skill_profiles", [profile])
    unreal.BlueprintEditorLibrary.compile_blueprint(character_bp)

    unreal.EditorAssetLibrary.save_loaded_asset(dodge, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_loaded_asset(profile, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_loaded_asset(character_data, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_loaded_asset(character_bp, only_if_is_dirty=False)
    log("기본 Character/맨손 스킬 프로필/회피 Data Asset을 CombatComponent에 연결했습니다.")


def configure_menu_widgets():
    created = {}
    for key, (asset_name, parent_path) in WIDGETS.items():
        widget_bp = create_or_load_widget_blueprint(asset_name, parent_path)
        unreal.BlueprintEditorLibrary.compile_blueprint(widget_bp)
        created[key] = asset_name
        unreal.EditorAssetLibrary.save_asset(created[key], only_if_is_dirty=False)

    skill_tree_cdo = unreal.get_default_object(generated_class(created["SkillTree"]))
    skill_tree_cdo.set_editor_property(
        "skill_tree_entry_widget_class", generated_class(created["SkillTreeEntry"])
    )

    controller_bp = require_asset(PLAYER_CONTROLLER_BP_PATH)
    unreal.BlueprintEditorLibrary.compile_blueprint(controller_bp)
    controller_cdo = unreal.get_default_object(generated_class(PLAYER_CONTROLLER_BP_PATH))
    controller_cdo.set_editor_property("inventory_window_widget_class", generated_class(created["Inventory"]))
    controller_cdo.set_editor_property("equipment_window_widget_class", generated_class(created["Equipment"]))
    controller_cdo.set_editor_property("skill_tree_window_widget_class", generated_class(created["SkillTree"]))
    unreal.BlueprintEditorLibrary.compile_blueprint(controller_bp)

    for path in created.values():
        unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_loaded_asset(controller_bp, only_if_is_dirty=False)
    log("인벤토리, 장비창, 스킬 트리 Widget Blueprint를 PlayerController에 연결했습니다.")


def main():
    log("캐릭터 Data Asset 및 메뉴 Widget 설정을 시작합니다.")
    configure_character_assets()
    configure_menu_widgets()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log("설정 완료")


if __name__ == "__main__":
    main()