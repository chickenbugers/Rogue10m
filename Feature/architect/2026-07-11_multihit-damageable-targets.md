# 다단히트·공격 가능 오브젝트 확장 설계

## 목표

- 한 번의 플레이어 공격이 설정된 횟수와 간격으로 여러 타격을 발생시킬 수 있게 한다.
- 몬스터, 부서지는 벽, 상자 등 서로 다른 Actor를 같은 공격 판정으로 처리한다.
- 다단히트 중 데미지 UserWidget 생성/제거와 GC 부하를 제한한다.

## Ultrawork Packets

### Packet 1 — 공격 대상 계약

- 목표: 공격 가능한 Actor의 공통 인터페이스와 기본 파괴 오브젝트를 제공한다.
- 수정 위치: `Components/Rogue10mAttackTargetInterface.*`, `World/Rogue10mBreakableActor.*`, Monster
- 완료 조건: Monster와 BreakableActor가 같은 인터페이스로 필터링되고 `TakeDamage()`를 사용한다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 인터페이스와 BreakableActor 파일 단위

### Packet 2 — Timer 기반 다단히트 실행

- 목표: 공격 Data Asset 설정에 따라 여러 Sweep 펄스를 실행한다.
- 수정 위치: `Data/Rogue10mAttackSkillData.h`, `Components/Rogue10mCombatComponent.*`
- 완료 조건: HitCount 1은 기존 동작, 3은 HitInterval 간격으로 3회 판정한다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: AttackSkillData 필드와 ActiveAttackExecution 코드

### Packet 3 — 데미지 UserWidget 풀링

- 목표: 다단히트 중 반복 CreateWidget/RemoveFromParent를 없앤다.
- 수정 위치: `Core/Rogue10mPlayerController.*`, `UI/Widgets/Rogue10mDamageIndicatorWidget.*`
- 완료 조건: 시작 시 기본 풀을 생성하고 최대 크기 안에서 위젯을 재사용한다.
- 검증 명령: PIE 다단히트 + Unreal Insights/Stat Slate 육안 확인
- 롤백 경계: PlayerController 풀과 위젯 활성/비활성 API

### Packet 4 — 검증과 문서화

- 목표: 빌드, PIE, 생성물 검사, DevLog를 완료한다.
- 수정 위치: `Feature/doc/`, `DevLog/20260711.txt`
- 완료 조건: Editor 빌드와 생성물 검사가 통과한다.
- 검증 명령: `Scripts/CheckGeneratedChanges.ps1`
- 롤백 경계: 문서 파일

## 성능 원칙

- 공격마다 Tick을 켜지 않고 Timer 펄스만 사용한다.
- `SweepMultiByObjectType` 한 번으로 펄스의 후보를 수집하고 Actor 단위로 중복 제거한다.
- 실행 컨텍스트는 공격 종료 즉시 제거한다.
- UserWidget은 미리 생성해 `Collapsed` 상태로 보관하고 활성화 시 재사용한다.
- 풀 최대치 도달 시 가장 오래된 활성 인디케이터를 재사용해 무제한 할당을 막는다.
