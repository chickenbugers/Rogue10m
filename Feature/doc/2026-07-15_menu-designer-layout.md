# Sprint#1-13 메뉴 UserWidget Designer 직접 배치 결과

## 결과

메뉴 UI 4종을 C++ `RebuildWidget()` 기반 런타임 Slate 생성에서 Widget Blueprint Designer 직접 배치로 전환했다. 각 에셋은 기존 C++ `UUserWidget` 부모를 유지하며, C++는 데이터·입력·Drag & Drop 동작을 담당하고 WBP가 레이아웃을 소유한다.

## 주요 변경

- `WBP_InventoryWindow`: Canvas/Frame, 빈 UniformGrid, NxM 아이템 Canvas, 하단 돈/무게 직접 배치
- `WBP_InventoryCell`: GridSize X×Y만큼 자동 생성되는 셀 전용 UserWidget
- `WBP_EquipmentWindow`: 캐릭터 프리뷰, 장비 슬롯 7종, 능력치 영역 직접 배치
- `WBP_SkillTreeEntry`: 아이콘, 이름, 설명, 잠금 상태를 갖는 반복용 UserWidget 구성
- `WBP_SkillTreeWindow`: 필터, ScrollBox, WrapBox 스킬 목록, 스킬 포인트 영역 직접 배치
- Inventory Grid와 ItemCanvas는 Overlay 없이 Root Canvas의 형제로 배치하고 ZOrder로 겹침 순서를 제어
- 네이티브 `RebuildWidget()` 선언·구현 및 관련 Slate/패널 include 제거
- 핵심 위젯 계약을 `BindWidgetOptional`에서 `BindWidget`으로 변경
- `BuildMenuDesignerLayouts.py`에 필수 위젯 이름과 Overlay 0개 자동 검증 추가

## 필수 BindWidget

| WBP | 필수 위젯 |
|---|---|
| Inventory | `UI_InventoryGrid`, `UI_InventoryItemCanvas`, `UI_InventoryMoneyText`, `UI_InventoryWeightText` |
| InventoryCell | `UI_InventoryCellFrame` |
| Equipment | `UI_EquipmentSlotContainer`, `UI_CharacterPreviewImage`, `UI_EquipmentStatsContainer` |
| SkillTreeEntry | `UI_SkillIconImage`, `UI_SkillNameText`, `UI_SkillDescriptionText`, `UI_SkillLockText` |
| SkillTreeWindow | `UI_SkillListContainer` |

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공
- 열린 Unreal Editor Remote Execution에서 메뉴 WBP 4종과 InventoryCell 컴파일 및 저장 성공
- Inventory: 8 widgets / 필수 4 / 제거 대상 0 / Overlay 0
- InventoryCell: 2 widgets / 필수 1 / Overlay 0
- Equipment: 23 widgets / 필수 3 / Overlay 0
- SkillTreeEntry: 8 widgets / 필수 4 / Overlay 0
- SkillTreeWindow: 11 widgets / 필수 1 / Overlay 0
- Python `py_compile` 성공

## 에디터 후속 설정

Designer에서 색상, 브러시, 폰트, 애니메이션은 자유롭게 조정할 수 있다. 필수 BindWidget의 이름과 타입은 C++ 계약이므로 변경하려면 C++ 선언도 함께 수정해야 한다. 인벤토리 셀 크기를 바꾸면 Grid와 ItemCanvas의 위치·크기를 동일하게 유지해야 NxM 아이템 좌표가 맞는다.

## Inventory 데이터 연결

- `URogue10mItemDataAsset::UnitWeight`: 아이템 한 개의 무게, 기본값 0.1kg
- `URogue10mInventoryComponent::GetTotalInventoryWeight`: 모든 Grid Entry의 무게×수량 합계
- `MaxCarryWeight`: UI 기준 최대 무게, 기본값 100kg
- `OnInventoryGridChanged`: 셀 크기와 돈/무게 UI를 이벤트 기반으로 갱신
- `InventoryCellWidgetClass`: `WBP_InventoryCell` 연결 확인

현재 MaxCarryWeight는 정보 표시 기준이다. 초과 시 획득·이동 제한은 후속 정책으로 남겨두었다.

## NxM Inventory Interaction 결과

- `WBP_InventoryCell`: `UI_InventoryGrid`가 X×Y만큼 생성하는 44×44 좌표/배경 셀이다.
- `WBP_InventoryItem`: `UI_InventoryItemCanvas`에 한 아이템당 하나씩 생성되며 Data Asset의 NxM 크기, 아이콘, 수량을 표시한다.
- `WBP_BagTab`: 설치된 인벤토리 컨테이너마다 생성되며 가방 이름과 선택 상태를 표시하고 클릭 시 해당 가방으로 전환한다.
- Grid Entry에 회전 상태를 저장하고, 현재 아이템과 기존 아이템 모두 회전 footprint를 적용해 경계 및 AABB 충돌을 검사한다.
- 드래그 중 잡은 셀 오프셋을 보존하며 R키 입력 시 시계 방향으로 footprint와 잡은 좌표를 회전한다.
- 드래그 프리뷰는 배치 가능 시 녹색, 경계 초과 또는 충돌 시 적색이다.
- 검증: `BuildEditor.ps1` 성공. 열린 Editor에서 세 WBP GeneratedClass 할당 및 필수 Designer 위젯 확인.
## 메뉴 폴더 및 UI 계약 정리 결과

- `URogue10mBagTabWidget`의 `UI_BagSizeText` 필수 바인딩과 GridSize 초기화 인자를 제거했다. BagTab은 가방 이름과 선택 상태만 표시한다.
- `WBP_InventoryCell`은 44×44 Frame 안에 0.5 패딩을 둔 Fill을 배치해 인접 셀 사이에 어두운 외곽선이 보이도록 구성했다.
- Inventory 관련 4종은 `/Game/Widget/Menu/Inventory`, Equipment는 `/Game/Widget/Menu/Equipment`, SkillTree 2종은 `/Game/Widget/Menu/SkillTree`로 이동했다.
- PlayerController 기본 SoftClassPath, Inventory/SkillTree 내부 WidgetClass, Editor 자동화 스크립트를 새 경로에 맞췄다.
- `UserInterfaceSettings`의 Font DPI 기본값을 명시하고 전체 Editor 모듈을 재빌드해 Hot Reload 잔여 리플렉션을 제거했다.

### 검증

- `Rogue10mEditor Win64 Development` 전체 빌드 성공
- 메뉴 WBP 7종 깨끗한 commandlet 컴파일: 오류 0, 경고 0
- `UI_BagSizeText` 부재, `UI_InventoryCellFill` 존재, 내부 WidgetClass 새 경로 참조 확인
- 최신 Unreal 로그에서 `LogEditCondition` 및 `bUseCustomFontDPI` 오류 재발 없음
## Inventory Grid 시각 균형 개선 결과

마비노기의 NxM Inventory Tetris와 Diablo II의 프레임형 그리드를 참고해 셀 간 빈 공간보다 선명한 경계와 그리드 전체 구획을 우선했다.

- Cell: 44×44 유지, 밝은 0.5선 대신 1px 다크 외곽선과 차콜 Fill 적용
- Grid Frame: 448×448 오목한 배경 프레임 안에 440×440 셀 Grid 배치
- 정렬: Grid와 ItemCanvas를 동일한 중심 `(0, 16)`에 배치해 NxM 좌표 일치 유지
- BagTab: 136×36, 14pt 텍스트, 어두운 배경으로 축소해 그리드 침범 제거
- Vertical rhythm: Title, Tab, Grid, BottomInfo 사이에 각각 독립 여백 확보
- 검증: `WBP_InventoryGridFrame` 포함 메뉴 WBP 7종 컴파일, 내부 WidgetClass 참조, Overlay 0개 확인

후속 PIE QA에서는 16:9와 16:10 해상도에서 10×10 기본 가방, 다중 BagTab, NxM 아이템 드래그 프리뷰의 픽셀 정렬을 확인한다.
