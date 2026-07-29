import unreal

AI_PATH = "/Game/AI/Monster"
BB_PATH = f"{AI_PATH}/BB_Monster"
BT_PATH = f"{AI_PATH}/BT_Monster"
MONSTER_DATA_PATH = "/Game/DataAsset/Monster"
BASE_MONSTER_BP = "/Game/Monster/BP_BaseMonster"


def load_or_create(asset_name, package_path, asset_class, factory_class):
    asset_path = f"{package_path}/{asset_name}"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset:
        return asset
    asset = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, package_path, asset_class, factory_class()
    )
    if not asset:
        raise RuntimeError(f"Failed to create {asset_path}")
    return asset


def make_key_type(blackboard, script_class_path):
    key_class = unreal.load_class(None, script_class_path)
    if not key_class:
        raise RuntimeError(f"Missing key class {script_class_path}")
    return unreal.new_object(key_class, outer=blackboard)


def make_entry(blackboard, name, script_class_path):
    entry = unreal.BlackboardEntry()
    entry.set_editor_property("entry_name", name)
    entry.set_editor_property("key_type", make_key_type(blackboard, script_class_path))
    return entry


def make_task_child(task):
    child = unreal.BTCompositeChild()
    child.set_editor_property("child_task", task)
    return child


def configure_blackboard():
    blackboard = load_or_create(
        "BB_Monster", AI_PATH, unreal.BlackboardData, unreal.BlackboardDataFactory
    )
    actor_entry = make_entry(
        blackboard, "TargetActor", "/Script/AIModule.BlackboardKeyType_Object"
    )
    actor_key = actor_entry.get_editor_property("key_type")
    for property_name in ("base_class", "allowed_base_class"):
        try:
            actor_key.set_editor_property(property_name, unreal.Actor)
            break
        except Exception:
            pass

    entries = [
        actor_entry,
        make_entry(blackboard, "HomeLocation", "/Script/AIModule.BlackboardKeyType_Vector"),
        make_entry(blackboard, "PatrolLocation", "/Script/AIModule.BlackboardKeyType_Vector"),
        make_entry(blackboard, "LastKnownLocation", "/Script/AIModule.BlackboardKeyType_Vector"),
        make_entry(blackboard, "HasTarget", "/Script/AIModule.BlackboardKeyType_Bool"),
        make_entry(blackboard, "IsInAttackRange", "/Script/AIModule.BlackboardKeyType_Bool"),
        make_entry(blackboard, "WasDamaged", "/Script/AIModule.BlackboardKeyType_Bool"),
    ]
    blackboard.set_editor_property("keys", entries)
    unreal.EditorAssetLibrary.save_loaded_asset(blackboard, only_if_is_dirty=False)
    return blackboard


def configure_behavior_tree(blackboard):
    tree = load_or_create(
        "BT_Monster", AI_PATH, unreal.BehaviorTree, unreal.BehaviorTreeFactory
    )
    root = unreal.new_object(unreal.BTComposite_Sequence, outer=tree)
    root.set_editor_property("node_name", "Monster Decision Loop")
    decision = unreal.new_object(unreal.BTTask_Rogue10mMonsterDecision, outer=tree)
    wait = unreal.new_object(unreal.BTTask_Wait, outer=tree)
    wait_time = unreal.ValueOrBBKey_Float()
    wait_time.import_text("(DefaultValue=0.25)")
    random_deviation = unreal.ValueOrBBKey_Float()
    random_deviation.import_text("(DefaultValue=0.0)")
    wait.set_editor_property("wait_time", wait_time)
    wait.set_editor_property("random_deviation", random_deviation)
    root.set_editor_property(
        "children", [make_task_child(decision), make_task_child(wait)]
    )
    tree.set_editor_property("blackboard_asset", blackboard)
    tree.set_editor_property("root_node", root)
    unreal.EditorAssetLibrary.save_loaded_asset(tree, only_if_is_dirty=False)
    return tree


def configure_monster_data(tree):
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = registry.get_assets_by_path(MONSTER_DATA_PATH, recursive=True)
    updated = 0
    for asset_data in assets:
        asset = asset_data.get_asset()
        if not asset or not asset.get_class().get_name().endswith("Rogue10mMonsterDataAsset"):
            continue
        asset.set_editor_property("behavior_tree_asset", tree)
        detection_range = asset.get_editor_property("detection_range")
        lose_sight_range = max(asset.get_editor_property("lose_sight_range"), detection_range)
        asset.set_editor_property("lose_sight_range", lose_sight_range)
        asset.set_editor_property(
            "max_chase_distance",
            max(asset.get_editor_property("max_chase_distance"), lose_sight_range),
        )
        unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
        updated += 1
    if updated == 0:
        raise RuntimeError("No monster data assets were configured")
    return updated


def configure_base_monster():
    blueprint = unreal.EditorAssetLibrary.load_asset(BASE_MONSTER_BP)
    controller_class = unreal.load_class(None, "/Script/Rogue10m.Rogue10mMonsterAIController")
    if not blueprint or not controller_class:
        raise RuntimeError("Missing BP_BaseMonster or monster AI controller class")
    generated_class = blueprint.generated_class()
    cdo = unreal.get_default_object(generated_class)
    cdo.set_editor_property("ai_controller_class", controller_class)
    cdo.set_editor_property("auto_possess_ai", unreal.AutoPossessAI.PLACED_IN_WORLD_OR_SPAWNED)
    unreal.EditorAssetLibrary.save_loaded_asset(blueprint, only_if_is_dirty=False)


try:
    unreal.EditorAssetLibrary.make_directory(AI_PATH)
    bb_asset = configure_blackboard()
    bt_asset = configure_behavior_tree(bb_asset)
    count = configure_monster_data(bt_asset)
    configure_base_monster()
    unreal.log_warning(
        f"MONSTER_AI_CONFIG_SUCCESS blackboard={BB_PATH} behavior_tree={BT_PATH} data_assets={count}"
    )
except Exception as exc:
    unreal.log_error(f"MONSTER_AI_CONFIG_FAILED {type(exc).__name__}: {exc}")
    raise
finally:
    unreal.SystemLibrary.quit_editor()
