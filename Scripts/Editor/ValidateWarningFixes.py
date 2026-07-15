"""Validate project-side fixes for startup warning regressions."""

from pathlib import Path

import unreal


MONSTER_BLUEPRINT = "/Game/Monster/BP_BaseMonster"
EXTERNAL_ACTOR_PACKAGE = (
    "/Game/__ExternalActors__/FirstPerson/Lvl_FirstPerson/"
    "D/NM/NF41K8EAB8CMOZDQZNT8BD"
)
EXPECTED_MONSTER_PARENT = "/Script/Rogue10m.Rogue10mBasicMonster"
EXPECTED_GAMEPLAY_CUE_PATH = "+GameplayCueNotifyPaths=/Game/GameplayCues"


def main():
    blueprint = unreal.EditorAssetLibrary.load_asset(MONSTER_BLUEPRINT)
    if not blueprint:
        raise RuntimeError(f"Missing monster blueprint: {MONSTER_BLUEPRINT}")
    parent = blueprint.get_blueprint_parent_class()
    if not parent or parent.get_path_name() != EXPECTED_MONSTER_PARENT:
        raise RuntimeError(f"Unexpected BP_BaseMonster parent: {parent}")

    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = registry.get_assets_by_package_name(
        unreal.Name(EXTERNAL_ACTOR_PACKAGE), include_only_on_disk_assets=True
    )
    if len(assets) != 1:
        raise RuntimeError(
            f"Expected one external monster actor, found {len(assets)}: "
            f"{EXTERNAL_ACTOR_PACKAGE}"
        )
    actor = assets[0].get_asset()
    if not actor:
        raise RuntimeError(f"Unable to load external monster actor: {EXTERNAL_ACTOR_PACKAGE}")
    component_classes = {
        component.get_class().get_name()
        for component in actor.get_components_by_class(unreal.ActorComponent)
    }
    if "Rogue10mVitalsComponent" in component_classes:
        raise RuntimeError("Legacy Rogue10mVitalsComponent is still serialized")
    required_components = {
        "Rogue10mAbilitySystemComponent",
        "Rogue10mVitalRegenerationComponent",
    }
    missing_components = required_components - component_classes
    if missing_components:
        raise RuntimeError(f"Missing current monster components: {sorted(missing_components)}")

    default_game = Path(unreal.Paths.project_config_dir()) / "DefaultGame.ini"
    config_text = default_game.read_text(encoding="utf-8")
    if EXPECTED_GAMEPLAY_CUE_PATH not in config_text:
        raise RuntimeError("GameplayCueNotifyPaths is not scoped to /Game/GameplayCues")

    unreal.log(
        "[WarningFixValidation] BP_BaseMonster uses GAS AttributeSet components, "
        "the external actor loads without the legacy Vitals component, and "
        "GameplayCueNotifyPaths is scoped to /Game/GameplayCues."
    )


if __name__ == "__main__":
    main()
