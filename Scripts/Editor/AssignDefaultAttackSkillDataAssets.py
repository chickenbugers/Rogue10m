import unreal


CHARACTER_BLUEPRINT_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"
DATA_ASSET_PATH = "/Game/DataAsset"


ATTACK_SKILL_ASSIGNMENTS = {
    "primary_attack_skill": "DA_Attack_Unarmed_Primary",
    "jump_primary_attack_skill": "DA_Attack_Unarmed_JumpPrimary",
}


CLEAR_ATTACK_SKILL_PROPERTIES = [
    "special_attack_skill",
    "jump_special_attack_skill",
    "charged_primary_attack_skill",
    "charged_special_attack_skill",
]


def load_required_asset(asset_path):
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset is None:
        raise RuntimeError(f"필수 에셋을 찾을 수 없습니다: {asset_path}")
    return asset


def main():
    character_blueprint = load_required_asset(CHARACTER_BLUEPRINT_PATH)
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(CHARACTER_BLUEPRINT_PATH)
    if generated_class is None:
        raise RuntimeError(f"Blueprint 클래스를 찾을 수 없습니다: {CHARACTER_BLUEPRINT_PATH}")

    class_default_object = unreal.get_default_object(generated_class)
    assigned_lines = []

    for property_name, asset_name in ATTACK_SKILL_ASSIGNMENTS.items():
        skill_asset = load_required_asset(f"{DATA_ASSET_PATH}/{asset_name}")
        class_default_object.set_editor_property(property_name, skill_asset)
        assigned_lines.append(f"{property_name} -> {skill_asset.get_path_name()}")

    for property_name in CLEAR_ATTACK_SKILL_PROPERTIES:
        class_default_object.set_editor_property(property_name, None)
        assigned_lines.append(f"{property_name} -> None")

    compile_function = getattr(getattr(unreal, "BlueprintEditorLibrary", None), "compile_blueprint", None)
    if compile_function:
        compile_function(character_blueprint)

    unreal.EditorAssetLibrary.save_asset(CHARACTER_BLUEPRINT_PATH, only_if_is_dirty=False)

    unreal.log("BP_FirstPersonCharacter 공격 Data Asset 연결 완료:")
    for line in assigned_lines:
        unreal.log(f"- {line}")


main()
