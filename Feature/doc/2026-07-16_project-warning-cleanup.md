# 프로젝트 경고 정리 결과

## 결과

- `BP_BaseMonster`와 배치된 External Actor를 현재 GAS 기반 몬스터 구성으로 다시 저장해 삭제된 `Rogue10mVitalsComponent` 참조를 제거했다.
- 현재 몬스터에는 `Rogue10mAbilitySystemComponent`와 `Rogue10mVitalRegenerationComponent`가 유지된다.
- `DefaultGame.ini`에 `GameplayCueNotifyPaths=/Game/GameplayCues`를 추가해 Ability System이 `/Game/` 전체를 검색하는 fallback을 제거했다.
- `ValidateWarningFixes.py`를 추가해 Blueprint 부모, External Actor의 컴포넌트 구성, GameplayCue 설정을 회귀 검사한다.

## MotionVector 경고 판정

`r.MotionVectorSimulation` 경고는 프로젝트에서 등록한 CVar가 아니다. UE 5.8 엔진의 `MotionVectorSimulation.cpp`가 Render Thread Safe 플래그 없이 변수를 등록하고, TSR 렌더 코드가 Render Thread에서 조회하면서 발생한다.

이번 작업에서는 경고를 숨기기 위한 TSR 비활성화나 렌더링 품질 저하를 적용하지 않았고, 설치된 엔진 소스도 수정하지 않았다. 근본 처리는 해당 등록에 엔진 수준의 Render Thread Safe 플래그를 적용해 엔진을 다시 빌드하거나 수정된 Epic 엔진 버전으로 이동하는 별도 작업이다.

## 검증

- 콘텐츠 검색: `Rogue10mVitalsComponent`, `Vitals Component` 잔여 참조 0건
- 새 `UnrealEditor-Cmd` 프로세스에서 `ValidateWarningFixes.py` 실행 성공
- 결과: `Success - 0 error(s), 0 warning(s)`
- 검증 프로세스에서 Vitals Linker 경고와 GameplayCue 경고 재발 없음
- Headless commandlet은 TSR 렌더 경로를 실행하지 않으므로 MotionVector 경고의 엔진 수정 여부를 검증하는 수단은 아니다.

## 적용 시 참고

이미 열려 있던 Editor는 `DefaultGame.ini` 변경 전 설정을 보유할 수 있으므로 한 번 재시작한 뒤 일반 Editor 로그를 확인한다.
