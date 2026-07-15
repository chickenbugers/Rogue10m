# Starter Item Icons - Development Summary

## Summary

회복 포션, 철제 장검, 철제 투구, 가죽 갑옷, 가죽 부츠, 초심자의 반지 아이콘을 제작해 Texture와 Item Data Asset으로 Import했다. 시작 인벤토리에 6종을 추가하고 장비 5종은 프로토타입 실행 시 해당 장비 슬롯에 자동 장착된다.

## Completed Ultrawork Packets

| Packet | Result | Validation |
| --- | --- | --- |
| 1 | 투명 배경 아이콘 6종과 M×N 캔버스 생성 | 4개 모서리 alpha 0, PNG 규격 검사 |
| 2 | UI Texture 6개와 Item Data Asset 6개 생성 | Unreal Python이 크기·아이콘 참조 검증 |
| 3 | Data Asset 기반 시작 인벤토리/장비 스택 연결 | Rogue10mEditor 빌드 성공 |
| 4 | 장비창 아이콘 7개와 ScaleBox Designer 구조 적용 | Commandlet 0 errors, 0 warnings |
| 5 | 현재 Editor에서 에셋과 필수 위젯 확인 | 6/6 Data Asset, 7/7 아이콘, 누락 0 |

## Asset Layout

| Item | Inventory Size | Source Canvas | Equip Slot |
| --- | ---: | ---: | --- |
| Health Potion | 1×1 | 256×256 | Consumable |
| Iron Longsword | 1×3 | 256×768 | MainWeapon |
| Iron Helmet | 2×2 | 512×512 | Head |
| Leather Armor | 2×3 | 512×768 | Armor |
| Leather Boots | 2×2 | 512×512 | Shoes |
| Novice Ring | 1×1 | 256×256 | Ring |

## Runtime Behavior

- 포션은 시작 수량 5개로 배치되어 우측 상단 수량 텍스트가 표시된다.
- 장비 아이템은 수량 텍스트를 숨긴다.
- 장착/해제 성공 시 `OnEquipmentChanged`가 발생하고 장비창 아이콘이 즉시 갱신된다.
- 장비 슬롯의 아이콘도 원본 비율을 유지하며 Data Asset별 `InventoryIconScale`을 적용한다.

## Build And Validation

- Editor Target build: 성공.
- Widget Blueprint validation: 전체 메뉴 위젯 컴파일 성공, 0 errors / 0 warnings.
- Current Editor readback: 스타터 Data Asset 6개 로드, PrototypeStartingItems 9개, 장비 아이콘 7개 모두 확인.

## Review Result

Data Asset이 정적 정의의 단일 출처이고 C++가 런타임 변환을 담당한다. 이미지 파일 자체를 슬롯마다 재조정하지 않으며 UI에서는 ScaleBox만 사용하므로 추가 M×N 아이템에도 같은 규칙을 적용할 수 있다.

## Remaining Notes

현재 프로토타입용 자동 장착은 `bEquipPrototypeStarterEquipment`로 끌 수 있다. Hands/Legs용 아이템은 이번 범위에 포함되지 않아 해당 슬롯은 비어 있는 상태로 검증된다.

## Tint Layer Fix (2026-07-16)

`WBP_InventoryItem`의 Grid 자식이 모두 Layer 0이어서 InventoryTint Border가 아이콘을 덮던 문제를 수정했다. `PreviewBorder=0`, `IconScale=1`, `QuantityText=2`로 렌더 순서를 고정하고 자동 검증에 동일한 Layer 계약을 추가했다. 열린 Editor 저장과 UE5.8 commandlet 전체 메뉴 검증을 통과했다.

## Runtime Icon Paint Fix (2026-07-16)

UImage::SetDesiredSizeOverride()가 내부 SImage 생성 전 호출되어 값이 유실되는 UE5.8 수명주기 문제를 수정했다. Inventory Item은 ScaleBox 없이 Border의 Image 콘텐츠를 사용하고, FSlateBrush::SetImageSize()로 44px 셀과 NxM footprint에 맞춘 종횡비 보존 크기를 저장한다. Menu WBP 자동 검증과 Editor Target 빌드를 통과했으며, PIE UI 캡처에서 포션, 장검, 투구, 갑옷, 부츠, 반지 아이콘이 정상 표시됐다.