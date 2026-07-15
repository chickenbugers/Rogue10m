# Content Asset Naming - Architecture Plan

## Feature Summary

`Content/Widget`, `Content/DataAsset`, `Content/FirstPerson` 범위를 UE 5.8 권장 에셋 명명 규칙으로 감사하고, 분류가 명확하게 잘못된 이름만 Unreal Editor에서 참조와 함께 변경한다.

## Existing Context

- Widget Blueprint는 `WBP_`, Animation Blueprint는 `ABP_`, Control Rig은 `Rig_`, Data Asset은 프로젝트 규칙인 `DA_`를 사용한다.
- DataAsset 폴더의 에셋은 모두 `DA_` 접두사를 사용하고 있어 변경하지 않는다.
- `Lvl_FirstPerson`은 World Partition 외부 액터 패키지가 많은 템플릿 맵이며, 공식 표에서 Level 접두사가 필수로 정의되지 않아 범위에서 제외한다.

## Technical Approach

| 기존 경로 | 변경 경로 | 근거 |
| --- | --- | --- |
| `/Game/Widget/UW_Rogue10mMainWidget` | `/Game/Widget/WBP_Rogue10mMainHUD` | Widget Blueprint 접두사 및 역할 명시 |
| `/Game/FirstPerson/Anims/ABP_FP_Copy` | `/Game/FirstPerson/Anims/ABP_FirstPerson` | 임시 사본 명칭 제거 |
| `/Game/FirstPerson/Anims/CtrlRig_FPWarp` | `/Game/FirstPerson/Anims/Rig_FirstPersonWarp` | Control Rig 권장 접두사 적용 |

`.uasset`을 파일 시스템에서 이동하지 않고 Unreal Editor `AssetTools` API로 이름을 바꿔 바이너리 참조를 함께 갱신한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 명명 감사 | Content 에셋 메타데이터 | 오명명 후보와 제외 근거 확정 | Asset Registry 조회 |
| 2 | 안전한 이름 변경 | Widget/FirstPerson 에셋 | 세 에셋을 Editor API로 변경 | 신·구 경로 검사 |
| 3 | 문자열 참조 갱신 | Source/Scripts/Docs | 활성 경로에 이전 이름이 남지 않음 | `rg` 검사 |
| 4 | 검증·문서화 | Build/Feature/DevLog | Editor 참조, C++ 빌드, Harness 검증 통과 | BuildEditor, CheckGeneratedChanges |

## Affected Files

- `Content/Widget/WBP_Rogue10mMainHUD.uasset`
- `Content/FirstPerson/Anims/ABP_FirstPerson.uasset`
- `Content/FirstPerson/Anims/Rig_FirstPersonWarp.uasset`
- Main HUD 소프트 클래스 경로와 Editor Python 유지보수 스크립트
- Widget Blueprint 가이드 및 Harness 문서

## C++ / Blueprint Boundary

C++은 `WBP_Rogue10mMainHUD` 소프트 클래스 기본 경로만 제공한다. Blueprint·Animation Blueprint·Control Rig 에셋의 이름과 내부 참조 갱신은 Unreal Editor가 담당한다.

## Implementation Steps

1. 지정된 세 폴더의 에셋 클래스와 이름을 감사한다.
2. 목적지 충돌 시 중단하는 Editor Python 스크립트로 이름을 변경한다.
3. PlayerController CDO의 Main HUD 참조를 새 클래스로 복구하고 저장한다.
4. 텍스트 기반 소프트 경로와 현재 가이드를 갱신한다.
5. 참조·dirty package·redirector·C++ 빌드를 검증한다.

## Build And Validation Plan

- Unreal Asset Registry에서 기존 경로 3개 부재와 신규 경로 3개 존재 확인
- Character → Animation Blueprint, Animation Blueprint → Control Rig, PlayerController → Main HUD 참조 확인
- `/Game/Widget`, `/Game/FirstPerson` redirector 및 dirty package 0건 확인
- `Scripts/BuildEditor.ps1`
- `Scripts/CheckGeneratedChanges.ps1`
- `git diff --check`

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- Main HUD를 CDO가 참조하여 Editor가 확인 대화상자를 표시하므로, 스크립트가 런타임 CDO 참조를 임시 해제한 후 성공·실패에 관계없이 복구한다.
- 기존 DevLog의 이전 에셋 이름은 역사 기록이므로 일괄 치환하지 않는다.
- World Partition 맵 이름 변경은 외부 액터 패키지 대량 변경을 초래하여 제외한다.