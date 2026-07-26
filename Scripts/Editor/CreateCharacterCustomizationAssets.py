"""Create the character customization catalog and character lobby Widget Blueprint."""

from pathlib import Path
import sys

import unreal

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from BuildMenuDesignerLayouts import (  # noqa: E402
    add,
    clear_tree,
    make_border,
    make_text,
    set_canvas_layout,
    set_fill_alignment,
    umg,
)

CATALOG_PATH = (
    "/Game/DataAsset/Character/Customization/DA_CharacterCustomizationCatalog"
)
WIDGET_PATH = "/Game/Widget/Character/WBP_CharacterLobby"
CHARACTER_BLUEPRINT_PATH = "/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"

BACKGROUND = unreal.LinearColor(0.008, 0.012, 0.022, 0.985)
FRAME = unreal.LinearColor(0.025, 0.035, 0.055, 0.97)
SECTION = unreal.LinearColor(0.055, 0.07, 0.105, 0.96)
BUTTON = unreal.LinearColor(0.09, 0.13, 0.2, 1.0)
ACCENT = unreal.LinearColor(0.18, 0.48, 0.78, 1.0)


def log(message):
    unreal.log(f"[Rogue10mCharacterCustomization] {message}")


def enum_value(enum_type, *names):
    for name in names:
        if hasattr(enum_type, name):
            return getattr(enum_type, name)
    raise RuntimeError(f"Enum value not found: {enum_type} {names}")


def require_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"필수 에셋을 찾을 수 없습니다: {path}")
    return asset


def assets(paths):
    return [require_asset(path) for path in paths]


def create_or_load_data_asset():
    existing = unreal.EditorAssetLibrary.load_asset(CATALOG_PATH)
    if existing:
        return existing
    data_class = unreal.load_class(
        None, "/Script/Rogue10m.Rogue10mCharacterCustomizationDataAsset"
    )
    if not data_class:
        raise RuntimeError("CharacterCustomizationDataAsset C++ 클래스를 찾지 못했습니다.")
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", data_class)
    package_path, asset_name = CATALOG_PATH.rsplit("/", 1)
    catalog = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, data_class, factory
    )
    if not catalog:
        raise RuntimeError("커스터마이징 카탈로그 생성 실패")
    return catalog


def full_body(race_code, gender_code, race_name, gender_name):
    return (
        f"/Game/StylizedCharacter/Meshes/Character/"
        f"{race_name}/{gender_name}/SK_{race_code}_{gender_code}_FullBody"
    )


def head_materials(race_code, gender_code, tones):
    return [
        (
            f"/Game/StylizedCharacter/Materials/Instances/Character/"
            f"{'Human' if race_code == 'Hu' else 'Orc'}/"
            f"{'Male' if gender_code == 'M' else 'Female'}/Head/"
            f"MI_{race_code}_{gender_code}_Head_{style:02d}_{tone}"
        )
        for style in range(1, 6)
        for tone in tones
    ]


def make_archetype(
    race,
    gender,
    display_name,
    body_path,
    head_count=1,
    skin_count=1,
    head_paths=(),
    eye_paths=(),
    hair_paths=(),
    hair_material_paths=(),
    facial_paths=(),
    facial_material_paths=(),
    body_detail_paths=(),
):
    entry = unreal.Rogue10mCharacterArchetype()
    entry.set_editor_property("race", race)
    entry.set_editor_property("gender", gender)
    entry.set_editor_property("display_name", display_name)
    entry.set_editor_property("body_mesh", require_asset(body_path))
    entry.set_editor_property("head_style_count", head_count)
    entry.set_editor_property("skin_tone_count", skin_count)
    entry.set_editor_property("head_materials", assets(head_paths))
    entry.set_editor_property("eye_materials", assets(eye_paths))
    entry.set_editor_property("hair_meshes", assets(hair_paths))
    entry.set_editor_property("hair_materials", assets(hair_material_paths))
    entry.set_editor_property("facial_meshes", assets(facial_paths))
    entry.set_editor_property("facial_materials", assets(facial_material_paths))
    entry.set_editor_property("body_detail_materials", assets(body_detail_paths))
    return entry


def build_catalog():
    race_enum = unreal.Rogue10mCharacterRace
    gender_enum = unreal.Rogue10mCharacterGender
    human = enum_value(race_enum, "HUMAN", "Human")
    dwarf = enum_value(race_enum, "DWARF", "Dwarf")
    orc = enum_value(race_enum, "ORC", "Orc")
    male = enum_value(gender_enum, "MALE", "Male")
    female = enum_value(gender_enum, "FEMALE", "Female")

    human_eyes = [
        f"/Game/StylizedCharacter/Materials/Instances/Character/Human/Eye/"
        f"MI_HU_Eye_{color}"
        for color in ("BlBln", "BrBln", "GnBln", "PeBln")
    ]
    orc_eyes = [
        f"/Game/StylizedCharacter/Materials/Instances/Character/Orc/Eye/"
        f"MI_OR_Eye_{color}"
        for color in (
            "BlBln", "BrBln", "CnGlw", "GnBln", "GnGlw",
            "PeBln", "PeGlw", "RdGlw", "YeGlw",
        )
    ]
    hair_materials = [
        f"/Game/StylizedCharacter/Materials/Instances/Character/Human/Hair/"
        f"MI_HU_Hair_01_{color}"
        for color in ("Bd", "Bk", "Br", "Gr")
    ]
    male_beard_materials = [
        f"/Game/StylizedCharacter/Materials/Instances/Character/Human/Male/"
        f"Facials/MI_Hu_M_Facials_{color}"
        for color in ("Bd", "Bk", "Br", "Gr")
    ]

    entries = []
    for gender, gender_code, gender_name, hair_ids in (
        (male, "M", "Male", (6, 10, 14, 15, 17)),
        (female, "F", "Female", (9, 11, 13, 16, 17)),
    ):
        human_hair = [
            f"/Game/StylizedCharacter/Meshes/Character/Human/{gender_name}/"
            f"Customization/Hair/SK_Hu_{gender_code}_Hair_{hair_id:02d}"
            for hair_id in hair_ids
        ]
        if gender == male:
            facial = [
                f"/Game/StylizedCharacter/Meshes/Character/Human/Male/"
                f"Customization/Facial/SK_Hu_M_Beard_{value:02d}"
                for value in (4, 8)
            ]
            facial_materials = male_beard_materials
        else:
            facial = [
                f"/Game/StylizedCharacter/Meshes/Character/Human/Female/"
                f"Customization/Facial/SK_Hu_F_Earring_{value:02d}"
                for value in range(1, 9)
            ] + [
                f"/Game/StylizedCharacter/Meshes/Character/Human/Female/"
                f"Customization/Facial/SK_Hu_F_Piercing_{value:02d}"
                for value in range(1, 5)
            ]
            facial_materials = []

        entries.append(
            make_archetype(
                human,
                gender,
                f"인간 {'남성' if gender == male else '여성'}",
                full_body("Hu", gender_code, "Human", gender_name),
                5,
                4,
                head_materials("Hu", gender_code, ("B", "C", "D", "E")),
                human_eyes,
                human_hair,
                hair_materials,
                facial,
                facial_materials,
            )
        )

    for gender, gender_code, gender_name in (
        (male, "M", "Male"),
        (female, "F", "Female"),
    ):
        entries.append(
            make_archetype(
                dwarf,
                gender,
                f"드워프 {'남성' if gender == male else '여성'}",
                full_body("Dw", gender_code, "Dwarf", gender_name),
            )
        )

    for gender, gender_code, gender_name in (
        (male, "M", "Male"),
        (female, "F", "Female"),
    ):
        tusk = (
            f"/Game/StylizedCharacter/Meshes/Character/Orc/{gender_name}/"
            f"Customization/Facial/SK_Or_{gender_code}_Tusks_01"
        )
        tattoos = [
            f"/Game/StylizedCharacter/Materials/Instances/Character/Orc/"
            f"{gender_name}/Body/Presets/MI_Or_{gender_code}_Body_Tattoo_{value:02d}"
            for value in range(1, 5)
        ]
        entries.append(
            make_archetype(
                orc,
                gender,
                f"오크 {'남성' if gender == male else '여성'}",
                full_body("Or", gender_code, "Orc", gender_name),
                5,
                3,
                head_materials("Or", gender_code, ("D", "E", "F")),
                orc_eyes,
                (),
                (),
                (tusk,),
                (),
                tattoos,
            )
        )

    catalog = create_or_load_data_asset()
    catalog.set_editor_property("archetypes", entries)
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        catalog, only_if_is_dirty=False
    ):
        raise RuntimeError("커스터마이징 카탈로그 저장 실패")
    log(f"카탈로그 갱신: {len(entries)} Archetype")


def create_or_load_widget():
    existing = unreal.EditorAssetLibrary.load_asset(WIDGET_PATH)
    if existing:
        return existing
    parent_class = unreal.load_class(
        None, "/Script/Rogue10m.Rogue10mCharacterLobbyWidget"
    )
    if not parent_class:
        raise RuntimeError("Rogue10mCharacterLobbyWidget C++ 클래스를 찾지 못했습니다.")
    factory = unreal.WidgetBlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    package_path, asset_name = WIDGET_PATH.rsplit("/", 1)
    widget = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, unreal.WidgetBlueprint, factory
    )
    if not widget:
        raise RuntimeError("Character Lobby Widget Blueprint 생성 실패")
    return widget


def canvas_place(slot, x, y, width, height, z=0):
    set_canvas_layout(
        slot,
        (x, y),
        (width, height),
        anchors=(0.0, 0.0),
        alignment=(0.0, 0.0),
        z_order=z,
    )


def button(widget, parent, name, label, x, y, width, height, variable=True):
    result, slot = add(widget, unreal.Button, name, parent, variable=variable)
    canvas_place(slot, x, y, width, height)
    result.set_background_color(BUTTON)
    if label is not None:
        text, text_slot = make_text(
            widget, f"{name}_Label", label, result, font_size=18
        )
        set_fill_alignment(text_slot)
        text.set_editor_property("justification", unreal.TextJustify.CENTER)
    return result


def value_row(widget, parent, key, label, y):
    _, label_slot = make_text(
        widget, f"UI_{key}Label", label, parent, font_size=16
    )
    canvas_place(label_slot, 20, y, 120, 42)
    button(
        widget, parent, f"UI_{key}PreviousButton", "◀", 145, y, 46, 42
    )
    value, value_slot = make_text(
        widget, f"UI_{key}ValueText", f"{label}: 1", parent,
        variable=True, font_size=16
    )
    canvas_place(value_slot, 200, y, 230, 42)
    value.set_editor_property("justification", unreal.TextJustify.CENTER)
    button(
        widget, parent, f"UI_{key}NextButton", "▶", 440, y, 46, 42
    )


def build_widget():
    widget = create_or_load_widget()
    clear_tree(widget)

    root, _ = add(widget, unreal.CanvasPanel, "UI_CharacterLobbyRoot")
    background, background_slot = make_border(
        widget, "UI_Background", root, BACKGROUND, 0.0
    )
    canvas_place(background_slot, 0, 0, 1920, 1080, 0)

    title, title_slot = make_text(
        widget, "UI_TitleText", "CHARACTER SELECT", root,
        font_size=38
    )
    canvas_place(title_slot, 70, 35, 700, 60, 2)

    slot_frame, slot_frame_slot = make_border(
        widget, "UI_SlotFrame", root, FRAME, 12.0
    )
    canvas_place(slot_frame_slot, 60, 120, 330, 760, 1)
    slot_canvas, slot_canvas_slot = add(
        widget, unreal.CanvasPanel, "UI_SlotCanvas", slot_frame
    )
    set_fill_alignment(slot_canvas_slot)
    make_text(
        widget, "UI_SlotTitle", "캐릭터 슬롯", slot_canvas, font_size=24
    )[1].set_position(unreal.Vector2D(12, 8))
    for index in range(3):
        slot_button = button(
            widget,
            slot_canvas,
            f"UI_Slot{index + 1}Button",
            None,
            10,
            65 + index * 180,
            285,
            150,
        )
        text, text_slot = make_text(
            widget,
            f"UI_Slot{index + 1}Text",
            f"슬롯 {index + 1}\n새 캐릭터",
            slot_button,
            variable=True,
            font_size=21,
        )
        set_fill_alignment(text_slot)
        text.set_editor_property("justification", unreal.TextJustify.CENTER)

    preview_frame, preview_slot = make_border(
        widget, "UI_PreviewFrame", root, SECTION, 4.0
    )
    canvas_place(preview_slot, 420, 120, 700, 820, 1)
    preview_image, preview_image_slot = add(
        widget,
        unreal.Image,
        "UI_CharacterPreviewImage",
        preview_frame,
        variable=True,
    )
    set_fill_alignment(preview_image_slot)

    info, info_slot = make_text(
        widget,
        "UI_SelectedCharacterInfoText",
        "접속할 캐릭터를 선택하세요.",
        root,
        variable=True,
        wrap=True,
        font_size=24,
    )
    canvas_place(info_slot, 440, 780, 660, 130, 3)
    info.set_editor_property("justification", unreal.TextJustify.CENTER)

    creation_frame, creation_slot = make_border(
        widget, "UI_CreationFrame", root, FRAME, 12.0
    )
    canvas_place(creation_slot, 1150, 120, 710, 820, 1)
    creation, creation_inner_slot = add(
        widget,
        unreal.CanvasPanel,
        "UI_CreationPanel",
        creation_frame,
        variable=True,
    )
    set_fill_alignment(creation_inner_slot)

    make_text(
        widget, "UI_CreationTitle", "캐릭터 생성", creation, font_size=26
    )[1].set_position(unreal.Vector2D(20, 8))
    name_input, name_slot = add(
        widget,
        unreal.EditableTextBox,
        "UI_CharacterNameInput",
        creation,
        variable=True,
    )
    canvas_place(name_slot, 20, 55, 620, 46)
    name_input.set_hint_text("캐릭터 이름 (2~12자)")

    make_text(widget, "UI_RaceLabel", "종족", creation, font_size=17)[1].set_position(
        unreal.Vector2D(20, 118)
    )
    button(widget, creation, "UI_HumanRaceButton", "인간", 110, 112, 150, 44)
    button(widget, creation, "UI_DwarfRaceButton", "드워프", 270, 112, 150, 44)
    button(widget, creation, "UI_OrcRaceButton", "오크", 430, 112, 150, 44)
    make_text(widget, "UI_GenderLabel", "성별", creation, font_size=17)[1].set_position(
        unreal.Vector2D(20, 172)
    )
    button(widget, creation, "UI_MaleGenderButton", "남성", 160, 166, 190, 44)
    button(widget, creation, "UI_FemaleGenderButton", "여성", 365, 166, 190, 44)

    for row, (key, label) in enumerate(
        (
            ("Head", "얼굴"),
            ("Skin", "피부"),
            ("Hair", "헤어"),
            ("HairColor", "헤어 색"),
            ("Facial", "얼굴 장식"),
            ("Eye", "눈"),
            ("BodyDetail", "신체 디테일"),
        )
    ):
        value_row(widget, creation, key, label, 225 + row * 58)

    button(
        widget, creation, "UI_RandomizeButton", "랜덤", 20, 655, 150, 48
    )
    create_button = button(
        widget, creation, "UI_CreateCharacterButton", "캐릭터 생성",
        180, 655, 260, 48
    )
    create_button.set_background_color(ACCENT)
    button(
        widget, creation, "UI_CancelCreationButton", "취소",
        450, 655, 150, 48
    )

    button(widget, root, "UI_NewCharacterButton", "새 캐릭터", 1160, 960, 190, 54)
    button(widget, root, "UI_DeleteCharacterButton", "캐릭터 삭제", 1360, 960, 190, 54)
    enter = button(widget, root, "UI_EnterGameButton", "게임 접속", 1560, 960, 280, 54)
    enter.set_background_color(ACCENT)

    status, status_slot = make_text(
        widget,
        "UI_StatusText",
        "캐릭터를 선택하거나 새로 생성하세요.",
        root,
        variable=True,
        wrap=True,
        font_size=17,
    )
    canvas_place(status_slot, 60, 970, 1000, 70, 2)

    unreal.BlueprintEditorLibrary.compile_blueprint(widget)
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(
        WIDGET_PATH
    )
    if not generated_class:
        raise RuntimeError("Character Lobby GeneratedClass 로드 실패")
    lobby_cdo = unreal.get_default_object(generated_class)
    lobby_cdo.set_editor_property(
        "customization_catalog", require_asset(CATALOG_PATH)
    )
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        widget, only_if_is_dirty=False
    ):
        raise RuntimeError("Character Lobby Widget 저장 실패")
    log(f"Widget 갱신: {WIDGET_PATH}")


def configure_player_character():
    character_blueprint = unreal.EditorAssetLibrary.load_asset(
        CHARACTER_BLUEPRINT_PATH
    )
    character_class = unreal.EditorAssetLibrary.load_blueprint_class(
        CHARACTER_BLUEPRINT_PATH
    )
    if not character_blueprint or not character_class:
        raise RuntimeError(
            f"Player Character Blueprint 로드 실패: {CHARACTER_BLUEPRINT_PATH}"
        )

    character_cdo = unreal.get_default_object(character_class)
    character_cdo.set_editor_property(
        "customization_catalog", require_asset(CATALOG_PATH)
    )
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        character_blueprint, only_if_is_dirty=False
    ):
        raise RuntimeError("Player Character Blueprint 저장 실패")
    log(f"Player Character 카탈로그 연결: {CHARACTER_BLUEPRINT_PATH}")


def main():
    build_catalog()
    build_widget()
    configure_player_character()
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    log("캐릭터 커스터마이징 에셋 생성 완료")


if __name__ == "__main__":
    main()
