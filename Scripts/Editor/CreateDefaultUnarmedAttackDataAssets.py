import unreal


ASSET_PATH = "/Game/DataAsset"
SOURCE_PRIMARY = f"{ASSET_PATH}/DA_Attack_Unarmed_Primary"


DEFAULT_UNARMED_ATTACKS = [
    {
        "name": "DA_Attack_Unarmed_Special",
        "slot": unreal.Rogue10mAttackInputSlot.SPECIAL,
        "skill_name": "주먹 특수 공격",
        "description": "맨손 상태에서 사용하는 강한 주먹 공격입니다.",
        "damage_multiplier": 1.35,
        "cooldown_multiplier": 1.25,
        "charge_seconds": 0.0,
        "icon_label": "특",
        "icon_tint": unreal.LinearColor(0.62, 0.82, 1.0, 1.0),
        "debug_color": unreal.LinearColor(0.62, 0.82, 1.0, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_Primary_Combo2",
        "slot": unreal.Rogue10mAttackInputSlot.PRIMARY,
        "skill_name": "주먹 연속 공격 2",
        "description": "좌클릭 기본 공격 뒤 이어지는 두 번째 주먹 콤보입니다.",
        "damage_multiplier": 1.25,
        "cooldown_multiplier": 0.75,
        "charge_seconds": 0.0,
        "icon_label": "연",
        "icon_tint": unreal.LinearColor(1.0, 0.68, 0.28, 1.0),
        "debug_color": unreal.LinearColor(1.0, 0.62, 0.28, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_Primary_Combo3",
        "slot": unreal.Rogue10mAttackInputSlot.PRIMARY,
        "skill_name": "주먹 연속 공격 3",
        "description": "좌클릭 기본 공격 콤보의 세 번째 마무리 공격입니다.",
        "damage_multiplier": 1.65,
        "cooldown_multiplier": 1.0,
        "charge_seconds": 0.0,
        "icon_label": "3",
        "icon_tint": unreal.LinearColor(1.0, 0.42, 0.24, 1.0),
        "debug_color": unreal.LinearColor(1.0, 0.35, 0.22, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_JumpPrimary",
        "slot": unreal.Rogue10mAttackInputSlot.JUMP_PRIMARY,
        "skill_name": "점프 주먹 공격",
        "description": "점프 중 좌클릭으로 사용하는 빠른 주먹 공격입니다.",
        "damage_multiplier": 1.1,
        "cooldown_multiplier": 1.1,
        "charge_seconds": 0.0,
        "icon_label": "도",
        "icon_tint": unreal.LinearColor(1.0, 0.82, 0.34, 1.0),
        "debug_color": unreal.LinearColor(1.0, 0.86, 0.42, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_JumpSpecial",
        "slot": unreal.Rogue10mAttackInputSlot.JUMP_SPECIAL,
        "skill_name": "점프 주먹 특수 공격",
        "description": "점프 중 우클릭으로 사용하는 강한 주먹 공격입니다.",
        "damage_multiplier": 1.55,
        "cooldown_multiplier": 1.45,
        "charge_seconds": 0.0,
        "icon_label": "공",
        "icon_tint": unreal.LinearColor(0.72, 0.54, 1.0, 1.0),
        "debug_color": unreal.LinearColor(0.72, 0.54, 1.0, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_ChargedPrimary",
        "slot": unreal.Rogue10mAttackInputSlot.CHARGED_PRIMARY,
        "skill_name": "차징 주먹 공격",
        "description": "좌클릭을 길게 눌러 사용하는 차징 주먹 공격입니다.",
        "damage_multiplier": 1.8,
        "cooldown_multiplier": 1.6,
        "charge_seconds": 0.65,
        "icon_label": "차",
        "icon_tint": unreal.LinearColor(1.0, 0.48, 0.28, 1.0),
        "debug_color": unreal.LinearColor(1.0, 0.48, 0.28, 1.0),
    },
    {
        "name": "DA_Attack_Unarmed_ChargedSpecial",
        "slot": unreal.Rogue10mAttackInputSlot.CHARGED_SPECIAL,
        "skill_name": "차징 주먹 특수 공격",
        "description": "우클릭을 길게 눌러 사용하는 강한 차징 주먹 공격입니다.",
        "damage_multiplier": 2.25,
        "cooldown_multiplier": 2.0,
        "charge_seconds": 0.85,
        "icon_label": "강",
        "icon_tint": unreal.LinearColor(1.0, 0.22, 0.22, 1.0),
        "debug_color": unreal.LinearColor(1.0, 0.22, 0.22, 1.0),
    },
]


def create_or_update_attack_asset(asset_tools, source_asset, attack_data):
    asset_path = f"{ASSET_PATH}/{attack_data['name']}"
    asset = None
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    else:
        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", unreal.Rogue10mAttackSkillData)
        asset = asset_tools.create_asset(
            attack_data["name"],
            ASSET_PATH,
            unreal.Rogue10mAttackSkillData,
            factory,
        )

    base_damage = source_asset.get_editor_property("damage")
    base_cooldown = source_asset.get_editor_property("attack_cooldown")

    asset.set_editor_property("skill_name", attack_data["skill_name"])
    asset.set_editor_property("skill_description", attack_data["description"])
    asset.set_editor_property("input_slot", attack_data["slot"])
    asset.set_editor_property("damage", round(base_damage * attack_data["damage_multiplier"], 2))
    asset.set_editor_property("attack_range", source_asset.get_editor_property("attack_range"))
    asset.set_editor_property("attack_trace_radius", source_asset.get_editor_property("attack_trace_radius"))
    asset.set_editor_property("attack_cooldown", round(base_cooldown * attack_data["cooldown_multiplier"], 2))
    asset.set_editor_property("charge_seconds", attack_data["charge_seconds"])
    asset.set_editor_property("icon_label", attack_data["icon_label"])
    asset.set_editor_property("icon_tint", attack_data["icon_tint"])
    asset.set_editor_property("enable_combo", False)
    asset.set_editor_property("combo_window_open_seconds", 0.15)
    asset.set_editor_property("combo_window_close_seconds", 0.55)
    asset.set_editor_property("combo_input_slot", unreal.Rogue10mAttackInputSlot.PRIMARY)
    asset.set_editor_property("next_combo_skill", None)
    asset.set_editor_property("attack_montage", source_asset.get_editor_property("attack_montage"))
    asset.set_editor_property("attack_effect", source_asset.get_editor_property("attack_effect"))
    asset.set_editor_property("draw_debug_attack", source_asset.get_editor_property("draw_debug_attack"))
    asset.set_editor_property("debug_color", attack_data["debug_color"])

    unreal.EditorAssetLibrary.save_loaded_asset(asset)
    return asset.get_path_name()


def configure_combo(asset_name, next_asset_name):
    asset = unreal.EditorAssetLibrary.load_asset(f"{ASSET_PATH}/{asset_name}")
    next_asset = unreal.EditorAssetLibrary.load_asset(f"{ASSET_PATH}/{next_asset_name}")
    if asset is None or next_asset is None:
        raise RuntimeError(f"콤보 연결 대상 Data Asset을 찾을 수 없습니다: {asset_name} -> {next_asset_name}")

    asset.set_editor_property("enable_combo", True)
    asset.set_editor_property("combo_window_open_seconds", 0.15)
    asset.set_editor_property("combo_window_close_seconds", 0.55)
    asset.set_editor_property("combo_input_slot", unreal.Rogue10mAttackInputSlot.PRIMARY)
    asset.set_editor_property("next_combo_skill", next_asset)
    unreal.EditorAssetLibrary.save_loaded_asset(asset)


def configure_primary_icon(source_asset):
    source_asset.set_editor_property("icon_label", "좌")
    source_asset.set_editor_property("icon_tint", unreal.LinearColor(1.0, 0.58, 0.24, 1.0))
    unreal.EditorAssetLibrary.save_loaded_asset(source_asset)


def main():
    source_asset = unreal.EditorAssetLibrary.load_asset(SOURCE_PRIMARY)
    if source_asset is None:
        raise RuntimeError(f"기준 Data Asset을 찾을 수 없습니다: {SOURCE_PRIMARY}")

    configure_primary_icon(source_asset)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    created_paths = []
    for attack_data in DEFAULT_UNARMED_ATTACKS:
        created_paths.append(create_or_update_attack_asset(asset_tools, source_asset, attack_data))

    configure_combo("DA_Attack_Unarmed_Primary", "DA_Attack_Unarmed_Primary_Combo2")
    configure_combo("DA_Attack_Unarmed_Primary_Combo2", "DA_Attack_Unarmed_Primary_Combo3")

    unreal.log("기본 주먹 공격 Data Asset 생성/갱신 완료:")
    for path in created_paths:
        unreal.log(f"- {path}")


main()
