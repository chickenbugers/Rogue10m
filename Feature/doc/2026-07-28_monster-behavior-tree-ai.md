# 몬스터 Behavior Tree AI 구현 결과

## 결과

기존 `ARogue10mBasicMonster`의 매 프레임 플레이어 추적을 제거하고, UE 5.8의 `AIController + AI Perception + Blackboard + Behavior Tree + NavigationSystem` 구조로 교체했다. 몬스터는 홈 위치 주변만 순찰하며 시야 거리 안의 플레이어 또는 자신을 공격한 플레이어를 전투 대상으로 인식한다. 대상이 추격 한계를 벗어나거나 시야를 잃은 뒤 기억 시간이 지나면 전투를 종료하고 홈 위치로 복귀한다.

## 구현 내용

- `ARogue10mMonsterAIController`: 시야·피격 감지, 대상 기억, 추격 제한, 순찰, 귀환, 공격 결정을 담당한다.
- `UBTTask_Rogue10mMonsterDecision`: BT가 0.25초 간격으로 C++ 결정을 실행하는 Task다.
- `BB_Monster`: `TargetActor`, `HomeLocation`, `PatrolLocation`, `LastKnownLocation`, `HasTarget`, `IsInAttackRange`, `WasDamaged` 키를 제공한다. UE 기본 `SelfActor` 키도 포함된다.
- `BT_Monster`: `Monster Decision → Wait(0.25초)` 반복 구조다.
- `URogue10mMonsterDataAsset`: BT, 추적 해제 거리, 기억 시간, 순찰 반경·대기, 최대 추격 거리를 몬스터별로 조정할 수 있다.
- `ARogue10mBasicMonster`: Tick 추적을 제거하고 데이터 적용 직후 AI를 재초기화한다. 피격 시 공격자를 Damage Sense 대상으로 전달한다.
- 외부 BT/BB가 없거나 불완전하면 같은 구조의 런타임 기본 BT/BB를 사용한다.

## 동작 흐름

```text
Idle/Patrol
  ├─ 시야 거리에서 플레이어 감지 → Chase/Attack
  └─ 플레이어에게 피격 → 즉시 Chase/Attack

Chase/Attack
  ├─ 공격 거리 안 → 이동 정지 + 기존 몬스터 공격 실행
  ├─ 공격 거리 밖 → NavMesh 경로 추격
  └─ 추격 한계 초과 또는 기억 시간 만료 → Target 해제

Return/Patrol
  └─ 홈 위치 복귀 → 순찰 대기 → 홈 반경 안 무작위 순찰
```

## 에디터 에셋

- `/Game/AI/Monster/BB_Monster`
- `/Game/AI/Monster/BT_Monster`
- `/Game/Monster/BP_BaseMonster`: `Rogue10mMonsterAIController`, Placed/Spawned Auto Possess AI
- `/Game/DataAsset/Monster/**`: 25개 몬스터 데이터에 BT 연결

## 검증

- UE 5.8 `Rogue10mEditor Win64 Development` 빌드 성공
- `ValidateMonsterBehaviorTreeAI.py` 통과: Blackboard 8키, Decision/Wait 2 Task, 25개 데이터 에셋, AI Controller 연결
- `ConfigureMonsterBehaviorTreeAI.py` 재실행 가능
- 실제 이동에는 플레이 레벨에 `NavMeshBoundsVolume`과 유효한 NavMesh가 필요하다.

## 수동 PIE QA

1. 몬스터가 홈 반경 밖으로 무제한 이동하지 않는지 확인한다.
2. 탐지 거리 밖에서는 플레이어를 추격하지 않는지 확인한다.
3. 탐지 거리 밖에서 몬스터를 공격하면 공격자를 즉시 추격하는지 확인한다.
4. 최대 추격 거리 초과 또는 기억 시간 만료 후 홈으로 돌아오는지 확인한다.
5. 공격 거리에서 기존 공격 데미지와 주기가 유지되는지 확인한다.