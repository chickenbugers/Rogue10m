# Starter Item Icons - Architecture Plan

## Feature Summary

포션과 기본 장비 아이콘 6종을 제작하고 M×N 인벤토리 점유 크기, Item Data Asset, 시작 인벤토리, 장비창 슬롯에 데이터 기반으로 연결한다.

## Existing Context

- 기본 인벤토리는 10×10 단일 가방이며 `URogue10mItemDataAsset::InventorySize`가 점유 크기를 소유한다.
- `WBP_InventoryItem`은 `ScaleBox`로 원본 종횡비를 유지하고 `InventoryIconScale`을 추가 적용한다.
- 장비 시스템은 `FRogue10mItemStack`을 사용하므로 Item Data Asset 포인터를 유지하는 변환 계층이 필요하다.

## Technical Approach

- AI 이미지 생성 원본은 녹색 크로마 배경으로 만들고 공식 chroma-key helper로 투명화한다.
- 최종 PNG 캔버스는 한 칸당 256px 기준으로 M×N 크기와 일치시킨다.
- Unreal Import 후 UI 압축, UI Texture Group, No Mipmaps, sRGB를 적용한다.
- 장비창 7개 슬롯은 `GridPanel -> ScaleBox -> Image` 구조로 구성하고 장착 변경 Delegate로 갱신한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 아이콘 6종 생성 및 투명화 | `SourceArt/ItemIcons/Starter` | M×N 캔버스와 투명 모서리 확인 | PNG 크기·알파 검사 |
| 2 | Texture/Data Asset 생성 | `Content/Texture/Item/Starter`, `Content/DataAsset/Item/Starter` | 6개 에셋의 크기·아이콘 참조 검증 | Unreal Python Import 로그 |
| 3 | 인벤토리·장비 런타임 연결 | InventoryComponent, MenuWindowWidgets | 시작 아이템 배치와 장비 아이콘 갱신 | Editor Target 빌드 |
| 4 | Widget Designer 연결 | `WBP_EquipmentWindow` | 필수 아이콘 7개 바인딩 | Widget Commandlet 검증 |
| 5 | 문서화와 변경 검사 | Feature/Docs/DevLog | 결과·검증·후속 위험 기록 | CheckGeneratedChanges |

## C++ / Blueprint Boundary

C++는 Data Asset 변환, 시작 배치, 장비 변경 이벤트와 이미지 갱신을 담당한다. Widget Blueprint는 슬롯 레이아웃과 `BindWidget` 대상만 구성하며 아이템별 데이터는 보유하지 않는다.

## Build And Validation Plan

- `Scripts/BuildEditor.ps1`
- `Scripts/Editor/CreateStarterItemAssets.py`
- `Scripts/Editor/BuildMenuDesignerLayouts.py`
- `Scripts/Editor/ValidateMenuWidgetAssets.py`
- `Scripts/CheckGeneratedChanges.ps1`

## Risks And Mitigations

- 열린 Editor와 Commandlet의 동시 WBP 저장 충돌: Remote Python으로 현재 Editor 세션에서 저장한다.
- 서로 다른 아이콘 종횡비 왜곡: 원본 캔버스와 M×N 크기를 일치시키고 ScaleBox로 Fit한다.
- 기존 슬롯 시스템과 Data Asset 분리: `MakeItemFromDataAsset`에서 포인터와 분류 값을 보존한다.
