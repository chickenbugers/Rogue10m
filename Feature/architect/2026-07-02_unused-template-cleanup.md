# 미사용 템플릿 클래스 정리 - Architecture Plan

## Feature Summary

현재 Rogue10m의 실제 게임 흐름에서 사용하지 않는 UE 템플릿 변형 C++ 클래스를 제거해 빌드 대상과 프로젝트 용량을 줄인다. C++ 클래스만 삭제하면 해당 클래스를 부모로 삼는 템플릿 블루프린트가 깨지므로, 대응되는 `Content/Variant_Shooter`, `Content/Variant_Horror` 에셋도 함께 제거한다.

## Existing Context

- 현재 기본 게임 흐름은 `Rogue10mGameMode`, `Rogue10mCharacter`, `Rogue10mPlayerController`, `Rogue10mHUD`를 사용한다.
- `Source/Rogue10m/Variant_Shooter`와 `Source/Rogue10m/Variant_Horror`는 템플릿 변형 코드로 확인된다.
- 관련 에셋은 `Content/Variant_Shooter`, `Content/Variant_Horror`, `Content/__ExternalActors__/Variant_*`, `Content/__ExternalObjects__/Variant_*` 아래에 있다.

## Technical Approach

- `Variant_Shooter`와 `Variant_Horror` C++ 소스 디렉터리를 제거한다.
- 해당 C++ 클래스를 부모로 사용하는 템플릿 Variant 콘텐츠 디렉터리를 함께 제거한다.
- 공용 캐릭터, Rogue10m 전용 시스템, Data Asset, Widget Blueprint, 현재 작업 중인 UI 아이콘은 유지한다.
- `Content/Weapons`와 Mannequin 무기 애니메이션은 이번 작업에서 제거하지 않는다. 이후 실제 무기/애니메이션 방향이 정해진 뒤 별도 콘텐츠 정리로 다룬다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 미사용 템플릿 후보 식별 | `Source`, `Content`, `Config` | Rogue10m 핵심 흐름과 분리된 Variant 후보 확인 | `rg` 참조 검색 |
| 2 | 템플릿 C++와 대응 콘텐츠 제거 | `Source/Rogue10m/Variant_*`, `Content/Variant_*` | 삭제 후 C++ 빌드가 통과 | `BuildEditor.ps1` |
| 3 | 변경 결과 기록 | `Feature/doc`, `DevLog` | 제거 범위와 검증 결과 기록 | 문서 확인 |

## Affected Files

- `Source/Rogue10m/Variant_Shooter/**`
- `Source/Rogue10m/Variant_Horror/**`
- `Content/Variant_Shooter/**`
- `Content/Variant_Horror/**`
- `Content/__ExternalActors__/Variant_Shooter/**`
- `Content/__ExternalActors__/Variant_Horror/**`
- `Content/__ExternalObjects__/Variant_Shooter/**`
- `Content/__ExternalObjects__/Variant_Horror/**`
- `Feature/doc/2026-07-02_unused-template-cleanup.md`
- `DevLog/20260702.txt`

## C++ / Blueprint Boundary

- 제거되는 블루프린트는 제거되는 템플릿 C++ 클래스를 부모로 하는 에셋이다.
- 현재 사용 중인 Rogue10m 전용 Widget Blueprint와 게임플레이 클래스는 유지한다.

## Implementation Steps

1. 참조 검색으로 Variant 템플릿의 사용 범위를 확인한다.
2. Source와 대응 콘텐츠 디렉터리를 Git 추적 삭제한다.
3. Unreal Editor 타깃 빌드를 실행한다.
4. 생성물 검사와 DevLog 기록을 수행한다.

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

- 삭제된 Variant 템플릿 맵이나 블루프린트를 다시 보고 싶다면 Git에서 복구해야 한다.
- `Content/Weapons`와 무기 애니메이션은 아직 남아 있으므로, 실제 용량 추가 최적화는 추후 콘텐츠 정리에서 더 진행할 수 있다.
