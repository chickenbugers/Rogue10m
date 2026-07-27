"""Configure and validate the default Adventurer unarmed spawn loadout in UE 5.8."""

import unreal


CHARACTER_DATA_PATH = "/Game/DataAsset/Character/DA_Character_Default"
UNARMED_PROFILE_PATH = "/Game/DataAsset/SkillProfile/DA_SkillProfile_Unarmed"
PRIMARY_ATTACK_PATH = "/Game/DataAsset/AttackSkill/Unarmed/DA_Attack_Unarmed_Primary"
CHARACTER_BLUEPRINT_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"


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


def main():
    character_data = require_asset(CHARACTER_DATA_PATH)
    unarmed_profile = require_asset(UNARMED_PROFILE_PATH)
    primary_attack = require_asset(PRIMARY_ATTACK_PATH)
    character_blueprint = require_asset(CHARACTER_BLUEPRINT_PATH)

    unarmed_weapon = enum_value(unreal.Rogue10mWeaponType, "UNARMED")
    primary_slot = enum_value(unreal.Rogue10mAttackInputSlot, "PRIMARY")

    character_data.set_editor_property("job_name", "모험가")
    character_data.set_editor_property("default_weapon_type", unarmed_weapon)

    profiles = [
        profile
        for profile in character_data.get_editor_property("weapon_skill_profiles")
        if profile and profile.get_path_name() != unarmed_profile.get_path_name()
    ]
    profiles.insert(0, unarmed_profile)
    character_data.set_editor_property("weapon_skill_profiles", profiles)

    unarmed_profile.set_editor_property("weapon_type", unarmed_weapon)
    bindings = dict(unarmed_profile.get_editor_property("default_skill_bindings"))
    bindings[primary_slot] = primary_attack
    unarmed_profile.set_editor_property("default_skill_bindings", bindings)

    unlocked = [
        skill
        for skill in unarmed_profile.get_editor_property("initially_unlocked_skills")
        if skill and skill.get_path_name() != primary_attack.get_path_name()
    ]
    unlocked.insert(0, primary_attack)
    unarmed_profile.set_editor_property("initially_unlocked_skills", unlocked)

    character_class = unreal.EditorAssetLibrary.load_blueprint_class(CHARACTER_BLUEPRINT_PATH)
    if not character_class:
        raise RuntimeError("BP_FirstPersonCharacter GeneratedClass를 찾을 수 없습니다.")
    character_cdo = unreal.get_default_object(character_class)
    combat_component = character_cdo.get_editor_property("combat_component")
    if not combat_component:
        raise RuntimeError("BP_FirstPersonCharacter CDO의 CombatComponent를 찾을 수 없습니다.")
    combat_component.set_editor_property("character_data", character_data)
    combat_component.set_editor_property("weapon_skill_profiles", profiles)

    unreal.BlueprintEditorLibrary.compile_blueprint(character_blueprint)
    for asset in (character_data, unarmed_profile, character_blueprint):
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)

    saved_job = character_data.get_editor_property("job_name")
    saved_weapon = character_data.get_editor_property("default_weapon_type")
    saved_bindings = dict(unarmed_profile.get_editor_property("default_skill_bindings"))
    saved_primary = saved_bindings.get(primary_slot)
    if str(saved_job) != "모험가":
        raise RuntimeError(f"기본 직업 검증 실패: {saved_job}")
    if saved_weapon != unarmed_weapon:
        raise RuntimeError(f"기본 무기 검증 실패: {saved_weapon}")
    if not saved_primary or saved_primary.get_path_name() != primary_attack.get_path_name():
        raise RuntimeError("좌클릭 Primary 주먹 공격 검증 실패")
    if combat_component.get_editor_property("character_data") != character_data:
        raise RuntimeError("Character Blueprint의 CharacterData 연결 검증 실패")

    unreal.log(
        "[Rogue10mAdventurer] 모험가 / Unarmed / 좌클릭 Primary 주먹 공격 설정 및 검증 완료"
    )


if __name__ == "__main__":
    main()
