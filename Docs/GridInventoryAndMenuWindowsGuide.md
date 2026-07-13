# Grid Inventory and Menu Windows Guide

## 1. Widget Blueprint 생성

아래 세 Widget Blueprint를 생성하고 지정된 C++ 부모를 선택한다.

| 권장 에셋 이름 | 부모 클래스 | 기본 입력 |
|---|---|---|
| `WBP_InventoryWindow` | `URogue10mInventoryWindowWidget` | I |
| `WBP_EquipmentWindow` | `URogue10mEquipmentWindowWidget` | P |
| `WBP_SkillTreeWindow` | `URogue10mSkillTreeWindowWidget` | K |

PlayerController Blueprint의 `Rogue10m > UI > Menu Windows`에서 각 클래스를 지정한다.

- `InventoryWindowWidgetClass`
- `EquipmentWindowWidgetClass`
- `SkillTreeWindowWidgetClass`

클래스를 지정하지 않으면 키 입력은 경고만 남기며 입력 모드를 잠그지 않는다.

## 2. Widget 이벤트

각 Widget Blueprint에서 다음 이벤트를 사용할 수 있다.

- `Menu Window Initialized`: Controller가 Widget을 생성하고 InventoryComponent를 전달한 직후 호출된다.
- `Menu Window Open Changed`: 창이 열리거나 닫힐 때 호출된다.
- `Get Inventory Component`: 인벤토리 데이터와 Blueprint API에 접근한다.

Inventory Widget은 `OnInventoryGridChanged`에 이벤트를 연결해 그리드를 다시 그린다. 창이 열릴 때도 전체 컨테이너를 다시 그리는 것을 권장한다.

## 3. 인벤토리 UI 배치

기본 인벤토리는 10×10이다. 셀 하나의 화면 크기를 예를 들어 48px로 정하면:

- 컨테이너 화면 크기: 480×480px
- 아이템 위치: `Position * CellPixelSize`
- 아이템 화면 크기: `ItemData.InventorySize * CellPixelSize`

권장 Widget 계층:

```text
CanvasPanel
└─ WindowFrame
   ├─ ContainerTabList
   ├─ GridCanvas
   │  ├─ CellBackgrounds
   │  └─ ItemWidgets
   ├─ ItemTooltip
   └─ CloseButton
```

`UniformGridPanel`은 NxM 아이템이 여러 셀을 점유하는 표현이 불편하므로, 배경 셀은 UniformGridPanel로 그리고 실제 아이템은 CanvasPanel에 절대 배치하는 방식을 권장한다.

## 4. Item Data Asset

콘텐츠 브라우저에서 `Miscellaneous > Data Asset`을 선택하고 `Rogue10mItemDataAsset`을 생성한다.

필수 설정:

- `ItemId`: 저장/로드와 컨테이너 식별에 사용할 고유 이름
- `DisplayName`: UI와 월드 드롭 이름
- `Description`: Hover 상세 설명
- `InventoryIcon`: 인벤토리 이미지
- `Rarity`: 일반/고급/희귀/영웅/전설
- `Category`: 장비/소비/재료/재화/퀘스트
- `EquipSlotType`: 장착 위치
- `WeaponType`: 무기인 경우 무기 유형
- `InventorySize`: 인벤토리 점유 크기. 예: 검 1×3, 갑옷 2×3
- `MaxStackSize`: 한 배치 항목의 최대 수량
- `DroppedWorldMesh`: 버렸을 때 월드에 표시되는 Static Mesh
- `bCanDrop`: 버리기 허용 여부

가방 아이템 추가 설정:

- `bProvidesInventory = true`
- `ProvidedInventorySize`: 가방이 제공하는 크기. 예: 5×6

## 5. 주요 Blueprint API

- `Get Inventory Containers`
- `Can Place Grid Item`
- `Try Add Grid Item`
- `Try Move Grid Item`
- `Remove Grid Item`
- `Add Inventory From Bag`
- `Drop Grid Item`

아이템은 `InstanceId` GUID로 식별한다. 배열 Index는 이동과 삭제 후 바뀔 수 있으므로 저장하거나 Drag Payload로 사용하지 않는다.

## 6. Drag & Drop

Drag 시작 시 저장할 데이터:

- Source Container Index
- Item InstanceId
- 원래 Position

Drop 위치는 마우스 로컬 좌표를 셀 크기로 나눈 뒤 내림해 `FIntPoint`로 변환한다. `CanPlaceGridItem`이 true일 때만 `TryMoveGridItem`을 호출한다.

## 7. 월드 드롭

`DropGridItem`에 컨테이너 Index, InstanceId, 생성 위치를 전달한다.

- 서버 권한에서만 실행된다.
- `bCanDrop`이 false면 거부된다.
- Spawn 실패 시 원래 인벤토리 위치로 복원된다.
- 생성된 `ARogue10mDroppedItem`은 `DroppedWorldMesh`와 `DisplayName`을 사용한다.

PlayerController나 Character에서 권장 드롭 위치:

```text
Character Location + Character Forward Vector * 120 + FVector(0, 0, 30)
```

## 8. 후속 작업

- 기존 프로토타입 `ItemSlots` 데이터를 Item Data Asset 기반 그리드 항목으로 마이그레이션
- 가방 제거 시 내부 아이템 처리 정책 결정
- 저장/로드 시 컨테이너 ID, InstanceId, Position, Quantity 직렬화
- 월드 드롭 아이템 상호작용 및 재획득
- 장비창 Drag & Drop과 그리드 인벤토리 연동

## 현재 프로젝트 연결 상태

BP_FirstPersonPlayerController의 Menu Windows 설정은 다음과 같이 연결되어 있다.

- InventoryWindowWidgetClass: WBP_InventoryWindow
- EquipmentWindowWidgetClass: WBP_EquipmentWindow
- SkillTreeWindowWidgetClass: WBP_SkillTreeWindow
- WBP_SkillTreeWindow의 SkillTreeEntryWidgetClass: WBP_SkillTreeEntry

연결이 손상되거나 에셋을 다시 생성해야 할 때 열린 Unreal Editor에서 Scripts/Editor/ConfigureMenuWindowWidgets.py를 실행한다. C++에는 명시 Class가 비어 있을 때 같은 경로의 기본 WBP를 로드하는 fallback도 포함되어 있다.