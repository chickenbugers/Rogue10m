# HUD 갱신 최적화 결과

- `UI_ValueProgressBar`, `UI_ValueText`, `LabelText`를 `URogue10mVitalBarWidget`에 직접 바인딩했다.
- 체력 ProgressBar는 빨간색, 스테미나 ProgressBar는 노란색을 사용한다.
- 수치 텍스트는 소수점 없이 `현재 스탯 / 최대 스탯` 형식으로 표시한다.
- Vital 값과 색상이 이전과 같으면 UMG setter 및 Blueprint 이벤트를 호출하지 않는다.
- HUD 전체 매 프레임 갱신을 10Hz 주요 정보 갱신과 4Hz 슬롯·로그 갱신으로 분리했다.
- 고정 Collapsed 설정은 `NativeConstruct()`에서 한 번만 처리한다.
- 퀵슬롯과 로그는 개수 변경 시 전체 삭제 대신 부족한 위젯만 추가하고 초과 위젯만 제거한다.

## 검증

- UnrealHeaderTool 성공
- Rogue10mEditor Win64 Development 빌드 성공
- WBP_VitalBar 내부 위젯 이름과 BindWidgetOptional 이름 일치 확인
