# Sprint#2-3 인벤토리 아이템 우클릭 사용·장착 설계

## 목표

- Grid 인벤토리의 소비 아이템을 우클릭하면 즉시 사용한다.
- 장비 아이템을 우클릭하면 Item Data Asset의 장착 위치에 장착한다.
- 같은 위치에 장비가 이미 있으면 새 장비와 기존 장비를 원자적으로 교체한다.
- 교체 장비가 인벤토리에 들어갈 공간이 없으면 아무 상태도 변경하지 않는다.
- 성공 후 인벤토리와 장비창을 동시에 갱신한다.

## 현재 구조와 제약

- 화면의 인벤토리는 `FRogue10mInventoryGridEntry` 기반 M×N Grid를 사용한다.
- 기존 `TryEquipItemFromSlot()`과 `UseConsumableQuickSlot()`은 구형 `ItemSlots` 배열용이므로 Grid Entry에 직접 사용할 수 없다.
- Grid Entry에는 Item Data와 수량만 있고 소비 효과 값은 Item Data Asset에 아직 없다.
- 장비 교체 시 기존 장비의 M×N 크기가 새 장비보다 클 수 있으므로 원래 위치에 항상 들어간다고 가정할 수 없다.

## 구현 계약

### 소비 아이템

1. Item Data Asset에 `RestoreHealth` 소비 효과를 추가한다.
2. 대상이 소비 아이템이고 수량이 1 이상인지 확인한다.
3. 플레이어 AttributeSet이 없거나 체력이 최대치이면 실패하며 수량을 줄이지 않는다.
4. 체력을 최대 체력 이하로 회복하고 수량을 1 감소시킨다.
5. 수량이 0이면 Grid Entry를 제거한다.
6. 성공 시 `OnInventoryGridChanged`를 한 번만 방송한다.

### 장비 아이템

1. Item Data의 `EquipSlotType`과 동일한 장비 슬롯을 찾는다.
2. 장비 슬롯이 잠겼거나 Grid 장비 수량이 1이 아니면 실패한다.
3. 기존 장비가 없다면 Grid Entry를 제거하고 장착한다.
4. 기존 장비가 있으면 새 장비 Entry 위치를 우선 교체 위치로 사용한다.
5. 기존 장비가 그 위치에 맞지 않으면 전체 인벤토리에서 첫 M×N 빈 공간을 찾는다.
6. 공간이 없으면 교체를 취소하고 기존 장비와 Grid를 모두 유지한다.
7. 성공 시 무기 타입을 반영하고 `OnInventoryGridChanged`, `OnEquipmentChanged`를 각각 한 번 방송한다.

## Ultrawork Packets

### Packet 1 - Data Asset 소비 효과

- 목표: 소비 아이템 회복량을 Data Asset에서 조절 가능하게 한다.
- 수정 위치: `Rogue10mItemDataAsset.h`, Data Asset 생성 스크립트
- 완료 조건: 기존 체력 물약이 기본 회복량으로 작동하고 추후 아이템별 조정이 가능하다.
- 검증: UHT/C++ 빌드, 기존 Data Asset 로드.
- 롤백 경계: 소비 효과 필드.

### Packet 2 - Grid 원자적 액션

- 목표: Grid 소비와 장비 장착·교체를 InventoryComponent에 구현한다.
- 수정 위치: `Rogue10mInventoryComponent.*`
- 완료 조건: 성공 시 상태와 Delegate가 함께 갱신되고 실패 시 원상태가 보존된다.
- 검증: C++ 빌드, 정적 상태 전이 리뷰.
- 롤백 경계: Grid 액션 API와 빈 공간 탐색 확장.

### Packet 3 - 우클릭 UI 연결

- 목표: Inventory Item Widget의 우클릭을 카테고리별 액션에 연결한다.
- 수정 위치: `Rogue10mMenuWindowWidgets.cpp`
- 완료 조건: 좌클릭 Drag는 유지되고 우클릭만 소비·장착을 실행한다.
- 검증: 전체 메뉴 WBP 컴파일, PIE 수동 입력 확인.
- 롤백 경계: MouseButtonDown 우클릭 분기.

### Packet 4 - 리뷰와 기록

- 목표: 빌드·WBP·생성물 검사를 통과하고 결과를 기록한다.
- 수정 위치: Feature 문서, DevLog, SprintChangeLog.
- 완료 조건: 자동 검증과 PIE 확인 목록이 남는다.
- 검증: `BuildEditor.ps1`, `ValidateMenuWidgetAssets.py`, `CheckGeneratedChanges.ps1`, `git diff --check`.
- 롤백 경계: 문서 변경.

## 수동 QA

1. 체력이 감소한 상태에서 물약 우클릭 시 체력이 회복되고 수량이 1 감소하는지 확인한다.
2. 체력이 최대일 때 물약 우클릭으로 수량이 감소하지 않는지 확인한다.
3. 빈 장비 슬롯에 장비를 우클릭하면 장착되고 Grid에서 사라지는지 확인한다.
4. 이미 장착된 위치에 다른 장비를 우클릭하면 기존 장비가 Grid로 돌아오는지 확인한다.
5. 기존 장비가 원래 위치에 맞지 않으면 다른 빈 공간으로 이동하는지 확인한다.
6. 기존 장비가 들어갈 빈 공간이 없으면 교체가 취소되는지 확인한다.
7. 우클릭 기능 추가 후 좌클릭 Drag & Drop과 Hover 툴팁이 정상 동작하는지 확인한다.
