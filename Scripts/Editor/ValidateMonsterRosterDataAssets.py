"""Validate the canonical Rogue10m Monster Data Asset roster."""

from collections import Counter
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

EXPECTED_COUNTS = {
    RANK_NORMAL: 20,
    RANK_MID_BOSS: 4,
    RANK_FINAL_BOSS: 1,
}


def log(message):
    unreal.log(f"[Rogue10mMonsterValidation] {message}")


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


def assert_close(actual, expected, label):
    if abs(float(actual) - float(expected)) > 0.001:
        raise RuntimeError(f"{label}: expected={expected}, actual={actual}")


def main():
    if len(MONSTER_ROSTER) != 25:
        raise RuntimeError(
            f"로스터 정의 수가 25가 아닙니다: {len(MONSTER_ROSTER)}"
        )

    rank_enum = unreal.Rogue10mMonsterRank
    rank_counts = Counter()
    monster_ids = set()
    asset_paths = set()
    expected_paths = {definition["asset_path"] for definition in MONSTER_ROSTER}

    for definition in MONSTER_ROSTER:
        asset_path = definition["asset_path"]
        if asset_path in asset_paths:
            raise RuntimeError(f"중복 Asset 경로: {asset_path}")
        asset_paths.add(asset_path)

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            raise RuntimeError(f"Monster Data Asset 누락: {asset_path}")

        monster_id = str(asset.get_editor_property("monster_id"))
        if monster_id != definition["monster_id"]:
            raise RuntimeError(
                f"{asset_path}.monster_id: expected={definition['monster_id']}, "
                f"actual={monster_id}"
            )
        if monster_id in monster_ids:
            raise RuntimeError(f"중복 MonsterId: {monster_id}")
        monster_ids.add(monster_id)

        expected_rank = enum_value(rank_enum, definition["rank"])
        actual_rank = asset.get_editor_property("monster_rank")
        if actual_rank != expected_rank:
            raise RuntimeError(
                f"{asset_path}.monster_rank: expected={expected_rank}, "
                f"actual={actual_rank}"
            )
        rank_counts[definition["rank"]] += 1

        if int(asset.get_editor_property("level")) != definition["level"]:
            raise RuntimeError(f"{asset_path}.level 불일치")
        if int(asset.get_editor_property("experience_reward")) != definition[
            "experience_reward"
        ]:
            raise RuntimeError(f"{asset_path}.experience_reward 불일치")
        if bool(asset.get_editor_property("is_boss")) != (
            definition["rank"] != RANK_NORMAL
        ):
            raise RuntimeError(f"{asset_path}.b_is_boss 불일치")

        assert_close(
            asset.get_editor_property("max_health"),
            definition["max_health"],
            f"{asset_path}.max_health",
        )
        assert_close(
            asset.get_editor_property("attack_damage"),
            definition["attack_damage"],
            f"{asset_path}.attack_damage",
        )
        assert_close(
            asset.get_editor_property("attack_range"),
            definition["attack_range"],
            f"{asset_path}.attack_range",
        )
        assert_close(
            asset.get_editor_property("attack_interval"),
            definition["attack_interval"],
            f"{asset_path}.attack_interval",
        )
        assert_close(
            asset.get_editor_property("walk_speed"),
            definition["walk_speed"],
            f"{asset_path}.walk_speed",
        )

        mesh = asset.get_editor_property("skeletal_mesh")
        expected_mesh = unreal.EditorAssetLibrary.load_asset(definition["mesh"])
        if not mesh or mesh != expected_mesh:
            raise RuntimeError(f"{asset_path}.skeletal_mesh 불일치")
        if int(asset.get_editor_property("experience_reward")) <= 0:
            raise RuntimeError(f"{asset_path}.experience_reward는 양수여야 합니다.")

        log(
            f"OK {definition['rank']} | {monster_id} | "
            f"Lv.{definition['level']} | EXP {definition['experience_reward']}"
        )

    if dict(rank_counts) != EXPECTED_COUNTS:
        raise RuntimeError(
            f"Rank별 수량 불일치: expected={EXPECTED_COUNTS}, "
            f"actual={dict(rank_counts)}"
        )

    discovered_paths = set()
    for directory in RANK_DIRECTORIES.values():
        for object_path in unreal.EditorAssetLibrary.list_assets(
            directory, recursive=True, include_folder=False
        ):
            asset_path = object_path.split(".", 1)[0]
            if asset_path.rsplit("/", 1)[-1].startswith("DA_Monster_"):
                discovered_paths.add(asset_path)
    if discovered_paths != expected_paths:
        raise RuntimeError(
            "Monster Data Asset 경로 집합 불일치: "
            f"missing={sorted(expected_paths - discovered_paths)}, "
            f"unexpected={sorted(discovered_paths - expected_paths)}"
        )

    log(
        "PASSED: Monster Data Asset 25종 "
        "(Normal 20 / MidBoss 4 / FinalBoss 1)"
    )


if __name__ == "__main__":
    main()
