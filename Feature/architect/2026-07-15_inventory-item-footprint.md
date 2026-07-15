# 단일 NxM 인벤토리와 아이템 비율 설계

## 참고 이미지 해석

- Diablo II 하단 인벤토리: 하나의 고정 그리드가 아이템 점유 면적과 위치를 즉시 보여 주는 구조를 채택한다.
- Diablo II 보관함 변형: 다칸 아이템의 밀도와 실루엣 표현만 참고하고, 개인/공유 탭은 현재 범위에서 제외한다.
- 마비노기: 아이템마다 서로 다른 W×H 셀을 점유하는 방식과 아이콘 실루엣을 보존하는 표현을 핵심 기준으로 삼는다. 여러 가방 창은 도입하지 않는다.

현재 Rogue10m의 화면 계약은 기본 인벤토리 하나, 10×10 셀, 회전 없는 고정 footprint다. 백엔드 배열 구조는 이후 확장 여지를 남기지만 현재 UI는 항상 컨테이너 인덱스 0만 표시하고 이동 대상으로 사용한다.

## 핵심 불변 조건

1. 셀 한 칸의 Designer 기준 크기는 44×44이다.
2. 아이템 점유 크기의 단일 원천은 Item Data Asset의 `InventorySize=(W,H)`다.
3. 아이템 Canvas 슬롯과 `UI_InventoryItemSize`는 모두 `(W×44, H×44)`로 설정한다.
4. Grid, ItemCanvas는 440×440이며 좌상단 원점과 중심 위치가 정확히 같다.
5. 회전 상태나 표시용 임의 배율은 사용하지 않는다.
6. 아이콘은 footprint를 채우도록 늘이지 않고 원본 종횡비를 유지한다.
7. 가방 탭은 생성하지 않으며 기본 인벤토리 하나만 렌더링한다.

## 좌표와 크기 계약

| 항목 | 계산 |
|---|---|
| Grid 크기 | `10×10` |
| Cell 크기 | `44×44` |
| Item 위치 | `(GridX×44, GridY×44)` |
| Item 점유 영역 | `(W×44, H×44)` |
| Icon 안전 영역 | 점유 영역에서 각 변 4px inset |
| Drop 셀 | `floor(LocalPointer/44)-GrabCellOffset` |

예시는 다음과 같다.

| InventorySize | 실제 점유 픽셀 | 용도 예시 |
|---|---:|---|
| 1×1 | 44×44 | 반지, 작은 재료 |
| 1×3 | 44×132 | 단검, 세로형 도구 |
| 2×3 | 88×132 | 갑옷, 중형 가방 형태 |
| 4×3 | 176×132 | 대형 장비, 넓은 물체 |

Unreal DPI 스케일이 적용되어도 Grid와 ItemCanvas가 같은 로컬 좌표계를 함께 스케일하므로, Drop 계산에는 Designer 기준 44를 그대로 사용한다.

## Widget 레이어 구조

```text
WBP_InventoryWindow
└─ UI_InventoryCanvas
   ├─ UI_InventoryGridFrame       448×448
   ├─ UI_InventoryGrid            440×440, 셀 배경
   └─ UI_InventoryItemCanvas      440×440, NxM 아이템

WBP_InventoryItem
└─ UI_InventoryItemSize           Designer 44×44, runtime W×44 by H×44
   └─ UI_InventoryItemRoot        Fill
      ├─ UI_InventoryItemPreviewBorder  Fill, footprint/배치 프리뷰
      ├─ UI_InventoryItemIconScale      Fill + 4px padding, ScaleToFit
      │  └─ UI_InventoryItemIcon
      └─ UI_InventoryItemQuantityText  우측 상단
```

Border는 점유 영역 전체를 채워 클릭·드래그·충돌 프리뷰 면적과 실제 footprint가 일치한다. Icon은 별도 ScaleBox 안에서만 크기가 결정되므로 점유 면적이 2×3이나 4×3으로 바뀌어도 찌그러지지 않는다.

## 아이콘 종횡비 규칙

텍스처 원본 크기를 `Tw×Th`, 4px inset을 적용한 안전 영역을 `Aw×Ah`라고 하면 ScaleBox의 표시 배율은 다음과 같다.

```text
s = min(Aw / Tw, Ah / Th)
DisplaySize = (Tw×s, Th×s)
```

따라서 남는 공간은 투명 여백처럼 중앙에 남고, 어느 축도 독립적으로 늘어나지 않는다. C++에서 `SetBrushFromTexture(Icon, true)`를 사용해 Brush Desired Size를 실제 텍스처 크기로 갱신한 뒤 `ScaleToFit`을 적용한다.

아이콘 제작 규칙:

- 배경은 투명 알파를 사용한다.
- 원본 캔버스 내부의 불필요한 투명 여백은 최소화한다.
- 세로형 아이템은 `W<H`, 가로형 아이템은 `W>H`가 되도록 `InventorySize`를 정한다.
- 그림 비율을 맞추기 위해 텍스처 자체를 강제로 늘이거나 찌그러뜨리지 않는다.
- Texture Group은 UI 용도로 설정하고 최종 화면에서 작은 아이콘의 가독성을 확인한다.

## 표시 정책

- 아이콘이 있는 장비: 크기 문자열과 수량을 표시하지 않고 실루엣만 보여 준다.
- 아이콘이 있는 비장비 스택: 수량이 2 이상일 때만 우측 상단에 수량을 표시한다.
- 아이콘이 없는 프로토타입: `InventoryTint` 배경과 `W×H` 문자열로 footprint를 구분한다. 스택이면 `W×H · Quantity`를 표시한다.
- 배치 가능 프리뷰는 녹색, 불가능 프리뷰는 적색 Border를 사용한다.

## 단일 가방 범위

- `WBP_InventoryWindow`에는 `UI_BagTabContainer`를 두지 않는다.
- `URogue10mInventoryWindowWidget`은 `PrimaryContainerIndex=0`만 읽고 쓴다.
- 기존 BagTab 자산과 백엔드 컨테이너 배열은 미래 마이그레이션 경계로 남길 수 있지만 현재 화면, 검증 계약, 클래스 참조에는 연결하지 않는다.
- 추후 다중 가방을 도입할 때는 별도 Sprint에서 탭/다중 창 UX, 자동 획득 대상, 저장 데이터, 가방 제거 정책을 함께 설계해야 한다.

## Ultrawork Packets

### Packet 1 - footprint와 회전 규칙

- 영역: Inventory Component, DragDrop Operation, Inventory Window
- 완료 조건: 배치·충돌·드래그가 원본 `InventorySize`만 사용
- 검증: 런타임 회전 상태/R키 처리 검색 0건, Editor 빌드

### Packet 2 - 비율 보존 표시

- 영역: `WBP_InventoryItem`, Inventory Item Widget
- 완료 조건: 점유 영역은 셀 배수, 아이콘은 4px inset ScaleToFit, 수량은 우측 상단
- 검증: WBP 필수 위젯·컴파일, 1×1/2×3/4×3 표시

### Packet 3 - 단일 가방 UI

- 영역: `WBP_InventoryWindow`, Inventory Window Widget, 검증 스크립트
- 완료 조건: BagTab 없음, 컨테이너 0만 표시 및 Drop 대상
- 검증: Designer에서 `UI_BagTabContainer` 0건, WBP 컴파일

### Packet 4 - 문서와 기록

- 영역: README, 인벤토리 가이드, Sprint 변경 기록, DevLog
- 완료 조건: 현재 규칙과 후속 확장 경계가 일치
- 검증: `CheckGeneratedChanges.ps1`

## QA 행렬

- 1×1, 2×3, 4×3이 각각 44×44, 88×132, 176×132인지 확인한다.
- 정사각형/세로형/가로형 텍스처가 어느 footprint에서도 찌그러지지 않는지 확인한다.
- 아이콘과 footprint 사이 여백이 네 변에서 동일한지 확인한다.
- 그리드 우측·하단 경계에서 배치 거부가 정확한지 확인한다.
- 다른 아이템과 한 셀이라도 겹치면 배치가 거부되는지 확인한다.
- 아이템 어느 셀을 잡아도 GrabCellOffset을 유지해 Drop되는지 확인한다.
- 인벤토리 창에 가방 탭이 없고 항상 기본 인벤토리만 보이는지 확인한다.
- 16:9, 16:10 및 UI DPI 스케일에서 Grid/ItemCanvas 원점이 어긋나지 않는지 확인한다.

## 롤백 경계

- Item Data Asset의 `InventorySize`, 기본 10×10 Grid, 44px Cell은 변경하지 않는다.
- 장비창과 아이템 저장/로드는 이번 변경 범위에 포함하지 않는다.
- 바이너리 WBP는 Editor 자동화로만 재구성하며 직접 편집하지 않는다.

## 추가 설계: Item Data Asset 아이콘 배율

- `InventoryIconScale`은 `ScaleToFit` 결과에 적용하는 균일 배율이며 기본값은 `1.0`이다.
- 에디터 입력 범위와 런타임 방어 범위는 `0.1~2.0`으로 동일하게 제한한다.
- 배율은 이미지 중앙을 기준으로 적용해 가로·세로 원본 비율을 유지한다.
- `2.0`처럼 확대했을 때도 인접 셀을 침범하지 않도록 아이템 루트에서 footprint 경계를 클리핑한다.
- 이 값은 그림 표시만 바꾸며 `InventorySize`, 충돌, 드래그 및 점유 셀 계산에는 영향을 주지 않는다.
