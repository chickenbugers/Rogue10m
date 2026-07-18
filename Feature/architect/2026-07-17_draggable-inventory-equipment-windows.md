# Sprint#2-1 인벤토리·장비창 동시 표시와 창 드래그 설계

## 목표

- 인벤토리와 장비창을 동시에 열 수 있다.
- 두 창은 각자의 타이틀 드래그 영역으로 화면 안에서 이동할 수 있다.
- 인벤토리 아이템 Drag & Drop은 기존 배치 기능을 유지하고 창 이동과 충돌하지 않는다.
- 장비창은 좌측 스탯 패널, 중앙 캐릭터 프리뷰, 우측 7개 장비 슬롯의 3열로 구성한다.
- 각 장비 슬롯은 고정 크기 안에서 아이콘 종횡비를 유지하고 우측 상단에 8px 장착 위치 텍스트를 항상 표시한다.

## Ultrawork Packets

### Packet 1 - 창 상태 정책과 공통 드래그 런타임

- 목표: 인벤토리·장비창 동시 활성화와 공통 타이틀 드래그를 제공한다.
- 수정 위치: `Rogue10mPlayerController.*`, `Rogue10mMenuWindowWidgets.*`
- 완료 조건: I/E 창이 서로를 닫지 않고, 타이틀 드래그 시 `UI_WindowRoot`만 Viewport 안에서 이동한다.
- 검증: `BuildEditor.ps1`, 자동화된 C++ 계약 검사, PIE 입력 검사
- 롤백 경계: PlayerController의 창 그룹 정책과 MenuWindow 기본 클래스의 드래그 코드

### Packet 2 - 인벤토리·장비창 Designer 구조

- 목표: 두 창에 공통 `UI_WindowRoot`와 `UI_WindowDragHandle`을 배치하고 장비창을 좌우 분할한다.
- 수정 위치: `BuildMenuDesignerLayouts.py`, `WBP_InventoryWindow`, `WBP_EquipmentWindow`
- 완료 조건: 200×500 스탯 패널, 340×500 프리뷰, 350×500 장비 슬롯이 3열로 배치되고 각 슬롯은 Frame > Canvas > ScaleBox/Image + LocationText 계층을 가진다.
- 검증: 활성 Editor Remote Python으로 트리·좌표·크기·폰트·레이어 검사
- 롤백 경계: 두 WBP와 레이아웃 생성기 변경

### Packet 3 - 회귀 검증과 문서

- 목표: 동시 표시·드래그·슬롯 레이아웃 계약을 자동 검사하고 결과를 기록한다.
- 수정 위치: `ValidateMenuWidgetAssets.py`, Feature 문서, DevLog, SprintChangeLog
- 완료 조건: 빌드, WBP 컴파일, 독립 검증 및 Harness 검사가 통과한다.
- 검증: Editor commandlet, `CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 검증 스크립트와 문서

## 입력과 레이아웃 계약

- 창 이동은 좌클릭이 `UI_WindowDragHandle` 안에서 시작했을 때만 활성화한다.
- 이동 대상은 전체화면 UserWidget이 아니라 그 안의 `UI_WindowRoot` CanvasPanel이다.
- 창 위치는 화면 경계 안으로 Clamp한다.
- 두 창은 서로 다른 초기 좌우 위치를 사용해 기본 상태에서 겹치지 않게 한다.
- 인벤토리와 장비창을 열면 스킬트리·설정은 닫지만 서로는 닫지 않는다.
- 스킬트리·설정을 열면 기존 정책대로 다른 Blocking 창을 닫는다.

## 장비 슬롯 계층 계약

```text
UI_*SlotFrame (장비 장착 칸 Border, 고정 크기)
└─ UI_*SlotLayer (CanvasPanel: 고정 슬롯 경계)
   ├─ UI_*SlotIconScale (ScaleBox: ScaleToFit)
   │  └─ UI_*SlotIcon (장비 이미지)
   └─ UI_*SlotLocationText (우측 상단, 8px, 항상 표시)
```

장비가 없으면 Image만 Collapsed되어 빈 슬롯과 위치 텍스트가 보인다. 장비가 있으면 Image가 표시되고 위치 텍스트는 Canvas ZOrder 1에서 아이콘 위에 유지된다.

아이콘 Texture 원본 크기는 Brush Desired Size로 복사하지 않는다. 긴 변을 64px로 정규화해 종횡비를 보존한 뒤, 사방 7px Stretch Anchor의 ScaleBox가 최종 슬롯 크기를 결정한다.

## 초기 배치

- 인벤토리: 화면 중심 기준 좌측
- 장비창: 화면 중심 기준 우측
- 장비창: 980×620, 초기 중심 위치 (200, 0)
- 장비창 내부: 좌측 200×500 스탯 패널, 중앙 340×500 캐릭터 프리뷰, 우측 350×500 장비 슬롯 영역
- 7개 슬롯은 `투구 | 목걸이`, `갑옷 | 장갑 | 무기`, `신발 | 반지`의 3행 구조로 배치한다. 갑옷은 100×150, 무기는 100×200, 나머지는 100×100으로 고정한다.

## 두 창 동시 입력 계약

- 열린 최상위 `URogue10mMenuWindowWidget`은 `SelfHitTestInvisible`로 표시한다.
- 전체화면 `UI_InventoryCanvas`와 `UI_EquipmentCanvas`도 `SelfHitTestInvisible`로 고정한다.
- 실제 `UI_WindowRoot` 아래의 프레임, 드래그 핸들, 인벤토리 아이템과 장비 슬롯 자식만 Hit Test에 참여한다.
- 장비창의 투명 화면 영역은 아래 인벤토리 입력을 차단하지 않는다.
- 실제 창 사각형이 서로 겹치면 위 창이 입력을 받으며, 각 타이틀 Drag로 겹침을 해소한다.
- PIE에서 실제 PlayerController로 두 창을 열어 상태와 Visibility 계약을 검사한다.
