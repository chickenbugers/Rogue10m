# 메인 메뉴와 3인 캐릭터 로비 흐름 설계

## Scope Gate

기존 캐릭터 생성·선택·삭제·접속 기능을 유지하면서 위젯 폴더의 책임을 분리하고, 게임 최초 진입 화면을 기본 메인 메뉴로 변경한다. 로비는 최대 3명의 캐릭터를 동시에 보여주며 각 프리뷰 폭은 1920 기준 360px(18.75%)로 제한한다.

## 목표 흐름

```text
WBP_MainMenu
  ├─ 게임 시작 → WBP_CharacterLobby
  └─ 게임 종료 → QuitGame

WBP_CharacterLobby
  ├─ 캐릭터 선택/생성/삭제
  ├─ 캐릭터 또는 하단 슬롯 정보 더블클릭 → 게임 접속
  └─ 게임 접속 버튼 → 현재 Start Map 플레이

Start Map
  └─ 후속 개발: World Partition 기반 Cell 단위 오픈월드 진입
```

현재 프로젝트에는 독립적인 오픈월드 맵이 없으므로 이번 작업은 `Menu → Lobby → 현재 Start Map`의 실행 흐름까지 구현한다. `Start Map → Cell 단위 오픈월드`는 World Partition 맵과 이동 규칙이 정해지는 후속 작업의 경계로 남긴다.

## 위젯 경로 역할

| 역할 | 경로 | 내용 |
|---|---|---|
| 최초 메뉴 | `Content/Widget/Menu` | `WBP_MainMenu`와 향후 메인 메뉴 전용 화면 |
| 캐릭터 로비 | `Content/Widget/Lobby` | `WBP_CharacterLobby` |
| 재사용 UI | `Content/Widget/Component` | Inventory, Equipment, SkillTree 위젯 |

## UI 규칙

- 선택 화면의 `UI_Slot1PreviewImage`~`UI_Slot3PreviewImage` 폭은 각각 360px로 둔다.
- `UI_SelectedCharacterInfoText`는 제거한다.
- 각 프리뷰 바로 아래에 동일 폭의 `UI_SlotNButton`을 배치한다.
- 캐릭터가 있는 슬롯은 이름·종족·성별을 표시하고, 빈 슬롯은 `캐릭터 생성하기`를 표시한다.
- 게임 접속·캐릭터 삭제 버튼은 하단 작업 영역에 유지한다.
- 캐릭터 프리뷰 더블클릭과 하단 슬롯 버튼 더블클릭은 동일한 접속 경로를 호출한다.

## 책임 분리

- `URogue10mMainMenuWidget`: 시작·종료 버튼 이벤트만 담당한다.
- `ARogue10mPlayerController`: Main Menu/Lobby/In-game UI 상태와 입력 모드를 전환한다.
- `URogue10mCharacterLobbyWidget`: 프로필 슬롯, 생성 모드, 선택 및 더블클릭 접속을 담당한다.
- Blueprint 자식은 레이아웃·텍스트·색상·배경 에셋을 구성한다.

## Ultrawork Packets

### Packet 1 — 경로 마이그레이션

- 대상: `Content/Widget/Menu`, `Content/Widget/Character/WBP_CharacterLobby`
- 완료 조건: 공용 위젯은 Component, 로비는 Lobby, 새 메뉴는 Menu에 존재
- 검증: `ValidateWidgetFlow.py`
- 롤백 경계: 에디터 자산 이동과 Redirector 정리

### Packet 2 — 로비 상호작용

- 대상: `Rogue10mCharacterLobbyWidget.*`, `WBP_CharacterLobby`
- 완료 조건: 360px 프리뷰 3개, 하단 슬롯 버튼, 빈 슬롯 생성, 더블클릭 접속
- 검증: Editor 빌드, `ValidateCharacterSelectionStage.py`
- 롤백 경계: Lobby C++ 및 WBP

### Packet 3 — 메인 메뉴와 화면 상태

- 대상: `Rogue10mMainMenuWidget.*`, `Rogue10mPlayerController.*`, `WBP_MainMenu`
- 완료 조건: 시작 시 Main Menu 표시, 시작 버튼으로 Lobby 표시, 종료 버튼으로 게임 종료
- 검증: Editor 빌드, `ValidateWidgetFlow.py`
- 롤백 경계: Main Menu C++/WBP와 Controller 상태 전환

### Packet 4 — 회귀 검증과 문서화

- 대상: Validator, Feature 문서, SprintChangeLog, DevLog
- 완료 조건: 커스터마이징·상속 캐릭터·공용 메뉴 위젯 검증 유지
- 검증: `ValidateCharacterLobbyFlowSuite.py`, `CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 스크립트와 문서

## 안전 조건

- `.uasset` 이동과 생성은 Unreal Editor API만 사용한다.
- `Config/DefaultGame.ini`의 기존 사용자 변경은 수정하지 않는다.
- 로비가 없거나 메인 메뉴 생성에 실패하면 PlayerController는 안전하게 기존 로비 경로로 폴백한다.
- Cell 오픈월드는 맵·World Partition 정책 없이 임의 구현하지 않는다.

## 2026-07-29 로비 프리뷰 고정·투명 합성 설계

### 목표

- 세 저장 캐릭터는 모두 카메라 정면을 바라보는 고정 포즈로 표시한다.
- 세 슬롯의 프리뷰 영역은 동일한 Y 위치와 동일한 360×720 크기를 사용한다.
- Scene Capture의 검은 배경은 로비 배경 위에 합성되지 않도록 알파로 제거한다.
- 마우스 드래그 회전은 장비창 전용 동작으로 유지하고 로비에서는 제거한다.

### 구현 결정

- 프리뷰 루트의 기준 Yaw를 -90도로 고정해 Stylized Character의 전방축을 캡처 카메라와 맞춘다.
- Scene Capture는 SCS_SceneColorHDR의 RGB와 Inverse Opacity 알파를 출력한다.
- UI Domain Translucent 머티리얼에서 RGB를 Final Color로, OneMinus(Alpha)를 Opacity로 연결한다.
- 로비 위젯은 각 Render Target마다 동적 머티리얼 인스턴스를 만들어 PreviewTexture 파라미터로 주입한다.
- 로비의 마우스 Down/Move/Up/CaptureLost 회전 경로와 상태를 제거하고 더블클릭 접속 판정만 유지한다.
- 장비창의 ARogue10mEquipmentPreviewActor::AddPreviewYaw 경로는 변경하지 않는다.

### 완료 조건

- 세 슬롯 프리뷰가 모두 Y=15, Width=360, Height=720을 사용한다.
- UI_Slot#StageGlow와 전체 배경 Shade 알파가 제거된다.
- 투명 합성 머티리얼과 Lobby CDO 참조가 존재한다.
- UE 5.8 Editor 빌드와 캐릭터 로비 전체 Validator가 통과한다.