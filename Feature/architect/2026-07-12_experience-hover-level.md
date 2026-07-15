# 경험치 Hover 및 레벨 표기 설계

## 목표

- 경험치 바 앞쪽 `UI_LevelText`에 현재 레벨을 표시한다.
- 경험치 바와 경험치 텍스트 Hover 시 현재 경험치와 다음 레벨까지 필요한 경험치를 표시한다.
- Blueprint Tick 바인딩 없이 진행도 데이터 변경 시에만 UMG 속성을 갱신한다.

## 변경 영역

- `URogue10mProgressionWidget`
- `WBP_Progression`의 기존 네이티브 바인딩 이름 사용

## 완료 조건

- `UI_LevelText`: `Lv. N`
- `UI_ExperienceText`: `현재 경험치 / 다음 레벨 요구 경험치`
- `UI_ExperienceBar`, `UI_ExperienceText`: 동일한 상세 툴팁 제공
- 동일 데이터의 중복 setter 호출 방지

## 검증

- `Scripts/BuildEditor.ps1`
- `Scripts/CheckGeneratedChanges.ps1`

## 롤백 경계

- `Rogue10mHudWidgetParts.h/.cpp`의 ProgressionWidget 변경만 되돌리면 기능 제거 가능