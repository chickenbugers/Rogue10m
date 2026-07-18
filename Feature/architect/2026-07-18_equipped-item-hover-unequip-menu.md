# 장착 장비 Hover 및 장착 해제 메뉴 설계

## 목표

- 장비창의 장착 아이템 Hover 시 이름·설명·무게·장착 스탯 Tooltip을 표시한다.
- 장착 아이템 우클릭 시 별도 UserWidget에 장착 해제 버튼을 표시한다.
- 장착 해제는 MxN 인벤토리의 첫 빈 공간으로 이동하며 공간이 없으면 상태를 변경하지 않는다.

## Ultrawork Packets

### Packet 1 - 런타임 입력과 데이터 이동

- 목표: 장비 슬롯 Hit Test, Hover Tooltip, 우클릭 메뉴, 안전한 장착 해제 API 구현
- 수정 위치: Rogue10mMenuWindowWidgets, Rogue10mInventoryComponent
- 완료 조건: 장착 슬롯만 입력을 처리하고 공간 부족 시 원상 유지
- 검증: Scripts/BuildEditor.ps1
- 되돌리기 경계: C++ 소스

### Packet 2 - Widget Blueprint

- 목표: WBP_EquipmentSlotAction 생성과 장비창 클래스 참조 저장
- 수정 위치: BuildMenuDesignerLayouts.py, Content/Widget/Menu/Equipment
- 완료 조건: 아이템명·장착 해제 버튼·결과 문구 위젯 존재 및 컴파일 성공
- 검증: ValidateMenuWidgetAssets.py
- 되돌리기 경계: 신규 WBP와 Editor Python

### Packet 3 - 검증과 문서

- 목표: 클래스 참조·위젯 계층·기본 가시성 검사와 작업 기록
- 수정 위치: Validator, Feature/doc, DevLog, SprintChangeLog
- 완료 조건: 빌드·commandlet·문서 갱신 완료
- 검증: git diff --check, CheckGeneratedChanges.ps1
- 되돌리기 경계: 검증 스크립트와 문서

## UI 구조

- Hover는 장비 슬롯 Frame을 대상으로 기존 WBP_InventoryItemTooltip을 재사용한다.
- 장착 아이템이 있으면 InitializeItemTooltip(ItemData, Quantity, nullptr)로 자체 정보를 표시한다.
- 빈 슬롯은 Tooltip을 제거한다.
- WBP_EquipmentSlotAction은 Item Name, Unequip Button, Result Text로 구성한다.
- 우클릭 메뉴는 마우스 근처에 표시하고 화면 경계를 넘지 않도록 보정한다.
- 장비창 닫기·장비 상태 변경·장착 해제 성공 시 메뉴를 닫는다.
- 공간 부족 시 메뉴를 유지하고 결과 문구를 표시한다.

## 데이터 이동

TryUnequipItemToFirstAvailableGrid는 장비 슬롯을 검증하고 InventoryContainers를 순회한다. FindFirstGridPosition으로 MxN 첫 좌표를 찾은 뒤 기존 TryUnequipItemToGrid로 이동과 Delegate 갱신을 수행한다. 공간이 없으면 false를 반환하며 상태를 변경하지 않는다.

## 생명주기와 안전성

- Action Widget은 Inventory Component를 GC 추적 참조하고 슬롯 enum을 저장한다.
- 버튼 delegate callback은 UFUNCTION으로 선언한다.
- Hover와 우클릭은 Tick 없이 장비 갱신과 입력 이벤트에서만 처리한다.
- 기존 좌클릭 Drag & Drop 경로는 유지한다.

## 수동 QA

- 7개 장비 부위 Hover 정보와 장착 스탯 확인
- 빈 슬롯 Hover에서 Tooltip 미표시 확인
- 우클릭 메뉴 위치와 장착 해제 버튼 확인
- 장착 해제 후 MxN 인벤토리 배치와 장비 슬롯 초기화 확인
- 공간 부족 시 장비 유지와 오류 문구 확인
- 좌클릭 Drag, 장비창 이동, 다른 메뉴 동시 상호작용 회귀 확인
