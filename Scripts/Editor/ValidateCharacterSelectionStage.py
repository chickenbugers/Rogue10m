"""Validate the full-screen, fixed three-character selection stage."""

import unreal


WIDGET_PATH = "/Game/Widget/Lobby/WBP_CharacterLobby"
BACKGROUND_TEXTURE_PATH = "/Game/Texture/UI/CharacterLobby/T_CharacterLobby_Background"
PREVIEW_MATERIAL_PATH = "/Game/Material/UI/M_CharacterPreviewTransparent"
MAX_SLOT_PREVIEW_WIDTH = 384.0
EXPECTED_PREVIEW_Y = 15.0
EXPECTED_PREVIEW_WIDTH = 360.0
EXPECTED_PREVIEW_HEIGHT = 720.0
MIN_ACTION_BUTTON_Z_ORDER = 8

FORBIDDEN_STAGE_WIDGETS = {
    "UI_LobbyBackdrop",
    "UI_SelectedCharacterInfoText",
    "UI_StatusText",
    "UI_Slot1StageGlow",
    "UI_Slot2StageGlow",
    "UI_Slot3StageGlow",
}
REQUIRED_STAGE_WIDGETS = {
    "UI_LobbyBackgroundImage",
    "UI_BackgroundShade",
    "UI_BottomBar",
    "UI_SelectionStagePanel",
    "UI_Slot1PreviewImage",
    "UI_Slot2PreviewImage",
    "UI_Slot3PreviewImage",
    "UI_CharacterPreviewImage",
    "UI_CreationPanel",
    "UI_Slot1Button",
    "UI_Slot2Button",
    "UI_Slot3Button",
    "UI_NewCharacterButton",
    "UI_DeleteCharacterButton",
    "UI_EnterGameButton",
}


def fail(message):
    raise RuntimeError(message)


def nearly_equal(left, right, tolerance=0.01):
    return abs(left - right) <= tolerance


def main():
    background_texture = unreal.EditorAssetLibrary.load_asset(
        BACKGROUND_TEXTURE_PATH
    )
    if not background_texture:
        fail(f"Lobby background texture missing: {BACKGROUND_TEXTURE_PATH}")

    preview_material = unreal.EditorAssetLibrary.load_asset(
        PREVIEW_MATERIAL_PATH
    )
    if not preview_material:
        fail(f"Preview transparency material missing: {PREVIEW_MATERIAL_PATH}")
    if (
        preview_material.get_editor_property("material_domain")
        != unreal.MaterialDomain.MD_UI
    ):
        fail("Preview material is not in the User Interface domain")
    if (
        preview_material.get_editor_property("blend_mode")
        != unreal.BlendMode.BLEND_TRANSLUCENT
    ):
        fail("Preview material is not translucent")

    widget = unreal.EditorAssetLibrary.load_asset(WIDGET_PATH)
    if not widget:
        fail(f"Character Lobby Widget missing: {WIDGET_PATH}")

    tool = unreal.get_default_object(unreal.UMGToolSet)
    tree = tool.call_method("GetWidgets", (widget,))
    widgets = {
        info.widget.get_name(): info.widget
        for info in tree.widgets
        if info.widget
    }
    names = set(widgets)
    missing = sorted(REQUIRED_STAGE_WIDGETS - names)
    if missing:
        fail(f"Required lobby widgets missing: {missing}")
    forbidden = sorted(FORBIDDEN_STAGE_WIDGETS & names)
    if forbidden:
        fail(f"Forbidden lobby widgets remain: {forbidden}")

    background = widgets["UI_LobbyBackgroundImage"]
    if (
        background.get_editor_property("visibility")
        != unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE
    ):
        fail("Lobby background blocks mouse input")
    background_slot = background.get_editor_property("slot")
    anchors = background_slot.get_anchors()
    offsets = background_slot.get_offsets()
    if (
        anchors.minimum != unreal.Vector2D(0.0, 0.0)
        or anchors.maximum != unreal.Vector2D(1.0, 1.0)
    ):
        fail(f"Lobby background is not full-screen stretched: {anchors}")
    if any(
        abs(value) > 0.01
        for value in (
            offsets.left,
            offsets.top,
            offsets.right,
            offsets.bottom,
        )
    ):
        fail(f"Lobby background stretch offsets are not zero: {offsets}")

    shade = widgets["UI_BackgroundShade"]
    shade_color = shade.get_editor_property("brush_color")
    if shade_color.a > 0.01:
        fail(f"Lobby background shade must be transparent: alpha={shade_color.a}")

    for name in ("UI_BackgroundShade", "UI_BottomBar"):
        if (
            widgets[name].get_editor_property("visibility")
            != unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE
        ):
            fail(f"Decorative widget blocks mouse input: {name}")

    preview_layouts = []
    for index in range(1, 4):
        preview = widgets[f"UI_Slot{index}PreviewImage"]
        slot = preview.get_editor_property("slot")
        position = slot.get_position()
        size = slot.get_size()
        preview_layouts.append((position.y, size.x, size.y))
        if size.x > MAX_SLOT_PREVIEW_WIDTH:
            fail(
                f"Slot {index} preview width {size.x} exceeds "
                f"{MAX_SLOT_PREVIEW_WIDTH}"
            )
        if (
            preview.get_editor_property("visibility")
            != unreal.SlateVisibility.SELF_HIT_TEST_INVISIBLE
        ):
            fail(f"Slot {index} preview blocks mouse input")

    for index, (y, width, height) in enumerate(preview_layouts, start=1):
        if not nearly_equal(y, EXPECTED_PREVIEW_Y):
            fail(f"Slot {index} preview Y differs: {y}")
        if not nearly_equal(width, EXPECTED_PREVIEW_WIDTH):
            fail(f"Slot {index} preview width differs: {width}")
        if not nearly_equal(height, EXPECTED_PREVIEW_HEIGHT):
            fail(f"Slot {index} preview height differs: {height}")

    for name in (
        "UI_Slot1Button",
        "UI_Slot2Button",
        "UI_Slot3Button",
        "UI_NewCharacterButton",
        "UI_DeleteCharacterButton",
        "UI_EnterGameButton",
    ):
        slot = widgets[name].get_editor_property("slot")
        if slot.get_z_order() < MIN_ACTION_BUTTON_Z_ORDER:
            fail(f"Action button ZOrder is too low: {name}")

    generated_class = unreal.EditorAssetLibrary.load_blueprint_class(
        WIDGET_PATH
    )
    if not generated_class:
        fail("Character Lobby GeneratedClass missing")
    lobby_cdo = unreal.get_default_object(generated_class)
    if not lobby_cdo.get_editor_property("customization_catalog"):
        fail("Character Lobby CustomizationCatalog default reference missing")
    if not lobby_cdo.get_editor_property("preview_transparency_material"):
        fail("Character Lobby PreviewTransparencyMaterial reference missing")

    unreal.log(
        "[Rogue10mCharacterSelectionStageValidation] PASSED: "
        "transparent fixed previews + equal 360x720 framing + full background"
    )


if __name__ == "__main__":
    main()