# Sprint#1-13 메뉴 Widget Designer 직접 배치

## 목표

`WBP_InventoryWindow`, `WBP_InventoryCell`, `WBP_EquipmentWindow`, `WBP_SkillTreeEntry`, `WBP_SkillTreeWindow`의 UI를 C++ `RebuildWidget()` 런타임 생성이나 Overlay 기반 합성이 아닌 Widget Blueprint Designer 트리에 직접 배치한다.

## 범위

### Ultrawork Packet 1 - 구조 감사

- 수정 위치: `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.*`, `Content/Widget/Menu`
- 완료 조건: 네이티브 fallback과 `BindWidgetOptional` 계약, 기존 WBP 부모 클래스를 확인한다.
- 검증: `rg`, Unreal Editor Asset 조회
- 롤백 경계: 문서만 제거

### Ultrawork Packet 2 - Designer 트리 생성

- 수정 위치: `Scripts/Editor/BuildMenuDesignerLayouts.py`, 메뉴 WBP 4종과 Inventory Cell WBP
- 완료 조건:
  - Inventory: Canvas, Frame, 빈 `UniformGridPanel`, NxM 아이템 `CanvasPanel`, 하단 돈/무게 텍스트
  - InventoryCell: 컨테이너 GridSize X×Y만큼 런타임 생성되는 44×44 UserWidget
  - Equipment: 캐릭터 프리뷰, 장비 슬롯 7종, 능력치 영역
  - SkillTreeEntry: 아이콘, 이름, 설명, 잠금 상태
  - SkillTreeWindow: 필터, `ScrollBox`, `WrapBox` 스킬 목록, 스킬 포인트 영역
- 검증: UE5.8 `UMGToolSet.GetWidgets`, Blueprint compile/save
- 롤백 경계: 다섯 WBP와 스크립트

### Ultrawork Packet 3 - 런타임 fallback 정리

- 수정 위치: `Rogue10mMenuWindowWidgets.cpp/.h`
- 완료 조건: Designer 트리가 정상일 때 C++가 별도 UI를 생성하지 않으며, 데이터 갱신과 이벤트만 담당한다.
- 검증: `BuildEditor.ps1`
- 롤백 경계: C++ 변경만 되돌림

### Ultrawork Packet 4 - 문서와 QA

- 수정 위치: `Feature/doc`, `Docs/SprintChangeLog.md`, `DevLog/20260715.txt`
- 완료 조건: 실제 Designer 위젯 이름, 검증 결과, 후속 설정을 기록한다.
- 검증: `CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 문서 변경만 되돌림

## 설계 원칙

- 게임 데이터와 동작은 C++/Data Asset이 소유한다.
- 배치, 색상, 브러시, 애니메이션은 Widget Blueprint가 소유한다.
- C++ 필수 `BindWidget` 이름과 Designer 위젯 이름을 동일하게 유지한다.
- `.uasset`은 Unreal Editor의 UMG ToolSet/AssetTools로만 수정한다.
- Inventory의 X×Y `UniformGridPanel`과 NxM 아이템 `CanvasPanel`은 `UI_InventoryCanvas`의 동일 좌표에 형제 위젯으로 직접 배치하고 ZOrder로 구분한다. `Overlay`는 사용하지 않는다.

## 위험과 대응

- Hot Reload 부모 클래스가 남아 있으면 WBP 저장을 중지하고 Editor를 재시작한다.
- 기존 Designer 트리가 비어 있지 않으면 대상 위젯 목록을 확인한 뒤 스크립트가 관리하는 트리만 교체한다.
- 컴파일 오류가 발생하면 저장하지 않고 로그에 누락된 `BindWidget` 이름을 출력한다.

## Inventory 후속 설계

- `UI_BagTabContainer`, `UI_InventoryCapacityText`, `UI_InventoryHintText`는 제거한다.
- 셀은 Designer에 100개를 고정 배치하지 않고 `InventoryContainers[0].GridSize`의 X×Y에 맞춰 `WBP_InventoryCell`을 `NativeConstruct` 및 `OnInventoryGridChanged` 시 생성한다.
- `UI_InventoryGrid`는 셀 좌표와 배경을 담당하고, `UI_InventoryItemCanvas`는 여러 셀을 차지하는 NxM 아이템 위젯과 Drag & Drop을 담당한다.
- 아이템 Data Asset은 `UnitWeight`를 소유하며, 인벤토리 총 무게는 모든 컨테이너의 `UnitWeight × Quantity` 합으로 계산한다.
- 하단에는 `Gold`와 `현재 무게 / MaxCarryWeight`를 표시한다. 무게 초과 제한은 이번 범위에 포함하지 않는다.

## Ultrawork Packet 5 - NxM 아이템 위젯과 배치 상호작용

- 목표: `WBP_InventoryCell`, `WBP_InventoryItem`, `WBP_BagTab`을 역할별 UserWidget으로 구성하고 NxM 좌표 배치, 충돌 프리뷰, R키 90도 회전을 연결한다.
- 수정 영역: `Rogue10mInventoryComponent`, `Rogue10mItemDragDropOperation`, `Rogue10mMenuWindowWidgets`, 메뉴 WBP 생성 스크립트와 3개 WBP 자산.
- 완료 조건: 셀은 `UI_InventoryGrid`의 (X,Y)에 생성되고, 아이템은 `UI_InventoryItemCanvas`에서 `Position * 44`, `Footprint * 44`로 배치되며, 드래그 중 R 입력과 유효/충돌 색상 프리뷰가 동작한다.
- 검증: Editor 타깃 빌드, 열린 Editor에서 WBP 컴파일·저장 및 클래스 할당/Designer 이름 조회.
- 롤백 경계: C++ 배치/DragDrop 변경과 `WBP_InventoryItem`, `WBP_BagTab`, `WBP_InventoryWindow` 자산 변경.
## Ultrawork Packet 6 - 메뉴 자산 정리와 UI 계약 보정

- 목표: BagTab의 제거된 크기 텍스트 계약을 정리하고, Inventory Cell 구분선을 추가하며, 메뉴 자산을 기능별 폴더로 분리한다.
- 수정 영역: `Rogue10mMenuWindowWidgets`, 메뉴 WBP 7종, PlayerController 기본 경로, Editor 자동화, `DefaultEngine.ini`.
- 완료 조건: `UI_BagSizeText` 없이 WBP가 컴파일되고, 셀마다 0.5 외곽선이 보이며, 모든 메뉴 자산과 내부 클래스 참조가 새 폴더 경로를 사용한다.
- 검증: Editor 타깃 빌드, 깨끗한 Unreal commandlet WBP 컴파일, `LogEditCondition` 재발 여부 확인.
- 롤백 경계: BagTab C++ 계약, 셀 Designer 트리, 메뉴 자산 이동 및 대응 경로 변경.
## Ultrawork Packet 7 - Inventory Grid 시각 균형 개선

- 목표: 마비노기/Diablo II식 NxM 그리드의 촘촘한 셀과 독립 프레임 구조를 참고해 Inventory Window의 겹침과 불균형을 제거한다.
- 입력: 472×481 PIE 스크린샷, 현재 44×44 Cell과 440×440 Grid Designer 치수.
- 수정 영역: `BuildMenuDesignerLayouts.py`, `ValidateMenuWidgetAssets.py`, `WBP_InventoryCell`, `WBP_BagTab`, `WBP_InventoryWindow`.
- 완료 조건: 셀은 선명한 1px 다크 경계를 사용하고, 탭·448×448 Grid Frame·하단 정보가 겹치지 않으며 좌우 여백이 대칭이어야 한다.
- 검증: UE5.8 Designer 재생성, 별도 commandlet WBP 컴파일 및 필수 위젯/클래스 참조 검사.
- 롤백 경계: 위 세 Inventory WBP와 대응 Editor Python 레이아웃 값.
