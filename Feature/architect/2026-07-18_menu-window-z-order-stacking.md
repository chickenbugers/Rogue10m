# Sprint#2-1 메뉴 창 호출 순서 ZOrder 설계

## 목표

- 인벤토리와 장비창이 겹칠 때 더 늦게 열린 창을 위에 표시한다.
- 겹치지 않은 노출 영역을 클릭하면 해당 창을 최상단으로 올린다.
- 여러 번 호출해도 메뉴 창 ZOrder가 다른 HUD 계층을 침범하지 않도록 제한된 범위에서 재정렬한다.
- Game and UI 입력 Focus가 현재 최상단 메뉴 창을 따른다.

## 현재 문제

- 인벤토리, 장비, 스킬트리 창은 모두 `AddToPlayerScreen(50)`으로 생성된다.
- 같은 ZOrder에서는 생성 순서에 따라 표시되어 실제 호출 순서가 반영되지 않는다.
- `RefreshInputMode()`는 인벤토리를 항상 장비창보다 먼저 Focus 대상으로 선택한다.

## 구현 계약

1. PlayerController가 메뉴 창의 비소유 참조를 `TWeakObjectPtr` 스택으로 관리한다.
2. 창을 열거나 노출 영역을 클릭하면 기존 위치에서 제거한 뒤 스택 마지막으로 이동한다.
3. 스택 순서대로 `MenuWindowBaseZOrder + Index`를 다시 부여한다.
4. 인벤토리와 장비창 동시 표시 상태에서는 나중에 호출된 창이 항상 더 높은 ZOrder를 갖는다.
5. 공통 `NativeOnPreviewMouseButtonDown()`에서 자식 아이템/슬롯이 입력을 처리하기 전 창을 앞으로 올린다.
6. `RefreshInputMode()`는 스택을 역순으로 조회해 가장 위에 열린 창을 Focus한다.

## Ultrawork Packets

### Packet 1 - PlayerController 창 스택

- 목표: 호출 순서 기반 ZOrder 재배치와 최상단 창 조회를 구현한다.
- 수정 위치: `Rogue10mPlayerController.*`
- 완료 조건: 인벤토리→장비 호출 시 장비가 위, 장비→인벤토리 호출 시 인벤토리가 위다.
- 검증: Editor 빌드, 정적 소스 계약 확인.
- 롤백 경계: 메뉴 창 스택 멤버와 승격 함수.

### Packet 2 - 공통 클릭 승격

- 목표: 창 내부 자식 입력보다 먼저 공통 창 승격을 처리한다.
- 수정 위치: `Rogue10mMenuWindowWidgets.*`
- 완료 조건: 노출된 창의 제목, 빈 공간, 아이템/장비 슬롯 클릭이 모두 해당 창을 최상단으로 만든다.
- 검증: WBP 컴파일, PIE 수동 클릭 확인.
- 롤백 경계: Preview Mouse override.

### Packet 3 - 검증과 기록

- 목표: 빌드, Widget 계약, 생성물 검사를 통과하고 작업 결과를 기록한다.
- 수정 위치: Feature 문서, DevLog, SprintChangeLog.
- 완료 조건: C++ 빌드와 commandlet 검증 성공, 잔여 수동 QA 명시.
- 검증: `BuildEditor.ps1`, `ValidateMenuWidgetAssets.py`, `CheckGeneratedChanges.ps1`, `git diff --check`.
- 롤백 경계: 문서 변경.

## Viewport 재등록 방지 보완

- 메뉴 승격은 이미 Viewport에 등록된 위젯을 다시 `AddToPlayerScreen()`하지 않는다.
- 최초 등록되지 않은 창만 `AddToPlayerScreen()`으로 추가한다.
- 등록된 창은 `UGameViewportSubsystem`에서 `FGameViewportWidgetSlot`을 조회하고 `ZOrder`만 갱신한다.
- 인벤토리 아이템 이동처럼 창의 Preview Mouse 입력이 반복되는 경우에도 메뉴 위젯 재등록 경고가 발생하지 않아야 한다.
- 완료 조건은 Editor C++ 빌드 성공과 PIE에서 `was already added to the screen` 경고가 재발하지 않는 것이다.
