# Rogue10m 애니메이션 및 설정 추가 위치 가이드

## 목적

캐릭터 공격 애니메이션과 몬스터 이동/공격/피격 애니메이션을 추가할 때 필요한 에셋 위치, Blueprint 연결 지점, C++ 확장 지점을 정리한다.

## 추천 Content 폴더 구조

- `Content/Rogue10m/Characters/Player/Animations`
  - 플레이어 1인칭/3인칭 기본 이동, 점프, 피격, 사망 애니메이션
  - 맨손 기본 공격, 맨손 특수 공격, 점프 공격, 차징 공격 애니메이션
- `Content/Rogue10m/Characters/Player/Blueprints`
  - `BP_FirstPersonCharacter`
  - 플레이어용 Animation Blueprint
- `Content/Rogue10m/Characters/Monsters/Common/Animations`
  - 몬스터 공통 Idle, Walk/Run, Attack, Hit, Death 애니메이션
- `Content/Rogue10m/Characters/Monsters/Common/Blueprints`
  - 기본 몬스터 Blueprint
  - 몬스터용 Animation Blueprint
- `Content/DataAsset/Animations`
  - 무기 타입별 애니메이션 설정 Data Asset
  - 몬스터 타입별 애니메이션 설정 Data Asset

## 플레이어 공격 애니메이션

현재 캐릭터는 다음 입력을 C++에서 구분한다.

- 좌클릭: 기본 공격
- 우클릭: 특수 공격
- 점프 중 좌클릭/우클릭: 점프 공격
- 길게 누른 뒤 떼기: 차징 공격

추가할 애니메이션은 무기 타입별로 나누는 것을 권장한다.

- `Unarmed`: 맨손 기본 공격, 맨손 특수 공격, 맨손 점프 공격, 맨손 차징 공격
- `Dagger`: 단검 기본/특수/점프/차징 공격
- `GreatSword`: 대검 기본/특수/점프/차징 공격
- `DualBlades`: 쌍검 기본/특수/점프/차징 공격
- `Bow`: 활 기본 사격, 차징 사격, 점프 사격
- `Staff`: 지팡이 기본 시전, 특수 시전, 차징 시전
- `Knuckle`: 권 기본/특수/점프/차징 공격

## 플레이어 Blueprint 연결 위치

1. `BP_FirstPersonCharacter`에 플레이어용 Animation Blueprint를 연결한다.
2. 1인칭 팔/무기 표현이 필요한 경우 `FirstPersonMesh`에 전용 Animation Blueprint를 연결한다.
3. 장비창 프리뷰와 외부에서 보는 캐릭터 표현은 기본 `GetMesh()` 쪽 Animation Blueprint를 사용한다.
4. 공격 애니메이션은 Animation Montage로 관리하고, C++ 공격 분기에서 Montage 재생 함수를 호출하는 방식으로 확장한다.

## 플레이어 C++ 확장 위치

- `Source/Rogue10m/Character/Rogue10mCharacter.cpp`
  - `ExecuteCombatAttack`: 공격 종류와 무기 타입이 결정되는 위치
  - `DoUnarmedAttack`: 현재 맨손 공격 판정 위치
  - 이후 `PlayAttackMontage` 같은 함수를 추가해 공격 판정 전에 Montage를 재생한다.

권장 흐름:

1. 입력을 받는다.
2. 무기 타입과 공격 타입을 결정한다.
3. 해당 공격 애니메이션 Montage를 재생한다.
4. Montage Notify에서 실제 피해 판정 함수를 호출한다.
5. 피해 숫자와 피격 효과는 현재 HUD 피드백을 재사용한다.

## 몬스터 애니메이션

기본 몬스터에 필요한 최소 애니메이션은 다음과 같다.

- Idle: 대기 상태
- Walk 또는 Run: 플레이어 추적 이동
- Attack: 공격 시도
- Hit: 피해를 받았을 때 짧은 반응
- Death: 사망 연출

## 몬스터 Blueprint 연결 위치

1. 기본 몬스터 Blueprint에 Skeletal Mesh를 지정한다.
2. 몬스터용 Animation Blueprint를 연결한다.
3. Animation Blueprint는 이동 속도, 사망 여부, 공격 여부, 피격 여부 변수를 읽어 상태를 전환한다.
4. 공격 Animation Montage에는 실제 피해 타이밍에 Anim Notify를 추가한다.

## 몬스터 C++ 확장 위치

- `Source/Rogue10m/Enemy/Rogue10mBasicMonster.cpp`
  - `MoveTowardTarget`: 이동 상태가 발생하는 위치
  - `TryAttackTarget`: 공격 시도가 발생하는 위치
  - `TakeDamage`: 피격 상태가 발생하는 위치
  - `Die`: 사망 상태가 발생하는 위치

권장 흐름:

1. `TryAttackTarget`에서 즉시 피해를 주기보다 공격 Montage를 재생한다.
2. Montage Notify 시점에 플레이어에게 피해를 적용한다.
3. `TakeDamage`에서 Hit Montage 또는 Hit 상태 변수를 짧게 활성화한다.
4. `Die`에서 Death Montage를 재생하고, Montage 종료 후 Destroy 여부를 결정한다.

## Data Asset 설계 후보

애니메이션 데이터는 다음 Data Asset으로 분리하는 것을 권장한다.

- `URogue10mWeaponAnimationData`
  - 무기 타입
  - 기본 공격 Montage
  - 특수 공격 Montage
  - 점프 기본 공격 Montage
  - 점프 특수 공격 Montage
  - 차징 기본 공격 Montage
  - 차징 특수 공격 Montage
  - 공격 판정 시작/종료 태그 또는 Notify 이름
- `URogue10mMonsterAnimationData`
  - 몬스터 타입
  - Idle/Move 상태용 Animation Blueprint 또는 BlendSpace 참조
  - Attack Montage
  - Hit Montage
  - Death Montage
  - 공격 피해 적용 Notify 이름

## 구현 우선순위

1. 맨손 공격 Montage 1개를 먼저 연결한다.
2. Montage Notify에서 현재 `DoUnarmedAttack` 판정을 호출하도록 바꾼다.
3. 기본 몬스터 Attack Montage를 연결하고 Notify로 피해 적용 타이밍을 옮긴다.
4. Hit/Death 애니메이션을 추가해 전투 피드백을 강화한다.
5. 무기별 Montage와 Data Asset 구조로 확장한다.
