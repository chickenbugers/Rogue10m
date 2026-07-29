# 메인 메뉴와 캐릭터 로비 흐름 구현 결과

## 결과

게임 최초 진입 화면을 `WBP_MainMenu`로 구성하고, 게임 시작 버튼을 누르면 캐릭터 로비가 열리도록 연결했다. 기존 캐릭터 생성·선택·삭제·게임 접속 기능은 유지했으며, 캐릭터 프리뷰와 하단 슬롯 정보의 더블클릭으로도 접속할 수 있다.

## 주요 변경

- `WBP_CharacterLobby`를 `/Game/Widget/Lobby/WBP_CharacterLobby`로 이동했다.
- 기존 Inventory·Equipment·SkillTree 위젯을 `/Game/Widget/Component`로 이동했다.
- `/Game/Widget/Menu/WBP_MainMenu`를 새로 만들고 게임 시작·게임 종료 버튼을 배치했다.
- 선택 로비의 세 프리뷰 폭을 각각 360px로 제한했다. 1920 기준 18.75%다.
- `UI_SelectedCharacterInfoText`를 제거했다.
- 프리뷰 아래에 360px 슬롯 버튼을 배치하고 빈 슬롯은 캐릭터 생성 화면을 연다.
- 캐릭터 프리뷰 또는 슬롯 버튼을 더블클릭하면 해당 프로필을 선택하고 기존 게임 접속 경로를 호출한다.
- 시작 시 HUD와 로비를 숨기고 Main Menu를 표시하며, 시작 버튼 이후 Lobby, 접속 이후 현재 Start Map 플레이 상태로 전환한다.

## 게임 흐름 범위

```text
Main Menu → Character Lobby → 현재 Start Map → (후속) Cell 단위 오픈월드
```

현재 저장소에는 독립된 World Partition 오픈월드 맵이 없으므로 Cell 단위 오픈월드 전환은 이번 구현에 포함하지 않았다. 다음 작업에서 오픈월드 맵, Data Layer, Cell 로딩 규칙과 진입 트리거를 정의해야 한다.

## 검증

- UE 5.8 `Rogue10mEditor Win64 Development` 빌드 성공
- `ValidateWidgetFlow.py` 통과
- `ValidateCharacterSelectionStage.py` 통과
- `ValidateCharacterLobbyFlowSuite.py`에서 위젯 흐름, 선택 스테이지, 커스터마이징, 상속 캐릭터 검증 통과
- Main Menu/Lobby 필수 Blueprint 바인딩과 새 경로 존재 확인
- `UI_SelectedCharacterInfoText` 부재와 Slot Preview 폭 384px 이하 확인

## 수동 QA 항목

- PIE 시작 시 Main Menu가 표시되는지 확인
- 게임 시작 클릭 시 Lobby가 표시되는지 확인
- 빈 슬롯 클릭 시 생성 화면으로 전환되는지 확인
- 캐릭터 프리뷰와 하단 슬롯 정보를 각각 더블클릭했을 때 접속되는지 확인
- 하단 게임 접속·캐릭터 삭제 버튼이 정상 동작하는지 확인
- 게임 종료 버튼은 PIE가 아닌 Standalone/패키지 환경에서도 확인
## 2026-07-28 UI 후속 보완

- `UI_StatusText`를 C++ BindWidget과 `WBP_CharacterLobby`에서 제거하고 상태 알림은 프로젝트 로그로만 남긴다.
- 캐릭터 프리뷰의 초기 Yaw를 0도로 변경해 세 캐릭터가 정면을 바라보도록 했다.
- 로비 배경 브러시가 원본 크기에 고정되지 않고 1920×1080 전체 캔버스에 맞춰 늘어나도록 설정했다.
- `ValidateCharacterSelectionStage.py`가 `UI_StatusText`, `UI_SelectedCharacterInfoText` 부재와 16:9 배경·3개 프리뷰 구성을 검증한다.
## 2026-07-29 슬롯 입력 및 프리뷰 캡처 수정

- `UI_BottomBar` ZOrder 4가 ZOrder 0인 슬롯·삭제·접속 버튼 위에서 Hit Test를 가로막던 문제를 수정했다.
- 버튼 ZOrder를 8로 올리고 장식 Border·프리뷰 Image를 `SelfHitTestInvisible`로 변경했다.
- `UI_LobbyBackgroundImage`를 유지하고 최소 `(0,0)`·최대 `(1,1)` Stretch 앵커와 0 오프셋으로 전체 화면을 채우도록 수정했다.
- 게임 일시정지 중에도 프리뷰 메시·SceneCapture가 갱신되도록 Tick Even When Paused를 적용했다.
- 메시와 조명을 켠 뒤 수동 캡처하도록 `SetPreviewActive` 순서를 수정하고, Retarget AnimBP 누락 시에도 기준 포즈 메시를 표시한다.
- 열린 Unreal Editor에 공식 Python 원격 실행으로 WBP를 재구성했으며 강화된 Validator를 통과했다.
## 2026-07-29 프리뷰 로드 및 게임플레이 입력 보완

프리뷰 소스 Manny 메시의 Soft Object Path를 전체 오브젝트 경로로 수정했다. 기존 패키지 전용 경로는 에디터 검색에서는 자산이 보여도 런타임 `LoadSynchronous()`에서 메시를 반환하지 못해 프리뷰 초기화가 중단될 수 있었다.

Main Menu와 Lobby의 이동·시점 입력 차단은 카운터 방식이므로 중복 호출되지 않게 보호했다. 캐릭터 접속 시에는 두 차단 카운터를 완전히 초기화하고 게임 일시정지를 해제한다. 접속 직후 `Gameplay input restored` 로그에서 `MoveIgnored=0`, `LookIgnored=0`, `Paused=0`을 확인할 수 있다.

UE 5.8 에디터 타깃 빌드는 성공했다. 열린 에디터에는 이전 모듈이 로드되어 있으므로 재시작 후 PIE에서 세 프리뷰 표시와 WASD 이동을 최종 확인한다.

## 2026-07-29 로비 프리뷰 정면 고정 및 투명 배경 보완

### 결과

캐릭터 선택 로비의 세 프리뷰를 모두 정면 고정으로 변경하고 360×720 동일 프레임으로 통일했다. Scene Capture가 만들던 검은 직사각형은 전용 UI 투명 합성 머티리얼로 제거해 로비 배경 위에는 캐릭터만 표시되도록 구성했다.

### 주요 변경

- 프리뷰 기준 회전: Yaw -90도 고정
- 세 Slot Preview: Y=15, 360×720 동일 크기
- Render Target: RGBA16f + SceneColor HDR/Inverse Opacity
- 신규 자산: /Game/Material/UI/M_CharacterPreviewTransparent
- 배경 전체 Shade 알파 0, Slot Stage Glow 제거
- Lobby 마우스 드래그 회전 제거
- 캐릭터/슬롯 더블클릭 접속 유지
- Equipment Window의 별도 마우스 회전 유지

### 검증

- UE 5.8 Rogue10mEditor Win64 Development 빌드 성공
- ValidateCharacterSelectionStage.py: 오류 0건
- ValidateCharacterLobbyFlowSuite.py: 오류 0건
- 투명 UI Material Domain/Translucent 설정과 Lobby 기본 참조 확인
- 세 프리뷰 위치·가로·세로 크기 일치 확인
- 금지 위젯 UI_Slot1~3StageGlow 부재 확인

### PIE 확인

- Menu Map에서 Character Lobby 정상 표시
- 로비 프리뷰 액터 4개 생성 확인: 생성 화면용 1개 + 슬롯용 3개
- Character preview source mesh/AnimBP 로드 오류 0건
- Character preview transparency material 로드 오류 0건