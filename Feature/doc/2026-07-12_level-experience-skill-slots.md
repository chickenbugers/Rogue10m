# 레벨·경험치 및 스킬 슬롯 UI 결과

## 레벨·경험치 패널

- `WBP_Progression`을 `WBP_LevelExperiencePanel`로 변경했다.
- `UI_ExperienceText`는 `현재 경험치 / 레벨업 경험치`를 소수점 셋째 자리까지 표시한다.
- `UI_LevelText`는 현재 레벨을 정수만 표시한다.
- 경험치 데이터는 AttributeSet의 float 정밀도를 유지한다.
- 기존 경험치 Hover 상세 정보도 소수점 셋째 자리까지 표시한다.

## 스킬 슬롯 패널

- `WBP_SkillSlotPanel`을 `URogue10mSkillSlotPanelWidget`의 Blueprint 자식으로 변경했다.
- 기존 `UI_SkillSlotFrame1~5`에 `WBP_QuickSlot` 인스턴스를 연결한다.
- 표시 순서는 좌클릭, 우클릭, 좌클릭 차징, 우클릭 차징, 스페이스다.
- 스페이스 슬롯은 현재 실제 입력 동작에 맞춰 회피로 표시하고 항상 활성화한다.
- 공격 스킬이 잠겨 있으면 슬롯은 35% 투명도와 비활성 상태로 유지한다.
- 콤보 표시 이름은 해금된 체인만 화살표로 연결하며 처음 잠긴 콤보에서 중단한다.

## Identity 호환성

- 삭제된 `UI_IdentityNameText`를 C++에서 참조하지 않으므로 추가 바인딩 오류가 없다.

## 조정 위치

- 입력별 스킬 데이터: `URogue10mCombatComponent` Blueprint 기본값
- 기본 해금 목록: `UnlockedAttackSkillNames`
- 슬롯 프레임 배치: `WBP_SkillSlotPanel`
- 개별 슬롯 모양: `WBP_QuickSlot`