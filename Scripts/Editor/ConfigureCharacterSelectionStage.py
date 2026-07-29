"""Configure the full-screen three-character selection stage."""

from pathlib import Path
import sys

import unreal

SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parent.parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import CreateCharacterCustomizationAssets as base  # noqa: E402


WIDGET_PATH = "/Game/Widget/Lobby/WBP_CharacterLobby"
BACKGROUND_TEXTURE_PATH = (
    "/Game/Texture/UI/CharacterLobby/T_CharacterLobby_Background"
)
PREVIEW_MATERIAL_PATH = "/Game/Material/UI/M_CharacterPreviewTransparent"
BACKGROUND_SOURCE = (
    PROJECT_ROOT
    / "SourceArt"
    / "UI"
    / "CharacterLobby"
    / "T_CharacterLobby_Background.png"
)

TOP_BAR = unreal.LinearColor(0.012, 0.025, 0.045, 0.82)
BOTTOM_BAR = unreal.LinearColor(0.008, 0.018, 0.034, 0.92)
CARD = unreal.LinearColor(0.025, 0.055, 0.085, 0.9)
CREATION_FRAME = unreal.LinearColor(0.018, 0.035, 0.06, 0.96)


def log(message):
    unreal.log(f"[Rogue10mCharacterSelectionStage] {message}")


def import_background():
    existing = unreal.EditorAssetLibrary.load_asset(
        BACKGROUND_TEXTURE_PATH
    )
    if not BACKGROUND_SOURCE.is_file():
        if existing:
            log("Git 제외 SourceArt가 없어 기존 배경 Texture를 재사용합니다.")
            return existing
        raise RuntimeError(f"배경 원본과 Texture 누락: {BACKGROUND_SOURCE}")

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", str(BACKGROUND_SOURCE))
    task.set_editor_property(
        "destination_path", "/Game/Texture/UI/CharacterLobby"
    )
    task.set_editor_property(
        "destination_name", "T_CharacterLobby_Background"
    )
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    texture = unreal.EditorAssetLibrary.load_asset(BACKGROUND_TEXTURE_PATH)
    if not texture:
        raise RuntimeError("캐릭터 선택 배경 Texture Import 실패")
    texture.set_editor_property("srgb", True)
    unreal.EditorAssetLibrary.save_loaded_asset(
        texture, only_if_is_dirty=False
    )
    log(f"배경 Texture 갱신: {BACKGROUND_TEXTURE_PATH}")
    return texture


def canvas_place(slot, x, y, width, height, z=0):
    base.set_canvas_layout(
        slot,
        (x, y),
        (width, height),
        anchors=(0.0, 0.0),
        alignment=(0.0, 0.0),
        z_order=z,
    )


def canvas_stretch(slot, z=0):
    slot.set_anchors(
        unreal.Anchors(
            minimum=unreal.Vector2D(0.0, 0.0),
            maximum=unreal.Vector2D(1.0, 1.0),
        )
    )
    slot.set_alignment(unreal.Vector2D(0.0, 0.0))
    slot.set_offsets(unreal.Margin(0.0, 0.0, 0.0, 0.0))
    slot.set_z_order(z)

def make_button(
    widget,
    parent,
    name,
    label,
    x,
    y,
    width,
    height,
    color=base.BUTTON,
):
    result = base.button(
        widget, parent, name, label, x, y, width, height
    )
    result.set_background_color(color)
    button_slot = result.get_editor_property("slot")
    if button_slot and hasattr(button_slot, "set_z_order"):
        button_slot.set_z_order(8)
    return result


def add_stage_character(widget, parent, index, x, y, width, height):
    image, image_slot = base.add(
        widget,
        unreal.Image,
        f"UI_Slot{index}PreviewImage",
        parent,
        variable=True,
    )
    canvas_place(image_slot, x, y, width, height, 1)
    image.set_visibility(unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE)


def add_creation_value_row(widget, parent, key, label, y):
    base.value_row(widget, parent, key, label, y)


def build_widget(background_texture):
    widget = base.create_or_load_widget()
    base.clear_tree(widget)

    root, _ = base.add(
        widget, unreal.CanvasPanel, "UI_CharacterLobbyRoot"
    )
    background, background_slot = base.add(
        widget, unreal.Image, "UI_LobbyBackgroundImage", root
    )
    canvas_stretch(background_slot, 0)
    background.set_brush_from_texture(background_texture, False)
    background.set_visibility(unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE)

    shade, shade_slot = base.make_border(
        widget,
        "UI_BackgroundShade",
        root,
        unreal.LinearColor(0.0, 0.0, 0.0, 0.0),
        0.0,
    )
    canvas_place(shade_slot, 0, 0, 1920, 1080, 1)
    shade.set_visibility(unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE)

    top_bar, top_bar_slot = base.make_border(
        widget, "UI_TopBar", root, TOP_BAR, 0.0
    )
    canvas_place(top_bar_slot, 0, 0, 1920, 108, 2)
    title, title_slot = base.make_text(
        widget,
        "UI_TitleText",
        "ROGUE 10M",
        root,
        font_size=38,
    )
    canvas_place(title_slot, 72, 24, 500, 60, 3)
    subtitle, subtitle_slot = base.make_text(
        widget,
        "UI_SubtitleText",
        "CHARACTER SELECT",
        root,
        font_size=24,
    )
    canvas_place(subtitle_slot, 1390, 32, 450, 48, 3)
    subtitle.set_editor_property(
        "justification", unreal.TextJustify.RIGHT
    )

    stage, stage_slot = base.add(
        widget,
        unreal.CanvasPanel,
        "UI_SelectionStagePanel",
        root,
        variable=True,
    )
    canvas_place(stage_slot, 0, 92, 1920, 760, 3)
    add_stage_character(widget, stage, 1, 360, 15, 360, 720)
    add_stage_character(widget, stage, 2, 780, 15, 360, 720)
    add_stage_character(widget, stage, 3, 1200, 15, 360, 720)

    draft_image, draft_slot = base.add(
        widget,
        unreal.Image,
        "UI_CharacterPreviewImage",
        root,
        variable=True,
    )
    canvas_place(draft_slot, 130, 120, 900, 820, 4)

    bottom_bar, bottom_bar_slot = base.make_border(
        widget, "UI_BottomBar", root, BOTTOM_BAR, 0.0
    )
    canvas_place(bottom_bar_slot, 0, 810, 1920, 270, 4)
    bottom_bar.set_visibility(unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE)

    slot_x = (360, 780, 1200)
    for index, x in enumerate(slot_x, start=1):
        slot_button = make_button(
            widget,
            root,
            f"UI_Slot{index}Button",
            None,
            x,
            830,
            360,
            96,
            CARD,
        )
        text, text_slot = base.make_text(
            widget,
            f"UI_Slot{index}Text",
            f"슬롯 {index}\n캐릭터 생성하기",
            slot_button,
            variable=True,
            font_size=20,
        )
        base.set_fill_alignment(text_slot)
        text.set_editor_property(
            "justification", unreal.TextJustify.CENTER
        )


    make_button(
        widget,
        root,
        "UI_NewCharacterButton",
        "새 캐릭터",
        1110,
        965,
        190,
        58,
    )
    make_button(
        widget,
        root,
        "UI_DeleteCharacterButton",
        "캐릭터 삭제",
        1315,
        965,
        190,
        58,
    )
    make_button(
        widget,
        root,
        "UI_EnterGameButton",
        "게임 접속",
        1520,
        965,
        300,
        58,
        base.ACCENT,
    )

    creation_frame, creation_frame_slot = base.add(
        widget,
        unreal.Border,
        "UI_CreationPanel",
        root,
        variable=True,
    )
    creation_frame.set_brush_color(CREATION_FRAME)
    creation_frame.set_padding(unreal.Margin(14.0, 14.0, 14.0, 14.0))
    canvas_place(creation_frame_slot, 1080, 125, 760, 805, 7)
    creation, creation_slot = base.add(
        widget,
        unreal.CanvasPanel,
        "UI_CreationCanvas",
        creation_frame,
    )
    base.set_fill_alignment(creation_slot)

    base.make_text(
        widget,
        "UI_CreationTitle",
        "새 모험가 생성",
        creation,
        font_size=28,
    )[1].set_position(unreal.Vector2D(24, 12))
    name_input, name_slot = base.add(
        widget,
        unreal.EditableTextBox,
        "UI_CharacterNameInput",
        creation,
        variable=True,
    )
    canvas_place(name_slot, 24, 62, 690, 46)
    name_input.set_hint_text("캐릭터 이름 (2~12자)")

    base.make_text(
        widget, "UI_RaceLabel", "종족", creation, font_size=17
    )[1].set_position(unreal.Vector2D(24, 126))
    make_button(
        widget, creation, "UI_HumanRaceButton", "인간",
        120, 118, 160, 44
    )
    make_button(
        widget, creation, "UI_DwarfRaceButton", "드워프",
        290, 118, 160, 44
    )
    make_button(
        widget, creation, "UI_OrcRaceButton", "오크",
        460, 118, 160, 44
    )
    base.make_text(
        widget, "UI_GenderLabel", "성별", creation, font_size=17
    )[1].set_position(unreal.Vector2D(24, 180))
    make_button(
        widget, creation, "UI_MaleGenderButton", "남성",
        180, 172, 190, 44
    )
    make_button(
        widget, creation, "UI_FemaleGenderButton", "여성",
        385, 172, 190, 44
    )

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
        add_creation_value_row(
            widget, creation, key, label, 228 + row * 58
        )

    make_button(
        widget, creation, "UI_RandomizeButton", "랜덤",
        24, 655, 150, 50
    )
    make_button(
        widget,
        creation,
        "UI_CreateCharacterButton",
        "캐릭터 생성",
        185,
        655,
        300,
        50,
        base.ACCENT,
    )
    make_button(
        widget, creation, "UI_CancelCreationButton", "취소",
        500, 655, 160, 50
    )

    unreal.BlueprintEditorLibrary.compile_blueprint(widget)
    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(
        WIDGET_PATH
    )
    if not generated_class:
        raise RuntimeError("Character Lobby GeneratedClass 로드 실패")
    lobby_cdo = unreal.get_default_object(generated_class)
    lobby_cdo.set_editor_property(
        "customization_catalog",
        base.require_asset(base.CATALOG_PATH),
    )
    lobby_cdo.set_editor_property(
        "preview_transparency_material",
        base.require_asset(PREVIEW_MATERIAL_PATH),
    )
    if not unreal.EditorAssetLibrary.save_loaded_asset(
        widget, only_if_is_dirty=False
    ):
        raise RuntimeError("Character Lobby Widget 저장 실패")
    log("전체 화면 3인 캐릭터 선택 무대 구성 완료")


def main():
    background_texture = import_background()
    build_widget(background_texture)
    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)


if __name__ == "__main__":
    main()
