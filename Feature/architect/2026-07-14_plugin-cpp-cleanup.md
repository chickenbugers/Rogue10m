# plugin cpp cleanup - Architecture Plan

## Feature Summary

UE 5.8 Editor 시작 시 자동 활성화되는 플러그인과 Runtime 모듈 의존성을 실제 코드·에셋 참조 기준으로 감사하고, 현재 비활성화된 Main HUD 미니맵의 잔여 C++ 설정과 생성 캐시를 제거한다.

## Existing Context

- GAS, Enhanced Input, UMG/Slate, AIController는 현재 런타임 코드에서 사용 중이다.
- AI Assistant/MCP/Editor Toolset 계열은 Unreal Editor 원격 자동화 환경에 사용 중이다.
- StateTree 및 GameplayStateTree는 Source, Config와 /Game Asset Registry에서 참조가 발견되지 않았다.
- Modeling Tools Editor Mode는 프로젝트 설정·코드에서 참조되지 않으며 현재 제작 흐름에 필수 플러그인이 아니다.
- MainWidget 미니맵은 의도적으로 비활성화되어 있으나 바인딩 변수와 갱신 함수가 남아 있다.

## Technical Approach

사용 증거가 없는 항목만 제거한다. 에셋 내부 참조 가능성이 있는 Blueprint 이벤트 설정과 향후 복귀 가능한 공용 미니맵 데이터 타입/위젯 클래스 자체는 유지한다. 프로젝트 플러그인 목록, Build.cs, Main HUD의 죽은 경로, Python 캐시 무시 규칙만 변경한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 플러그인·모듈 사용 감사 | .uproject, Build.cs, Source, Asset Registry | 유지/제거 목록에 근거가 있음 | rg, Editor Asset Registry |
| 2 | 미사용 자동 활성화와 모듈 의존성 제거 | Rogue10m.uproject, Rogue10m.Build.cs | StateTree 계열 및 Modeling Tools 자동 활성화 제거 | 프로젝트 파일 생성, Editor 빌드 |
| 3 | 죽은 Main HUD 설정 정리 | Main HUD header/cpp | 비활성 미니맵 바인딩·클래스 설정·갱신 함수 제거 | 컴파일, 참조 검색 |
| 4 | 생성 캐시와 문서 정리 | .gitignore, Scripts/Editor, Feature/Docs/DevLog | Python 캐시 미추적 및 결과 기록 | CheckGeneratedChanges, git diff --check |

## Affected Files

- Rogue10m.uproject
- Source/Rogue10m/Rogue10m.Build.cs
- Source/Rogue10m/UI/Widgets/Rogue10mMainHUDWidget.h
- Source/Rogue10m/UI/Widgets/Rogue10mMainHUDWidget.cpp
- .gitignore
- Harness 결과 문서와 변경 로그

## C++ / Blueprint Boundary

C++에서는 프로젝트 모듈 의존성과 Main HUD의 더 이상 노출하지 않을 미니맵 설정만 정리한다. 기존 Widget Blueprint에 같은 이름의 위젯이 남아 있어도 BindWidgetOptional 제거이므로 컴파일을 막지 않는다. AI/GAS/UI 에디터 플러그인과 Blueprint 이벤트는 유지한다.

## Implementation Steps

1. 코드, 설정, 에셋 레지스트리에서 플러그인 사용을 교차 확인한다.
2. 미사용 플러그인 자동 활성화와 StateTree 모듈 의존성을 제거한다.
3. Main HUD의 비활성 미니맵 전용 설정과 죽은 함수를 제거한다.
4. Python bytecode 캐시를 삭제하고 무시 규칙을 추가한다.
5. 프로젝트 파일 생성, Editor 빌드, 생성물 검사와 diff 리뷰를 수행한다.
6. Feature 문서, DevLog, Sprint 변경 로그를 갱신한다.

## Build And Validation Plan

- UE Build Tool 프로젝트 파일 재생성
- Scripts/BuildEditor.ps1
- Scripts/CheckGeneratedChanges.ps1
- git diff --check
- 제거 대상 심볼 재검색

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- Editor가 열려 있으므로 플러그인 로드 목록 변경은 Editor 재시작 후 완전히 적용된다.
- StateTree를 향후 AI 구조에 도입할 경우 .uproject와 Build.cs에 다시 명시해야 한다.
- 기존 작업 트리의 다른 기능 변경은 수정하거나 되돌리지 않는다.