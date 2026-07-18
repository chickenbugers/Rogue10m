# 장비 툴팁 등급 이름 색상 설계

## 목표

- 장비 아이템 이름을 Item Data Asset의 등급에 맞는 색상으로 표시한다.
- 장비 스탯 목록 위의 `장착 시 증가` 제목은 표시하지 않는다.

## 설계

- 등급 배경색과 텍스트색은 알파와 명도 요구가 다르므로 별도 API를 사용한다.
- `URogue10mItemDataAsset::GetRarityTextColor()`가 불투명 텍스트 팔레트를 제공한다.
- Hover 대상, 비교 중인 장착 장비, 우클릭 장착 해제 메뉴의 이름에 동일한 색상 규칙을 적용한다.
- 기존 `UI_ItemStatsTitleText`는 호환성을 위해 선택적 바인딩으로 유지하되 런타임과 생성 스크립트에서 `Collapsed` 처리한다.

## 등급 색상

- 일반: 흰색
- 레어: 파랑
- 에픽: 보라
- 유니크: 주황
- 신화: 빨강

## 검증

- `Scripts/BuildEditor.ps1`로 UHT 및 Editor C++ 빌드를 검증한다.
- `ValidateMenuWidgetAssets.py`로 기존 Tooltip Widget Blueprint 바인딩과 컴파일을 검증한다.
- PIE에서 각 등급 장비 Hover 이름 색상과 제목 미표시를 수동 확인한다.

## 롤백 경계

- Item Data Asset 텍스트 색상 함수
- Tooltip 및 장착 해제 메뉴 TextBlock 색상 적용
- Tooltip 제목 선택적 바인딩과 레이아웃 생성 스크립트 기본 가시성
