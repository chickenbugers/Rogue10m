"""Validate the Rogue10m character customization catalog and lobby widget."""

import unreal

CATALOG_PATH = (
    "/Game/DataAsset/Character/Customization/DA_CharacterCustomizationCatalog"
)
WIDGET_PATH = "/Game/Widget/Lobby/WBP_CharacterLobby"
CHARACTER_BLUEPRINT_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"

REQUIRED_WIDGETS = {
    "UI_CharacterPreviewImage",
    "UI_CreationPanel",
    "UI_CharacterNameInput",
    "UI_Slot1Button",
    "UI_Slot2Button",
    "UI_Slot3Button",
    "UI_Slot1Text",
    "UI_Slot2Text",
    "UI_Slot3Text",
    "UI_NewCharacterButton",
    "UI_DeleteCharacterButton",
    "UI_EnterGameButton",
    "UI_CreateCharacterButton",
    "UI_CancelCreationButton",
    "UI_RandomizeButton",
    "UI_HumanRaceButton",
    "UI_DwarfRaceButton",
    "UI_OrcRaceButton",
    "UI_MaleGenderButton",
    "UI_FemaleGenderButton",
    "UI_HeadPreviousButton",
    "UI_HeadNextButton",
    "UI_HeadValueText",
    "UI_SkinPreviousButton",
    "UI_SkinNextButton",
    "UI_SkinValueText",
    "UI_HairPreviousButton",
    "UI_HairNextButton",
    "UI_HairValueText",
    "UI_HairColorPreviousButton",
    "UI_HairColorNextButton",
    "UI_HairColorValueText",
    "UI_FacialPreviousButton",
    "UI_FacialNextButton",
    "UI_FacialValueText",
    "UI_EyePreviousButton",
    "UI_EyeNextButton",
    "UI_EyeValueText",
    "UI_BodyDetailPreviousButton",
    "UI_BodyDetailNextButton",
    "UI_BodyDetailValueText",
}


def log(message):
    unreal.log(f"[Rogue10mCharacterCustomizationValidation] {message}")


def main():
    catalog = unreal.EditorAssetLibrary.load_asset(CATALOG_PATH)
    if not catalog:
        raise RuntimeError(f"카탈로그 누락: {CATALOG_PATH}")
    archetypes = list(catalog.get_editor_property("archetypes"))
    if len(archetypes) != 6:
        raise RuntimeError(f"Archetype 수량 불일치: {len(archetypes)}")

    pairs = set()
    for entry in archetypes:
        pair = (
            str(entry.get_editor_property("race")),
            str(entry.get_editor_property("gender")),
        )
        if pair in pairs:
            raise RuntimeError(f"중복 종족·성별 Archetype: {pair}")
        pairs.add(pair)
        body = entry.get_editor_property("body_mesh")
        if not body:
            raise RuntimeError(f"전신 메시 누락: {pair}")
        head_count = int(entry.get_editor_property("head_style_count"))
        skin_count = int(entry.get_editor_property("skin_tone_count"))
        head_materials = list(entry.get_editor_property("head_materials"))
        if head_materials and len(head_materials) != head_count * skin_count:
            raise RuntimeError(
                f"Head Material 수량 불일치 {pair}: "
                f"{len(head_materials)} != {head_count * skin_count}"
            )
        for property_name in (
            "head_materials",
            "eye_materials",
            "hair_meshes",
            "hair_materials",
            "facial_meshes",
            "facial_materials",
            "body_detail_materials",
        ):
            if any(
                value is None
                for value in entry.get_editor_property(property_name)
            ):
                raise RuntimeError(f"Null 옵션 참조: {pair}.{property_name}")
        log(
            f"OK {pair} | head={head_count} skin={skin_count} "
            f"hair={len(entry.get_editor_property('hair_meshes'))} "
            f"facial={len(entry.get_editor_property('facial_meshes'))}"
        )

    widget = unreal.EditorAssetLibrary.load_asset(WIDGET_PATH)
    if not widget:
        raise RuntimeError(f"Widget 누락: {WIDGET_PATH}")
    tool = unreal.get_default_object(unreal.UMGToolSet)
    tree = tool.call_method("GetWidgets", (widget,))
    names = {info.widget.get_name() for info in tree.widgets if info.widget}
    missing = sorted(REQUIRED_WIDGETS - names)
    if missing:
        raise RuntimeError(f"필수 Widget 바인딩 누락: {missing}")

    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(WIDGET_PATH)
    if not generated_class:
        raise RuntimeError("Widget GeneratedClass를 로드하지 못했습니다.")
    lobby_cdo = unreal.get_default_object(generated_class)
    catalog_reference = lobby_cdo.get_editor_property("customization_catalog")
    if not catalog_reference:
        raise RuntimeError(
            "Rogue10mCharacterLobbyWidget 기본 카탈로그 참조가 비어 있습니다."
        )

    character_class = unreal.EditorAssetLibrary.load_blueprint_class(
        CHARACTER_BLUEPRINT_PATH
    )
    if not character_class:
        raise RuntimeError(
            f"플레이어 Character Blueprint를 로드하지 못했습니다: "
            f"{CHARACTER_BLUEPRINT_PATH}"
        )
    character_cdo = unreal.get_default_object(character_class)
    for property_name in (
        "animation_source_mesh",
        "appearance_hair_mesh",
        "appearance_facial_mesh",
    ):
        if not character_cdo.get_editor_property(property_name):
            raise RuntimeError(
                f"플레이어 Character CDO 상속 외형 컴포넌트 누락: "
                f"{property_name}"
            )
    for property_name in (
        "customization_catalog",
        "animation_source_skeletal_mesh",
        "animation_source_anim_class",
    ):
        if not character_cdo.get_editor_property(property_name):
            raise RuntimeError(
                f"플레이어 Character CDO 기본 참조 누락: {property_name}"
            )
    log(
        "PASSED: 6 Archetype + Character Lobby 필수 바인딩 "
        "+ Inherited Player Character CDO"
    )


if __name__ == "__main__":
    main()
