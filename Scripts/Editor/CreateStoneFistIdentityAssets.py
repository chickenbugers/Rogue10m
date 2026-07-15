"""Import Stone Fist icons and create/configure its gameplay Data Assets.

Run in the open Unreal Editor after rebuilding the Rogue10mEditor target.
The script is idempotent and only creates or updates Stone Fist assets.
"""

from pathlib import Path

import unreal


PROJECT_ROOT = Path(unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir()))
ICON_SOURCE_DIR = PROJECT_ROOT / "Content" / "UI" / "Icons" / "StoneFist"
ICON_DESTINATION = "/Game/UI/Icons/StoneFist"
IDENTITY_ICON_PATH = "/Game/UI/Icons/T_Identity_StoneFist"
ATTACK_ROOT = "/Game/DataAsset/AttackSkill/StoneFist"
DODGE_PATH = "/Game/DataAsset/DodgeSkill/DA_Dodge_StoneFist"
PROFILE_PATH = "/Game/DataAsset/SkillProfile/DA_SkillProfile_StoneFist"
CHARACTER_DATA_PATH = "/Game/DataAsset/Character/DA_Character_Default"
CHARACTER_BP_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"

ICON_FILES = {
    "jab": "T_Skill_StoneFist_Jab.png",
    "straight": "T_Skill_StoneFist_Straight.png",
    "charged_shockwave": "T_Skill_StoneFist_ChargedShockwave.png",
    "dodge": "T_Skill_StoneFist_Dodge.png",
    "jump_slam": "T_Skill_StoneFist_JumpSlam.png",
    "double_jump": "T_Skill_StoneFist_DoubleJump.png",
}


def log(message):
    unreal.log(f"[StoneFistSetup] {message}")


def enum_value(enum_type, name):
    for candidate in (name, name.upper(), name.title().replace("_", "")):
        if hasattr(enum_type, candidate):
            return getattr(enum_type, candidate)
    raise RuntimeError(f"Enum value not found: {enum_type}.{name}")


def require_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"Required asset not found: {path}")
    return asset


def create_or_load_data_asset(asset_path, data_asset_class):
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset:
        return asset

    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", data_asset_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, data_asset_class, factory
    )
    if not asset:
        raise RuntimeError(f"Failed to create Data Asset: {asset_path}")
    log(f"Created {asset_path}")
    return asset


def import_icons():
    import_data = unreal.AutomatedAssetImportData()
    import_data.set_editor_property("destination_path", ICON_DESTINATION)
    import_data.set_editor_property("replace_existing", True)
    import_data.set_editor_property("filenames", [
        str(ICON_SOURCE_DIR / file_name) for file_name in ICON_FILES.values()
    ])
    unreal.AssetToolsHelpers.get_asset_tools().import_assets_automated(import_data)

    icons = {}
    for key, file_name in ICON_FILES.items():
        asset_path = f"{ICON_DESTINATION}/{Path(file_name).stem}"
        texture = require_asset(asset_path)
        try:
            texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
            texture.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
            texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_EDITOR_ICON)
            texture.set_editor_property("srgb", True)

        except Exception as error:
            unreal.log_warning(f"[StoneFistSetup] Texture tuning skipped for {asset_path}: {error}")
        unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)
        icons[key] = texture
    return icons


def stamina_cost(amount):
    cost = unreal.Rogue10mAttackResourceCost()
    cost.set_editor_property(
        "resource_type", enum_value(unreal.Rogue10mAttackResourceType, "STAMINA")
    )
    cost.set_editor_property("cost", amount)
    return [cost]


def set_attack(asset, **properties):
    for name, value in properties.items():
        asset.set_editor_property(name, value)
    unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)


def create_attacks(icons):
    attack_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mAttackSkillData")
    ability_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mGameplayAbility_Attack")
    if not attack_class or not ability_class:
        raise RuntimeError("Stone Fist C++ attack classes are not loaded. Restart the Editor after building.")

    input_slot = unreal.Rogue10mAttackInputSlot
    shape = unreal.Rogue10mAttackShape
    hit_mode = unreal.Rogue10mAttackHitMode

    jab = create_or_load_data_asset(f"{ATTACK_ROOT}/DA_Attack_StoneFist_Jab", attack_class)
    straight = create_or_load_data_asset(f"{ATTACK_ROOT}/DA_Attack_StoneFist_Straight", attack_class)
    charged = create_or_load_data_asset(
        f"{ATTACK_ROOT}/DA_Attack_StoneFist_ChargedShockwave", attack_class
    )
    jump_slam = create_or_load_data_asset(
        f"{ATTACK_ROOT}/DA_Attack_StoneFist_JumpSlam", attack_class
    )

    set_attack(
        straight,
        skill_name="오른손 스트레이트",
        skill_description="왼손 잽에서 이어지는 2타 콤보. 오른손을 곧게 뻗어 단일 대상에게 강한 충격을 준다.",
        input_slot=enum_value(input_slot, "PRIMARY"),
        gameplay_ability_class=ability_class,
        damage=14.0,
        attack_shape=enum_value(shape, "LINEAR_BOX"),
        hit_mode=enum_value(hit_mode, "SINGLE"),
        attack_range=165.0,
        box_half_width=42.0,
        box_half_height=55.0,
        attack_cooldown=0.38,
        resource_costs=[],
        enable_combo=False,
        skill_icon=icons["straight"],
        icon_label="2타",
        draw_debug_attack=True,
    )
    set_attack(
        jab,
        skill_name="왼손 잽",
        skill_description="권의 좌클릭 1타. 빠른 왼손 잽 후 입력 창 안에 좌클릭하면 오른손 스트레이트로 연계한다.",
        input_slot=enum_value(input_slot, "PRIMARY"),
        gameplay_ability_class=ability_class,
        damage=8.0,
        attack_shape=enum_value(shape, "LINEAR_BOX"),
        hit_mode=enum_value(hit_mode, "SINGLE"),
        attack_range=140.0,
        box_half_width=38.0,
        box_half_height=52.0,
        attack_cooldown=0.22,
        resource_costs=[],
        enable_combo=True,
        combo_window_open_seconds=0.10,
        combo_window_close_seconds=0.42,
        combo_input_slot=enum_value(input_slot, "PRIMARY"),
        next_combo_skill=straight,
        skill_icon=icons["jab"],
        icon_label="잽",
        draw_debug_attack=True,
    )
    set_attack(
        charged,
        skill_name="권압",
        skill_description="주먹을 뒤로 당겨 기를 모은 뒤 전방으로 내질러 풍압과 함께 넓은 직선 범위에 피해를 준다. Niagara와 전용 Ability 연결 전에는 범위 판정으로 동작한다.",
        input_slot=enum_value(input_slot, "CHARGED_SPECIAL"),
        gameplay_ability_class=ability_class,
        damage=30.0,
        attack_shape=enum_value(shape, "LINEAR_BOX"),
        hit_mode=enum_value(hit_mode, "SINGLE"),
        attack_range=600.0,
        box_half_width=100.0,
        box_half_height=90.0,
        max_targets_per_hit=8,
        attack_cooldown=4.0,
        charge_seconds=0.8,
        resource_costs=stamina_cost(12.0),
        enable_combo=False,
        skill_icon=icons["charged_shockwave"],
        icon_label="권압",
        draw_debug_attack=True,
    )
    set_attack(
        jump_slam,
        skill_name="낙권",
        skill_description="공중에서 양손을 모아 아래로 내려찍어 착지 지점 주변의 적에게 원형 범위 피해를 준다.",
        input_slot=enum_value(input_slot, "JUMP_PRIMARY"),
        gameplay_ability_class=ability_class,
        damage=24.0,
        attack_shape=enum_value(shape, "CIRCLE"),
        hit_mode=enum_value(hit_mode, "SINGLE"),
        attack_range=220.0,
        circle_forward_offset=30.0,
        attack_cooldown=1.2,
        resource_costs=stamina_cost(8.0),
        enable_combo=False,
        skill_icon=icons["jump_slam"],
        icon_label="낙권",
        draw_debug_attack=True,
    )
    return {"jab": jab, "straight": straight, "charged": charged, "jump_slam": jump_slam}


def configure_profile(icons, skills):
    dodge_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mDodgeSkillDataAsset")
    profile_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mWeaponSkillProfileDataAsset")
    character_data_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mCharacterDataAsset")
    if not dodge_class or not profile_class or not character_data_class:
        raise RuntimeError("Stone Fist profile C++ classes are not loaded. Restart the Editor after building.")

    dodge = create_or_load_data_asset(DODGE_PATH, dodge_class)
    dodge.set_editor_property("dodge_id", "StoneFistDodge")
    dodge.set_editor_property("display_name", "권보")
    dodge.set_editor_property("description", "입력 방향으로 짧은 거리를 빠르게 이동한다. 공중에서는 사용할 수 없다.")
    dodge.set_editor_property("icon", icons["dodge"])
    dodge.set_editor_property("distance", 250.0)
    dodge.set_editor_property("duration", 0.16)
    dodge.set_editor_property("cooldown", 0.55)
    dodge.set_editor_property("stamina_cost", 6.0)

    slot = unreal.Rogue10mAttackInputSlot
    bindings = {
        enum_value(slot, "PRIMARY"): skills["jab"],
        enum_value(slot, "CHARGED_SPECIAL"): skills["charged"],
        enum_value(slot, "JUMP_PRIMARY"): skills["jump_slam"],
    }

    profile = create_or_load_data_asset(PROFILE_PATH, profile_class)
    profile.set_editor_property("profile_id", "StoneFist")
    profile.set_editor_property("display_name", "권")
    profile.set_editor_property("description", "주먹과 기를 사용해 빠른 2타 콤보, 전방 권압, 공중 내려찍기를 구사하는 근접 아이덴티티.")
    profile.set_editor_property("identity_icon", require_asset(IDENTITY_ICON_PATH))
    profile.set_editor_property("weapon_type", enum_value(unreal.Rogue10mWeaponType, "KNUCKLE"))
    profile.set_editor_property("default_skill_bindings", bindings)
    profile.set_editor_property("skill_tree_skills", [skills["jab"], skills["charged"], skills["jump_slam"]])
    profile.set_editor_property("initially_unlocked_skills", [skills["jab"], skills["charged"], skills["jump_slam"]])
    profile.set_editor_property("default_dodge_skill", dodge)
    profile.set_editor_property("max_jump_count", 2)
    profile.set_editor_property("extra_jump_display_name", "답공")
    profile.set_editor_property("extra_jump_description", "공중에서 한 번 더 도약한다. 착지하면 사용 횟수가 초기화된다.")
    profile.set_editor_property("extra_jump_icon", icons["double_jump"])

    character_data = create_or_load_data_asset(CHARACTER_DATA_PATH, character_data_class)
    character_data.set_editor_property("character_id", "DefaultCharacter")
    character_data.set_editor_property("display_name", "기본 캐릭터")
    character_data.set_editor_property("max_health", 100.0)
    character_data.set_editor_property("max_stamina", 100.0)
    character_data.set_editor_property("max_mana", 100.0)
    character_data.set_editor_property("walk_speed", 600.0)
    character_data.set_editor_property("sprint_speed", 900.0)
    character_data.set_editor_property("default_weapon_type", enum_value(unreal.Rogue10mWeaponType, "KNUCKLE"))
    profiles = list(character_data.get_editor_property("weapon_skill_profiles"))
    profiles = [entry for entry in profiles if entry and entry.get_path_name() != profile.get_path_name()]
    profiles.append(profile)
    character_data.set_editor_property("weapon_skill_profiles", profiles)

    character_bp = require_asset(CHARACTER_BP_PATH)
    character_class = unreal.EditorAssetLibrary.load_blueprint_class(CHARACTER_BP_PATH)
    character_cdo = unreal.get_default_object(character_class)

    combat_component = character_cdo.get_editor_property("combat_component")
    combat_component.set_editor_property("character_data", character_data)
    combat_component.set_editor_property("weapon_skill_profiles", profiles)
    unreal.BlueprintEditorLibrary.compile_blueprint(character_bp)

    for asset in [dodge, profile, character_data, character_bp]:
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
    return profile


def main():
    log("Starting Stone Fist identity setup")
    icons = import_icons()
    skills = create_attacks(icons)
    profile = configure_profile(icons, skills)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log(f"Completed: {profile.get_path_name()}")


if __name__ == "__main__":
    main()
