# Menu Map 기반 UI와 인게임 전환 구현 결과

## 결과

프로젝트 시작 맵을 `/Game/Rogue10m/Maps/L_Menu`로 분리했다. 이 맵은 플레이 Pawn과 인게임 HUD를 만들지 않고 메인 메뉴와 캐릭터 로비만 표시한다. 캐릭터 접속을 선택하면 선택 프로필을 유지한 채 `/Game/FirstPerson/Lvl_FirstPerson?StartRun=1`을 열고, 인게임 GameMode가 선택된 캐릭터 클래스를 Spawn/Possess한다.

## 주요 변경

- `ARogue10mMenuGameMode`: UI 전용 GameMode, Pawn·HUD·Spectator 생성 비활성
- `ARogue10mMenuPlayerController`: Blueprint 경로에 의존하지 않는 concrete 메뉴 Controller
- `ARogue10mPlayerController`: Menu World와 Gameplay World 초기화 분리
- 캐릭터 접속 시 `GameplayMap` Soft Object Path를 이용한 `OpenLevel`
- `L_Menu.umap` 생성 및 World Settings에 Menu GameMode 지정
- `EditorStartupMap`과 `GameDefaultMap`을 `L_Menu`로 변경
- Main Menu 배경·Shade를 전체 화면 Stretch로 변경
- 제목, 부제, 시작·종료 버튼 크기 확대

## 최종 실행 흐름

```text
L_Menu
  → Main Menu
  → Character Lobby
  → 선택 프로필 저장
  → Lvl_FirstPerson?StartRun=1
  → 선택 CharacterClass Spawn/Possess
  → 인게임 HUD + Enhanced Input
```

## 검증

- UE 5.8 `Rogue10mEditor Win64 Development` 빌드 성공
- `ValidateMenuMapFlow.py` 통과
- `ValidateWidgetFlow.py` 통과
- 실제 PIE 자동 검증 통과
  - Menu Map에서 Lobby 열림
  - `Lvl_FirstPerson` 레벨 이동 성공
  - Pawn: `BP_Rogue10m_HumanMaleCharacter_C`
  - `MoveIgnored=False`
  - `LookIgnored=False`
  - `Paused=False`

## 참고

에디터의 작은 도킹 PIE 창은 게임 뷰포트 자체가 작은 것이므로 UI가 그 크기에 맞게 축소된다. Standalone Game 또는 패키지 실행에서는 `L_Menu`가 게임 창 전체를 차지하며, 배경은 Stretch 앵커로 전체 뷰포트를 채운다.
