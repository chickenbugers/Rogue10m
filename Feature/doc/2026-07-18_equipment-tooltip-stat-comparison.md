# 장비 Hover 스탯 및 장착 장비 비교 Tooltip 결과

## 구현 결과

- Item Data Asset에 `FRogue10mEquipmentStatModifiers`를 추가했다.
- 공격력, 방어력, 최대 체력, 치명타 확률, 공격 속도, 이동 속도 증가량을 장비별로 설정할 수 있다.
- 인벤토리 장비 Hover 시 기존 이름·설명·무게 아래에 `장착 시 증가` 목록을 표시한다.
- 같은 부위에 장비가 장착되어 있으면 Tooltip 오른쪽에 `현재 장착` 장비 카드를 함께 표시한다.
- 오른쪽 카드에는 장착 장비 아이콘·이름·설명·현재 증가량과 `교체 시 변화` 목록이 표시된다.
- 변화량은 `Hover 장비 - 현재 장착 장비`로 계산한다.
- 증가량은 초록색, 감소량은 붉은색, 동일 값은 회색으로 표시한다.
- 한쪽 장비에만 존재하는 스탯도 비교해 장착 해제 시 잃는 능력치를 숨기지 않는다.
- 동일 부위 장착 장비가 없으면 오른쪽 카드를 접고 기존 300px Tooltip 폭을 유지한다.
- 비교 장비가 있으면 Tooltip 폭을 620px로 확장한다.
- 소비·재료 아이템은 장비 스탯 영역을 접고 기존 Tooltip 정보를 유지한다.

## 스타터 장비 샘플 값

- 철제 장검: 공격력 +12, 치명타 확률 +2%
- 철제 투구: 방어력 +5, 최대 체력 +10
- 가죽 갑옷: 방어력 +12, 최대 체력 +25
- 가죽 장화: 방어력 +3, 이동 속도 +20
- 초심자의 반지: 공격력 +3, 치명타 확률 +3%, 공격 속도 +4%

## 변경 파일

- `Source/Rogue10m/Data/Rogue10mItemDataAsset.h`
- `Source/Rogue10m/Components/Rogue10mInventoryComponent.h`
- `Source/Rogue10m/Components/Rogue10mInventoryComponent.cpp`
- `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.h`
- `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.cpp`
- `Content/Widget/Menu/Inventory/WBP_InventoryItemTooltip.uasset`
- `Content/DataAsset/Item/Starter/DA_Item_*.uasset` 장비 5종
- `Scripts/Editor/BuildMenuDesignerLayouts.py`
- `Scripts/Editor/CreateStarterItemAssets.py`
- `Scripts/Editor/ConfigureStarterEquipmentStats.py`
- `Scripts/Editor/ValidateMenuWidgetAssets.py`

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공
- `WBP_InventoryItemTooltip` 필수 2열 Widget 구조 검증 성공
- 기본 Tooltip 폭 300px 및 비교 패널 기본 `Collapsed` 검증 성공
- 전체 메뉴 Widget Blueprint commandlet 컴파일 성공
- 스타터 장비 5종의 저장된 스탯 값 검증 성공
- 최종 commandlet 0 errors / 0 warnings
- Python 스크립트 문법 검사 성공
- `git diff --check` 성공

## 범위 경계

이번 작업의 `EquipmentStats`는 Tooltip에 표시하고 비교하기 위한 장비 증가량 메타데이터다. 실제 캐릭터 GAS Attribute에 장착·해제 효과를 적용하고 장비창의 캐릭터 스탯 합계를 갱신하는 작업은 별도의 장비 효과 집계 기능으로 분리한다.

## PIE 수동 확인 항목

1. 소비 아이템 Hover 시 기존 이름·설명·무게만 표시되는지 확인한다.
2. 빈 장비 부위의 아이템 Hover 시 왼쪽 `장착 시 증가`만 표시되는지 확인한다.
3. 같은 부위 장비가 장착된 상태에서 오른쪽 `현재 장착` 카드가 표시되는지 확인한다.
4. 더 높은 값은 초록색, 더 낮은 값은 붉은색, 동일 값은 회색인지 확인한다.
5. 현재 장비에만 있는 스탯이 음수 변화로 표시되는지 확인한다.
6. 620px 비교 Tooltip이 화면 가장자리에서 Unreal 기본 Tooltip 배치에 의해 화면 안으로 보정되는지 확인한다.
7. 좌클릭 Drag 시작 시 Tooltip이 정상적으로 사라지는지 확인한다.
