# Sprint#2-1 인벤토리·장비창 동시 표시와 창 드래그 구현 결과

## 구현 결과

- 인벤토리와 장비창은 서로를 닫지 않고 동시에 열 수 있다.
- 두 창을 열 때 스킬트리와 설정창만 닫히며, 스킬트리·설정창을 열 때는 기존처럼 다른 Blocking 창이 닫힌다.
- 두 창은 상단 44px `UI_WindowDragHandle`에서 좌클릭 Drag를 시작하면 `UI_WindowRoot`의 Canvas 위치가 이동한다.
- 이동 좌표는 현재 Viewport 경계 안으로 Clamp되어 창 전체가 화면 밖으로 사라지지 않는다.
- 인벤토리 아이템 Drag는 아이템 위젯에서만 시작하므로 타이틀 기반 창 Drag와 분리된다.

## 장비창 구조

- 좌측: 200×500 캐릭터 스탯 패널
- 중앙: 340×500 캐릭터 실시간 프리뷰
- 우측: 350×500 장비 슬롯 전용 Canvas
- 부위: 무기, 투구, 갑옷, 장갑, 반지, 신발, 목걸이
- 슬롯 크기: 갑옷 100×150, 무기 100×200, 투구·목걸이·장갑·신발·반지 100×100이며 `ScaleBox(ScaleToFit)`으로 아이콘을 슬롯 안에 제한한다.

슬롯 행 구성은 `투구 | 목걸이`, `갑옷 | 장갑 | 무기`, `신발 | 반지`이며 무기는 가운데 행의 세 번째 열에 독립 배치한다.

각 슬롯은 다음 계층을 사용한다.

```text
UI_*SlotFrame
└─ UI_*SlotFrame_Layer (CanvasPanel)
   ├─ UI_*SlotFrame_IconScale
   │  └─ UI_*SlotIcon
   └─ UI_*SlotLocationText
```

`UI_*SlotLocationText`는 우측 상단, Font Size 8, Canvas ZOrder 1로 고정된다. 빈 슬롯은 아이콘만 `Collapsed`되므로 위치 텍스트가 남고, 장비가 있으면 텍스트가 장비 이미지 위에 표시된다.

GridPanel이 512~768px Texture Desired Size로 확장되던 문제를 제거했다. 아이콘 긴 변을 64px로 정규화하고 Canvas 안의 ScaleBox를 사방 7px Stretch Anchor로 제한해 원본 비율을 유지하면서 슬롯 밖으로 넘치지 않게 했다.

## 변경 영역

- `ARogue10mPlayerController`: 인벤토리·장비창 전용 동시 표시 그룹 정책
- `URogue10mMenuWindowWidget`: 공통 타이틀 Drag, 마우스 캡처, Viewport 경계 Clamp
- `WBP_InventoryWindow`: 이동 가능한 WindowRoot와 DragHandle
- `WBP_EquipmentWindow`: 이동 가능한 980×620 WindowRoot, 좌측 스탯·중앙 프리뷰·우측 7슬롯
- 스탯 필드: 공격력, 방어력, 최대 체력, 치명타 확률, 공격 속도, 이동 속도를 `BindWidgetOptional`로 준비
- 레이아웃 생성·회귀 검증 Python

## 검증

- `Rogue10mEditor Win64 Development`: 성공
- 활성 Editor Hot Reload에서 `UI_WindowRoot`, `UI_WindowDragHandle` UPROPERTY 확인
- 활성 Editor에서 두 WBP 재구성·컴파일·저장 성공
- 인벤토리 11개, 장비창 46개 Designer Widget 확인
- 장비 슬롯 7개의 계층, 좌표, 크기, 8px 폰트, 우측 상단 Anchor, ZOrder 1 검사 성공
- PIE 장착 아이콘 정규화 Desired Size 확인: 투구 64×64, 갑옷 42.67×64, 무기 21.33×64
- 새 UE5.8 commandlet 전체 메뉴 WBP 컴파일: `0 errors / 0 warnings`
- Python `py_compile`: 성공

## 남은 수동 QA

PIE 시작 후 활성 Editor의 Remote Execution 노드가 중단되어 자동 마우스 상호작용 검증은 완료하지 못했다. 다음 항목은 Editor 재시작 후 수동 확인한다.

- I와 E를 순서대로 눌렀을 때 두 창이 동시에 표시되는지
- 각 타이틀 바 Drag가 해당 창만 이동시키는지
- 화면 가장자리에서 창 위치가 Clamp되는지
- 인벤토리 아이템 Drag & Drop이 창 Drag와 충돌하지 않는지
- 장비가 있는 슬롯과 빈 슬롯 모두 우측 상단 위치 텍스트가 유지되는지

## 2026-07-18 두 창 동시 상호작용 보강

- 열린 메뉴 UserWidget의 Visibility를 `Visible`에서 `SelfHitTestInvisible`로 변경했다.
- 인벤토리·장비창의 전체화면 Canvas도 `SelfHitTestInvisible`로 저장했다.
- 투명 화면 영역은 다른 창으로 입력을 통과시키고 실제 WindowRoot 자식만 입력을 받는다.
- PIE PlayerController 상태: 인벤토리 `True`, 장비창 `True`.
- PIE Visibility: 두 UserWidget과 화면 Canvas는 `SelfHitTestInvisible`, 드래그 핸들과 장비 슬롯은 `Visible`.
- 전체 메뉴 Widget Blueprint 컴파일과 Designer 계약 검사를 통과했다.
- Windows ACL 오류로 실제 마우스 자동화는 실행하지 못했으므로 최종 클릭·드래그 감각은 에디터에서 수동 확인한다.
