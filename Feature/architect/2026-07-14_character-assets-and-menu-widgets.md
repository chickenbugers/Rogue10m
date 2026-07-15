# character assets and menu widgets - Architecture Plan

## Feature Summary

플레이어 캐릭터에 필요한 기본 맨손 전투/회피 Data Asset을 만들고 Combat Component에 프로필로 연결한다. 인벤토리, 장비창, 스킬 트리에는 빈 Blueprint 상태에서도 확인 가능한 필수 UserWidget 골격을 제공한다.

## Existing Context

- Combat Component는 무기 타입별 `URogue10mWeaponSkillProfileDataAsset` 배열을 사용한다.
- 맨손 공격 Data Asset 6종은 `/Game/DataAsset/AttackSkill/Unarmed`에 존재한다.
- PlayerController는 세 메뉴 Widget Class가 지정되면 시작 시 생성하고 단축키로 표시 상태를 전환한다.
- 메뉴 네이티브 클래스와 스킬 Drag & Drop 로직은 이미 존재하지만 실제 WBP 에셋과 기본 레이아웃이 없다.

## Technical Approach

- `URogue10mDodgeSkillDataAsset`: 맨손 기본 회피(1m, 0.16초, 쿨타임 0.45초, 스테미나 10)를 정의한다.
- `URogue10mWeaponSkillProfileDataAsset`: 기존 맨손 공격 6종과 기본 회피를 묶는다.
- Character Blueprint의 Combat Component `WeaponSkillProfiles`에 프로필을 연결한다.
- 메뉴 C++ 클래스의 `RebuildWidget()`에서 최소 UI를 생성해 Blueprint가 비어 있어도 동작하게 한다.
- Editor Python 스크립트가 Data Asset/WBP를 만들고 Character/PlayerController 기본값을 연결한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 네이티브 메뉴 UI 골격 | MenuWindowWidgets C++ | 4개 위젯 클래스가 빈 트리에서도 필수 컨트롤 생성 | Editor target build |
| 2 | 캐릭터 Data Asset 구성 | Editor Python, Content | 회피/무기 프로필 생성 및 Combat Component 연결 | Editor asset load/save |
| 3 | 메뉴 WBP 구성 | Editor Python, Content | 4개 WBP 생성 및 PlayerController 연결 | WBP compile/save |
| 4 | 문서화와 검증 | Feature/DevLog/Sprint log | 변경/검증/잔여 설정 기록 | CheckGeneratedChanges, diff review |

## Affected Files

- `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.h/.cpp`
- `Scripts/Editor/CreateCharacterAssetsAndMenuWidgets.py`
- `/Game/DataAsset/DodgeSkill/DA_Dodge_Unarmed`
- `/Game/DataAsset/SkillProfile/DA_SkillProfile_Unarmed`
- `/Game/Widget/Menu/WBP_*`
- `BP_FirstPersonCharacter`, `BP_FirstPersonPlayerController`

## C++ / Blueprint Boundary

게임 로직, 기본 컨트롤 이름, Data Asset 참조 구조는 C++/Data Asset에서 관리한다. Widget Blueprint는 색상, 브러시, 배치, 애니메이션 같은 표현을 확장한다.

## Implementation Steps

1. 메뉴 헤더에 `RebuildWidget()`과 필수 위젯 참조를 선언한다.
2. 인벤토리 그리드/용량, 장비 슬롯 컨테이너, 스킬 엔트리, 스킬 목록을 네이티브로 구성한다.
3. 멱등 Editor Python 스크립트를 추가한다.
4. Editor 빌드 후 열린 Editor에서 스크립트를 실행한다.
5. 생성된 에셋을 컴파일/저장하고 연결 상태를 확인한다.

## Build And Validation Plan

- `Scripts/BuildEditor.ps1`
- UE Python `py_compile`
- `Scripts/CheckGeneratedChanges.ps1`
- 생성 에셋 경로 및 Blueprint CDO 참조 확인

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- 실행 중 Editor의 Python Remote Execution이 꺼져 있으면 사용자가 설정을 한 번 활성화해야 한다.
- Widget Blueprint는 기능 골격이며 최종 아트/레이아웃은 Designer에서 확장해야 한다.
- Editor 스크립트 실행 전에 새 C++ DLL이 현재 Editor 프로세스에 로드되어 있어야 한다.