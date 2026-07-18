# 장비 툴팁 등급 이름 색상 구현 결과

## 구현 내용

- Item Data Asset에 불투명 등급 텍스트 색상 조회 함수를 추가했다.
- 공통 `SetRarityNameColor()` helper로 TextBlock 적용 방식을 통일했다.
- Hover 대상 장비의 이름에 등급 색상을 적용했다.
- 동일 부위 비교 대상인 현재 장착 장비 이름에도 자체 등급 색상을 적용했다.
- 우클릭 장착 해제 UserWidget의 장비 이름에도 등급 색상을 적용했다.
- `장착 시 증가` TextBlock은 선택적 바인딩 후 `Collapsed` 처리했다.
- 레이아웃 재생성 스크립트도 해당 제목을 기본 `Collapsed`로 생성한다.

## 텍스트 팔레트

- 일반: `(0.92, 0.92, 0.92, 1.0)`
- 레어: `(0.22, 0.52, 1.0, 1.0)`
- 에픽: `(0.72, 0.32, 1.0, 1.0)`
- 유니크: `(1.0, 0.46, 0.08, 1.0)`
- 신화: `(1.0, 0.12, 0.08, 1.0)`

## 검증 결과

- Rogue10mEditor Win64 Development UHT/C++ 빌드 성공
- 신규 BlueprintPure API의 UHT 생성 성공
- `ValidateMenuWidgetAssets.py` 전체 메뉴 Widget Blueprint 검증 통과
- `git diff --check` 통과

## PIE 확인 항목

1. 일반·레어·에픽·유니크·신화 장비 이름이 지정 색상으로 표시되는지 확인한다.
2. 비교 Tooltip의 두 장비 이름이 각각 자신의 등급 색상을 사용하는지 확인한다.
3. 스탯 첫 줄 위에 `장착 시 증가` 제목과 빈 여백이 남지 않는지 확인한다.

## 상태

- 코드 구현, Editor 빌드, Widget Blueprint 정적 검증 완료
