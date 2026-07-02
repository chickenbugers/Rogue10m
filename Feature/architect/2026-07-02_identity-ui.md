# 아이덴티티 UI - Architecture Plan

## Feature Summary

아이덴티티 위젯을 설명 텍스트 중심 자리표시에서 이미지 중심 구조로 확장한다. 현재 기본 무기 흐름은 주먹/권이므로 회색 돌주먹 아이콘을 제공하고, 무기 숙련도에 비례해 외곽선 색과 굵기를 조절할 수 있는 데이터를 Widget Blueprint에 노출한다.

## Existing Context

- `URogue10mRunHUD::GetIdentityView`가 `PlayerState`의 아이덴티티 자원과 무기 숙련도를 읽어 Widget Blueprint에 전달한다.
- `URogue10mIdentityWidget`은 `WBP_Identity`의 C++ 부모 위젯이다.
- 실제 Designer 배치와 스타일 조정은 Unreal Editor의 Widget Blueprint에서 진행한다.

## Technical Approach

- `FRogue10mHudIdentityView`에 아이콘 텍스처, 무기 타입, 숙련도 레벨, 숙련도 정규화 값, 외곽선 색, 외곽선 굵기, 특수 자원 라벨을 추가한다.
- 권/기본 주먹은 `/Game/UI/Icons/T_Identity_StoneFist` 경로를 기본 아이콘 후보로 사용한다.
- 아이콘 원본 PNG는 `Content/UI/Icons/T_Identity_StoneFist.png`에 배치한다.
- Widget Blueprint 가이드에 `WBP_Identity` 내부 권장 구조를 기록한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 아이덴티티 View 데이터 확장 | `Rogue10mRunHUD` | 숙련도/외곽선/아이콘/자원 데이터가 BlueprintReadOnly로 노출됨 | C++ 빌드 |
| 2 | 기본 돌주먹 아이콘 추가 | `Content/UI/Icons` | 투명 배경 PNG가 프로젝트에 추가됨 | 이미지 확인 |
| 3 | WBP 작업 가이드 보강 | `Docs/WidgetBlueprintHUDGuide.md` | Designer에서 배치할 구조와 바인딩 값이 문서화됨 | 문서 확인 |

## Affected Files

- `Source/Rogue10m/UI/Rogue10mRunHUD.h`
- `Source/Rogue10m/UI/Rogue10mRunHUD.cpp`
- `Source/Rogue10m/UI/Widgets/Rogue10mHudWidgetParts.cpp`
- `Content/UI/Icons/T_Identity_StoneFist.png`
- `Docs/WidgetBlueprintHUDGuide.md`
- `DevLog/20260702.txt`

## C++ / Blueprint Boundary

- C++: 런타임 데이터 계산과 Blueprint에 전달할 View 구조체를 담당한다.
- Widget Blueprint: `WBP_Identity` 내부 배치, 이미지 크기, 외곽선 표현, 자원바 디자인을 담당한다.

## Implementation Steps

1. `FRogue10mHudIdentityView`를 확장한다.
2. `GetIdentityView`에서 현재 무기 숙련도 기반 외곽선 데이터를 계산한다.
3. 기본 주먹 아이콘 PNG를 추가한다.
4. Widget Blueprint 작업 가이드를 보강한다.
5. 빌드와 생성물 검사를 실행한다.

## Build And Validation Plan

- `powershell -ExecutionPolicy Bypass -File .\Scripts\BuildEditor.ps1`
- `powershell -ExecutionPolicy Bypass -File .\Scripts\CheckGeneratedChanges.ps1`

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- PNG를 Unreal Texture Asset으로 import하는 작업은 Unreal Editor에서 진행해야 한다.
- 기존 `Content/Widget/Parts/*.uasset` 변경은 작업 시작 전부터 존재했으므로 이번 C++/PNG 작업에서는 직접 수정하지 않는다.
