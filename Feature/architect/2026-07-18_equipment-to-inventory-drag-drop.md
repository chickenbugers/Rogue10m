# Sprint#2-1 장비 → 인벤토리 Drag & Drop 설계

## 목표

- 장비창의 장착 아이콘을 마우스 왼쪽 버튼으로 끌어 인벤토리의 원하는 빈 칸에 놓을 수 있게 한다.
- 아이템의 `InventorySize(MxN)`를 그대로 사용해 배치 가능 여부를 판정한다.
- 아이템 배치가 실패하거나 드래그가 취소되면 장비 상태를 변경하지 않는다.
- 배치가 성공한 경우에만 Grid 인벤토리에 아이템을 추가하고 장비 슬롯을 비운다.

## 현재 구조와 문제

- 장비 슬롯은 `FRogue10mInventorySlot::EquippedItem`을 사용한다.
- 화면 인벤토리는 `FRogue10mInventoryContainer::Entries` 기반 NxM Grid를 사용한다.
- 기존 `TryUnequipItemFromSlot()`은 구형 `ItemSlots` 배열로만 이동하므로 현재 인벤토리 화면에는 결과가 표시되지 않는다.
- 장비창은 개별 슬롯 Widget 클래스가 아니라 하나의 장비창 Widget 안에 7개 Frame/Image를 직접 보유한다.

## Ultrawork Packets

### Packet 1 - 타입이 있는 장비 Drag Payload

- 목표: 장비 아이콘에서 Drag를 시작하고 장비 부위, Item Data, 클릭한 셀 오프셋을 Payload로 전달한다.
- 수정 위치: `Rogue10mItemDragDropOperation.*`, `Rogue10mMenuWindowWidgets.*`
- 완료 조건: 비어 있지 않은 7개 장비 아이콘만 Drag를 시작하며 창 제목 Drag와 충돌하지 않는다.
- 검증: C++ 빌드, PIE에서 장비 아이콘 Drag Visual 확인.
- 롤백 경계: Drag Source enum과 장비창 입력 override.

### Packet 2 - NxM Grid 원자적 장착 해제

- 목표: Drop 위치의 MxN 점유 가능성을 확인한 뒤 Grid 추가와 장비 해제를 한 번에 처리한다.
- 수정 위치: `Rogue10mInventoryComponent.*`, `Rogue10mMenuWindowWidgets.*`
- 완료 조건: 성공 시 Grid/장비 Delegate가 모두 갱신되고, 실패 시 어느 쪽 데이터도 변경되지 않는다.
- 검증: 빈 칸 Drop 성공, 겹치는 칸·경계 밖·취소 Drop 실패 확인.
- 롤백 경계: `TryUnequipItemToGrid()` API와 Inventory Window Drop 분기.

### Packet 3 - 검증과 문서화

- 목표: 컴파일, 정적 계약, PIE 수동 QA 항목을 확인하고 결과를 기록한다.
- 수정 위치: Feature 문서, `DevLog/20260718.txt`, `Docs/SprintChangeLog.md`.
- 완료 조건: Editor 빌드와 생성물 검사 통과, 변경 파일 및 잔여 수동 QA가 명확하다.
- 검증: `Scripts/BuildEditor.ps1`, `Scripts/CheckGeneratedChanges.ps1`, `git diff --check`.
- 롤백 경계: 문서 변경.

## 입력 및 상태 계약

1. 장비창에서 마우스 누름 위치가 표시 중인 장비 아이콘 Geometry 안인지 확인한다.
2. 해당 슬롯에 유효한 `ItemData`가 있을 때만 Drag 감지를 시작한다.
3. Drag Payload는 `Equipment` Source, 장비 부위, Item Data, 수량, MxN 클릭 오프셋을 가진다.
4. 인벤토리 위에서 매 프레임 `CanPlaceGridItem()`으로 Drop 가능 여부를 계산한다.
5. Drop 시 `TryUnequipItemToGrid()`가 같은 조건을 다시 검증한다.
6. 성공하면 새 Grid Entry를 만든 뒤 장비 슬롯을 초기화하고 `OnInventoryGridChanged`, `OnEquipmentChanged`를 방송한다.
7. 실패/취소 시 Payload의 정리만 수행하며 장비 데이터는 보존한다.

## 예외 처리

- `ItemData`가 없는 구형 장비는 NxM 크기와 아이콘을 결정할 수 없으므로 Drag 대상에서 제외한다.
- 잠긴 장비 슬롯은 이동하지 않는다.
- 대상 Grid가 없거나 공간이 부족하면 Drop을 거부한다.
- 주무기 해제 성공 시 캐릭터 무기 타입을 `Unarmed`로 되돌린다.
