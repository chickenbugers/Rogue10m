# 몬스터 Behavior Tree / Blackboard 전투 AI 설계

## Scope Gate

기존 `ARogue10mBasicMonster`의 매 프레임 플레이어 탐색·직선 이동을 제거하고, UE 5.8의 `AIController`, `AIPerception`, `BehaviorTree`, `Blackboard`, Navigation 이동을 사용한다. 몬스터는 스폰 위치를 기준으로 제한된 범위만 이동하며, 시야 거리 또는 피격 사건으로 플레이어를 인식한 경우에만 추적·전투한다.

## 상태 흐름

```text
Idle / Patrol
  ├─ Sight: DetectionRange 안에서 플레이어 감지
  └─ Damage: 플레이어에게 피격
        ↓
Acquire Target
        ↓
Chase (Home 기준 MaxChaseDistance 제한)
  ├─ 공격 범위 진입 → Attack
  ├─ 시야 상실 → LastKnownLocation / ForgetTargetSeconds 유지
  └─ Leash 이탈·대상 사망·기억 만료 → Clear Target
        ↓
Return Home
        ↓
Idle / Patrol
```

## Blackboard 키

| 키 | 타입 | 용도 |
|---|---|---|
| `TargetActor` | Object/Actor | 현재 전투 대상 |
| `HomeLocation` | Vector | 스폰 기준 복귀 위치 |
| `PatrolLocation` | Vector | 제한 반경 내 다음 이동 위치 |
| `LastKnownLocation` | Vector | 마지막으로 인식한 대상 위치 |
| `HasTarget` | Bool | 전투 분기 여부 |
| `IsInAttackRange` | Bool | 추적/공격 판단 |
| `WasDamaged` | Bool | 피격으로 획득한 어그로 표시 |

## C++ 책임

- `ARogue10mMonsterAIController`
  - AI Perception Sight 구성
  - 피격 인식과 Blackboard 갱신
  - Home/Patrol/Leash/Forget 조건 판단
  - Navigation 기반 이동 및 공격 요청
  - 외부 BT가 없을 때 네이티브 기본 BT/BB 제공
- `UBTTask_Rogue10mMonsterDecision`
  - 일정 간격으로 AIController의 단일 판단을 실행
- `ARogue10mBasicMonster`
  - Tick 기반 탐색·이동 제거
  - AIController가 지정한 대상에 기존 공격 시퀀스 실행
  - 피격 시 Damage Sense/Controller에 즉시 어그로 전달
- `URogue10mMonsterDataAsset`
  - BT, 시야 상실 거리, 기억 시간, 순찰 반경, 순찰 대기 시간, 최대 추적 거리 보유

## 기본 Behavior Tree

```text
Root Sequence
  ├─ Rogue10m Monster Decision
  └─ Wait (0.25s)
```

Decision Task는 Blackboard 상태를 기준으로 Patrol, Chase, Attack, Return Home 중 하나를 수행한다. 외부 `BT_Monster`와 `BB_Monster` 에셋은 같은 키와 노드 구조를 저장하고, 몬스터 Data Asset에서 교체할 수 있다.

## Ultrawork Packets

### Packet 1 — 로비 UI 보완

- 목표: Status 제거, 정면 프리뷰, 16:9 화면 채움 배경
- 수정 위치: CharacterLobbyWidget, PreviewActor, WBP 구성 스크립트
- 완료 조건: Status BindWidget 부재, 초기 Yaw 0도, 배경 브러시가 1920×1080 할당 영역 사용
- 검증: Editor 빌드, Character Selection Validator
- 롤백 경계: UI C++/WBP/스크립트

### Packet 2 — AI 데이터와 Controller

- 목표: 몬스터별 인식·순찰·추적 제한 데이터와 AIController 추가
- 수정 위치: MonsterDataAsset, BasicMonster, Enemy/AI
- 완료 조건: 시야·피격 대상 획득, Home/Leash/Forget 조건 동작
- 검증: Editor 빌드
- 롤백 경계: AI C++와 Data Asset 필드

### Packet 3 — BT/BB 에셋

- 목표: 공통 `BT_Monster`, `BB_Monster` 생성 및 몬스터 기본 참조 연결
- 수정 위치: `Content/AI/Monster`, Editor Python
- 완료 조건: 필수 7개 키, Decision+Wait 트리, Data Asset 참조
- 검증: Monster AI Asset Validator
- 롤백 경계: AI uasset과 구성 스크립트

### Packet 4 — 회귀 검증과 문서화

- 목표: 기존 공격·사망·경험치와 로비 기능 회귀 방지
- 수정 위치: Validator, Feature/doc, SprintChangeLog, DevLog
- 완료 조건: UE 5.8 빌드·에셋 검증·생성물 검사 통과
- 검증: BuildEditor, Editor Python Validator, CheckGeneratedChanges, diff check
- 롤백 경계: 스크립트와 문서

## 안전 조건

- 몬스터 Actor Tick은 사용하지 않는다.
- 생성자에서 World·Player·Subsystem 조회를 하지 않는다.
- 동적 Perception Delegate 콜백은 `UFUNCTION()`으로 선언한다.
- UObject 참조는 `UPROPERTY`와 `TObjectPtr`, 대상 캐시는 Blackboard/TWeakObjectPtr로 관리한다.
- 이동은 NavMesh를 사용하며 순찰 지점은 Home 기준 PatrolRadius 내부로 제한한다.
- 대상이 MaxChaseDistance를 벗어나면 공격을 중단하고 Home으로 복귀한다.
- `.uasset`은 Unreal Editor API로만 생성·수정한다.