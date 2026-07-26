"""Canonical monster roster definitions shared by creation and validation scripts."""

RANK_NORMAL = "Normal"
RANK_MID_BOSS = "MidBoss"
RANK_FINAL_BOSS = "FinalBoss"

RANK_DIRECTORIES = {
    RANK_NORMAL: "/Game/DataAsset/Monster/Normal",
    RANK_MID_BOSS: "/Game/DataAsset/Monster/MidBoss",
    RANK_FINAL_BOSS: "/Game/DataAsset/Monster/FinalBoss",
}


def monster(
    asset_name,
    monster_id,
    display_name,
    rank,
    level,
    health,
    damage,
    experience,
    mesh_name,
    render_style,
    scale,
    walk_speed,
    attack_range=170.0,
    attack_interval=1.25,
    detection_range=1800.0,
    stop_distance=140.0,
    health_regeneration=0.1,
):
    return {
        "asset_name": asset_name,
        "asset_path": f"{RANK_DIRECTORIES[rank]}/{asset_name}",
        "monster_id": monster_id,
        "display_name": display_name,
        "rank": rank,
        "level": level,
        "max_health": float(health),
        "max_stamina": 100.0 if rank == RANK_NORMAL else 250.0,
        "max_mana": 100.0 if rank == RANK_NORMAL else 250.0,
        "experience_reward": experience,
        "health_regeneration": health_regeneration,
        "stamina_regeneration": 0.1,
        "mana_regeneration": 0.1,
        "mesh": (
            f"/Game/MonsterForSurvivalGame/Mesh/{render_style}/{mesh_name}_SK"
        ),
        "mesh_location": (0.0, 0.0, -90.0),
        "mesh_rotation": (0.0, -90.0, 0.0),
        "mesh_scale": (scale, scale, scale),
        "walk_speed": walk_speed,
        "detection_range": detection_range,
        "stop_distance": stop_distance,
        "attack_range": attack_range,
        "attack_damage": float(damage),
        "attack_interval": attack_interval,
    }


MONSTER_ROSTER = [
    monster("DA_Monster_Normal_VerdantSlime", "VerdantSlime", "초록 점액체",
            RANK_NORMAL, 1, 70, 6, 18, "Slime", "Polyart", 0.75, 220.0, 120.0, 1.45, 1200.0, 105.0),
    monster("DA_Monster_Normal_CrimsonSlime", "CrimsonSlime", "붉은 점액체",
            RANK_NORMAL, 2, 90, 7, 22, "Slime", "PBR", 0.85, 235.0, 125.0, 1.4, 1300.0, 110.0),
    monster("DA_Monster_Normal_FrostSlime", "FrostSlime", "서리 점액체",
            RANK_NORMAL, 3, 110, 8, 26, "Slime", "Polyart", 0.95, 205.0, 130.0, 1.38, 1350.0, 115.0),
    monster("DA_Monster_Normal_ToxicSlime", "ToxicSlime", "맹독 점액체",
            RANK_NORMAL, 4, 130, 9, 30, "Slime", "PBR", 1.05, 245.0, 135.0, 1.35, 1450.0, 120.0),
    monster("DA_Monster_Normal_Sporeling", "Sporeling", "어린 포자 버섯",
            RANK_NORMAL, 5, 150, 10, 35, "Mushroom", "Polyart", 0.8, 230.0, 150.0, 1.35, 1500.0, 125.0),
    monster("DA_Monster_Normal_EmberMushroom", "EmberMushroom", "불씨 버섯",
            RANK_NORMAL, 6, 175, 11, 40, "Mushroom", "PBR", 0.95, 240.0, 155.0, 1.32, 1550.0, 130.0),
    monster("DA_Monster_Normal_ThornMushroom", "ThornMushroom", "가시 버섯",
            RANK_NORMAL, 7, 195, 12, 45, "Mushroom", "Polyart", 1.1, 250.0, 160.0, 1.3, 1600.0, 135.0),
    monster("DA_Monster_Normal_CactusScout", "CactusScout", "선인장 정찰병",
            RANK_NORMAL, 8, 220, 13, 50, "Cactus", "Polyart", 0.9, 280.0, 170.0, 1.25, 1700.0, 140.0),
    monster("DA_Monster_Normal_CactusRaider", "CactusRaider", "선인장 약탈자",
            RANK_NORMAL, 9, 245, 14, 55, "Cactus", "PBR", 1.05, 295.0, 175.0, 1.22, 1750.0, 145.0),
    monster("DA_Monster_Normal_MossShell", "MossShell", "이끼 등껍질",
            RANK_NORMAL, 10, 280, 14, 60, "TurtleShell", "Polyart", 0.95, 190.0, 165.0, 1.4, 1500.0, 140.0),
    monster("DA_Monster_Normal_IronShell", "IronShell", "철갑 등껍질",
            RANK_NORMAL, 11, 330, 15, 66, "TurtleShell", "PBR", 1.1, 170.0, 175.0, 1.38, 1550.0, 150.0),
    monster("DA_Monster_Normal_CaveBeholder", "CaveBeholder", "동굴 감시자",
            RANK_NORMAL, 12, 260, 17, 72, "Beholder", "Polyart", 0.85, 260.0, 180.0, 1.2, 1800.0, 150.0),
    monster("DA_Monster_Normal_ArcaneBeholder", "ArcaneBeholder", "비전 감시자",
            RANK_NORMAL, 13, 290, 18, 78, "Beholder", "PBR", 1.0, 275.0, 190.0, 1.18, 1900.0, 155.0),
    monster("DA_Monster_Normal_SwarmDroneEight", "SwarmDroneEight", "군집 드론 8호",
            RANK_NORMAL, 14, 220, 19, 84, "Swarm08", "Polyart", 0.8, 360.0, 145.0, 1.05, 1900.0, 120.0),
    monster("DA_Monster_Normal_SwarmStingerEight", "SwarmStingerEight", "군집 독침 8호",
            RANK_NORMAL, 15, 250, 20, 90, "Swarm08", "PBR", 0.9, 380.0, 150.0, 1.0, 2000.0, 125.0),
    monster("DA_Monster_Normal_SwarmDroneNine", "SwarmDroneNine", "군집 드론 9호",
            RANK_NORMAL, 16, 280, 21, 98, "Swarm09", "Polyart", 0.9, 390.0, 155.0, 0.98, 2050.0, 130.0),
    monster("DA_Monster_Normal_SwarmRipperNine", "SwarmRipperNine", "군집 절단자 9호",
            RANK_NORMAL, 17, 310, 22, 106, "Swarm09", "PBR", 1.0, 410.0, 160.0, 0.95, 2100.0, 135.0),
    monster("DA_Monster_Normal_YoungMimic", "YoungMimic", "어린 미믹",
            RANK_NORMAL, 18, 380, 24, 116, "ChestMonster", "Polyart", 0.9, 230.0, 180.0, 1.18, 1650.0, 150.0),
    monster("DA_Monster_Normal_HungryMimic", "HungryMimic", "굶주린 미믹",
            RANK_NORMAL, 19, 430, 26, 128, "ChestMonster", "PBR", 1.05, 245.0, 190.0, 1.12, 1750.0, 160.0),
    monster("DA_Monster_Normal_DuneShell", "DuneShell", "사막 철갑수",
            RANK_NORMAL, 20, 500, 28, 140, "TurtleShell", "PBR", 1.25, 210.0, 195.0, 1.25, 1800.0, 170.0),

    monster("DA_Monster_MidBoss_SporeMatriarch", "SporeMatriarch", "포자 군락의 어미",
            RANK_MID_BOSS, 8, 1200, 22, 300, "Mushroom", "PBR", 1.55, 235.0, 220.0, 1.2, 2400.0, 180.0, 0.5),
    monster("DA_Monster_MidBoss_ThornWarden", "ThornWarden", "가시 성채의 수문장",
            RANK_MID_BOSS, 12, 1800, 30, 450, "Cactus", "PBR", 1.65, 250.0, 235.0, 1.1, 2500.0, 195.0, 0.65),
    monster("DA_Monster_MidBoss_AbyssEye", "AbyssEye", "심연을 보는 눈",
            RANK_MID_BOSS, 16, 2400, 38, 650, "Beholder", "PBR", 1.75, 280.0, 250.0, 1.0, 2700.0, 210.0, 0.8),
    monster("DA_Monster_MidBoss_GoldenMimic", "GoldenMimic", "황금 포식자",
            RANK_MID_BOSS, 20, 3200, 48, 900, "ChestMonster", "PBR", 1.8, 265.0, 260.0, 0.95, 2600.0, 220.0, 1.0),

    monster("DA_Monster_FinalBoss_AbyssOverlordAzathor", "AbyssOverlordAzathor", "심연의 군주 아자토르",
            RANK_FINAL_BOSS, 25, 10000, 70, 3000, "Beholder", "PBR", 2.4, 240.0, 300.0, 0.8, 3200.0, 260.0, 2.0),
]


def roster_by_rank(rank):
    return [definition for definition in MONSTER_ROSTER if definition["rank"] == rank]
