# Sprint#2-1 메뉴 창 호출 순서 ZOrder 구현 결과

## 구현 결과

- 인벤토리와 장비창을 포함한 메뉴 창을 제한된 ZOrder 스택으로 관리하도록 변경했다.
- 창을 열면 해당 창을 스택 마지막으로 이동시키고 `50 + StackIndex` 순서로 재배치한다.
- 이미 열린 창의 노출 영역을 좌클릭해도 자식 위젯이 입력을 소비하기 전에 해당 창이 최상단으로 올라간다.
- 인벤토리와 장비창이 동시에 열린 상태에서 다른 창을 열어도 기존 창이 불필요하게 다시 승격되지 않도록 실제 표시 상태가 바뀔 때만 갱신한다.
- 키보드 Focus도 고정된 인벤토리 우선순위 대신 현재 최상단에 열린 메뉴 창을 따른다.
- Blueprint 또는 C++에서 `SetWindowOpen(true)`를 직접 호출하는 경로도 동일한 승격 규칙을 적용한다.

## 구현 세부 사항

- `ARogue10mPlayerController`
  - `TWeakObjectPtr` 기반 `MenuWindowStack` 추가
  - `BringMenuWindowToFront()` 및 `GetTopmostOpenMenuWindow()` 추가
  - 인벤토리·장비창 전용 동시 표시 처리에서도 활성 창을 전달하도록 수정
  - 스택을 계속 증가시키지 않고 현재 메뉴 수만큼 `50~52` 범위에서 재정렬
- `URogue10mMenuWindowWidget`
  - 창이 열릴 때 소유 PlayerController에 최상단 승격 요청
  - `NativeOnPreviewMouseButtonDown()`에서 좌클릭 승격 처리

## 검증 결과

- `Rogue10mEditor Win64 Development` 빌드 성공
- `Scripts/Editor/ValidateMenuWidgetAssets.py` 성공
  - 전체 메뉴 Widget Blueprint 컴파일 성공
  - 0 errors / 0 warnings
- UE 5.8의 `UGameViewportSubsystem::AddWidgetForPlayer()`가 이미 등록된 위젯의 기존 슬롯을 조회하고 새 ZOrder로 갱신하는 구현을 확인했다.
- 실제 겹침 순서와 클릭 승격은 PIE에서 수동 상호작용 확인이 남아 있다.

## PIE 확인 항목

1. 인벤토리를 먼저 열고 장비창을 열면 장비창이 위에 표시되는지 확인한다.
2. 장비창을 먼저 열고 인벤토리를 열면 인벤토리가 위에 표시되는지 확인한다.
3. 아래에 있는 창의 노출 영역·아이템·장비 슬롯을 좌클릭하면 그 창이 위로 올라오는지 확인한다.
4. 승격 후에도 아이템 및 장비 Drag & Drop과 창 이동이 정상 동작하는지 확인한다.
5. 창을 닫았을 때 남아 있는 최상단 창으로 입력 Focus가 이동하는지 확인한다.

## 상태

- 코드 구현, C++ 빌드, Widget Blueprint 정적 검증 완료
- PIE 수동 상호작용 QA 대기

## 중복 화면 등록 경고 수정

- 증상: 인벤토리 아이템 이동 시 인벤토리·장비·스킬트리 위젯의 `was already added to the screen` 경고가 반복 출력됐다.
- 원인: 공통 Preview Mouse 입력이 `BringMenuWindowToFront()`를 호출하고, 이 함수가 스택의 모든 메뉴 위젯에 `AddToPlayerScreen()`을 다시 호출했다.
- 수정:
  - 메뉴 초기화에서는 `BringMenuWindowToFront()`가 최초 등록까지 담당하도록 진입점을 통합했다.
  - 이미 등록된 위젯은 `UGameViewportSubsystem::GetWidgetSlot()`으로 슬롯을 조회한다.
  - 변경된 `FGameViewportWidgetSlot::ZOrder`만 `SetWidgetSlot()`으로 반영한다.
  - Viewport에 없는 위젯만 `AddToPlayerScreen()`으로 추가한다.
- 검증: `Rogue10mEditor Win64 Development` 빌드 성공, `git diff --check` 통과.
- 수동 확인: 최신 DLL을 로드한 PIE에서 인벤토리 아이템을 여러 번 이동하고 동일 경고가 출력되지 않는지 확인한다.

