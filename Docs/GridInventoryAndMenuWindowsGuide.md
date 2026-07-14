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

Inventory Widget C++가 `OnInventoryGridChanged`를 자동 구독한다. Construct, 창 열기, Grid 변경 시 셀 크기와 돈·무게 정보를 갱신하므로 Blueprint Tick이나 수동 Property Binding은 필요하지 않다.

## 3. 메뉴 Widget Designer 필수 배치

네 메뉴 에셋은 모두 C++ `RebuildWidget()`이 아닌 Widget Blueprint Designer 트리를 사용한다. C++는 데이터와 동작만 담당하며, 레이아웃·스타일·애니메이션은 WBP에서 편집한다.

### WBP_InventoryWindow

- Root: `UI_InventoryCanvas` (`CanvasPanel`)
- `UI_InventoryWindowFrame`
- `UI_InventoryGrid` (`UniformGridPanel`, 필수 BindWidget, 런타임 셀 컨테이너)
- `UI_InventoryItemCanvas` (`CanvasPanel`, 필수 BindWidget, NxM 아이템 직접 배치)
- `UI_InventoryMoneyText` (`TextBlock`, 필수 BindWidget)
- `UI_InventoryWeightText` (`TextBlock`, 필수 BindWidget)

제거된 항목:

- `UI_InventoryCapacityText`
- `UI_InventoryHintText`

`UI_InventoryGrid`와 `UI_InventoryItemCanvas`를 분리한 이유는 역할이 다르기 때문이다.

- Grid: X×Y 좌표, 동일 크기 셀 배경, Drop 위치 계산 기준
- ItemCanvas: 1×1부터 NxM까지 하나의 아이템 위젯으로 표시, 자유 위치와 Drag & Drop

`UniformGridPanel` 하나만 사용하면 2×3 아이템을 여섯 셀에 걸친 단일 위젯으로 다루기 어렵다. 두 위젯은 Root Canvas에서 같은 위치와 크기를 사용하며 ItemCanvas의 ZOrder가 더 높다.

### WBP_InventoryCell

- 부모: `URogue10mInventoryCellWidget`
- `UI_InventoryCellFrame`
- `UI_InventoryCellFill`
- 기본 크기: 44×44
- `UI_InventoryCellFrame`의 0.5 패딩으로 각 셀 사이에 어두운 외곽선을 표시한다.

Inventory Window가 `InventoryContainers[0].GridSize`를 읽어 X×Y개의 `WBP_InventoryCell`을 자동 생성한다. 기본 10×10이면 100개, 5×6 가방이면 30개가 생성된다. GridSize가 바뀌지 않았다면 기존 셀 개수가 맞는지 확인하고 불필요한 재생성을 생략한다.
### WBP_EquipmentWindow

- Root: `UI_EquipmentCanvas`
- `UI_CharacterPreviewImage` (`Image`, 필수 BindWidget)
- `UI_EquipmentSlotContainer` (`VerticalBox`, 필수 BindWidget)
- 장비 슬롯 Frame 7종: Weapon, Head, Chest, Hands, Legs, Feet, Accessory
- `UI_EquipmentStatsContainer` (`VerticalBox`, 필수 BindWidget)

### WBP_SkillTreeEntry

- `UI_SkillIconImage`
- `UI_SkillNameText`
- `UI_SkillDescriptionText`
- `UI_SkillLockText`

네 항목 모두 필수 BindWidget이며, 스킬 목록에 반복 생성되는 단위 자체가 `UUserWidget`이다.

### WBP_SkillTreeWindow

- Root: `UI_SkillTreeCanvas`
- 전체/해금/잠김 필터 영역
- `UI_SkillListScrollBox`
- `UI_SkillListContainer` (`WrapBox`, 필수 BindWidget)
- `UI_SkillPointText`

Designer 트리를 복구하거나 다시 생성할 때 Unreal Editor를 닫은 상태에서 다음 명령을 실행한다.

```powershell
& "D:\Program Files\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\Project\Rogue10m\Rogue10m.uproject" `
  -ExecutePythonScript="D:\Project\Rogue10m\Scripts\Editor\BuildMenuDesignerLayouts.py" `
  -unattended -nop4 -nosplash -nullrhi
```

스크립트는 네 WBP를 컴파일·저장하고 필수 위젯 누락 및 Overlay 사용 여부를 검사한다.
Editor가 열려 있고 Remote Execution이 활성화된 경우에는 파일 잠금을 피하기 위해 현재 세션에서 실행한다.

```powershell
py -3 Scripts/RunEditorRemote.py `
  --file Scripts/Editor/BuildMenuDesignerLayouts.py `
  --timeout 20
```

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
- `UnitWeight`: 아이템 한 개의 무게(kg). 총 무게는 수량을 곱해 계산
- `DroppedWorldMesh`: 버렸을 때 월드에 표시되는 Static Mesh
- `bCanDrop`: 버리기 허용 여부

가방 아이템 추가 설정:

- `bProvidesInventory = true`
- `ProvidedInventorySize`: 가방이 제공하는 크기. 예: 5×6

## 5. 주요 Blueprint API

- `Get Inventory Containers`
- `Get Total Inventory Weight`
- `Get Max Carry Weight`
- `Get Gold`
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

## Inventory Cell / Item / BagTab 배치 구조

`WBP_InventoryCell`은 독립 창에 붙는 것이 아니라 `WBP_InventoryWindow`의 `UI_InventoryGrid` 자식으로 런타임 생성됩니다. `(X,Y)`는 `UniformGridPanel`의 `(Row=Y, Column=X)`에 대응합니다.

`UI_InventoryItemCanvas`는 Grid와 같은 440×440 위치에 더 높은 ZOrder로 겹쳐 배치됩니다. `WBP_InventoryItem`의 픽셀 위치는 `(GridX*44, GridY*44)`, 크기는 `(회전 적용 Width*44, Height*44)`입니다. 따라서 Cell은 좌표와 배경, Item은 NxM 표현과 Drag & Drop을 담당합니다.

드래그 중 포인터의 Canvas 로컬 좌표를 44로 나눈 뒤 잡은 셀 오프셋을 빼서 좌상단 Grid 좌표를 계산합니다. R키는 잡은 상태에서 footprint를 시계 방향 90도로 바꾸며, `CanPlaceGridItem` 결과에 따라 프리뷰 Border가 녹색/적색으로 표시됩니다.

`WBP_BagTab`은 `UI_BagTabContainer`에 컨테이너 수만큼 생성되며, 가방 이름만 표시하고 클릭 시 해당 컨테이너의 Cell/Item을 다시 구성합니다. `UI_BagSizeText`는 사용하지 않습니다.
## 메뉴 에셋 폴더

- Inventory: `/Game/Widget/Menu/Inventory`
- Equipment: `/Game/Widget/Menu/Equipment`
- SkillTree: `/Game/Widget/Menu/SkillTree`

에셋을 다시 구성할 때는 `Scripts/Editor/OrganizeMenuWidgetAssets.py`로 Unreal Asset API 이동을 수행하고, `Scripts/Editor/ValidateMenuWidgetAssets.py`로 Designer 필수 위젯과 내부 WidgetClass 참조를 검증합니다.
## Inventory Grid 시각 스타일 기준

- 기본 셀: 44×44
- 셀 경계: 1px 다크 프레임. 셀 사이에 별도 Margin을 두지 않아 NxM 아이템 면적이 끊겨 보이지 않게 한다.
- 셀 Fill: 중간 명도의 차콜. 아이템·희귀도·배치 가능 프리뷰 색보다 낮은 시각 우선순위를 유지한다.
- Grid Frame: 448×448, 내부 Grid/ItemCanvas는 440×440이며 모두 같은 중심 좌표를 사용한다.
- 기본 10×10은 정확히 440×440을 채우므로 우측 또는 하단에 잔여 픽셀이 생기지 않는다.
- BagTab은 136×36, Grid Frame과 겹치지 않는 별도 행에 배치한다.

마비노기처럼 NxM 점유 크기를 즉시 읽을 수 있는 촘촘한 셀 구조와 Diablo II처럼 그리드 전체를 하나의 오목한 패널로 묶는 구조를 조합했다. Diablo IV는 모든 아이템을 한 슬롯으로 단순화한 구조이므로 현재 NxM 설계의 직접 기준으로 사용하지 않는다.
