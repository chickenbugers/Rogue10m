import unreal

BB_PATH = "/Game/AI/Monster/BB_Monster"
BT_PATH = "/Game/AI/Monster/BT_Monster"
MONSTER_DATA_PATH = "/Game/DataAsset/Monster"
BASE_MONSTER_BP = "/Game/Monster/BP_BaseMonster"
EXPECTED_KEYS = {
    "SelfActor",
    "TargetActor",
    "HomeLocation",
    "PatrolLocation",
    "LastKnownLocation",
    "HasTarget",
    "IsInAttackRange",
    "WasDamaged",
}


def require(condition, message):
    if not condition:
        raise AssertionError(message)


try:
    blackboard = unreal.EditorAssetLibrary.load_asset(BB_PATH)
    tree = unreal.EditorAssetLibrary.load_asset(BT_PATH)
    require(blackboard is not None, f"Missing {BB_PATH}")
    require(tree is not None, f"Missing {BT_PATH}")

    keys = blackboard.get_editor_property("keys")
    key_names = {str(entry.get_editor_property("entry_name")) for entry in keys}
    require(key_names == EXPECTED_KEYS, f"Blackboard keys differ: {sorted(key_names)}")

    require(tree.get_editor_property("blackboard_asset") == blackboard, "Behavior Tree blackboard mismatch")
    root = tree.get_editor_property("root_node")
    require(root is not None, "Behavior Tree root node is missing")
    children = root.get_editor_property("children")
    require(len(children) == 2, f"Behavior Tree must have Decision + Wait, got {len(children)}")
    task_names = [child.get_editor_property("child_task").get_class().get_name() for child in children]
    require(task_names[0] == "BTTask_Rogue10mMonsterDecision", f"Unexpected decision task: {task_names[0]}")
    require(task_names[1] == "BTTask_Wait", f"Unexpected wait task: {task_names[1]}")

    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = registry.get_assets_by_path(MONSTER_DATA_PATH, recursive=True)
    configured = 0
    for asset_data in assets:
        asset = asset_data.get_asset()
        if not asset or not asset.get_class().get_name().endswith("Rogue10mMonsterDataAsset"):
            continue
        assigned_tree = asset.get_editor_property("behavior_tree_asset")
        require(assigned_tree == tree, f"Behavior Tree not assigned: {asset.get_path_name()}")
        require(asset.get_editor_property("detection_range") > 0.0, f"Invalid detection range: {asset.get_path_name()}")
        require(asset.get_editor_property("lose_sight_range") >= asset.get_editor_property("detection_range"), f"Lose sight range is too short: {asset.get_path_name()}")
        require(asset.get_editor_property("patrol_radius") >= 0.0, f"Invalid patrol radius: {asset.get_path_name()}")
        require(asset.get_editor_property("max_chase_distance") > 0.0, f"Invalid chase distance: {asset.get_path_name()}")
        configured += 1
    require(configured == 25, f"Expected 25 configured monster data assets, got {configured}")

    blueprint = unreal.EditorAssetLibrary.load_asset(BASE_MONSTER_BP)
    require(blueprint is not None, f"Missing {BASE_MONSTER_BP}")
    cdo = unreal.get_default_object(blueprint.generated_class())
    controller_class = cdo.get_editor_property("ai_controller_class")
    require(controller_class.get_name() == "Rogue10mMonsterAIController", f"Unexpected AI controller: {controller_class}")
    require(cdo.get_editor_property("auto_possess_ai") == unreal.AutoPossessAI.PLACED_IN_WORLD_OR_SPAWNED, "Auto Possess AI is not enabled for placed and spawned monsters")

    unreal.log_warning(
        f"[Rogue10mMonsterBehaviorTreeAIValidation] PASSED: keys={len(key_names)} tasks={task_names} data_assets={configured}"
    )
except Exception as exc:
    unreal.log_error(f"[Rogue10mMonsterBehaviorTreeAIValidation] FAILED: {type(exc).__name__}: {exc}")
    raise
finally:
    unreal.SystemLibrary.quit_editor()
