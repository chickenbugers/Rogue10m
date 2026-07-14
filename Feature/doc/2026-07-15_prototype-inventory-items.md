# 프로토타입 NxM 인벤토리 아이템 결과

## 완료 내용

- 1x1 DA_Item_Prototype_1x1
- 2x3 DA_Item_Prototype_2x3
- 4x3 DA_Item_Prototype_4x3
- 아이템별 이름, 설명, 무게, InventorySize, 구분 색상 설정
- 빈 기본 인벤토리에 시작 시 한 번만 자동 추가
- 아이콘이 없는 아이템은 InventoryTint를 인벤토리 배경색으로 사용

## 기본 배치

10x10 기본 인벤토리에서 행 우선 탐색 결과:

- 1x1: (0, 0)
- 2x3: (1, 0)
- 4x3: (3, 0)

기존 아이템이 하나라도 있으면 프로토타입 자동 추가를 생략한다.

## 조정 위치

BP_FirstPersonCharacter의 Inventory Component 또는 C++ 기본값에서 다음을 조정할 수 있다.

- bAddPrototypeStartingItems: 자동 추가 활성화
- PrototypeStartingItems: 시작 아이템 Data Asset 목록
- 각 Data Asset의 InventorySize, UnitWeight, InventoryTint

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- Unreal Python으로 Data Asset 3개 생성 및 저장 성공
- 저장 후 InventorySize를 다시 읽어 1x1, 2x3, 4x3 일치 확인