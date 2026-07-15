# 몬스터 데미지 인디케이터 구현 결과

## 구현 내용

- 기존 `ARogue10mBasicMonster::TakeDamage()` → `AddFloatingDamageNumber()` 호출을 유지했습니다.
- 데이터만 저장하던 `AddFloatingDamageNumber()`에 실제 Screen Space `UWidgetComponent` 생성 경로를 추가했습니다.
- `URogue10mDamageIndicatorWidget`을 코드 기반으로 추가해 Widget Blueprint가 없어도 즉시 표시됩니다.

## 표시 동작

- 위치: 몬스터 충돌 Bounds 상단 + 기본 30cm
- 지속시간: 기본 1.2초
- 애니메이션: 짧은 팝 스케일 → 위로 상승 → 후반 페이드아웃
- 색상: 낮은 피해는 금색, 피해량이 커질수록 적색으로 변화
- 크기: 피해량에 따라 0.9~1.45 배율
- 연속 공격: 작은 위치 랜덤 오프셋으로 숫자 중첩 완화
- 종료: 위젯이 자신을 소유한 WidgetComponent를 제거

## 네트워크와 수명주기

- Listen Server의 로컬 플레이어는 즉시 표시합니다.
- Dedicated Server의 원격 플레이어는 Client RPC로 계산된 월드 위치와 피해량을 전달합니다.
- 표시 컴포넌트는 PlayerController가 소유하므로 몬스터가 치명타 직후 제거돼도 인디케이터 수명은 유지됩니다.
- 기존 floating damage 엔트리는 만료 항목을 제거하고 최대 32개로 제한했습니다.

## 조정 지점

PlayerController 기본값:

- `DamageIndicatorWidgetClass`
- `DamageIndicatorDuration`
- `DamageIndicatorHeightOffset`

DamageIndicatorWidget 기본값:

- `VerticalTravel`
- `FadeStartAlpha`
- `LowDamageColor`
- `HighDamageColor`

필요하면 `URogue10mDamageIndicatorWidget`의 Blueprint 자식을 만들어 `DamageIndicatorWidgetClass`에 지정할 수 있습니다.

## 검증

- UHT 성공
- `Rogue10mEditor Win64 Development` 컴파일/링크 성공
- 별도 `.uasset` 또는 `.umap` 변경 없음

## PIE 확인 항목

- 일반 공격 적중 시 몬스터 머리 위 피해량 표시
- 연속 공격 시 숫자 중첩과 상승/페이드 확인
- 몬스터 사망 공격에서도 인디케이터가 지속되는지 확인
- 화면 가장자리와 먼 거리에서 Screen Space 표시 위치 확인
