# Menu Map 기반 UI와 인게임 전환 설계

## 목표

메인 메뉴와 캐릭터 선택을 독립된 `L_Menu` 맵에서 전체 화면 UI로 제공하고, 선택한 캐릭터로 접속할 때 실제 인게임 맵 `Lvl_FirstPerson`을 연다.

## 실행 흐름

```text
L_Menu
  -> Main Menu
  -> Character Lobby
  -> 선택 프로필 SaveGame 저장
  -> OpenLevel(Lvl_FirstPerson?StartRun=1)
  -> Rogue10mGameMode가 선택 CharacterClass Spawn/Possess
  -> 인게임 HUD와 Enhanced Input 활성화
```

## Ultrawork Packet 1 - Menu 전용 런타임

- 목표: 메뉴 맵에서 플레이 Pawn과 인게임 HUD를 만들지 않는다.
- 수정 위치: `Source/Rogue10m/Core`
- 완료 조건: Menu GameMode는 같은 PlayerController를 사용하되 Pawn/HUD를 생성하지 않고, Controller는 메뉴 월드에서만 Main Menu와 Lobby를 초기화한다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: Menu GameMode 신규 클래스와 PlayerController 월드 분기

## Ultrawork Packet 2 - 캐릭터 선택 후 레벨 이동

- 목표: 선택 캐릭터 정보를 유지한 채 인게임 맵으로 이동한다.
- 수정 위치: `Rogue10mPlayerController`, `Rogue10mCharacterProfileSubsystem`
- 완료 조건: 유효한 선택 프로필이 있을 때 `Lvl_FirstPerson`을 열고, 새 월드의 GameMode가 해당 캐릭터를 기본 Pawn으로 생성한다.
- 검증 명령: UE 5.8 빌드 및 PIE 로그 확인
- 롤백 경계: `EnterSelectedCharacter`의 OpenLevel 경로

## Ultrawork Packet 3 - Menu Map과 반응형 UI

- 목표: Menu Map과 UI가 게임 뷰포트를 가득 사용한다.
- 수정 위치: `Content/Rogue10m/Maps`, `Config/DefaultEngine.ini`, `Scripts/Editor`
- 완료 조건: `L_Menu.umap` 존재, 기본 게임 맵이 `L_Menu`, 배경과 루트가 Stretch 앵커, 주요 UI가 1920x1080 기준으로 더 크게 배치된다.
- 검증 명령: Unreal Python Validator와 PIE
- 롤백 경계: 신규 Menu Map, 맵 설정, 위젯 레이아웃

## 위험과 대응

- `OpenLevel`은 PlayerState를 다시 만들지만 선택 프로필은 GameInstanceSubsystem과 SaveGame에 있으므로 유지된다.
- Menu Map에서 기본 Pawn이 생성되면 카메라와 입력이 불필요하게 초기화되므로 Menu GameMode의 Pawn을 비운다.
- 에디터가 열려 있을 때 C++ Hot Reload 클래스가 맵에 저장되지 않도록 빌드 후 에디터를 재시작하고 맵 자산을 생성한다.
