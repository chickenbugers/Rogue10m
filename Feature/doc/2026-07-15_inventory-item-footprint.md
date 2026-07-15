# 단일 NxM 인벤토리와 아이템 비율 수정 결과

## 결과

- 인벤토리 창은 기본 컨테이너 인덱스 0 하나만 표시하고 Drop 대상으로 사용한다.
- `UI_BagTabContainer`와 `BagTabWidgetClass` 연결을 제거해 현재 화면에 개인/공유/추가 가방 탭이 생성되지 않는다.
- Item Data Asset의 `InventorySize`만 고정 footprint로 사용하며 회전 상태와 R키 회전은 지원하지 않는다.
- Designer의 `UI_InventoryItemSize` 기본값을 44×44로 명시하고, 런타임에는 Canvas 슬롯과 함께 `Width×44`, `Height×44`로 덮어써 표시·입력·충돌 영역을 통일했다.
- `WBP_InventoryItem`에 `UI_InventoryItemIconScale`을 추가하고 4px inset + `ScaleToFit`을 적용했다.
- C++가 `SetBrushFromTexture(Icon, true)`로 실제 텍스처 크기를 Brush에 반영하므로 아이콘 원본 종횡비가 유지된다.
- 1×1은 44×44, 2×3은 88×132, 4×3은 176×132의 footprint를 사용한다.
- 아이콘이 있는 장비는 크기/수량 문자열을 숨기고, 중첩 가능한 비장비만 우측 상단에 수량을 표시한다.
- 아이콘이 없는 프로토타입은 `InventoryTint`와 `W×H` 또는 `W×H · Quantity`로 구분한다.
- Grid Frame/Grid/ItemCanvas 중심을 `(0,-12)`로 통일해 제거된 탭 공간을 정리했다.

## 구현 구조

```text
Item Data Asset.InventorySize
  -> Canvas Slot = W×44 by H×44
  -> UI_InventoryItemSize = W×44 by H×44
  -> Preview Border = Fill
  -> Icon ScaleBox = Fill, 4px padding, ScaleToFit
  -> Icon Image = source texture aspect ratio
```

Grid와 ItemCanvas는 같은 440×440 로컬 좌표계를 사용한다. Drop 좌표는 포인터의 ItemCanvas 로컬 위치를 44로 나눈 뒤 잡은 셀 오프셋을 빼서 계산한다.

## 변경 영역

- Inventory Component의 고정 footprint 배치·충돌 API
- Inventory DragDrop Operation의 회전 상태 제거
- Inventory Item/Window C++ UserWidget
- `WBP_InventoryItem`, `WBP_InventoryWindow`
- Designer 생성·검증 Python
- README, 인벤토리 가이드, SprintChangeLog, DevLog

## 검증

- `Rogue10mEditor Win64 Development`: 성공
- `FixInventoryItemFootprint.py`: 두 WBP 재구성·컴파일·저장 성공
- 새 commandlet에서 메뉴 WBP 7종 독립 컴파일: 성공
- `WBP_InventoryItem`: 열린 Editor에서 SizeBox 44×44, Border Padding 0, IconScale 4px/ScaleToFit 및 필수 위젯 확인
- `WBP_InventoryWindow`: `UI_BagTabContainer` 없음, 필수 Grid/Canvas/돈/무게 위젯 확인
- Inventory Window CDO: Cell/Item WidgetClass 참조 확인, BagTab 클래스 참조 없음
- 최신 독립 검증 로그: `LogBlueprint: Error`, `LogPython: Error` 0건
- Python `py_compile`: 성공

## 남은 수동 QA

PIE에서 실제 아이콘이 설정된 정사각형·세로형·가로형 아이템으로 다음을 확인한다.

- 1×1, 2×3, 4×3 footprint가 셀 경계에 정확히 맞는지
- 원본 아이콘이 늘어나거나 눌리지 않고 4px 안전 여백 안에 표시되는지
- 우측/하단 경계, 아이템 중첩, 잡은 셀 오프셋 기반 Drop이 정상인지
- 16:9, 16:10 및 UI DPI 스케일에서 Grid와 ItemCanvas가 어긋나지 않는지

## Item Data Asset 아이콘 배율

- `URogue10mItemDataAsset`에 `InventoryIconScale`을 추가했다.
- `1.0`은 기존 `ScaleToFit` 크기, `0.5`는 절반, `2.0`은 두 배 표시 크기다.
- 위젯은 중앙 Pivot과 균일 Render Scale을 사용하므로 텍스처 종횡비가 유지된다.
- 루트 `SizeBox`를 footprint 경계로 클리핑해 확대된 그림이 이웃 셀을 덮지 않는다.
- 아이콘 배율은 시각 표현 전용이며 아이템 점유 크기와 배치 충돌에는 영향을 주지 않는다.
- `Rogue10mEditor Win64 Development` 빌드에 성공했다.
