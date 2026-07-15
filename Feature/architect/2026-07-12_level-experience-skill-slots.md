# 레벨·경험치 및 스킬 슬롯 UI 설계

## Ultrawork Packet 1: 레벨·경험치 패널

- 목표: 경험치 소수점 3자리와 정수 레벨을 표시하고 에셋 이름을 명확하게 변경한다.
- 변경 영역: PlayerState 진행도 Getter, HUD View, ProgressionWidget, WBP_Progression 에셋 이름.
- 완료 조건: `0.000 / 100.000`, 정수 레벨, `WBP_LevelExperiencePanel` 이름 사용.
- 검증: Editor 빌드, 에셋 참조 확인.
- 롤백 경계: 진행도 타입/표시 변경 및 에셋 rename.

## Ultrawork Packet 2: 스킬 입력 슬롯

- 목표: 다섯 입력 유형을 고정 순서로 표시하고 해금 상태와 콤보 공개 범위를 반영한다.
- 변경 영역: CombatComponent 조회 API, HUD QuickSlot View, QuickSlotWidget 네이티브 바인딩.
- 완료 조건: 좌클릭, 우클릭, 좌클릭 차징, 우클릭 차징, 스페이스 순서 유지. 잠긴 슬롯 비활성. 콤보는 처음 잠긴 단계 전까지만 표시.
- 검증: Editor 빌드, WBP_QuickSlot 바인딩 이름 확인.
- 롤백 경계: QuickSlot View 및 Widget 변경.

## 에셋 안전 규칙

- `.uasset`은 Unreal Editor API를 통해서만 이름을 변경한다.
- 사용자가 편집한 Identity 및 SkillSlotPanel 레이아웃은 직접 바이너리 수정하지 않는다.