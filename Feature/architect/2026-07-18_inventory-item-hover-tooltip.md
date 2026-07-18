# Sprint#2-2 인벤토리 아이템 Hover 툴팁 설계

## 목표

- 인벤토리 아이템에 마우스를 올리면 별도 UserWidget으로 구성된 설명 UI를 표시한다.
- 툴팁은 Item Data Asset의 아이템 이름, 설명, 무게를 단일 데이터 원본으로 사용한다.
- 수량이 여러 개인 아이템은 개당 무게와 현재 묶음의 총 무게를 구분해 보여준다.
- 툴팁은 마우스 입력을 가로채지 않고 Unreal 기본 Tooltip 배치와 화면 경계 보정을 사용한다.

## UI 계약

- C++ 부모 클래스: `URogue10mInventoryItemTooltipWidget`
- Widget Blueprint: `/Game/Widget/Menu/Inventory/WBP_InventoryItemTooltip`
- 필수 위젯:
  - `UI_ItemNameText`
  - `UI_ItemDescriptionText`
  - `UI_ItemWeightText`
- 기본 크기: 너비 300px, 내용에 따른 높이
- 이름: 18px, 밝은 강조색
- 설명: 13px, 자동 줄바꿈
- 무게: 12px, 보조색
- 전체 루트: `HitTestInvisible`

## 런타임 흐름

1. `URogue10mInventoryItemWidget::InitializeGridItem()`이 Item Data와 수량을 받는다.
2. 설정된 `InventoryItemTooltipWidgetClass`로 툴팁 UserWidget을 생성한다.
3. `InitializeItemTooltip()`이 이름·설명·무게 TextBlock을 갱신한다.
4. 아이템 위젯의 `SetToolTip()`에 툴팁을 연결한다.
5. Unreal Slate Tooltip 시스템이 Hover 지연, 마우스 추적, 화면 경계 배치를 담당한다.
6. Drag Preview에는 툴팁을 제거해 불필요한 Hover UI를 만들지 않는다.

## Ultrawork Packets

### Packet 1 - Tooltip C++ 계약

- 목표: Item Data를 표시하는 독립 UserWidget 부모 클래스와 아이템 연결 속성을 추가한다.
- 수정 위치: `Rogue10mMenuWindowWidgets.*`
- 완료 조건: 이름·설명·무게 포맷이 C++에서 결정되고 아이템 위젯에 Tooltip이 연결된다.
- 검증: `BuildEditor.ps1`
- 롤백 경계: Tooltip 클래스와 `InitializeGridItem()` 연결 코드.

### Packet 2 - Widget Blueprint 자산

- 목표: 필수 TextBlock을 가진 `WBP_InventoryItemTooltip`을 생성하고 기존 Inventory Item 기본값에 연결한다.
- 수정 위치: `Scripts/Editor`, `Content/Widget/Menu/Inventory`
- 완료 조건: WBP 컴파일 성공, 클래스 참조가 `WBP_InventoryItem` CDO에 저장된다.
- 검증: 좁은 Editor Python 생성 스크립트와 `ValidateMenuWidgetAssets.py`.
- 롤백 경계: 새 WBP와 Editor 스크립트 변경.

### Packet 3 - 리뷰와 기록

- 목표: 빌드·WBP·생성물 검사를 수행하고 결과와 수동 QA를 기록한다.
- 수정 위치: `Feature/doc`, `DevLog/20260718.txt`, `Docs/SprintChangeLog.md`
- 완료 조건: 자동 검증 결과와 PIE 확인 목록이 남는다.
- 검증: `CheckGeneratedChanges.ps1`, `git diff --check`.
- 롤백 경계: 문서 변경.

## 수동 QA

1. 1x1, 2x3, 4x3 아이템 어디에 Hover해도 툴팁이 표시되는지 확인한다.
2. 아이템 이름과 설명이 Data Asset 값과 일치하는지 확인한다.
3. 단일 수량은 `무게: N kg`, 복수 수량은 `무게: N kg × Q = T kg`로 보이는지 확인한다.
4. 화면 가장자리 아이템의 툴팁이 화면 밖으로 잘리지 않는지 확인한다.
5. Hover 후 Drag & Drop과 창 클릭 승격이 그대로 동작하는지 확인한다.
