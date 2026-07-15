# plugin cpp cleanup - Development Summary

## Summary

실제 참조가 없는 Editor 플러그인 자동 활성화와 Runtime 모듈 의존성을 제거하고, 비활성 Main HUD 미니맵의 잔여 C++ 설정 및 Python 생성 캐시를 정리했다. 현재 사용 중인 GAS, Enhanced Input, AI, UMG/Slate, AI Assistant/MCP 도구 체인은 유지했다.

## Completed Ultrawork Packets

| Packet | Result | Validation |
| --- | --- | --- |
| 1 | Source, Config, 에셋 레지스트리 기준 플러그인·모듈 사용 감사 완료 | rg 및 열린 Editor Asset Registry에서 StateTree 에셋 0건 |
| 2 | ModelingToolsEditorMode, StateTree, GameplayStateTree 자동 활성화와 StateTree 모듈 2개 제거 | 프로젝트 파일 생성 성공 |
| 3 | Main HUD의 MinimapMarkerContainer, MinimapMarkerWidgetClass, 죽은 갱신 함수와 미사용 색상 상수 제거 | Rogue10mEditor 빌드 성공 |
| 4 | Python bytecode 캐시 삭제 및 Git 무시 규칙 추가 | 생성물 검사, diff 검사 통과 |

## Changed Files

- Rogue10m.uproject: 미사용 플러그인 3개 자동 활성화 제거
- Source/Rogue10m/Rogue10m.Build.cs: 미사용 StateTree 모듈, 빈 Private 의존성, 샘플 주석 제거
- Source/Rogue10m/UI/Widgets/Rogue10mMainHUDWidget.h/.cpp: 비활성 미니맵 전용 설정과 죽은 코드 제거
- .gitignore: Python bytecode 및 __pycache__ 무시
- Feature/architect 및 Feature/doc, DevLog, SprintChangeLog: 근거와 결과 기록

## Build Result

- Unreal 프로젝트 파일 생성: 성공
- Rogue10mEditor Win64 Development: 성공
- XGE 라이선스 미활성 경고로 standalone 실행되었으나 빌드 결과에는 영향 없음
- Android SDK 미설정으로 프로젝트 파일 생성 대상에서 Android가 제외되었으나 Win64 Editor 검증에는 영향 없음

## Review Result

- GameplayAbilities, GameplayTags, GameplayTasks, EnhancedInput, AIModule, UMG, Slate, SlateCore는 실제 코드 사용 때문에 유지했다.
- ModelContextProtocol, MCPClientToolset, EditorToolset, UMGToolSet, GASToolsets, AIAssistant는 현재 Editor 자동화 환경 때문에 유지했다.
- Blueprint 구현 가능성이 있는 BP_OnHudDataUpdated와 bBroadcastHudDataUpdatedEveryTick는 정적 검색만으로 미사용을 단정할 수 없어 유지했다.
- 공용 미니맵 데이터 타입과 URogue10mMinimapMarkerWidget 자체는 향후 기능 복귀 경계를 보존하기 위해 제거하지 않았다.
- 기존 작업 트리의 uasset 및 다른 기능 변경은 수정하지 않았다.

## Editor Or Blueprint Setup

열려 있는 Unreal Editor를 재시작해야 변경된 프로젝트 플러그인 로드 목록이 적용된다. 추후 StateTree AI를 도입하면 StateTree와 GameplayStateTree 플러그인 및 Build.cs 모듈을 다시 추가해야 한다. Modeling Tools가 필요할 때는 Plugins 창에서 수동 활성화할 수 있다.

## Playtest Notes

컴파일 검증은 완료했다. 플러그인 목록 적용 확인은 Editor 재시작 후 Output Log에서 누락 모듈 경고가 없는지 확인한다. 현재 플레이 기능은 제거 대상 플러그인과 직접 연결되지 않는다.

## ULW Loop Exit Gate

- [x] Requested work is implemented or documented.
- [x] Validation result is recorded.
- [x] Review result is recorded.
- [x] DevLog is updated in Korean.
- [x] Remaining risks are listed.