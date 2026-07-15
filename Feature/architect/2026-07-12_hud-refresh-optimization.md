# HUD 갱신 최적화 설계

## 문제

`URogue10mMainHUDWidget::NativeTick()`이 매 프레임 체력, 스테미나, 진행도, 아이덴티티, 몬스터 정보, 퀵슬롯, 로그, 미니맵과 Blueprint 이벤트를 전부 갱신하고 있었다. 값이 변하지 않아도 UMG setter와 컨테이너 순회가 반복됐다.

## 목표

- VitalBar의 실제 ProgressBar와 TextBlock을 C++에서 직접 갱신한다.
- 동일한 Vital 데이터는 setter 및 Blueprint 이벤트를 생략한다.
- 프레임 단위 전체 HUD 갱신을 주기별 갱신으로 분리한다.
- 컨테이너 크기 변경 시 전체 위젯 재생성을 피한다.

## 구현

- 주요 HUD 정보와 미니맵: 기본 0.1초 간격
- 퀵슬롯과 로그: 기본 0.25초 간격
- 고정 단축키 Visibility: Construct에서 한 번만 설정
- HealthBar: 빨간색
- StaminaBar: 노란색
- 수치: 반올림 정수 `현재 / 최대`
- 컨테이너: 필요한 수만큼 뒤에서 추가 또는 제거

## 향후 단계

AttributeSet 및 인벤토리·로그 이벤트 API가 안정화되면 주기 갱신을 직접 이벤트 갱신으로 교체할 수 있다. 현재 구조는 기존 Blueprint 계약을 유지하면서 프레임당 폴링 비용을 크게 줄이는 안전한 중간 단계다.
