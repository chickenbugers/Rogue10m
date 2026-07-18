# Sprint#2-2 인벤토리 아이템 Hover 툴팁 구현 결과

## 구현 결과

- 인벤토리 아이템 Hover 시 별도 UserWidget 기반 툴팁이 표시되도록 구현했다.
- 툴팁은 Item Data Asset의 `DisplayName`, `Description`, `UnitWeight`를 직접 사용한다.
- 설명이 비어 있으면 `아이템 정보가 없습니다.`를 표시한다.
- 단일 아이템은 `무게: N kg`, 중첩 아이템은 `무게: N kg × 수량 = 총 무게 kg` 형식으로 표시한다.
- Drag Preview에는 툴팁을 제거해 드래그 중 설명 UI가 따라오지 않도록 했다.
- Unreal 기본 `SetToolTip()` 경로를 사용해 Hover 지연, 마우스 추적, 화면 경계 배치를 Slate에 위임했다.

## UserWidget 구조

- C++ 부모: `URogue10mInventoryItemTooltipWidget`
- Widget Blueprint: `/Game/Widget/Menu/Inventory/WBP_InventoryItemTooltip`
- 필수 Designer 위젯:
  - `UI_ItemNameText`
  - `UI_ItemDescriptionText`
  - `UI_ItemWeightText`
- 레이아웃:
  - 기본 너비 300px
  - 어두운 배경 Border와 12px 내부 여백
  - 이름 18px
  - 설명 13px 자동 줄바꿈
  - 무게 12px
  - 전체 Root `HitTestInvisible`

## 연결 방식

- `URogue10mInventoryItemWidget`은 편집 가능한 `TSoftClassPtr` 속성을 가진다.
- 기본값은 `/Game/Widget/Menu/Inventory/WBP_InventoryItemTooltip`의 Generated Class다.
- 기존 `WBP_InventoryItem` 바이너리를 다시 저장하지 않아도 native 기본값으로 툴팁이 즉시 연결된다.
- 필요하면 Inventory Item Blueprint에서 다른 Tooltip UserWidget 클래스로 교체할 수 있다.

## 검증 결과

- `Rogue10mEditor Win64 Development` 빌드 성공
- `WBP_InventoryItemTooltip` 생성 및 저장 성공
- 전체 메뉴 Widget Blueprint commandlet 검증 성공
- Tooltip 필수 Widget 3개와 native Soft Class 기본값 검증 성공
- 최종 결과: 0 errors / 0 warnings
- 실제 Hover 위치·문구·Drag 전환은 PIE 수동 QA가 남아 있다.

## PIE 확인 항목

1. 1x1, 2x3, 4x3 아이템 Hover 시 동일한 툴팁이 표시되는지 확인한다.
2. 이름·설명이 각 Item Data Asset 값과 일치하는지 확인한다.
3. 수량 1개와 중첩 아이템의 무게 표현이 각각 올바른지 확인한다.
4. 인벤토리 우측·하단 가장자리에서 툴팁이 화면 밖으로 잘리지 않는지 확인한다.
5. Hover 직후 Drag & Drop을 시작하면 툴팁이 사라지고 드래그가 정상 동작하는지 확인한다.

## 상태

- 코드 구현, C++ 빌드, Widget Blueprint 생성·컴파일·참조 검증 완료
- PIE 수동 상호작용 QA 대기
