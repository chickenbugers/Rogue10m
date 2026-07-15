# 공격 공간 형태 및 피해 방식 분리 결과

## 구현 결과

공격 Data Asset에서 공간 형태와 피해 방식을 서로 독립적으로 조합할 수 있다. 기본값은 기존 공격과 호환되는 `직선 박스 + 단타`이다.

## 공간 형태

- `LinearBox`: 전방 `AttackRange` 길이, `BoxHalfWidth`, `BoxHalfHeight` 크기의 직선 박스
- `Projectile`: `HitCount` 펄스에 걸쳐 `AttackRange`까지 전진하는 경량 투사체 경로
- `Arc`: `AttackRange`와 `ArcAngleDegrees`로 구성하는 전방 부채꼴
- `Circle`: 공격자 전방 `CircleForwardOffset` 위치를 중심으로 하는 반경 `AttackRange` 원형

## 피해 방식

- `Single`: 공간을 한 번 판정하고 대상당 한 번 피해
- `Continuous`: `HitCount`만큼 공간을 다시 판정하고 현재 범위 안의 대상에게 매 펄스 피해
- `MultiHit`: 첫 공간 판정에서 맞은 대상을 고정하고 `HitCount`만큼 연속 피해

## 조합 예시

- 검 휘두르기: `Arc + Single`
- 화염 장판: `Circle + Continuous`
- 관통 빔: `LinearBox + Continuous`
- 마법탄 연속 피해: `Projectile + MultiHit`
- 기관총 형태: `Projectile + Continuous`

## 플레이어와 몬스터

- 플레이어는 `Rogue10mCombatComponent`가 형태별 후보를 수집하고 피해 방식을 실행한다.
- 몬스터는 `BP_BaseMonster`의 `AttackSkillData`에 같은 Data Asset 타입을 지정한다.
- 몬스터의 `AttackSkillData`가 비어 있으면 기존 `AttackRange`, `AttackDamage`, `AttackInterval` 단타 동작을 유지한다.

## 성능

- Tick 기반 공격 판정을 추가하지 않았다.
- 타이머 펄스와 약한 대상 참조를 사용한다.
- 투사체 공격별 Actor를 생성하지 않는다.
- `MaxTargetsPerHit`로 펄스당 처리 대상을 제한한다.

## 검증

- UnrealHeaderTool 성공
- Rogue10mEditor Win64 Development 빌드 성공
- 생성 파일 및 diff 검사 예정

## 최소·최대 피해 범위

모든 공격자는 AttributeSet에서 다음 비율을 가진다.

- `MinDamageRatio`: 기본값 `0.9`
- `MaxDamageRatio`: 기본값 `1.1`

스킬은 캐릭터 비율을 기본으로 사용하며 다음 값으로 추가 보정한다.

- `MinDamageRatioMultiplier`: 기본값 `1.0`
- `MaxDamageRatioMultiplier`: 기본값 `1.0`

최종 계산식:

```text
최종 최소 비율 = 캐릭터 최소 비율 × 스킬 최소 보정
최종 최대 비율 = 캐릭터 최대 비율 × 스킬 최대 보정
최종 피해 = 스킬 기본 피해 × Random(최종 최소 비율, 최종 최대 비율)
```

최소·최대 결과가 역전되면 자동으로 정렬하며 음수 비율은 0으로 제한한다. 연속 공격과 다단히트는 각 타수마다 피해량을 독립적으로 계산한다.

예시: 기본 피해가 100이고 캐릭터 범위가 90~110%, 스킬 보정이 80~120%이면 최종 피해 범위는 72~132이다.

## 치명타

캐릭터 Attribute 기본값:

- `CriticalChance`: `0.0` — 기본 치명타 확률 0%
- `CriticalDamageMultiplier`: `1.5` — 치명타 피해 150%

스킬 Data Asset 보정:

- `CriticalChanceBonus`: 캐릭터 치명타 확률에 더하는 값. `0.1`은 10%p 증가
- `CriticalDamageMultiplierBonus`: 캐릭터 치명타 피해 배율에 더하는 값. `0.2`는 150%를 170%로 변경

피해 범위 랜덤 계산 후 치명타를 판정하며, 치명타가 발생하면 계산된 피해에 최종 치명타 배율을 곱한다. 연속 공격과 다단히트는 각 타수마다 독립적으로 치명타를 판정한다.

### 장비·패시브·액티브 연결

- 장비: 장착 중 Infinite Gameplay Effect로 치명타 Attribute 변경
- 패시브: 패시브 습득 시 Infinite Gameplay Effect 적용, 스킬 제거 시 Effect 해제
- 액티브 버프·디버프: Duration Gameplay Effect로 일시 증가 또는 감소
- 개별 공격 스킬: Data Asset의 치명타 보너스 필드 사용

치명타 확률은 최종적으로 0~100%로 제한한다. 치명타 피해 배율은 음수가 되지 않도록 제한한다.
