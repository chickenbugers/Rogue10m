# Content Asset Naming - Development Summary

## Summary

`Content/Widget`, `Content/DataAsset`, `Content/FirstPerson`의 38개 에셋을 UE 5.8 권장 명명 규칙과 프로젝트 접두사 규칙으로 감사했다. 명확한 오명명 3건을 Unreal Editor에서 참조와 함께 정리했다.

| 기존 이름 | 최종 이름 |
| --- | --- |
| `UW_Rogue10mMainWidget` | `WBP_Rogue10mMainHUD` |
| `ABP_FP_Copy` | `ABP_FirstPerson` |
| `CtrlRig_FPWarp` | `Rig_FirstPersonWarp` |

## Completed Ultrawork Packets

| Packet | Result | Validation |
| --- | --- | --- |
| 1 | Widget 21개, DataAsset 10개, FirstPerson 7개 감사 | Asset Registry 전체 조회 |
| 2 | WBP, Anim Blueprint, Control Rig 이름 변경 | 구 경로 부재·신규 경로 존재 확인 |
| 3 | C++ Soft Class와 Editor 스크립트·가이드 경로 갱신 | 활성 텍스트 경로 검색 |
| 4 | 참조·빌드·문서 검증 | Editor 조회 및 Rogue10mEditor 빌드 |

## Changed Files

- `Content/Widget/WBP_Rogue10mMainHUD.uasset`
- `Content/FirstPerson/Anims/ABP_FirstPerson.uasset`
- `Content/FirstPerson/Anims/Rig_FirstPersonWarp.uasset`
- `Content/FirstPerson/Blueprints/BP_FirstPersonCharacter.uasset`
- `Content/FirstPerson/Blueprints/BP_FirstPersonPlayerController.uasset`
- `Source/Rogue10m/Core/Rogue10mPlayerController.cpp`
- `Scripts/Editor/RenameContentAssetsToConvention.py`
- Main Widget 유지보수 Python 스크립트 3개와 `Docs/WidgetBlueprintHUDGuide.md`

## Build Result

- `Scripts/BuildEditor.ps1`: 성공
- UnrealBuildTool: `Rogue10mEditor Win64 Development` 성공

## Review Result

- Widget Blueprint는 `WBP_`, Animation Blueprint는 `ABP_`, Control Rig은 `Rig_`로 통일했다.
- DataAsset 10개는 모두 프로젝트 규칙인 `DA_`를 사용해 변경하지 않았다.
- `Lvl_FirstPerson`은 공식 접두사 표에서 필수 Level 접두사가 정의되지 않았고 World Partition 외부 액터 변경 위험이 커 유지했다.
- 구 경로 3개는 모두 제거됐고 Object Redirector는 0건이다.
- Character → `ABP_FirstPerson` → `Rig_FirstPersonWarp`, PlayerController → `WBP_Rogue10mMainHUD` 참조를 확인했다.
- 기존 DevLog의 과거 이름은 역사 기록 보존을 위해 수정하지 않았다.

## Editor Or Blueprint Setup

추가 수동 설정은 없다. 재실행 가능한 `Scripts/Editor/RenameContentAssetsToConvention.py`가 목적지 충돌을 검사하고 PlayerController HUD 참조 복구 및 FirstPerson 구 소스 배치 삭제를 처리한다.

## Playtest Notes

에셋 명명과 참조 갱신 작업이므로 별도 플레이 조작 테스트보다 Editor Asset Registry, 참조 조회, 컴파일 검증을 우선했다. 다음 PIE 확인 시 1인칭 팔 애니메이션과 Main HUD 표시를 확인하면 된다.

## References

- https://dev.epicgames.com/documentation/en-us/unreal-engine/recommended-asset-naming-conventions-in-unreal-engine-projects
- https://dev.epicgames.com/documentation/en-us/unreal-engine/working-with-assets-in-unreal-engine

## ULW Loop Exit Gate

- [x] Requested work is implemented or documented.
- [x] Validation result is recorded.
- [x] Review result is recorded.
- [x] DevLog is updated in Korean.
- [x] Remaining risks are listed.