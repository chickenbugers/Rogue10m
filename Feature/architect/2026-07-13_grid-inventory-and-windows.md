# 그리드 인벤토리 및 메뉴 창 기반 설계

## Scope Gate

- 코드: 메뉴 Widget 생명주기, 아이템 Data Asset, NxM 배치, 가방 확장, 월드 드롭.
- 에셋: 사용자 제작 Widget Blueprint와 Item Data Asset이 연결될 C++ 부모/프로퍼티만 제공한다.
- 바이너리: 기존 `.uasset`을 직접 수정하지 않는다.
- 문서: Widget Blueprint 배치 계약과 Item Data Asset 작성 가이드를 제공한다.

## Packet 1: 메뉴 UserWidget 기반

- 목표: 인벤토리, 장비, 스킬트리 창을 키 입력으로 생성·표시한다.
- 수정 위치: PlayerController, UI/Widgets.
- 완료 조건: I/B/K로 개별 창 토글, 한 창을 열 때 다른 메뉴 닫기, 마우스/입력 모드 전환.
- 검증: Editor 빌드.
- 롤백: 메뉴 Widget 클래스와 Controller 변경.

## Packet 2: Item Data Asset

- 목표: 표시/배치/드롭에 필요한 정적 데이터를 한 에셋에 모은다.
- 필드: ID, 이름, 설명, 아이콘, 등급, 분류, 장비 슬롯, 무기 타입, NxM 크기, 최대 스택, 월드 Mesh, 드롭 허용, 가방 제공 크기.
- 완료 조건: BlueprintType Data Asset 생성 가능.
- 검증: UHT.
- 롤백: ItemDataAsset 파일.

## Packet 3: NxM 그리드 컨테이너

- 목표: 기본 10x10 그리드에 NxM 아이템을 충돌 없이 배치한다.
- 데이터: 컨테이너 ID/이름/크기와 GUID 기반 배치 항목.
- 완료 조건: 범위/충돌 검사, 첫 빈 위치 탐색, 추가, 이동, 제거.
- 검증: Editor 빌드 및 BlueprintCallable API 확인.
- 롤백: InventoryComponent 그리드 API.

## Packet 4: 가방 확장

- 목표: 가방 Data Asset을 통해 추가 인벤토리 컨테이너를 등록한다.
- 완료 조건: 기본 인벤토리는 유지되고 가방마다 독립 NxM 그리드가 추가된다.
- 검증: 동일 가방 중복 ID 방지 및 크기 유효성 검사.
- 롤백: AdditionalInventoryContainers 관련 변경.

## Packet 5: 월드 드롭

- 목표: 그리드 항목을 제거하고 Data Asset Mesh를 가진 DroppedItem Actor를 생성한다.
- 완료 조건: 드롭 불가 아이템 거부, Spawn 실패 시 원래 위치 복원, 이름·Mesh 적용.
- 검증: Editor 빌드, PIE 수동 확인.
- 롤백: DropInventoryItem 및 DroppedItem 변경.

## 호환성

- 기존 1차원 `ItemSlots`, 장비 슬롯, 소비 퀵슬롯 API는 유지한다.
- 새 콘텐츠는 Data Asset 기반 `InventoryContainers` API를 사용한다.
- 후속 마이그레이션에서 기존 프로토타입 아이템을 Data Asset으로 전환한다.
