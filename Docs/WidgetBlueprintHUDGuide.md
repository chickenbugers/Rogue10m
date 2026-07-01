# Widget Blueprint HUD 연동 가이드

## 목표

메인 HUD의 배치, 크기, 색상, 애니메이션, 반응형 처리는 Widget Blueprint에서 수정한다.

C++은 다음 역할만 담당한다.

- 플레이어/몬스터/런 상태 데이터 제공
- 인벤토리, 장비, 스킬, 설정 패널 표시 상태 제공
- 로그와 획득 알림 데이터 제공
- Canvas 기반 임시 HUD fallback 제공

## 현재 구조

- C++ HUD 클래스: `ARogue10mHUD`
- Widget Blueprint 베이스 클래스: `URogue10mRunHUD`
- 메인 HUD Widget C++ 부모 클래스: `URogue10mMainHUDWidget`
- 하위 HUD Widget C++ 부모 클래스 폴더: `Source/Rogue10m/UI/Widgets`
- Widget 생성 위치: `ARogue10mPlayerController::InitializeRunHUD`
- Widget 지정 위치: `BP_FirstPersonPlayerController` 또는 `ARogue10mPlayerController` 기반 Blueprint의 `RunHUDClass`
- 기존 Canvas HUD 토글: `ARogue10mHUD::SetDrawCanvasPrototypeHUD`

## 현재 Content/Widget 에셋 연결 기준

- `Content/Widget/UW_Rogue10mMainWidget.uasset`
  - 메인 HUD 배치용 Widget Blueprint로 사용한다.
  - 부모 클래스는 `Rogue10mMainHUDWidget` 또는 최소 `Rogue10mRunHUD` 계열이어야 한다.
  - `RunHUDClass`가 비어 있으면 `ARogue10mPlayerController`가 `/Game/Widget/UW_Rogue10mMainWidget.UW_Rogue10mMainWidget_C`를 기본값으로 로드한다.
  - 위젯이 정상 생성되면 Canvas 기반 프로토타입 HUD는 자동으로 꺼져서 Widget Blueprint 배치와 겹치지 않는다.

- `Content/Widget/BP_Rogue10mHUD.uasset`
  - HUD 관리자 Blueprint로 쓰려면 부모 클래스가 `Rogue10mHUD` 계열이어야 한다.
  - 일반 Actor Blueprint라면 GameMode의 HUD Class로는 사용할 수 없으므로, 새 HUD Blueprint를 만들거나 부모 클래스를 `Rogue10mHUD`로 맞춘다.
  - HUD Blueprint에서는 `Draw Canvas Prototype HUD`를 꺼두면 Widget Blueprint 레이아웃만 확인하기 쉽다.

## C++ 부모 클래스 분류

Widget Blueprint는 `Content/Widget` 아래에 만들고, 부모 클래스는 아래 C++ 클래스를 사용한다.

| Widget Blueprint 예시 | 부모 C++ 클래스 | 용도 |
| --- | --- | --- |
| `WBP_MainHUD` | `Rogue10mMainHUDWidget` | 메인 HUD 루트 |
| `WBP_VitalBar` | `Rogue10mVitalBarWidget` | 체력/스테미나/마나 바 |
| `WBP_Progression` | `Rogue10mProgressionWidget` | 레벨/경험치 |
| `WBP_Identity` | `Rogue10mIdentityWidget` | 아이덴티티와 특수 자원 |
| `WBP_MonsterInfo` | `Rogue10mMonsterInfoWidget` | 몬스터 이름/속성/체력 |
| `WBP_QuickSlot` | `Rogue10mQuickSlotWidget` | 스킬/아이템 슬롯 |
| `WBP_LogLine` | `Rogue10mLogLineWidget` | 시스템 로그 한 줄 |
| `WBP_ItemAcquisitionLine` | `Rogue10mLogLineWidget` | 아이템 획득 알림 한 줄 |
| `WBP_MinimapMarker` | `Rogue10mMinimapMarkerWidget` | 미니맵 마커 |
| `WBP_ShortcutHint` | `Rogue10mShortcutHintWidget` | 장비/인벤/스킬/설정 단축키 |
| 커스텀 하위 패널 | `Rogue10mHudPartWidget` | 공통 하위 위젯 베이스 |

## 생성된 파트 Widget Blueprint

아래 에셋은 `Content/Widget/Parts`에 생성되어 있다. 내부 디자인은 비워 두었으므로 Designer에서 자유롭게 제작한다.

| 에셋 | 부모 C++ 클래스 | 메인 HUD 배치 위치 |
| --- | --- | --- |
| `WBP_VitalBar` | `Rogue10mVitalBarWidget` | `HealthBarWidget`, `StaminaBarWidget`, `ManaBarWidget` |
| `WBP_QuickSlot` | `Rogue10mQuickSlotWidget` | 스킬 슬롯, 아이템 슬롯 반복 요소 |
| `WBP_MonsterInfo` | `Rogue10mMonsterInfoWidget` | `MonsterInfoWidget` |
| `WBP_Progression` | `Rogue10mProgressionWidget` | `ProgressionWidget` |
| `WBP_Identity` | `Rogue10mIdentityWidget` | `IdentityWidget` |
| `WBP_ShortcutHint` | `Rogue10mShortcutHintWidget` | `EquipmentShortcutWidget`, `ItemWindowShortcutWidget`, `SkillTreeShortcutWidget`, `SettingsShortcutWidget` |
| `WBP_LogLine` | `Rogue10mLogLineWidget` | 시스템 로그 한 줄 |
| `WBP_ItemAcquisitionLine` | `Rogue10mLogLineWidget` | 아이템 획득 알림 한 줄 |
| `WBP_MinimapMarker` | `Rogue10mMinimapMarkerWidget` | 미니맵 마커 반복 요소 |

`UW_Rogue10mMainWidget`에는 위 파트 위젯을 배치한 뒤, 자동 바인딩이 필요한 위젯은 이름을 정확히 맞춘다. 예를 들어 `WBP_VitalBar`를 체력바로 사용할 경우 배치된 인스턴스 이름을 `HealthBarWidget`으로 변경한다.

### 파트 위젯 기본 Canvas 설정

UE 5.7 명령줄 Python 환경에서는 Widget Blueprint의 `WidgetTree.RootWidget`이 보호 속성이라 Designer 트리를 자동 저장하지 않는다. 대신 각 파트의 C++ 부모 클래스가 비어 있는 Widget Blueprint에 대해 `NativePreConstruct`에서 임시 Canvas 미리보기 구조를 만든다.

- 루트가 비어 있으면 `CanvasPanel_Root`가 런타임/프리뷰용으로 생성된다.
- 내부에 `Border_DesignScaffold`, `Text_DesignTitle`, `Text_DesignDescription`이 생성되어 파트 용도와 권장 크기를 보여준다.
- Designer에서 직접 루트 위젯이나 자식 위젯을 배치하면 C++ 임시 구조는 더 이상 생성되지 않는다.
- 각 파트의 `bCreatePrototypeDesignScaffold`를 끄면 미리보기 구조를 비활성화할 수 있다.

권장 작업 순서:

1. `WBP_*` 파트 위젯을 연다.
2. Designer에서 원하는 루트 패널을 직접 배치한다. 기본은 `Canvas Panel`을 추천한다.
3. 내부 디자인을 만들고 Compile/Save한다.
4. `UW_Rogue10mMainWidget`에 배치한 뒤 인스턴스 이름을 자동 바인딩 규칙에 맞춘다.

## Widget Blueprint 생성 절차

1. 콘텐츠 브라우저에서 `Content/Widget` 폴더를 사용한다.
2. `User Interface > Widget Blueprint`를 생성한다.
3. 이름은 예시로 `WBP_MainHUD`를 사용한다.
4. 부모 클래스를 `Rogue10mMainHUDWidget`으로 지정한다.
5. `BP_FirstPersonPlayerController`를 열고 `RunHUDClass`에 `WBP_MainHUD`를 지정한다.
6. PIE 실행 후 Widget이 보이는지 확인한다.
7. Canvas 임시 HUD가 겹치면 `ARogue10mHUD` 기본값 또는 HUD Blueprint에서 `Draw Canvas Prototype HUD`를 끈다.

## 자동 바인딩 이름 규칙

`WBP_MainHUD` 안에 아래 이름으로 하위 위젯을 배치하면 C++에서 자동으로 데이터를 넣어준다.

| WBP_MainHUD 내부 위젯 이름 | 권장 부모 클래스 | 자동 입력 데이터 |
| --- | --- | --- |
| `HealthBarWidget` | `Rogue10mVitalBarWidget` | 체력 |
| `StaminaBarWidget` | `Rogue10mVitalBarWidget` | 스테미나 |
| `ManaBarWidget` | `Rogue10mVitalBarWidget` | 마나 |
| `ProgressionWidget` | `Rogue10mProgressionWidget` | 레벨/경험치 |
| `IdentityWidget` | `Rogue10mIdentityWidget` | 아이덴티티 |
| `MonsterInfoWidget` | `Rogue10mMonsterInfoWidget` | 조준 중인 몬스터 |
| `EquipmentShortcutWidget` | `Rogue10mShortcutHintWidget` | 장비 단축키 |
| `ItemWindowShortcutWidget` | `Rogue10mShortcutHintWidget` | 인벤 단축키 |
| `SkillTreeShortcutWidget` | `Rogue10mShortcutHintWidget` | 스킬 단축키 |
| `SettingsShortcutWidget` | `Rogue10mShortcutHintWidget` | 설정 단축키 |

이름을 다르게 쓰고 싶으면 자동 바인딩을 사용하지 않고 `BP_OnHudDataUpdated`에서 Getter를 직접 호출하면 된다.

## 권장 Widget 트리

아래 구조를 기본으로 시작하면 현재 C++ Getter와 잘 맞는다.

```text
WBP_MainHUD
└─ CanvasPanel Root
   ├─ MonsterInfoWidget
   ├─ Panel_Minimap
   ├─ Panel_ItemAcquisition
   ├─ Panel_SystemLog
   ├─ Panel_BottomHUD
   │  ├─ HealthBarWidget
   │  ├─ Box_SkillSlots
   │  ├─ IdentityWidget
   │  ├─ StaminaBarWidget
   │  └─ Box_ItemSlots
   └─ Box_ShortcutHints
```

권장 Anchor:

- `Overlay_MonsterInfo`: 상단 중앙
- `Panel_Minimap`: 우측 상단
- `Panel_ItemAcquisition`: 중앙 우측
- `Panel_SystemLog`: 좌측 하단
- `Panel_BottomHUD`: 하단 중앙
- `Box_ShortcutHints`: 우측 하단

## 업데이트 이벤트 구성

`URogue10mRunHUD`는 기본적으로 매 Tick마다 `BP_OnHudDataUpdated`를 호출한다.

Widget Blueprint의 Event Graph에서 다음 흐름을 만든다.

```text
Event BP_OnHudDataUpdated
├─ UpdateVitals
├─ UpdateProgression
├─ UpdateIdentity
├─ UpdateMonsterInfo
├─ UpdateSkillSlots
├─ UpdateItemSlots
├─ UpdateSystemLog
├─ UpdateItemAcquisition
├─ UpdateMinimap
└─ UpdateShortcutHints
```

처음에는 모든 갱신을 Tick 이벤트에서 처리해도 된다. UI가 많아지면 변경 이벤트 방식으로 분리한다.

## 주요 Getter 사용법

### 체력

사용 함수:

- `GetHealthView`

반환값:

- `Current`
- `Max`
- `Normalized`
- `bVisible`

Blueprint 연결 예:

```text
GetHealthView
├─ ProgressBar_Health.SetPercent(Normalized)
└─ Text_Health.SetText("{Current} / {Max}")
```

### 스테미나와 마나

사용 함수:

- `GetStaminaView`
- `GetManaView`

마나는 `bVisible`이 false면 위젯을 숨긴다.

```text
GetManaView
├─ Branch bVisible
│  ├─ true: ManaPanel.SetVisibility(Visible)
│  └─ false: ManaPanel.SetVisibility(Collapsed)
└─ ProgressBar_Mana.SetPercent(Normalized)
```

### 레벨과 경험치

사용 함수:

- `GetProgressionView`

표시 권장:

- 레벨 텍스트: `Lv.{Level}`
- 경험치 텍스트: `{CurrentExperience} / {ExperienceToNextLevel}`
- 경험치 바: `ExperienceNormalized`

### 아이덴티티

사용 함수:

- `GetIdentityView`

표시 권장:

- 제목: `아이덴티티`
- 보조 텍스트: `Label`
- 게이지: `Normalized`

현재 아이덴티티는 기획 중이므로 무기 숙련도와 특수 자원 자리로 사용한다.

### 몬스터 정보

사용 함수:

- `GetLookedAtMonsterInfoView`

표시 규칙:

- `bHasMonster`가 false면 몬스터 정보 패널을 숨긴다.
- true면 이름, 레벨, 속성, 상태, 체력 수치, 체력 퍼센트를 표시한다.

Blueprint 연결 예:

```text
GetLookedAtMonsterInfoView
├─ Branch bHasMonster
│  ├─ false: MonsterInfo.SetVisibility(Collapsed)
│  └─ true: MonsterInfo.SetVisibility(Visible)
├─ Text_Name.SetText(Name)
├─ Text_Level.SetText("Lv.{Level}")
├─ Text_Attribute.SetText(AttributeText)
├─ Text_State.SetText(StateText)
└─ ProgressBar_MonsterHealth.SetPercent(Health.Normalized)
```

### 스킬 슬롯

사용 함수:

- `GetSkillQuickSlotViews`

반환 배열의 각 원소:

- `SlotNumber`
- `DisplayName`
- `IconColor`
- `CooldownRemaining`
- `CooldownDuration`
- `CooldownNormalized`
- `bUnlocked`

권장 구현:

- `Box_SkillSlots`를 `HorizontalBox`로 만든다.
- `WBP_QuickSlot` 하위 위젯을 별도로 만든다.
- `BP_OnHudDataUpdated`에서 슬롯 수가 바뀌면 자식 위젯을 재생성한다.
- 슬롯 수가 같으면 기존 자식 위젯의 표시 데이터만 갱신한다.

쿨타임 표시:

- `CooldownNormalized > 0`이면 어두운 Overlay 표시
- 텍스트는 `CooldownRemaining`을 소수점 1자리로 표시

잠금 표시:

- `bUnlocked == false`이면 아이콘을 어둡게 하고 `잠김` 텍스트를 표시한다.

### 아이템 슬롯

사용 함수:

- `GetItemQuickSlotViews`

스킬 슬롯과 같은 방식으로 처리하되, `Box_ItemSlots`에 배치한다.

### 시스템 로그

사용 함수:

- `GetSystemLogEntries`

권장 Widget:

- `ScrollBox_SystemLog`
- 로그 한 줄용 `WBP_LogLine`

표시 규칙:

- 최신 로그가 위에 온다.
- 항목 수가 많으면 ScrollBox가 스크롤을 담당한다.
- `Color`를 텍스트 색상에 반영한다.

### 아이템 획득 알림

사용 함수:

- `GetItemAcquisitionEntries`

표시 규칙:

- 최신 알림을 위에 표시한다.
- `RemainingSeconds`가 0에 가까울수록 투명도를 낮춘다.
- Widget Animation을 사용하면 로스트아크처럼 자연스럽게 사라지는 연출을 만들 수 있다.

권장 투명도 계산:

```text
Opacity = Clamp(RemainingSeconds / 3.0, 0.0, 1.0)
```

### 미니맵

사용 함수:

- `GetPrototypeMinimapMarkers`

현재는 임시 마커다.

각 마커:

- `NormalizedPosition`: 0~1 범위의 미니맵 내부 좌표
- `Color`: 마커 색상
- `Label`: 마커 설명

Widget에서 배치할 때:

```text
X = MinimapWidth * NormalizedPosition.X
Y = MinimapHeight * NormalizedPosition.Y
```

추후 실제 미니맵은 월드 좌표를 정규화해서 이 구조로 공급한다.

### 패널 상태와 단축키

사용 함수:

- `GetPanelStateView`
- `GetEquipmentShortcutText`
- `GetItemWindowShortcutText`
- `GetSkillTreeShortcutText`
- `GetSettingsShortcutText`

사용 예:

- 장비 버튼이 열려 있으면 강조 색상
- 아이템 창이 열려 있으면 강조 색상
- 스킬트리/설정도 동일하게 처리

## 하위 위젯 분리 권장

처음부터 모든 UI를 `WBP_MainHUD`에 몰아넣으면 관리가 어려워진다.

권장 분리:

- `WBP_VitalBar`: 부모 `Rogue10mVitalBarWidget`
- `WBP_QuickSlot`: 부모 `Rogue10mQuickSlotWidget`
- `WBP_SystemLogLine`: 부모 `Rogue10mLogLineWidget`
- `WBP_ItemAcquisitionLine`: 부모 `Rogue10mLogLineWidget`
- `WBP_MonsterInfo`: 부모 `Rogue10mMonsterInfoWidget`
- `WBP_MinimapMarker`: 부모 `Rogue10mMinimapMarkerWidget`
- `WBP_ShortcutHint`: 부모 `Rogue10mShortcutHintWidget`

각 하위 위젯은 `SetData` 함수 하나를 만들어 외부에서 View 구조체를 넘기는 방식으로 관리한다.

현재 C++ 부모 클래스들은 이미 `SetVitalView`, `SetQuickSlotView`, `SetLogEntryView` 같은 데이터 입력 함수를 제공한다. Blueprint에서는 각 `BP_On...Changed` 이벤트에서 표시만 갱신하면 된다.

## Canvas HUD와 병행 사용

초기 개발 중에는 Canvas HUD와 Widget HUD를 같이 켤 수 있다.

- 빠른 C++ 디버그 확인: Canvas HUD 켜기
- 실제 UI 배치 검토: Canvas HUD 끄기, Widget Blueprint 사용

Canvas HUD를 끄는 방법:

1. HUD Blueprint 또는 C++ 기본값에서 `Draw Canvas Prototype HUD`를 false로 설정한다.
2. 또는 Blueprint에서 `Get HUD > Cast to Rogue10mHUD > Set Draw Canvas Prototype HUD(false)`를 호출한다.

## 성능 주의사항

- 매 Tick마다 모든 위젯을 삭제/재생성하지 않는다.
- 슬롯/로그 개수가 바뀔 때만 자식 위젯을 재생성한다.
- 숫자 텍스트는 값이 바뀐 경우에만 `SetText`한다.
- ProgressBar Percent 갱신은 Tick에서 해도 된다.
- 아이템 획득 알림은 Widget Animation을 쓰면 Tick 계산을 줄일 수 있다.

## 문제 해결

### Widget이 보이지 않음

- PlayerController Blueprint의 `RunHUDClass`가 비어 있는지 확인한다.
- 부모 클래스가 `Rogue10mRunHUD`인지 확인한다.
- PlayerController가 실제 GameMode에서 사용 중인지 확인한다.

### Canvas HUD와 겹침

- `Draw Canvas Prototype HUD`를 false로 설정한다.
- 아직 Widget이 완성되지 않았다면 Canvas HUD를 켜둔 상태로 비교해도 된다.

### 체력/스테미나가 0으로 보임

- 플레이어 Pawn이 `Rogue10mCharacter`인지 확인한다.
- 캐릭터에 `Rogue10mVitalsComponent`가 붙어 있는지 확인한다.

### 몬스터 정보가 보이지 않음

- 화면 중앙 조준선이 몬스터를 바라보고 있는지 확인한다.
- 몬스터가 `Rogue10mBasicMonster` 계열인지 확인한다.
- UI는 `bHasMonster`가 false면 숨겨야 한다.

### 로그가 갱신되지 않음

- `BP_OnHudDataUpdated`에서 `GetSystemLogEntries`를 호출하고 있는지 확인한다.
- C++ 쪽에서 `AddCombatLogMessage`가 호출되는지 확인한다.

## 다음 작업 후보

- `WBP_MainHUD` 샘플 에셋 제작
- `WBP_QuickSlot`, `WBP_VitalBar`, `WBP_MonsterInfo` 샘플 제작
- 로그 스크롤 입력을 Widget `ScrollBox` 중심으로 이전
- 아이템 획득 알림을 Widget Animation으로 페이드 처리
- 실제 미니맵 RenderTarget 또는 맵 데이터 기반 2D 위젯 연결
- 장비/아이템/스킬/설정 패널을 각각 독립 Widget Blueprint로 분리
