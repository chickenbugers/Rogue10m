# Rogue10m AI 개발 세팅 참조

UE 5.8 Editor를 AI 또는 외부 Python 자동화에서 안전하게 조작하기 위한 최소 설정이다.

## 1. 프로젝트 환경

- Engine: `D:\Program Files\UE_5.8`
- Project: `D:\Project\Rogue10m\Rogue10m.uproject`
- Editor Python 스크립트: `D:\Project\Rogue10m\Scripts\Editor`
- 주요 AI 플러그인: `ModelContextProtocol`, `MCPClientToolset`, `EditorToolset`, `UMGToolSet`, `GASToolsets`, `AIAssistant`

## 2. 필요한 Unreal 플러그인

`Edit → Plugins`에서 다음 플러그인을 확인한다.

- `Python Editor Script Plugin`: Unreal Python API와 Python 스크립트 실행
- `Editor Scripting Utilities`: 에셋 생성·저장·편집 자동화 보조
- 프로젝트의 AI/MCP 플러그인: `.uproject`에 활성화된 항목 사용

플러그인을 새로 켰다면 Editor를 재시작한다. 팀 환경에서 항상 필요하다면 `.uproject`의 `Plugins` 목록에 명시적으로 추가한다.

## 3. Remote Execution 설정

외부 도구가 현재 열린 Editor에 Python 명령을 전달해야 할 때만 사용한다.

1. `Edit → Project Settings → Plugins → Python`
2. `Python Remote Execution` 펼치기
3. `Enable Remote Execution?` 체크
4. 로컬 작업에서는 기본값 유지
   - Multicast Group: `239.0.0.1:6766`
   - Bind Address: `127.0.0.1`
   - TTL: `0`

작업이 끝나면 Remote Execution을 꺼도 된다. 신뢰할 수 없는 네트워크에서 Bind Address를 `0.0.0.0`으로 변경하지 않는다.

## 4. Editor 안에서 스크립트 실행

가장 단순한 방법은 `Tools → Execute Python Script`이다.

```python
import unreal

asset = unreal.EditorAssetLibrary.load_asset(
    "/Game/DataAsset/Character/DA_Character_Default"
)
unreal.log(f"Loaded: {asset}")
```

Rogue10m 캐릭터/메뉴 에셋 자동 설정 스크립트:

`D:\Project\Rogue10m\Scripts\Editor\CreateCharacterAssetsAndMenuWidgets.py`

## 5. 외부 Python에서 열린 Editor 연결

UE에 포함된 Python과 다음 모듈을 사용한다.

```text
D:\Program Files\UE_5.8\Engine\Plugins\Experimental\PythonScriptPlugin\Content\Python\remote_execution.py
```

기본 흐름은 `RemoteExecution.start()` → `remote_nodes` 확인 → `open_command_connection()` → `run_command(..., MODE_EXEC_FILE)` → `stop()` 순서다.

## 6. 에셋 자동화 기본 패턴

```python
import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory = unreal.DataAssetFactory()
factory.set_editor_property("data_asset_class", unreal.MyDataAsset)

asset = asset_tools.create_asset(
    "DA_Example", "/Game/DataAsset", unreal.MyDataAsset, factory
)
asset.set_editor_property("display_name", "예제")
unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
```

Widget Blueprint는 `unreal.WidgetBlueprintFactory`에 `parent_class`를 지정하고, 생성 후 `BlueprintEditorLibrary.compile_blueprint()`와 저장을 수행한다.

## 7. C++ 변경 후 작업 순서

1. Editor에서 필요한 에셋 저장
2. `Scripts\BuildEditor.ps1`로 Editor 타깃 빌드
3. C++ UCLASS/UPROPERTY 또는 Widget 부모가 변경됐다면 Editor 재시작
4. Python 자동화 실행
5. Blueprint 컴파일·저장
6. PIE 검증
7. `Scripts\CheckGeneratedChanges.ps1` 실행

실행 중 외부 빌드 후 `HOTRELOAD_*` 타입 오류가 발생하면 Python으로 강제 우회하지 말고 Editor를 재시작한다. Hot Reload 클래스에 WBP를 영구 연결하면 다음 실행에서 부모 클래스가 깨질 수 있다.

## 8. 자주 발생하는 문제

- `remote_nodes = []`: Remote Execution이 꺼져 있거나 Python 플러그인이 로드되지 않은 상태
- `allowed Class type: HOTRELOAD_*`: 이전 DLL 클래스가 Editor에 남아 있으므로 저장 후 재시작
- `ClassProperty 변환 실패`: WBP 부모와 CDO 속성이 서로 다른 Hot Reload 세대를 참조
- 에셋이 Content Browser에 보이지만 디스크에 없음: `save_asset()` 또는 `save_loaded_asset()` 누락
- Python 클래스가 로드되지 않음: Editor 타깃 빌드 및 재시작 필요

## 9. 안전 규칙

- `Intermediate`, `Binaries`, `Saved`, 생성 코드 파일을 직접 수정하지 않는다.
- `.uasset`과 `.umap`은 바이너리를 직접 편집하지 않고 Editor API로 변경한다.
- 스크립트는 기존 에셋을 먼저 조회해 중복 생성을 방지한다.
- 실행 전 대상 경로와 변경 목록을 출력하고 실행 후 생성 에셋을 검증한다.
- Editor가 열린 상태에서 별도 Headless Editor를 동시에 실행해 같은 에셋을 저장하지 않는다.
- Remote Execution은 로컬 개발용으로만 사용하고 외부 네트워크에 노출하지 않는다.
