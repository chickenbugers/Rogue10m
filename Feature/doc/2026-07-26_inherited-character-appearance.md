# 상속 기반 캐릭터 외형 구조 전환 결과

## 결과

캐릭터 생성 후 게임에 입장할 때 서로 다른 Skeleton을 `Leader Pose`로 직접 연결하던 구조를 제거하고, 종족·성별 전용 Character Blueprint 상속 구조로 전환했다.

```text
ARogue10mCharacter
└─ ARogue10mStylizedCharacter
   └─ BP_FirstPersonCharacter
      ├─ BP_Rogue10m_HumanMaleCharacter
      ├─ BP_Rogue10m_HumanFemaleCharacter
      ├─ BP_Rogue10m_DwarfMaleCharacter
      ├─ BP_Rogue10m_DwarfFemaleCharacter
      ├─ BP_Rogue10m_OrcMaleCharacter
      └─ BP_Rogue10m_OrcFemaleCharacter
```

런타임 컴포넌트 구조는 다음과 같다.

```text
Capsule
└─ AnimationSourceMesh (숨김 Manny + ABP_Unarmed)
   └─ Character Mesh (종족 전신 Mesh + 종족 Retarget AnimBP)
      ├─ FirstPersonMesh
      ├─ AppearanceHairMesh
      └─ AppearanceFacialMesh
```

`Character Mesh`는 자신의 Skeleton을 유지하면서 `Retarget Pose From Mesh` 노드로 부모 Manny의 포즈를 변환한다. Hair와 Facial Mesh만 동일한 종족 전신 Mesh를 Leader Pose로 사용한다.

## 주요 구현

- `ARogue10mStylizedCharacter`를 추가해 애니메이션 소스, 전신, 머리카락, 얼굴 장식의 책임을 분리했다.
- 기본 `ARogue10mCharacter`는 이동·전투·인벤토리와 프로필 정체성만 담당하도록 정리했다.
- 캐릭터 카탈로그 Archetype에 `CharacterClass`, `Retargeter`, `RetargetAnimClass`를 추가했다.
- GameMode가 선택 프로필의 종족·성별에 대응하는 Pawn 클래스를 결정하고 안전하게 Spawn/Possess하도록 변경했다.
- 기존 Pawn은 새 Pawn 생성, 프로필 적용, Possess가 모두 성공한 뒤에만 제거한다.
- Human, Dwarf, Orc 남녀 6조합의 자식 Character Blueprint를 생성했다.
- 각 Skeleton용 프로젝트 로컬 Target IK Rig, IK Retargeter, Retarget AnimBP를 생성했다.
- 원본 Orc Male IK Rig의 시작·끝 본이 없는 `Cape` 체인은 프로젝트 로컬 복제본에서 제거해 컴파일 경고를 없앴다.
- 기존 로비·미리보기는 동일 카탈로그를 사용하며, 실제 게임 입장 시에는 선택된 상속 Pawn으로 교체된다.

## 생성 에셋

- `/Game/Character/Customization/Characters/BP_Rogue10m_*Character` 6개
- `/Game/Character/Customization/Retargeting/IK_Target_*` 6개
- `/Game/Character/Customization/Retargeting/IKR_Manny_To_*` 6개
- `/Game/Character/Customization/Retargeting/ABP_Retarget_*` 6개
- 갱신: `/Game/DataAsset/Character/Customization/DA_CharacterCustomizationCatalog`
- 재부모화: `/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter`

## 검증

- `Rogue10mEditor Win64 Development` 전체 빌드 성공
- `ValidateInheritedCharacterAssets.py` 통과
  - 공통 부모 상속
  - 6개 자식 Character의 종족·성별·Body·AnimClass
  - Target IK Rig의 유효하지 않은 체인 부재
  - Retarget AnimBP Skeleton 및 `Retarget Pose From Mesh` 연결
- `ValidateCharacterCustomizationAssets.py` 통과
  - 6 Archetype
  - 로비 필수 바인딩
  - 새 상속 컴포넌트와 AnimationSource 기본 참조
- `CheckGeneratedChanges.ps1` 통과
- Retarget AnimBP 재생성 로그: 오류 0건, Skeleton 체인 경고 0건

## PIE 수동 확인 항목

에디터를 완전히 재시작한 뒤 다음을 확인한다.

1. 인간·드워프·오크 남녀 각각 생성 후 입장했을 때 머리, 몸, 팔, 다리가 겹치거나 늘어나지 않는다.
2. 이동·점프·공격 애니메이션이 각 종족 체형에 맞게 재생된다.
3. 머리카락과 얼굴 장식이 전신 Mesh와 같은 포즈를 따른다.
4. 1인칭 팔과 카메라가 기존과 동일하게 작동한다.
5. 장비창 Preview Character가 선택 외형을 표시하고 좌클릭 회전이 유지된다.
6. 게임 재실행 후 마지막 선택 프로필에 대응하는 자식 Character가 생성된다.

## Notion 요약 후보

서로 다른 Skeleton에 Leader Pose를 직접 적용해 발생하던 캐릭터 왜곡을 구조적으로 제거했다. 공통 `ARogue10mStylizedCharacter` 아래에 인간·드워프·오크 남녀 6개 Blueprint를 상속시키고, 숨김 Manny 애니메이션 소스의 포즈를 종족별 IK Retargeter와 AnimBP로 변환한다. GameMode는 선택 프로필의 CharacterClass를 Spawn/Possess하며, 새 Pawn 준비가 끝난 뒤 기존 Pawn을 제거한다. 전체 Editor 빌드와 로비·상속·IK·AnimGraph 자동 검증을 통과했다.

## 접속 실패 후속 보완

Hot Reload 세션에서 공통 `BP_FirstPersonCharacter`가 먼저 프로필을 적용하고 선택 CharacterClass 조회가 실패하는 현상을 수정했다. 공통 부모의 BeginPlay 자동 적용을 제거하고 GameMode에 카탈로그 및 종족·성별 자식 클래스 fallback을 추가했다.

실제 PIE에서 `BP_Rogue10m_HumanMaleCharacter_C`가 Spawn/Possess되는 것을 확인했으며, `EnterSelectedCharacter()` 호출 후 로비가 닫히고 동일 자식 Pawn이 유지됐다. 해당 PIE 구간의 `LogRogue10m: Error`와 fallback 경고는 0건이다.