# 캐릭터 데이터 소유 구조

## 목적

캐릭터 클래스에 기능을 누적하지 않고 지속 상태, 전투, 아이템, UI 상태를 각 책임 클래스에 분리한다.

## PlayerState

ARogue10mPlayerState는 플레이어에게 지속되는 상태를 소유한다.

- AbilitySystemComponent
- Rogue10mAttributeSet
- 캐릭터 표시 이름과 직업
- 현재 장착 무기 타입
- 캐릭터 사망 상태
- 아이덴티티 타입과 마나 활성 여부

레벨, 경험치, 체력, 스테미나, 마나, 아이덴티티 수치는 AttributeSet만 원본으로 사용한다.

## AttributeSet

URogue10mAttributeSet은 모든 전투/성장 수치의 단일 원본이다.

- Health / MaxHealth
- Stamina / MaxStamina
- Mana / MaxMana
- Identity / MaxIdentity
- PlayerLevel
- Experience
- ExperienceToNextLevel

플레이어는 PlayerState가 AttributeSet을 소유한다. 몬스터는 자신의 ASC와 AttributeSet을 직접 소유한다.

## CombatComponent

URogue10mCombatComponent는 공격 실행 전체를 담당한다.

- 공격 Data Asset 참조와 입력별 선택
- 차징 시간 측정
- 스킬 해금 상태
- GAS Ability 부여와 활성화
- 자원 비용 검사와 소비
- 카메라 기준 공격 판정과 피해 적용
- Montage 재생
- 콤보 입력 시간과 공유 쿨타임
- 공격 디버그와 전투 로그

Character는 입력을 CombatComponent에 전달하는 얇은 어댑터만 유지한다.

## Character

ARogue10mCharacter의 역할:

- 이동, 점프, 시점 입력
- 공격 입력 전달
- PlayerState의 ASC ActorInfo 초기화
- 피격과 사망 생명주기
- InventoryComponent와 CombatComponent 소유
- 카메라와 1인칭 Mesh 제공

## PlayerController

ARogue10mPlayerController의 역할:

- Enhanced Input Mapping Context
- Windows/Android 입력 모드
- UMG 메인 HUD 생성
- 인벤토리/아이템/스킬트리/설정 패널 상태
- 마우스 커서와 GameAndUI 입력 모드
- 감도와 FPS 제한
- 전투 로그, 획득 알림, 피해 피드백
- 조준 중인 몬스터 탐색

## UI

URogue10mRunHUD와 URogue10mMainHUDWidget은 PlayerState, Character, CombatComponent, PlayerController 데이터를 View 구조체로 변환한다. 화면 배치와 디자인은 Widget Blueprint가 담당한다.

## 다음 분리 후보

- 무기별 숙련도와 스킬 해금 상태를 전용 ProgressionComponent로 이전
- 공격 적중 시점을 Animation Montage Notify 또는 Gameplay Event로 전환
- 인벤토리 아이템 정의를 Item Data Asset으로 이전