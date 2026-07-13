# 그리드 인벤토리 및 메뉴 창 기반 결과

## 구현

- I/B/K 입력으로 생성된 인벤토리/장비/스킬트리 Widget을 실제 토글하는 기반을 추가했다.
- 세 전용 C++ UserWidget 부모 클래스와 Blueprint 초기화/열림 이벤트를 제공한다.
- 메뉴는 한 번에 하나만 열리고 활성 Widget에 입력 포커스가 지정된다.
- 기본 10×10 인벤토리와 NxM 아이템 범위·충돌 검사를 추가했다.
- GUID 기반 항목 추가, 이동, 제거와 Blueprint 변경 이벤트를 제공한다.
- 가방 Data Asset으로 독립적인 추가 그리드 컨테이너를 등록할 수 있다.
- Item Data Asset에 이름, 설명, 아이콘, 등급, 분류, NxM 크기, 월드 Mesh, 가방 크기를 정의한다.
- 월드 드롭 시 Data Asset Mesh와 이름을 적용하며 Spawn 실패 시 아이템을 복원한다.

## 호환성

- 기존 ItemSlots, 장비 슬롯, 소비 퀵슬롯 API는 유지했다.
- 새 콘텐츠부터 Data Asset 기반 InventoryContainers를 사용한다.

## 검증

- UnrealHeaderTool 통과
- Rogue10mEditor Win64 Development 빌드 성공
