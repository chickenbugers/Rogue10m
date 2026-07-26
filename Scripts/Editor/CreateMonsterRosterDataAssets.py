"""Create or update the canonical 25 Rogue10m Monster Data Assets."""

from pathlib import Path
import sys

import unreal

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from MonsterRosterDefinitions import (  # noqa: E402
    MONSTER_ROSTER,
    RANK_DIRECTORIES,
    RANK_FINAL_BOSS,
    RANK_MID_BOSS,
    RANK_NORMAL,
)


def log(message):
    unreal.log(f"[Rogue10mMonsterRoster] {message}")


def require_asset(path):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        raise RuntimeError(f"필수 에셋을 찾을 수 없습니다: {path}")
    return asset


def enum_value(enum_type, rank):
    names = {
        RANK_NORMAL: ("NORMAL", "Normal"),
        RANK_MID_BOSS: ("MID_BOSS", "MIDBOSS", "MidBoss"),
        RANK_FINAL_BOSS: ("FINAL_BOSS", "FINALBOSS", "FinalBoss"),
    }[rank]
    for name in names:
        if hasattr(enum_type, name):
            return getattr(enum_type, name)
    raise RuntimeError(f"Monster Rank enum 값을 찾을 수 없습니다: {rank}")


def create_or_load(asset_path, data_asset_class):
    existing = unreal.EditorAssetLibrary.load_asset(asset_path)
    if existing:
        return existing, False

    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", data_asset_class)
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, data_asset_class, factory
    )
    if not asset:
        raise RuntimeError(f"Monster Data Asset 생성 실패: {asset_path}")
    return asset, True


def configure_asset(asset, definition, rank_enum):
    asset.set_editor_property("monster_id", definition["monster_id"])
    asset.set_editor_property("display_name", definition["display_name"])
    asset.set_editor_property("level", definition["level"])
    asset.set_editor_property(
        "monster_rank", enum_value(rank_enum, definition["rank"])
    )
    asset.set_editor_property(
        "is_boss", definition["rank"] != RANK_NORMAL
    )
    asset.set_editor_property(
        "experience_reward", definition["experience_reward"]
    )
    asset.set_editor_property("max_health", definition["max_health"])
    asset.set_editor_property("max_stamina", definition["max_stamina"])
    asset.set_editor_property("max_mana", definition["max_mana"])
    asset.set_editor_property(
        "health_regeneration_per_second", definition["health_regeneration"]
    )
    asset.set_editor_property(
        "stamina_regeneration_per_second", definition["stamina_regeneration"]
    )
    asset.set_editor_property(
        "mana_regeneration_per_second", definition["mana_regeneration"]
    )
    asset.set_editor_property("skeletal_mesh", require_asset(definition["mesh"]))
    asset.set_editor_property(
        "mesh_relative_location", unreal.Vector(*definition["mesh_location"])
    )
    pitch, yaw, roll = definition["mesh_rotation"]
    asset.set_editor_property(
        "mesh_relative_rotation",
        unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll),
    )
    asset.set_editor_property(
        "mesh_relative_scale", unreal.Vector(*definition["mesh_scale"])
    )
    asset.set_editor_property("walk_speed", definition["walk_speed"])
    asset.set_editor_property(
        "detection_range", definition["detection_range"]
    )
    asset.set_editor_property("stop_distance", definition["stop_distance"])
    asset.set_editor_property("attack_skill", None)
    asset.set_editor_property("attack_range", definition["attack_range"])
    asset.set_editor_property("attack_damage", definition["attack_damage"])
    asset.set_editor_property("attack_interval", definition["attack_interval"])
    asset.set_editor_property("destroy_on_death", True)


def main():
    data_asset_class = unreal.load_class(
        None, "/Script/Rogue10m.Rogue10mMonsterDataAsset"
    )
    rank_enum = unreal.Rogue10mMonsterRank
    if not data_asset_class or not rank_enum:
        raise RuntimeError(
            "Rogue10mMonsterDataAsset 또는 Rogue10mMonsterRank를 로드하지 못했습니다. "
            "Editor 빌드를 먼저 완료하세요."
        )

    for directory in RANK_DIRECTORIES.values():
        unreal.EditorAssetLibrary.make_directory(directory)

    created_count = 0
    updated_count = 0
    for definition in MONSTER_ROSTER:
        asset, created = create_or_load(
            definition["asset_path"], data_asset_class
        )
        configure_asset(asset, definition, rank_enum)
        if not unreal.EditorAssetLibrary.save_loaded_asset(
            asset, only_if_is_dirty=False
        ):
            raise RuntimeError(
                f"Monster Data Asset 저장 실패: {definition['asset_path']}"
            )
        created_count += int(created)
        updated_count += int(not created)
        log(
            f"{'생성' if created else '갱신'}: {definition['asset_path']} "
            f"(Lv.{definition['level']}, EXP {definition['experience_reward']})"
        )

    log(
        f"완료: 총 {len(MONSTER_ROSTER)}종 "
        f"(신규 {created_count}, 갱신 {updated_count})"
    )


if __name__ == "__main__":
    main()
