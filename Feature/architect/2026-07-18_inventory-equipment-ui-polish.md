# 인벤토리·장비 UI 비율 및 DragDrop 보정 설계

## 목표

- 모든 MxN 인벤토리 아이콘에 동일한 상대 여백과 Fit 규칙을 적용한다.
- Hover Tooltip을 컴팩트하게 제한하고 하단에 우클릭 동작 안내를 표시한다.
- 인벤토리 장비를 대응하는 장비 슬롯에 Drag & Drop으로 장착한다.
- 장비에서 시작한 Drag Payload를 마우스 중심에 고정한다.

## 원인

- Tooltip SetBrushFromTexture의 Match Size가 원본 Texture 크기로 썸네일 Desired Size를 덮어썼다.
- 인벤토리 아이콘이 MxN과 관계없이 고정 4px 여백을 사용해 상대 비율이 달랐다.
- 스타터 아이템 InventoryIconScale 값이 0.85~0.95로 서로 달랐다.
- Equipment Window에 GridInventory Operation용 NativeOnDrop 경로가 없었다.
- Equipment Window 전체가 Drag Source인데 MouseDown Pivot을 사용해 전체 창 기준 offset이 적용됐다.

## Ultrawork Packets

### Packet 1 - 아이콘과 Tooltip

- 목표: 상대 여백 Fit, 공통 Scale, 고정 Tooltip 썸네일, 동적 우클릭 안내
- 수정 위치: Rogue10mMenuWindowWidgets, Item DataAsset 설정, Tooltip WBP
- 완료 조건: 아이콘이 footprint 안에서 같은 규칙을 사용하고 Tooltip이 원본 Texture 크기에 영향받지 않음
- 검증: Editor 빌드, WBP Validator
- 되돌리기 경계: 아이콘 계산·Tooltip·스타터 Scale

### Packet 2 - Inventory to Equipment Drop

- 목표: 장비 슬롯 Hit Test, 호환 부위 Preview, Drop 장착
- 수정 위치: Equipment Window C++ 부모
- 완료 조건: 대응 슬롯 Drop 시 TryEquipGridItem 성공 및 Operation 정리
- 검증: Editor 빌드, DragDrop override 정적 검사
- 되돌리기 경계: Equipment Window DragDrop 메서드

### Packet 3 - Equipment Payload Pivot

- 목표: 장비 Drag Payload를 커서 중심에서 시작
- 수정 위치: Equipment NativeOnDragDetected
- 완료 조건: Payload가 원본 장비창 좌표와 무관하게 커서 중심에 표시
- 검증: Pivot 값 정적 검사, PIE 수동 QA
- 되돌리기 경계: Equipment Drag Operation Pivot

### Packet 4 - 문서와 회귀 검증

- 목표: 전체 메뉴 WBP, 기존 우클릭·Hover·양방향 Drag 회귀 점검
- 수정 위치: Validator, Feature/doc, DevLog, SprintChangeLog
- 완료 조건: 빌드·commandlet·문서·생성물 검사 통과
- 검증: ValidateMenuWidgetAssets.py, git diff --check, CheckGeneratedChanges.ps1
- 되돌리기 경계: 검증 스크립트와 문서

## 아이콘 규칙

- Total Widget Size는 InventorySize × CellSize다.
- Icon Fit Area는 각 축의 84%로 계산해 모든 footprint에 8% 상대 여백을 둔다.
- 원본 종횡비는 유지하고 InventoryIconScale은 0.75~1.0 보정만 허용한다.
- 스타터 아이템 6종은 기준값 1.0으로 통일한다.
- Texture 원본 픽셀 크기는 Widget Desired Size를 직접 결정하지 않는다.

## Tooltip 규칙

- 기본 폭 280px, 장비 비교 폭 580px
- 아이콘 Desired Size 52x52 고정
- Inventory 장비: 우클릭 : 장착하기
- 소비 아이템: 우클릭 : 사용하기
- 장비 슬롯 장비: 우클릭 : 장착 해제
- 동작이 없는 아이템은 안내 문구를 접는다.

## Drop 규칙

- GridInventory Operation만 장비창 Drop 대상으로 받는다.
- 커서 아래 슬롯과 ItemData EquipSlotType이 같을 때만 장착 가능 Preview를 표시한다.
- 성공 시 MarkDropHandled와 InventoryWindow EndGridItemDrag를 호출한다.
- 실패·이탈 시 슬롯과 Payload Preview 색을 원상 복구한다.
- 장비에서 시작한 Drag Visual Pivot은 CenterCenter를 사용한다.

## 수동 QA

- 1x1·1x3·2x2·2x3·4x3 상대 여백과 원본 종횡비
- Tooltip 크기와 우클릭 안내 문구
- 인벤토리 장비를 대응·비대응 장비 슬롯에 Drop
- 교체 장비가 인벤토리에 돌아오는지 확인
- 장비 Drag Payload가 커서 중심에서 시작하는지 확인
- 장비 Hover·우클릭 해제·인벤토리 내부 이동 회귀 확인

## 추가 원인 분석 — 장비 Payload 좌측 상단 비행

### 확인된 원인

- 장비 Drag 감지는 전체 화면 크기의 `URogue10mEquipmentWindowWidget`을 Drag Source로 등록하고 있었다.
- UE 5.8의 `SObjectWidget::OnDragDetected`는 Source Widget의 절대 좌표를 Drag Decorator의 `StartingScreenPos`로 전달한다.
- `FUMGDragDropOp::OnDragged`는 Drag 시작 후 0.15초 동안 `StartingScreenPos`에서 최종 Pivot 위치까지 보간한다.
- 따라서 `CenterCenter` Pivot은 최종 커서 정렬만 수정하며, 전체 화면 Widget의 좌측 상단에서 시작하는 기본 보간은 제거하지 못한다.

### 보정 설계

- 장비 슬롯 Canvas에 `URogue10mEquipmentDragSourceWidget` 투명 프록시를 미리 배치해 MouseDown Hit-Test 경로에 포함한다.
- 프록시는 장착 아이템이 있는 슬롯에서만 입력을 받고 Tooltip·우클릭 메뉴를 장비창에 위임한다.
- MouseDown 시 같은 프록시의 Canvas Layout을 커서 중심의 `Item Footprint × InventoryDragCellSize` 크기로 이동한다.
- 화면 좌표를 부모 Canvas 로컬 좌표로 변환하므로 Viewport DPI Scale이 자동 반영된다.
- Payload는 기존 `CenterCenter` Pivot을 유지하므로 첫 프레임의 시작 좌표와 최종 커서 중심 좌표가 일치한다.
- Drop, Drag Cancel, Drag 임계값 미도달 MouseButtonUp, 창 닫기에서는 프록시를 원래 슬롯 Layout으로 복원한다.
- 장비 표시 재구성과 Widget Destruct에서는 프록시 배열을 제거·재생성한다.

### 완료 조건과 검증

- 장비 Payload가 화면 좌측 상단에서 이동하지 않고 클릭한 장비 위치 근처에서 즉시 커서를 추적한다.
- 1×1부터 M×N 장비까지 Payload 크기와 커서 중심 정렬이 유지된다.
- 장비→인벤토리 Drop, 취소, 창 닫기 후 투명 프록시가 Viewport에 남지 않는다.
- `Rogue10mEditor Win64 Development` UHT/C++ 빌드를 통과한다.
- 최종 시각 동작은 최신 모듈을 로드한 PIE에서 수동 확인한다.
