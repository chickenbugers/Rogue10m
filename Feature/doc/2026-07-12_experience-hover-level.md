# 경험치 Hover 및 레벨 표기 결과

## 반영 내용

- `WBP_Progression`의 `UI_ExperienceBar`, `UI_ExperienceText`, `UI_LevelText`를 네이티브로 연결했다.
- 레벨은 `Lv. N`, 경험치는 `현재 / 다음 레벨 요구량` 형식으로 표시한다.
- 경험치 바 또는 경험치 텍스트에 마우스를 올리면 현재 경험치와 다음 레벨까지 남은 경험치를 확인할 수 있다.
- 동일한 진행도 데이터가 들어오면 UI setter와 Blueprint 이벤트 호출을 생략한다.

## Blueprint 구성 조건

- `UI_LevelText`: TextBlock
- `UI_ExperienceBar`: ProgressBar
- `UI_ExperienceText`: TextBlock