# 프로젝트 경고 정리 설계

## 범위

사용자가 제시한 세 경고를 프로젝트에서 고칠 수 있는 항목과 UE 5.8 엔진 자체 항목으로 분리한다.

1. `BP_BaseMonster`의 삭제된 `Rogue10mVitalsComponent` 직렬화 참조
2. Gameplay Ability System의 전역 GameplayCue 검색 경고
3. `r.MotionVectorSimulation`의 Render Thread 안전 플래그 경고

## 원인 분석

- 몬스터 생명력은 현재 `URogue10mAttributeSet`, `URogue10mAbilitySystemComponent`, `URogue10mVitalRegenerationComponent`로 이전됐지만, `BP_BaseMonster` 및 레벨 External Actor 패키지에는 삭제된 이전 컴포넌트 클래스 경로가 남아 있었다.
- `DefaultGame.ini`에 `GameplayCueNotifyPaths`가 없어서 Ability System이 `/Game/` 전체를 검색했다.
- UE 5.8의 `MotionVectorSimulation.cpp`는 `r.MotionVectorSimulation`을 Render Thread Safe 플래그 없이 등록하지만 TSR 렌더 경로가 이를 Render Thread에서 조회한다. 프로젝트 코드나 설정에서 등록한 CVar가 아니다.

## 처리 계획

- 현재 Editor에서 `BP_BaseMonster`를 로드·컴파일·저장해 클래스 기본 오브젝트의 구형 직렬화 참조를 제거한다.
- 해당 몬스터가 배치된 World Partition External Actor를 Asset Registry로 로드하고 다시 저장한다.
- `DefaultGame.ini`의 `AbilitySystemGlobals`에 `/Game/GameplayCues` 전용 검색 경로를 지정한다.
- 자동 검증 스크립트에서 Blueprint 부모 클래스, External Actor 컴포넌트 구성, GameplayCue 설정을 검사한다.
- MotionVector 경고를 숨기기 위해 TSR이나 모션 벡터 기능을 끄지 않는다. 설치된 엔진 소스도 이 작업에서 수정하지 않는다.

## 완료 조건

- 콘텐츠 바이너리에서 `Rogue10mVitalsComponent` 및 `Vitals Component` 문자열 참조가 발견되지 않는다.
- 새 UE 프로세스에서 검증 스크립트가 성공하고 프로젝트 측 두 경고가 출력되지 않는다.
- 생성물 검사와 diff 공백 검사가 통과한다.

## 위험과 롤백 경계

- External Actor는 레벨 배치 인스턴스 하나만 다시 저장하며 다른 레벨 자산을 변경하지 않는다.
- GameplayCue 경로는 향후 Cue 자산의 표준 위치를 `/Game/GameplayCues`로 고정한다.
- MotionVector 경고의 근본 수정은 Epic 엔진 업데이트 또는 별도 커스텀 엔진 패치 범위다.
