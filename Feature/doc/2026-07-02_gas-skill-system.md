# GAS 기반 스킬 시스템 전환 1단계 - 개발 요약

## 요약

Rogue10m의 스킬 시스템을 GAS로 전환하기 위한 1단계 기반을 추가했다.
기존 공격 Data Asset과 콤보/쿨타임 로직은 유지하면서, 공격 실행이 먼저 GAS Ability를 통과하도록 브리지 구조를 만들었다.

## 변경 내용

- `Rogue10m.uproject`
  - `GameplayAbilities` 플러그인 활성화
- `Rogue10m.Build.cs`
  - `GameplayAbilities`, `GameplayTags`, `GameplayTasks` 모듈 의존성 추가
  - `Rogue10m/Ability` include path 추가
- 신규 C++ 클래스
  - `URogue10mAbilitySystemComponent`
  - `URogue10mAttributeSet`
  - `URogue10mGameplayAbility_Attack`
- `ARogue10mPlayerState`
  - `IAbilitySystemInterface` 구현
  - `AbilitySystemComponent`, `AttributeSet` 보유
- `ARogue10mCharacter`
  - `IAbilitySystemInterface` 구현
  - PlayerState ASC를 반환
  - 소유/PlayerState 복제 시 `InitAbilityActorInfo` 호출
  - 기본 공격 Ability를 grant
  - 공격 실행 전 GAS Ability 활성화를 먼저 시도
- `URogue10mAttackSkillData`
  - 스킬별 `GameplayAbilityClass` 지정 필드 추가

## 검증 결과

- `powershell -ExecutionPolicy Bypass -File .\Scripts\BuildEditor.ps1`: 성공
- `powershell -ExecutionPolicy Bypass -File .\Scripts\CheckGeneratedChanges.ps1`: 통과

## 남은 작업

- AttributeSet 값을 기존 VitalsComponent와 동기화하거나 VitalsComponent를 GAS 조회 래퍼로 축소한다.
- 스킬 비용/쿨타임을 GameplayEffect로 옮긴다.
- 스킬 해금/무기 숙련도/입력 슬롯을 GameplayTag 기반으로 연결한다.
- 몬스터도 ASC/AttributeSet 적용 대상으로 확장한다.

## Notion 정리 후보

- 스킬 시스템을 GAS로 전환하기 위한 1단계 브리지를 추가했다.
- PlayerState가 ASC와 AttributeSet을 소유하고, Character는 PlayerState ASC를 사용한다.
- 기존 공격 Data Asset은 유지하며, 각 Data Asset은 선택적으로 GameplayAbilityClass를 지정할 수 있다.
- 현재 공격 Ability는 기존 공격 판정 로직을 호출하는 전환용 구조이며, 비용/쿨타임 GameplayEffect 분리는 다음 단계에서 진행한다.
