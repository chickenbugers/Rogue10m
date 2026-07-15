# Rogue10m 현재 프로그램 구조

## 전체 구조

~~~mermaid
flowchart TB
    Input["Enhanced Input / PlayerController"] --> Character["ARogue10mCharacter"]
    Character --> Components["Combat · Inventory · Feedback · VitalRegeneration"]
    Character --> PlayerState["ARogue10mPlayerState"]
    PlayerState --> GAS["AbilitySystemComponent + AttributeSet"]
    GAS --> Vitals["Health · Stamina · Mana · Experience"]
    Monster["ARogue10mBasicMonster"] --> MonsterData["URogue10mMonsterDataAsset"]
    Monster --> MonsterASC["Monster ASC + AttributeSet"]
    Monster --> MonsterRegen["VitalRegenerationComponent"]
    MonsterData --> MonsterVisual["SkeletalMesh · AnimBP · Transform"]
    MonsterData --> MonsterBalance["Level · Vitals · Regen · AI · XP Reward"]
    MonsterData --> AttackData["AttackSkill Data Asset"]
    Inventory["Rogue10mInventoryComponent"] --> ItemData["Item Data Asset"]
    Inventory --> Grid["10x10 Grid + Bag Containers"]
    Inventory --> PickupFeed["Item Acquisition Feed"]
    Controller["ARogue10mPlayerController"] --> HUD["URogue10mRunHUD / MainHUDWidget"]
    HUD --> MonsterUI["MonsterInfo: LV N : Name + HP"]
    HUD --> VitalsUI["Health · Stamina · Identity"]
    HUD --> ItemFeedUI["Image | Quantity"]
    HUD --> SlotsUI["Skill · Consumable Slots"]
~~~

## 핵심 책임

- ARogue10mCharacter: 플레이어 이동과 플레이어 전용 컴포넌트의 조립 지점입니다.
- ARogue10mPlayerState: 레벨, 경험치와 GAS 기반 지속 상태를 담당합니다.
- URogue10mAttributeSet: 플레이어와 몬스터가 공유하는 체력, 스테미나, 마나 및 최대값 저장소입니다.
- URogue10mVitalRegenerationComponent: Tick 없이 1초 타이머로 AttributeSet을 회복시킵니다. 기본 회복량은 각 자원 초당 0.1입니다.
- ARogue10mBasicMonster: Monster Data Asset을 읽어 외형, 능력치, AI 범위, 공격, 보상을 초기화합니다.
- URogue10mMonsterDataAsset: 몬스터별 정적 설정을 한 에셋에 모읍니다.
- URogue10mRunHUD: 런타임 상태를 UMG가 소비할 View 구조로 변환합니다.
- URogue10mMainHUDWidget: 빠른 상태와 느린 목록 갱신을 나누고 각 하위 위젯에 전달합니다.
- URogue10mInventoryComponent: NxM 아이템 배치와 가방 컨테이너, 아이템 획득 성공 이벤트를 담당합니다.

## 몬스터 생성 흐름

~~~mermaid
sequenceDiagram
    participant World
    participant Monster
    participant Data as MonsterDataAsset
    participant Attr as AttributeSet
    participant Regen as RegenerationComponent
    World->>Monster: BeginPlay
    Monster->>Data: 외형/레벨/능력치/AI/보상 조회
    Monster->>Attr: 최대값과 현재값 초기화
    Monster->>Regen: 초당 회복량 설정
    Regen->>Attr: 1초마다 서버에서 Clamp 회복
~~~

## 기능 구현 흐름

### 몬스터 정보 UI

RunHUD가 조준 중인 몬스터를 FRogue10mHudMonsterInfoView로 만들고, URogue10mMonsterInfoWidget이 UI_MonsterNameText, UI_MonsterHealthBar, UI_MonsterHealthText를 직접 갱신합니다. 속성 텍스트는 현재 바인딩하지 않습니다.

### 아이템 획득 피드

TryAddGridItem 성공 → PlayerController::AddItemAcquisitionItem → RunHUD View 변환 → WBP_ItemAcquisitionLine의 UI_ItemIconImage | UI_ItemCountText 순서입니다. 기존 가이드/목업/이름 텍스트는 런타임 의존성이 없습니다.

## 에디터 설정

1. Content Browser에서 Rogue10mMonsterDataAsset 기반 Data Asset을 생성합니다.
2. 이름, 레벨, 보상 경험치, 최대 자원, 초당 회복량, Mesh, Anim Blueprint, AI 범위와 공격 스킬을 지정합니다.
3. 몬스터 Blueprint의 Monster Data에 생성한 에셋을 지정합니다.
4. WBP_MonsterInfo는 UI_MonsterNameText, UI_MonsterHealthBar, UI_MonsterHealthText 이름을 유지합니다.
5. WBP_ItemAcquisitionLine은 UI_ItemIconImage, UI_ItemCountText 이름을 유지합니다.