# Rogue10m

## Technical Design Document (TDD)

- **문서 버전:** v0.1
- **작성 기준일:** 2026-07-14
- **엔진:** Unreal Engine 5.8
- **작성 기준:** Rogue10m GDD 및 현재까지 확정된 프로젝트 구조·게임 진행·저장·성장 요구사항
- **문서 상태:** 기술 설계 초안 / 실제 프로젝트 명칭 검증 반영
- **검증 대상:** `D:\Project\Rogue10m\Source\Rogue10m`, `Content`, `Docs`

---

## 1. 기술 목표

Rogue10m은 PC 싱글 플레이를 1차 목표로 개발한다. C++와 Blueprint를 혼합하며, Enhanced Input과 Gameplay Ability System(GAS)을 사용한다.

초기 버전은 싱글 플레이 전용이지만 다음 확장 가능성을 구조에 반영한다.

- Listen Server 기반 협동 멀티플레이
- 모바일 터치 입력과 UI
- 적당한 크기의 오픈 월드형 플레이 맵
- 데이터 기반 무기·스킬·몬스터 확장

멀티플레이와 모바일 기능 자체는 1차 개발 범위에 포함하지 않는다. 다만 서버 권한, 복제 가능한 상태, 입력 추상화 및 플랫폼 독립 UI를 고려해 핵심 시스템의 재작성 비용을 줄인다.

## 2. 확정된 게임 규칙

| 항목 | 결정 내용 |
| --- | --- |
| 플레이 방식 | 싱글 플레이 전용, 향후 Listen Server 확장 고려 |
| 플랫폼 | 1차 PC, 2차 모바일 |
| 개발 방식 | C++와 Blueprint 혼합 |
| 입력 | Enhanced Input |
| 능력 시스템 | GAS |
| 목표 플레이 시간 | 30분~1시간 |
| 맵 | 실제 플레이 가능한 시작 마을, 오픈 월드형 플레이 맵 |
| 웨이브 | 사용하지 않음 |
| 보스 | 플레이 맵의 정해진 위치에 배치되며 처음부터 접근 가능 |
| 승리 | 제한 시간 안에 보스 처치 |
| 패배 | 제한 시간 만료 또는 플레이어 사망 |
| 패배 결과 | 현재 런 제거 후 시작 마을로 복귀 |
| 전투 방식 | 입력 기반, 자동 공격 미지원 |
| 캐릭터 구조 | 하나의 캐릭터가 무기를 교체하여 전투 방식 변경 |
| 세션 성장 | 런 종료 시 초기화 |
| 메타 성장 | 특정 재화로 소규모 영구 강화 제공 |

### 2.1 실제 프로젝트 명칭 확인 결과

| 구분 | 실제 명칭 | 상태 |
| --- | --- | --- |
| 런 규칙 | `ARogue10mGameMode` | 구현됨 |
| 런 상태 | `ARogue10mGameState` | 구현됨 |
| 런 단계 | `ERogue10mRunPhase` | 구현됨 |
| 플레이어 컨트롤러 | `ARogue10mPlayerController` | 구현됨 |
| 플레이어 상태 | `ARogue10mPlayerState` | 구현됨 |
| 플레이어 캐릭터 | `ARogue10mCharacter` | 구현됨 |
| 일반 몬스터 | `ARogue10mBasicMonster` | 구현됨 |
| 플레이어 GAS | `URogue10mAbilitySystemComponent`, `URogue10mAttributeSet` | `ARogue10mPlayerState`가 소유 |
| 전투 | `URogue10mCombatComponent` | 구현됨 |
| 인벤토리 | `URogue10mInventoryComponent` | 구현됨 |
| 피드백 | `URogue10mPlayerFeedbackComponent` | 구현됨 |
| 자원 회복 | `URogue10mVitalRegenerationComponent` | 구현됨 |
| 무기 타입 | `ERogue10mWeaponType` | 구현됨 |
| 공격 데이터 | `URogue10mAttackSkillData` | 구현됨 |
| 무기 스킬 프로필 | `URogue10mWeaponSkillProfileDataAsset` | 구현됨 |
| 회피 데이터 | `URogue10mDodgeSkillDataAsset` | 구현됨 |
| 캐릭터 데이터 | `URogue10mCharacterDataAsset` | 구현됨 |
| 몬스터 데이터 | `URogue10mMonsterDataAsset` | 구현됨 |
| 아이템 데이터 | `URogue10mItemDataAsset` | 구현됨 |
| 시작 맵 | `StartHub` | 정의됨 |
| 플레이 맵 | `OpenRunMap` | 정의됨 |
| 런 포탈 | `ARogue10mRunPortal` | 구현됨 |

위 목록에 없는 클래스명은 신규 구현을 위한 계획 명칭으로 표기한다.

## 3. 게임 진행 상태

### 3.1 진행 흐름

```text
시작 마을
  → 플레이 준비
  → 플레이 맵 진입
  → 런 시작 및 제한 시간 진행
  → 탐험 / 전투 / 성장
  → 보스 전투
      ├─ 제한 시간 안에 보스 처치: 승리
      ├─ 제한 시간 만료: 패배
      └─ 플레이어 사망: 패배
  → 런 데이터 정산 및 제거
  → 시작 마을 복귀
```

### 3.2 현재 구현된 런 단계

실제 코드는 `ERogue10mRunPhase`를 사용한다.

```cpp
UENUM(BlueprintType)
enum class ERogue10mRunPhase : uint8
{
    WaitingToStart,
    Running,
    Victory,
    Defeat
};
```

`ARogue10mGameState`가 현재 단계를 보관하고 `OnRunPhaseChanged`를 통해 변경을 알린다. 마을·맵 진입·마을 복귀는 현재 이 enum에 포함되지 않으며 `ARogue10mRunPortal`과 레벨 이동으로 처리한다.

일시정지는 Unreal의 Pause 상태로 별도 관리한다. 향후 맵 전환 UI에 명시적인 단계가 필요해질 때 기존 `ERogue10mRunPhase`를 확장하며, 별도의 `ERogue10mGamePhase`를 중복 생성하지 않는다.
## 4. 맵 구조

### 4.1 시작 마을 맵

시작 마을은 메뉴 전용 공간이 아니라 캐릭터가 직접 이동하고 상호작용하는 실제 플레이 공간이다.

주요 기능:

- 런 시작 및 플레이 맵 입장
- 무기와 장비 관리
- 인벤토리 관리
- 스킬북 사용
- 퀘스트 수락 및 보상 수령
- 메타 재화 사용
- 영구 강화 적용
- 설정 및 저장 관리

### 4.2 플레이 맵

30분~1시간 동안 탐험과 성장을 수행하는 적당한 규모의 오픈 월드형 맵이다.

주요 기능:

- 지역 단위 몬스터 조우
- 탐험 및 아이템 획득
- 퀘스트 진행
- 세션 경험치와 스킬 성장
- 처음부터 접근 가능한 보스 지역
- 제한 시간 기반 승패 판정

### 4.3 맵 전환

맵 전환 시 GameInstance 또는 전용 Session Subsystem이 다음 데이터를 임시 보관한다.

- 선택한 무기와 장비
- 영구 성장 정보
- 현재 저장 슬롯
- 새 런 초기화 데이터
- 맵 이동 목적과 목적지

향후 Listen Server를 적용할 경우 Server Travel과 Seamless Travel을 검토한다.

## 5. Unreal Gameplay Framework 설계

### 5.1 현재 구현 클래스 구조

```text
ARogue10mGameMode
ARogue10mGameState
ARogue10mPlayerController
ARogue10mPlayerState
├─ URogue10mAbilitySystemComponent
└─ URogue10mAttributeSet

ARogue10mCharacter
├─ URogue10mInventoryComponent
├─ URogue10mCombatComponent
├─ URogue10mPlayerFeedbackComponent
└─ URogue10mVitalRegenerationComponent

ARogue10mBasicMonster
├─ URogue10mAbilitySystemComponent
├─ URogue10mAttributeSet
└─ URogue10mVitalRegenerationComponent

ARogue10mRunPortal
URogue10mMapDataLibrary
ARogue10mHUD
URogue10mRunHUD
└─ URogue10mMainHUDWidget
```

`StartHub`와 `OpenRunMap`은 현재 `URogue10mMapDataLibrary`에 정의되어 있다. 플레이 맵 이동은 `ARogue10mRunPortal`이 `StartRun=1` Travel Option을 전달하고 `ARogue10mGameMode`가 이를 읽어 런을 시작하는 구조다.

다음 명칭은 아직 코드에 없으며 계획 명칭이다.

- `ARogue10mBossCharacter`
- `ARogue10mEncounterRegion`
- `URogue10mWorldSpawnSubsystem`
- `URogue10mMetaSaveGame`
- `URogue10mRunSaveGame`
- 저장 및 런 관리용 GameInstance 또는 Subsystem

### 5.2 GameMode 책임

현재 구현 클래스는 `ARogue10mGameMode` 하나다.

현재 책임:

- `StartPlay()`에서 런 시작 조건 확인
- `StartRun=1` Travel Option 확인
- `ARogue10mGameState`의 런 시작 호출
- 기본 20분 또는 프로토타입 30초 실행 시간 선택
- 런 단계 변경 이벤트 수신
- 패배 처리

추가할 책임:

- 보스 처치 이벤트 검증 및 승리 확정
- 사망·시간 만료 시 Run Save 무효화
- 승패 정산 후 `StartHub` 복귀

현재 단계에서는 존재하지 않는 `ARogue10mVillageGameMode`나 `ARogue10mRunGameMode`로 문서를 분리하지 않는다. 마을 규칙이 복잡해져 맵별 GameMode 분리가 실제로 필요해질 때 파생 클래스를 추가한다.

### 5.3 GameState 책임

현재 구현 클래스는 `ARogue10mGameState`다.

현재 관리 정보:

- `ERogue10mRunPhase`
- `RunDurationSeconds`와 `RemainingRunSeconds`
- `RunSeed`
- `CurrentFloor`
- `DefeatedEnemyCount`
- `OnRunTimerUpdated`
- `OnRunPhaseChanged`

현재 기본 제한 시간은 코드상 1,200초(20분)이며 프로토타입 검증용 30초 오버라이드가 있다. 기획의 전체 목표 플레이 시간인 30분~1시간과 보스 제한 시간 20분은 서로 다른 값으로 문서화한다. 보스 제한 시간을 30분 이상으로 변경하려면 별도 밸런스 결정을 거쳐 `RunDurationSeconds`를 수정한다.

향후 Listen Server에서는 공개 상태를 Replicated 또는 RepNotify로 동기화하고 UI는 GameState의 Delegate를 구독한다.

### 5.4 PlayerController 책임

- Enhanced Input Mapping Context 등록과 전환
- 이동, 시점, 공격, 스킬 및 상호작용 입력 전달
- HUD 생성과 입력 모드 전환
- 일시정지 요청
- 서버에 게임플레이 요청 전달

게임 규칙, 인벤토리 데이터 또는 데미지 계산은 PlayerController에 두지 않는다.

### 5.5 PlayerState 책임

`ARogue10mPlayerState`는 이미 다음 상태를 소유한다.

- `URogue10mAbilitySystemComponent`
- `URogue10mAttributeSet`
- 캐릭터 표시 이름과 직업 표시 이름
- `ERogue10mWeaponType` 기반 현재 장착 무기
- 사망 상태
- 플레이어 레벨과 경험치 접근 API
- 무기 숙련도 접근 API
- Identity 타입과 마나 활성 여부

따라서 ASC 소유 위치는 미정이 아니라 PlayerState로 확정한다. `ARogue10mCharacter`는 Possess 및 PlayerState 변경 시 ASC ActorInfo를 초기화하고 PlayerState의 GAS 상태를 사용한다.

## 6. 플레이어 캐릭터와 컴포넌트

### 6.1 현재 캐릭터 구성

```text
ARogue10mCharacter
├─ FirstPersonMesh
├─ FirstPersonCameraComponent
├─ URogue10mInventoryComponent
├─ URogue10mCombatComponent
├─ URogue10mPlayerFeedbackComponent
└─ URogue10mVitalRegenerationComponent

ARogue10mPlayerState
├─ URogue10mAbilitySystemComponent
└─ URogue10mAttributeSet
```

TDD 초안에 있던 `URogue10mEquipmentComponent`, `URogue10mProgressionComponent`, `URogue10mInteractionComponent`, `URogue10mWeaponComponent`는 현재 구현되어 있지 않다. 장비는 Inventory 및 UI 구조에 일부 포함되어 있고, 레벨·경험치와 무기 상태는 PlayerState·AttributeSet·CombatComponent가 담당한다.

### 6.2 현재 컴포넌트 책임

| 컴포넌트 | 실제 책임 |
| --- | --- |
| `URogue10mCombatComponent` | 공격 데이터 선택, 입력별 스킬 바인딩, 차징, GAS Ability, 자원 비용, 판정, 콤보와 쿨타임 |
| `URogue10mInventoryComponent` | NxM 그리드, 가방 컨테이너, 아이템 스택, 획득 이벤트 |
| `URogue10mPlayerFeedbackComponent` | 플레이어 전투 피드백 처리 |
| `URogue10mVitalRegenerationComponent` | Tick 없이 타이머로 AttributeSet 자원 회복 |

향후 분리 후보:

- 장비 규칙이 Inventory에서 커질 경우 `URogue10mEquipmentComponent`
- 무기 숙련도와 스킬 해금이 PlayerState·CombatComponent에서 커질 경우 `URogue10mProgressionComponent`
- NPC·포탈·상자 상호작용이 공통화될 경우 `URogue10mInteractionComponent`

현재 존재하지 않는 컴포넌트를 구현 완료된 의존성으로 사용하지 않는다.

## 7. 무기 기반 전투 클래스 구조

검사, 권사, 마법사, 도적은 별도의 플레이어 Character 클래스가 아니다. 하나의 `ARogue10mCharacter`가 무기를 교체하면서 해당 전투 역할을 수행한다.

| 무기 타입 | `ERogue10mWeaponType` 값 | 전투 성격 |
| --- | --- | --- |
| 맨손 | `Unarmed` | 기본 주먹 상태 |
| 단검 | `Dagger` | 빠른 근접 공격 |
| 대검 | `GreatSword` | 느리지만 강한 근접 공격 |
| 쌍검 | `DualBlades` | 다단 히트와 빠른 연계 |
| 활 | `Bow` | 거리 유지와 차지 공격 |
| 지팡이 | `Staff` | 마법·기력 계열 스킬 |
| 권갑 | `Knuckle` | 주먹 기반 전투 강화 |

### 7.1 현재 무기·공격 데이터

현재 별도의 `URogue10mWeaponDataAsset`은 없다. 다음 구조를 사용한다.

- `ERogue10mWeaponType`: 장착 무기와 숙련도 분류
- `URogue10mWeaponSkillProfileDataAsset`: 무기 타입별 입력 스킬 및 기본 회피 구성
- `URogue10mDodgeSkillDataAsset`: 이동 거리, 지속 시간, 쿨타임, 스태미나 비용
- `URogue10mAttackSkillData`: 공격 입력 슬롯, 자원 비용, 공격 형태, Hit Mode 및 실행 데이터
- `URogue10mItemDataAsset`: 인벤토리·장비 아이템 정의
- `ARogue10mPlayerState`: 현재 장착 무기 타입 저장
- `URogue10mCombatComponent`: 무기 프로필 적용과 스킬 바인딩

현재 콘텐츠에는 `DA_SkillProfile_Unarmed`, `DA_Dodge_Unarmed` 및 맨손 공격 Data Asset들이 존재한다.

### 7.2 무기 교체 흐름

```text
교체 요청
→ 교체 가능 상태 확인
→ 기존 무기 Ability 및 Gameplay Effect 제거
→ 기존 무기 외형 제거
→ 신규 무기 데이터 적용
→ 신규 Ability Set 부여
→ 애니메이션 및 HUD 갱신
→ 교체 완료 이벤트 발생
```

Ability 제거 시 다른 시스템이 부여한 동일 Ability까지 제거하지 않도록 부여 핸들 또는 출처 정보를 저장한다.

## 8. GAS 설계 방향

GAS 담당 범위:

- 기본 공격과 액티브 스킬
- 무기별 Ability Set
- 체력, 공격력, 방어력, 이동속도 등의 Attribute
- 버프와 디버프
- 쿨다운과 자원 비용
- 상태이상
- 데미지 계산

권장 Gameplay Tag 예시:

```text
State.Dead
State.Attacking
State.Stunned
State.WeaponChanging
Weapon.Sword
Weapon.Gauntlet
Weapon.Staff
Weapon.Dagger
Ability.Attack.Basic
Ability.Skill
Event.Combat.Hit
Event.Character.Death
```

현재 `URogue10mAbilitySystemComponent`와 `URogue10mAttributeSet`은 구현되어 있다. 플레이어는 `ARogue10mPlayerState`가 ASC와 AttributeSet을 소유하고 `ARogue10mCharacter`가 ASC ActorInfo를 초기화한다. 몬스터는 `ARogue10mBasicMonster`가 자신의 ASC와 AttributeSet을 직접 소유한다.

## 9. 입력 설계

### 9.1 PC

- 이동: WASD
- 시점 및 조준: Mouse
- 기본 공격: 입력 기반
- 스킬: Skill Action별 입력
- 상호작용: Interaction Action
- 무기 교체: Weapon Change Action
- 일시정지: Pause Action

### 9.2 모바일 확장

게임 로직은 키 값을 직접 확인하지 않고 Input Action만 처리한다.

- 가상 조이스틱 이동
- 터치 드래그 시점 조작
- 기본 공격 버튼
- 스킬 버튼
- 상호작용 버튼
- 무기 교체 UI

플랫폼별 Mapping Context와 HUD Layout을 분리하고 Ability 및 Combat 로직은 공유한다.

## 10. 지역 진입 기반 몬스터 스폰

### 10.1 기본 방식

플레이 맵을 논리적인 Encounter Region으로 나누고, 플레이어가 지역 활성 범위에 진입하면 해당 지역의 Spawn Director가 몬스터를 생성한다.

```text
플레이어가 지역 활성 범위 진입
→ 지역 상태와 재생성 쿨다운 확인
→ 지역 Spawn Budget 계산
→ Spawn Group 선택
→ 유효한 Spawn Point 또는 NavMesh 위치 선택
→ 몬스터 생성
→ 지역 단위로 생존 몬스터 추적
```

### 10.2 오픈 월드 적합성 의견

지역 진입 스폰은 현재 규모의 오픈 월드에 적합하다. 맵 전체의 몬스터를 항상 생성하는 방식보다 CPU, 메모리 및 AI 비용을 제어하기 쉽고 모바일 확장에도 유리하다.

단순한 Trigger 진입·이탈만으로 즉시 생성하고 제거하면 경계에서 몬스터가 반복 생성되는 문제가 발생한다. 다음 보완이 필요하다.

- 활성화 거리와 비활성화 거리를 다르게 두는 히스테리시스 적용
- 지역별 최대 동시 개체 수와 Spawn Budget 설정
- 플레이어 시야 안과 너무 가까운 위치에서는 생성 금지
- NavMesh 투영 및 충돌 검증 후 생성
- 지역을 떠나도 전투 중인 몬스터는 즉시 제거하지 않음
- 일정 거리와 유예 시간이 모두 충족된 몬스터만 비활성화 또는 풀 반환
- 처치 직후 지역 경계를 재진입해도 즉시 재생성되지 않도록 쿨다운 적용
- 보스와 퀘스트 몬스터는 일반 지역 스폰과 분리

### 10.3 권장 구성

```text
ARogue10mEncounterRegion
├─ Region ID
├─ Activation Bounds
├─ Spawn Point 목록
├─ Spawn Table
├─ Spawn Budget
├─ Respawn Cooldown
├─ 활성 몬스터 목록
└─ 지역 런타임 상태

URogue10mWorldSpawnSubsystem
├─ 활성 Region 등록 및 해제
├─ 전역 몬스터 예산
├─ 스폰 요청 우선순위
└─ 오브젝트 풀 연동
```

작은 초기 맵에서는 Region Actor가 직접 스폰을 관리해도 된다. 지역 수와 동시 몬스터 수가 증가하면 WorldSubsystem에 전역 예산과 업데이트 스케줄링을 이전한다.

### 10.4 저장 정책

일반 몬스터의 개별 위치와 HP는 저장하지 않는다. 런 재개 시 지역 상태와 시드 또는 재생성 쿨다운을 이용해 다시 구성한다.

다음 상태만 별도 저장한다.

- 보스 생존 여부
- 중요 퀘스트 몬스터 상태
- 이미 완료한 일회성 Encounter
- 중요한 상자와 상호작용 오브젝트

## 11. 보스 시스템

보스는 고정된 월드 위치에 존재하며 런 시작 직후부터 접근할 수 있다. 보스 접근을 막는 강제 퀘스트 조건은 두지 않는다.

권장 구성:

- `ARogue10mBossCharacter`
- `URogue10mBossDataAsset`
- Boss Area 또는 Boss Encounter Actor
- 보스 페이즈 상태
- 보스 사망 Gameplay Event

보스는 직접 승리를 처리하지 않는다. 사망 이벤트를 현재 구현된 `ARogue10mGameMode`에 전달하고 GameMode가 제한 시간과 `ARogue10mGameState` 상태를 확인한 후 승리를 확정한다.

## 12. 승리와 패배

### 12.1 승리

- 현재 상태가 `Playing`인지 확인
- 제한 시간이 남아 있는지 확인
- 보스 사망을 검증
- 상태를 `Victory`로 변경
- 획득한 메타 재화와 보상 정산
- 런 저장 제거
- 결과 UI 표시 후 마을 복귀

### 12.2 패배

다음 조건에서 즉시 패배한다.

- 플레이어 사망
- 제한 시간 만료

패배 처리:

- 상태를 `Defeat`로 변경
- 입력과 전투 처리 중단
- 현재 런 데이터 무효화 및 삭제
- 허용된 메타 재화 정산
- 결과 UI 표시
- 시작 마을 복귀

## 13. 저장 시스템

### 13.1 저장 데이터 분리

```text
URogue10mMetaSaveGame
├─ 메타 재화
├─ 영구 강화 단계
├─ 획득 또는 해금한 스킬
├─ 완료 퀘스트
├─ 설정 및 접근성 옵션
└─ 마을 진행 상태

URogue10mRunSaveGame
├─ 현재 플레이 맵
├─ 캐릭터 위치
├─ 남은 제한 시간
├─ 런 레벨 및 경험치
├─ 런 중 획득 스킬
├─ 런 인벤토리 및 장비
├─ 중요 월드 상태
└─ 보스 생존 여부
```

### 13.2 런 재개와 런 손실

- 정상 종료 또는 중단: 유효한 Run Save가 있으면 마지막 저장 상태에서 재개할 수 있음
- 플레이어 사망: Run Save 삭제 후 마을 복귀
- 제한 시간 만료: Run Save 삭제 후 마을 복귀
- 승리: 보상 정산 후 Run Save 삭제 및 마을 복귀

이를 통해 전투 중 게임을 종료한 경우 재개할 수 있지만, 게임 규칙에 따른 사망과 패배는 되돌릴 수 없다.

### 13.3 저장 안정성

- 임시 파일에 먼저 기록한 뒤 저장 완료 시 본 파일로 교체
- 저장 버전 번호 포함
- 저장 중복 요청 직렬화
- 객체 직접 참조 대신 ID 저장
- 런 종료 판정 직후 Run Save 무효화 플래그 기록

## 14. 세션 성장과 메타 성장

### 14.1 세션 성장

런 종료 시 초기화되는 정보:

- 런 레벨과 경험치
- 런 중 선택한 일반 스킬
- 런 전용 강화
- 런 전용 아이템
- 임시 능력치 보정

### 14.2 메타 성장

승리, 패배 또는 특정 목표를 통해 메타 재화를 일부 획득할 수 있다. 메타 성장은 다음 런을 보조하지만 로그라이크의 난이도를 무력화하지 않는 소규모 강화로 제한한다.

강화 후보:

- 시작 체력 소폭 증가
- 시작 공격력 소폭 증가
- 시작 재화 증가
- 회복 아이템 보유량 증가
- 신규 무기 또는 스킬 해금

영구 강화 값은 Meta Save에 저장하고, 런 시작 시 Gameplay Effect로 적용한다.

## 15. 스킬 획득

- 일반 스킬: 레벨업 성장 선택으로 획득
- 제한 스킬: 스킬북 사용 또는 퀘스트 완료로 획득·해금
- 액티브 스킬: 플레이어 입력으로 사용
- 자동 공격: 지원하지 않음

스킬북과 퀘스트로 해금한 스킬은 Meta Save에 기록한다. 실제 런에서 즉시 사용 가능한지, 이후 성장 선택 풀에 추가되는지는 스킬 데이터의 획득 정책으로 구분한다.

## 16. 일시정지

싱글 플레이에서는 월드 전체 일시정지를 제공한다.

향후 Listen Server에서는 다음 정책을 적용한다.

- 개인 메뉴는 게임을 정지하지 않음
- 전체 정지는 호스트 권한과 모든 플레이어 상태를 검증
- UI의 Pause 상태와 실제 World Pause 상태를 분리

## 17. 멀티플레이 확장 준비

1차 버전에서 네트워크 기능을 구현하지 않더라도 다음 원칙을 지킨다.

- 데미지, 아이템 획득, 스폰, 승패는 서버 권한을 전제로 설계
- GameMode 전용 정보를 UI가 직접 참조하지 않음
- 공개 상태는 GameState 또는 PlayerState에 배치
- Gameplay Ability의 Net Execution Policy를 명시
- Actor Component의 복제 필요 여부를 문서화
- 랜덤 스폰은 서버에서 결정
- 저장 데이터와 복제 데이터를 분리
- 정적 전역 Singleton에 플레이어별 상태를 보관하지 않음

## 18. 데이터 에셋

### 18.1 현재 구현

- `URogue10mCharacterDataAsset`
- `URogue10mMonsterDataAsset`
- `URogue10mItemDataAsset`
- `URogue10mAttackSkillData`
- `URogue10mWeaponSkillProfileDataAsset`
- `URogue10mDodgeSkillDataAsset`

### 18.2 계획

- 보스 전용 데이터 또는 `URogue10mMonsterDataAsset` 확장 정책
- Encounter Region 데이터
- 메타 강화 데이터
- 맵 데이터의 Data Asset 전환 여부

현재 맵 메타데이터는 Data Asset이 아니라 `URogue10mMapDataLibrary`의 `FRogue10mMapDefinition`으로 관리한다. `StartHub`는 120m × 120m, `OpenRunMap`은 1,800m × 1,800m 목표로 정의되어 있다.

각 저장 대상 데이터는 안정적인 ID를 가져야 하며 SaveGame에는 UObject 직접 참조 대신 ID 또는 Primary Asset ID를 저장한다.

## 19. 개발 우선순위

### 1단계: 핵심 런

- 마을과 플레이 맵 전환
- 입력 기반 기본 공격
- 제한 시간
- 보스 사망과 승패
- 사망 시 런 제거 및 마을 복귀

### 2단계: 성장과 저장

- 런 경험치와 스킬 선택
- 인벤토리와 장비
- Run Save와 Meta Save 분리
- 메타 재화와 소규모 영구 강화

### 3단계: 오픈 월드 콘텐츠

- Encounter Region
- 지역 진입 스폰
- 전역 Spawn Budget
- 퀘스트와 스킬북
- 중요 월드 상태 저장

### 4단계: 확장성

- 모바일 Mapping Context와 HUD
- 성능 프로파일링 및 오브젝트 풀링
- Listen Server 프로토타입
- 복제 및 Seamless Travel 검증

## 20. 미정 및 추가 확인 사항

- 보스 클래스의 최종 명칭과 `ARogue10mBasicMonster` 상속·공통화 정책
- 플레이어가 동시에 장착할 수 있는 무기 수
- 전투 중 무기 교체 허용 여부와 교체 쿨다운
- 장비, Progression, Interaction 기능의 컴포넌트 분리 시점
- 런 중 정상 종료 시 자동 저장 주기와 정확한 재개 위치
- 승리와 패배 시 보존되는 메타 재화 비율
- 일반 몬스터의 지역별 리스폰 시간
- Encounter Region과 World Spawn 관리 클래스의 최종 명칭
- SaveGame과 저장 Subsystem의 최종 명칭
- 오픈 월드 구현 방식: 단일 Persistent Level, Level Streaming 또는 World Partition
- 목표 동시 몬스터 수와 PC 최소 사양
- 전체 세션 30분~1시간과 현재 보스 제한 시간 20분의 최종 밸런스 관계
