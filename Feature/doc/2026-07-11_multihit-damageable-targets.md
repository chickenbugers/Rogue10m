# 다단히트 및 범용 공격 대상 구현 결과

## 결과

- 플레이어 공격을 `HitCount`, `HitInterval` 기반의 타이머 펄스로 분리했다.
- 실행 단위로 대상별 적중 횟수를 기록해 같은 대상의 최대 적중 수를 제한한다.
- 펄스마다 동일 액터 중복을 제거하고 `MaxTargetsPerHit`로 처리량을 제한한다.
- Tick과 공격별 Actor 생성을 사용하지 않아 다단히트 수가 늘어도 비용을 제한한다.
- `Rogue10mAttackTargetInterface`를 추가해 몬스터와 파괴 가능 오브젝트를 동일한 공격 경로로 처리한다.
- `ARogue10mBreakableActor`를 블루프린트 파생용 기본 클래스로 추가했다.
- 데미지 인디케이터는 Player Screen의 `UUserWidget` 풀로 재사용하며 24개를 미리 생성하고 최대 48개로 제한한다.

## 데이터 설정

- `HitCount`: 공격 펄스 수
- `HitInterval`: 펄스 사이 시간
- `MaxTargetsPerHit`: 한 펄스에서 처리할 최대 대상 수
- `MaxHitsPerTarget`: 한 공격 실행에서 동일 대상이 맞을 최대 횟수

같은 적을 4회 타격하는 공격은 `HitCount=4`, `MaxHitsPerTarget=4`로 설정한다.

## 확장 방법

- 몬스터 또는 공격 가능한 월드 액터는 `Rogue10mAttackTargetInterface`를 구현한다.
- 부서지는 벽은 `ARogue10mBreakableActor`의 블루프린트 자식을 만들고 메시, 체력, 파괴 연출을 설정한다.
- 데미지 표시 모양은 `URogue10mDamageIndicatorWidget`의 블루프린트 자식에서 `BP_OnDamageIndicatorInitialized`, `BP_OnDamageIndicatorProgress`를 구현해 수정한다.

## 검증

- `Scripts/BuildEditor.ps1`: 성공
- Unreal Editor PIE: 플레이어 기본 공격이 소환한 `ARogue10mBasicMonster`에 5 피해 적용됨
- PIE 로그: 새 런타임 경고 및 오류 없음
- `git diff --check`: 통과

## 잔여 확인

- 실제 다단히트용 Attack Skill Data Asset은 콘텐츠 밸런스에 맞춰 위 값을 1보다 크게 설정해야 한다.
- 파괴 벽의 메시와 파괴 VFX는 블루프린트 콘텐츠에서 지정한다.
