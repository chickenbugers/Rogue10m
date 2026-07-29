"""Create the UI material that composites lobby character captures with transparency."""

import unreal


MATERIAL_PATH = "/Game/Material/UI/M_CharacterPreviewTransparent"
MATERIAL_FOLDER = "/Game/Material/UI"
MATERIAL_NAME = "M_CharacterPreviewTransparent"
TEXTURE_PARAMETER = "PreviewTexture"


def fail(message):
    raise RuntimeError(message)


def create_or_load_material():
    material = unreal.EditorAssetLibrary.load_asset(MATERIAL_PATH)
    if material:
        unreal.MaterialEditingLibrary.delete_all_material_expressions(material)
        return material

    material = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        MATERIAL_NAME,
        MATERIAL_FOLDER,
        unreal.Material,
        unreal.MaterialFactoryNew(),
    )
    if not material:
        fail(f"Failed to create preview material: {MATERIAL_PATH}")
    return material


def main():
    material = create_or_load_material()
    material.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

    texture = unreal.MaterialEditingLibrary.create_material_expression(
        material,
        unreal.MaterialExpressionTextureSampleParameter2D,
        -320,
        0,
    )
    texture.set_editor_property("parameter_name", TEXTURE_PARAMETER)

    inverse_opacity = unreal.MaterialEditingLibrary.create_material_expression(
        material,
        unreal.MaterialExpressionOneMinus,
        -40,
        140,
    )
    unreal.MaterialEditingLibrary.connect_material_expressions(
        texture, "A", inverse_opacity, ""
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        texture,
        "RGB",
        unreal.MaterialProperty.MP_EMISSIVE_COLOR,
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        inverse_opacity,
        "",
        unreal.MaterialProperty.MP_OPACITY,
    )
    unreal.MaterialEditingLibrary.recompile_material(material)

    if not unreal.EditorAssetLibrary.save_loaded_asset(
        material, only_if_is_dirty=False
    ):
        fail(f"Failed to save preview material: {MATERIAL_PATH}")

    unreal.log(
        "[Rogue10mCharacterPreviewMaterial] CREATED: "
        "SceneColor RGB + OneMinus(InvOpacity)"
    )


if __name__ == "__main__":
    main()