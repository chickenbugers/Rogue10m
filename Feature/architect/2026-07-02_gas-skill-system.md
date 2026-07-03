# GAS 기반 스킬 시스템 전환 설계

## 목표

기존 `Rogue10mCombatComponent` 중심의 공격/스킬 실행 구조를 Unreal Gameplay Ability System 기반으로 점진 전환한다.

## 1단계 범위

- 프로젝트에 `GameplayAbilities`, `GameplayTags`, `GameplayTasks` 의존성을 추가한다.
- `PlayerState`에 `AbilitySystemComponent`와 `AttributeSet`을 배치한다.
- 캐릭터는 `IAbilitySystemInterface`를 구현하고 `PlayerState`의 ASC를 반환한다.
- 공격 Data Asset에서 실행할 `GameplayAbility` 클래스를 지정할 수 있게 한다.
- 기본 공격용 `GameplayAbility`를 추가하고 기존 공격 실행 로직을 Ability에서 호출할 수 있게 연결한다.

## 현재 유지하는 것

- 기존 공격 판정, 데미지 적용, 몽타주 재생, 콤보 윈도우, 자원 소비, HUD 쿨타임 표시는 그대로 유지한다.
- 기존 Data Asset은 계속 사용한다.
- 비용과 쿨타임은 아직 GameplayEffect로 옮기지 않는다.

## 다음 단계

- 체력/스테미나/마나를 `AttributeSet` 기반으로 이전한다.
- 공격 비용과 쿨타임을 `GameplayEffect`로 분리한다.
- 스킬 해금 상태를 GameplayTag 또는 ASC Granted Ability로 연결한다.
- `AttackSkillData`를 GAS Ability/Effect/Tag 구성 Data Asset으로 확장한다.
- UI 쿨타임은 ASC 쿨타임 태그 또는 GameplayEffect 남은 시간 조회로 전환한다.

## 위험 요소

- 기존 VitalsComponent와 AttributeSet이 동시에 존재하므로 값 동기화 정책이 필요하다.
- 현재 Ability는 기존 실행 로직을 호출하는 브리지이며 완전한 GAS 비용/쿨타임 모델은 아니다.
- 에디터에서 Data Asset마다 `GameplayAbilityClass`를 지정하지 않아도 기본 Ability로 동작하지만, 스킬별 Ability 분화는 별도 작업이 필요하다.
