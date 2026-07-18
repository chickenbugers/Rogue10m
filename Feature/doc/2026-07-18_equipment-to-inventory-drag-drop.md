# Sprint#2-1 장비 → 인벤토리 Drag & Drop 구현 결과

## 결과

장비창의 장착 아이콘을 인벤토리 Grid의 원하는 빈 위치로 끌어 장착 해제할 수 있는 C++ 경로를 구현했다. Drop 대상은 아이템 Data Asset의 `InventorySize(MxN)`를 사용하며, 빈 공간 판정이 성공한 경우에만 Grid 추가와 장비 슬롯 초기화를 함께 수행한다.

## 주요 변경

- `ERogue10mItemDragSource::Equipment` Source를 추가했다.
- Drag Payload에 장비 부위, Item Data, 수량, 클릭한 MxN 셀 오프셋을 저장한다.
- 장비창의 7개 슬롯 Frame 전체를 장착 아이템 Drag 시작 영역으로 사용한다.
- 장비 아이템의 원본 비율을 보존하는 MxN 크기 Drag Visual을 생성한다.
- 인벤토리 DragOver에서 기존 Grid 아이템과 장비 아이템을 함께 처리한다.
- 배치 가능하면 초록색, 불가능하면 빨간색 Preview Tint를 표시한다.
- `TryUnequipItemToGrid()`가 대상 위치를 다시 검증한 뒤 Grid Entry 생성과 장비 해제를 원자적으로 처리한다.
- 주무기 장착 해제 성공 시 캐릭터 무기 타입을 `Unarmed`로 되돌린다.
- 자동 장착된 시작 장비는 Grid에 중복으로 남지 않도록 정리했다. 이후 Drag 해제 시에만 인벤토리로 돌아온다.
- DragOver 중 동일 Operation에 Focus를 반복 설정하지 않도록 가드를 추가했다.

## 실패 안전성

- 잠긴 장비, 비어 있는 슬롯, `ItemData`가 없는 구형 장비는 Drag를 시작하지 않는다.
- 다른 아이템과 겹치거나 Grid 경계를 벗어나면 Drop을 거부한다.
- 공간 부족, Drag 취소, 인벤토리 밖 Drop에서는 장비 상태를 보존한다.
- 성공 후에만 `OnInventoryGridChanged`와 `OnEquipmentChanged`를 방송한다.

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공.
- 첫 빌드의 C4458(`UWidget::Slot` 가림) 4건을 `EquipmentSlot` 변수명으로 수정한 뒤 재빌드 성공.
- `Scripts/Editor/ValidateMenuWidgetAssets.py` commandlet 통과.
- `WBP_InventoryItem`, `WBP_InventoryWindow`, `WBP_EquipmentWindow` 포함 전체 메뉴 Widget Blueprint 컴파일 성공.
- commandlet 결과: `0 error(s), 0 warning(s)`.
- `git diff --check` 통과.

## PIE 수동 확인 항목

재시작한 Editor에서 Remote Execution이 비활성화되어 자동 마우스 입력 검증은 수행하지 못했다. 다음 항목은 PIE에서 확인한다.

1. 인벤토리와 장비창을 동시에 연다.
2. 장비 아이콘 또는 해당 슬롯 Frame을 누르고 인벤토리 빈 칸으로 끈다.
3. 초록 Preview 위치에 Drop하면 장비 아이콘이 사라지고 동일 아이템이 정확한 MxN 크기로 생성되는지 확인한다.
4. 점유된 칸, Grid 밖, 인벤토리 밖에 Drop하면 장비가 유지되는지 확인한다.
5. 무기를 성공적으로 해제하면 캐릭터 무기 타입이 비무장으로 바뀌는지 확인한다.

## 관련 문서

- `Feature/architect/2026-07-18_equipment-to-inventory-drag-drop.md`
- `Docs/GridInventoryAndMenuWindowsGuide.md`
