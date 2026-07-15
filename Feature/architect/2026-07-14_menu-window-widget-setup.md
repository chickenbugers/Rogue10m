# menu window widget setup - Architecture Plan

## Feature Summary

기존 C++ 메뉴 창 부모 위젯을 사용하는 Inventory/Equipment/SkillTree Widget Blueprint를 생성 또는 복구하고, 실제 게임의 BP_FirstPersonPlayerController 기본 클래스 설정에 연결한다.

## Existing Context

- 메뉴 창 C++ 부모 클래스와 런타임 생성·토글 로직은 이미 구현되어 있다.
- WBP_InventoryWindow, WBP_EquipmentWindow, WBP_SkillTreeEntry, WBP_SkillTreeWindow 에셋도 존재한다.
- Editor 조회 결과 PlayerController CDO의 세 Widget Class 설정이 모두 비어 있어 런타임 경고가 발생한다.
- 첫 Editor 연결 시 이전 Hot Reload 부모 클래스로 인해 ClassProperty 직접 저장이 차단되었다.

## Technical Approach

Unreal Editor Python API로 기존 에셋을 우선 재사용하고, 없을 때만 지정된 C++ 부모 클래스로 생성한다. PlayerController에는 명시적 Blueprint Class가 비어 있어도 기본 WBP를 Soft Class로 불러오는 fallback을 제공한다. Editor 재시작 후 자동화 스크립트를 다시 실행하면 CDO에도 세 클래스를 영구 지정할 수 있다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 현재 메뉴 에셋과 CDO 설정 감사 | Content/Widget/Menu, BP_FirstPersonPlayerController | 누락 원인이 확인됨 | Editor Remote Execution 조회 |
| 2 | 멱등 메뉴 설정 스크립트 추가 | Scripts/Editor | 존재하면 재사용하고 없으면 생성 | py_compile |
| 3 | 런타임 기본 WBP fallback 연결 | PlayerController | CDO Class가 비어도 세 창 생성 | Editor 빌드 |
| 4 | Editor 재시작 후 에셋 CDO 영구 연결 | 메뉴 WBP, PlayerController BP | 세 ClassProperty가 WBP를 가리킴 | Editor 재조회 |
| 5 | 리뷰·문서화 | Feature/Docs/DevLog | 결과와 잔여 설정 기록 | CheckGeneratedChanges, diff check |

## Affected Files

- Source/Rogue10m/Core/Rogue10mPlayerController.h/.cpp
- Scripts/Editor/ConfigureMenuWindowWidgets.py
- Content/Widget/Menu의 메뉴 WBP
- Content/FirstPerson/Blueprints/BP_FirstPersonPlayerController.uasset
- Feature 결과 문서, DevLog, SprintChangeLog

## C++ / Blueprint Boundary

C++는 창의 데이터 처리, 기본 레이아웃, 토글 동작과 안전한 기본 클래스 경로를 담당한다. Widget Blueprint는 부모 클래스를 선택하고 Designer 스타일을 확장한다. PlayerController Blueprint는 필요할 경우 프로젝트별 WBP 클래스를 명시적으로 덮어쓴다.

## Implementation Steps

1. 열린 Editor에서 에셋 존재 여부와 PlayerController CDO 설정을 조회한다.
2. 메뉴 WBP 생성·복구·연결 전용 Python 스크립트를 추가한다.
3. 스크립트 문법을 검증하고 Editor Remote Execution으로 실행한다.
4. Hot Reload 타입 충돌 시 반복하지 않고 C++ Soft Class fallback을 추가한다.
5. Editor 타깃 빌드와 생성물 검사를 수행한다.
6. 안전한 재시작 후 스크립트를 다시 실행하고 CDO 설정을 재조회한다.
7. 결과 문서와 로그를 갱신한다.

## Build And Validation Plan

- UE bundled Python py_compile
- Editor Remote Execution 설정 전/후 조회
- Scripts/BuildEditor.ps1
- Scripts/CheckGeneratedChanges.ps1
- git diff --check

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- 현재 Editor는 이전 Hot Reload 부모 클래스를 보유하므로 에셋 CDO 저장 전에 재시작이 필요하다.
- 미저장 사용자 작업 손실을 막기 위해 Codex가 Editor를 임의 종료하지 않는다.
- 기존 작업 트리의 다른 uasset 변경은 보존한다.