# 인벤토리·장비 UI 비율 및 Drag & Drop 보정 결과

## 결과

MxN 인벤토리 아이콘의 상대 크기 규칙을 통일하고, Hover Tooltip을 축소했으며, 인벤토리 장비를 대응 장비 슬롯에 직접 Drop해 장착하는 경로를 추가했다. 장비창에서 시작하는 Drag Visual은 커서 중심에서 시작하도록 변경했다.

## 주요 변경

### 인벤토리 아이콘

- 아이템 위젯 전체 크기는 `InventorySize × CellSize`를 유지한다.
- 아이콘 Fit 영역은 가로·세로 각각 전체 footprint의 84%로 계산한다.
- 원본 Texture 종횡비를 유지하고 `InventoryIconScale`은 0.75~1.0 범위에서만 보정한다.
- Texture 원본 해상도가 UMG Desired Size를 덮어쓰지 않도록 Match Size를 비활성화했다.
- 스타터 아이템 6종 Data Asset의 `InventoryIconScale`을 1.0으로 통일했다.

### Hover Tooltip

- 기본 폭을 300px에서 280px로 축소했다.
- 장착 장비 비교 폭을 620px에서 580px로 축소했다.
- 아이콘은 원본 Texture 크기와 무관하게 52×52px로 고정했다.
- 하단 안내 문구를 별도 `UI_ItemActionHintText` UserWidget 요소로 추가했다.
- 인벤토리 장비는 `우클릭 : 장착하기`, 소비 아이템은 `우클릭 : 사용하기`, 장착 장비는 `우클릭 : 장착 해제`를 표시한다.

### 인벤토리에서 장비창으로 Drop

- 장비창이 `GridInventory` Drag Operation을 받도록 DragOver·Drop·DragLeave를 구현했다.
- 커서 아래 장비 슬롯과 아이템 `EquipSlotType`이 일치할 때만 장착할 수 있다.
- 호환 슬롯은 초록색, 비호환 슬롯은 빨간색 Preview를 표시한다.
- 성공 시 기존 `TryEquipGridItem`을 사용해 동일 부위 교체와 인벤토리 반환을 원자적으로 처리한다.
- 성공·실패·이탈·창 닫기·장비 변경 시 슬롯 및 Payload Preview 상태를 정리한다.

### 장비 Drag Visual

- 장비창 전체 Widget 기준 `MouseDown` offset을 제거했다.
- Drag Visual Pivot을 `CenterCenter`로 바꿔 Payload가 커서 중심에서 시작한다.

## 자산 및 도구

- `WBP_InventoryItemTooltip`을 26개 위젯 구조로 재생성·컴파일·저장했다.
- 스타터 아이템만 갱신하는 `NormalizeStarterItemIconScales.py`를 추가했다.
- Validator에 Tooltip 280px, 안내 문구 기본 Collapsed, 스타터 스케일 1.0 검증을 추가했다.

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공
- Python 스크립트 문법 검사 성공
- `WBP_InventoryItemTooltip` 필수 위젯 14개 및 전체 26개 계층 검증 성공
- 전체 메뉴 Widget Blueprint 참조·계층 검증 성공
- 스타터 아이템 6종 `InventoryIconScale == 1.0` 검증 성공
- 별도 UE 명령형 에디터 검증 성공
- `git diff --check` 통과

## 수동 확인 필요

현재 실행 중인 에디터는 빌드 이전 네이티브 모듈을 로드하고 있으므로 에디터 재시작 후 PIE 검증이 필요하다.

1. 1x1·1x3·2x2·2x3·4x3 아이콘의 상대 여백과 종횡비
2. 장비·소비 아이템 Tooltip 크기와 하단 우클릭 안내
3. 인벤토리 장비를 대응 슬롯에 Drop했을 때 장착·교체
4. 비대응 슬롯의 빨간 Preview와 장착 거부
5. 장비 Drag Payload가 커서 중심에서 시작하는지
6. 장비에서 인벤토리로 Drop, Hover, 우클릭 장착 해제 회귀

## 관련 문서

- `Feature/architect/2026-07-18_inventory-equipment-ui-polish.md`
- `DevLog/20260718.txt`
- `Docs/SprintChangeLog.md`

## 추가 보정 — 장비 Drag Payload 좌측 상단 비행

`CenterCenter` Pivot만으로는 사라지지 않던 Payload의 좌측 상단 출발 현상을 UE 5.8의 Drag Decorator 생성 경로까지 추적했다. 원인은 전체 화면 장비창이 Drag Source였기 때문에 엔진이 Viewport 좌측 상단을 `StartingScreenPos`로 사용하고, 첫 0.15초 동안 해당 위치에서 커서까지 보간한 것이었다.

### 변경 결과

- 장착 아이템이 있는 7개 슬롯 위에 네이티브 투명 Drag Source 프록시를 미리 배치했다.
- 프록시가 기존 MouseDown Hit-Test 경로에 포함된 상태에서 같은 Widget을 커서 중심으로 이동한 후 Drag를 감지한다.
- 화면 좌표를 슬롯 부모 Canvas 로컬 좌표로 변환해 DPI 배율과 실제 Decorator 시작 좌표를 일치시켰다.
- 기존 Payload의 `CenterCenter` Pivot, M×N 크기, 아이콘 종횡비는 그대로 유지했다.
- Drop·Drag Cancel·클릭 종료·창 닫기에서 프록시를 원래 슬롯으로 복원하고, 장비 갱신·Destruct에서 제거한다.
- 인벤토리·장비창 Asset을 다시 만들지 않고 C++ 경로만 보정했다.

### 검증

- UE 5.8 `SObjectWidget`과 `FUMGDragDropOp` 소스의 시작 좌표 및 0.15초 보간 경로 대조 완료
- `Rogue10mEditor Win64 Development` UHT/C++ 빌드 성공
- `git diff --check` 및 `CheckGeneratedChanges.ps1` 통과
- 새 `UnrealEditor-Cmd` 프로세스에서 `ValidateMenuWidgetAssets.py`의 전체 메뉴 Widget 검증 통과
- 실제 커서 추적 감각과 1×1/M×N Payload 정렬은 최신 빌드 모듈로 Editor를 재실행한 뒤 PIE 수동 QA 필요
