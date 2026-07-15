import os
import unreal


ASSET_PATH = "/Game/Widget/WBP_Rogue10mMainHUD"
OUTPUT_PATH = "D:/Project/Rogue10m/Saved/MainWidgetBlueprintInspect.txt"


def describe_object(obj, label):
    lines = [f"## {label}"]
    if not obj:
        lines.append("None")
        return lines

    lines.append(f"repr={obj}")
    lines.append(f"class={obj.get_class().get_name()}")
    for name in ["widget_tree", "WidgetTree", "generated_class", "GeneratedClass", "skeleton_generated_class"]:
        try:
            value = obj.get_editor_property(name)
            lines.append(f"property {name}: {value}")
        except Exception as error:
            lines.append(f"property {name}: ERROR {error}")

    methods = [name for name in dir(obj) if "widget" in name.lower() or "tree" in name.lower() or "class" in name.lower()]
    lines.append("dir matches:")
    lines.extend(f"- {name}" for name in methods)
    return lines


def main():
    asset = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
    lines = []
    lines.extend(describe_object(asset, "loaded asset"))

    if asset:
        try:
            generated_method = asset.generated_class()
        except Exception as error:
            generated_method = f"ERROR {error}"
        lines.extend(describe_object(generated_method if not isinstance(generated_method, str) else None, f"asset.generated_class() {generated_method if isinstance(generated_method, str) else ''}"))

        if generated_method and not isinstance(generated_method, str):
            try:
                cdo = unreal.get_default_object(generated_method)
            except Exception as error:
                cdo = f"ERROR {error}"
            lines.extend(describe_object(cdo if not isinstance(cdo, str) else None, f"CDO {cdo if isinstance(cdo, str) else ''}"))

        for property_name in ["generated_class", "GeneratedClass", "skeleton_generated_class"]:
            try:
                generated = asset.get_editor_property(property_name)
            except Exception:
                generated = None
            lines.extend(describe_object(generated, f"asset.{property_name}"))

    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, "w", encoding="utf-8") as file:
        file.write("\n".join(lines))

    unreal.log(f"[Rogue10m] 캐릭터 메인 위젯 진단 파일 저장: {OUTPUT_PATH}")


main()
