# 인벤토리 아이템 등급 배경 설계

## 목표

- 인벤토리 아이템의 MxN 영역 뒤에 등급을 즉시 구분할 수 있는 색상 배경을 표시한다.
- 등급 체계를 `일반 → 레어 → 에픽 → 유니크 → 신화`로 통일한다.
- 배경이 아이콘 색과 디테일을 가리지 않고 Drag 배치 가능/불가능 Preview와도 충돌하지 않게 한다.

## 등급과 색상

| 등급 | 내부 enum | 배경색 | 의미 |
|---|---|---|---|
| 일반 | `Common` | 하양 | 기본 아이템 |
| 레어 | `Rare` | 파랑 | 희귀 아이템 |
| 에픽 | `Epic` | 보라 | 상위 희귀 아이템 |
| 유니크 | `Unique` | 주황 | 고유 아이템 |
| 신화 | `Mythic` | 빨강 | 최상위 아이템 |

모든 배경색은 아이콘 아래에서 낮은 알파로 표시하며, 아이콘 Brush와 색상은 항상 흰색으로 유지한다.

## UI 계층

`WBP_InventoryItem`의 GridPanel 레이어를 다음 순서로 구성한다.

1. `UI_InventoryItemRarityBackground` — 등급색 배경, Layer 0
2. `UI_InventoryItemPreviewBorder` — Drag 가능/불가능 Overlay와 아이콘, Layer 1
3. `UI_InventoryItemQuantityText` — 수량, Layer 2

Rarity Background는 아이템 위젯 전체를 채우므로 1x1과 MxN 모두 별도 Texture 크기 조정 없이 동일하게 적용된다.

## 데이터 및 런타임

- `ERogue10mItemRarity`를 요청된 5단계 이름으로 정리한다.
- `URogue10mItemDataAsset::GetInventoryRarityBackgroundColor()`가 등급별 색상과 알파를 단일 기준으로 반환한다.
- `URogue10mInventoryItemWidget::InitializeGridItem()`이 배경 Border 색상을 설정한다.
- 평상시 Preview Border는 투명에 가까운 중립색을 사용하고, Drag 중에만 초록/빨강 Overlay를 표시한다.
- 스타터 Item Data Asset에 샘플 등급을 저장해 실제 UI에서 여러 색을 확인할 수 있게 한다.

## 완료 조건

- 5개 등급 enum과 표시명이 정확하다.
- 등급 색상이 아이콘 뒤에서 MxN 전체 영역을 채운다.
- 아이콘은 등급색에 Tint되지 않는다.
- 수량은 항상 배경과 아이콘 위에 표시된다.
- Drag Preview 가능/불가능 색상이 등급 배경보다 위에 표시된다.
- C++ 빌드와 전체 메뉴 WBP/스타터 Data Asset 검증이 성공한다.

## 수동 QA

- 일반·레어·에픽·유니크·신화 각 색상 확인
- 1x1, 1x3, 2x2, 2x3 아이템 배경 영역 확인
- 밝은 아이콘과 어두운 아이콘의 가독성 확인
- 수량 우측 상단 표시 확인
- Drag 가능 초록색·불가능 빨강색 Overlay 확인
- Hover Tooltip 및 우클릭 사용·장착 회귀 확인
