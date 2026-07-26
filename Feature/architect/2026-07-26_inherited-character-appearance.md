# 상속 기반 캐릭터 외형 Pawn 재설계

## Scope Gate

### 문제

현재 `ARogue10mCharacter`는 Manny `GetMesh()`를 숨겨진 애니메이션 리더로 유지하고 서로 다른 Skeleton의 StylizedCharacter 전신 메시를 `Leader Pose`로 연결한다. `Leader Pose`는 같은 본 구조를 전제로 하므로 Human·Dwarf·Orc Skeleton에 Manny 포즈를 직접 복사하면 본 인덱스와 비율이 어긋나 얼굴, 팔, 몸통이 분리되거나 겹친다.

첨부 이미지의 변형은 외형 데이터 문제가 아니라 서로 다른 Skeleton 사이의 직접 Leader Pose 설계가 원인이다.

### 변경 범위

- C++ 캐릭터 상속 구조
- 캐릭터 선택 시 Pawn 클래스 결정 및 재생성
- 캐릭터 커스터마이징 Data Asset 스키마
- 종족·성별 Character Blueprint 6종
- Manny → 종족별 Skeleton IK Retargeter 6종
- 종족별 Retarget Animation Blueprint 6종
- 에셋 생성·검증 스크립트
- 장비창 프리뷰의 가시 메시 수집 경로
- Feature 문서, DevLog, Sprint 변경 로그

### 제외 범위

- 종족별 전용 모션 제작
- IK Retarget Pose의 아티스트 수준 손·발 미세 보정
- 인벤토리·스탯·월드 진행도 저장
- 멀티플레이 캐릭터 선택 RPC

## 목표 구조

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

`ARogue10mCharacter`는 이동, 전투, 인벤토리, 1인칭 카메라를 소유한다.

`ARogue10mStylizedCharacter`는 다음 외형 계층을 소유한다.

```text
Capsule
└─ AnimationSourceMesh (숨겨진 Manny + ABP_Unarmed)
   └─ Character Mesh (선택 종족의 실제 전신 메시 + Retarget AnimBP)
      ├─ FirstPersonMesh
      ├─ AppearanceHairMesh
      └─ AppearanceFacialMesh
```

- 전신 Character Mesh는 자기 Skeleton으로 애니메이션을 평가한다.
- `Retarget Pose From Mesh`가 부모 `AnimationSourceMesh`의 Manny 포즈를 IK Retargeter로 변환한다.
- Hair·Facial 파츠만 같은 종족·성별 전신 메시를 Leader Pose로 사용한다.
- 서로 다른 Skeleton 사이에는 Leader Pose를 사용하지 않는다.

## 선택 및 접속 흐름

1. 로비에서 프로필을 선택한다.
2. `CharacterProfileSubsystem`이 선택 ID를 저장한다.
3. GameMode가 Data Asset의 종족·성별 `CharacterClass`를 조회한다.
4. 현재 Pawn 클래스가 다르면 기존 Pawn의 Transform을 보존해 선택된 상속 Pawn으로 교체한다.
5. 새 Pawn이 이름, 머리, 피부, 눈, 헤어, 얼굴·몸 장식을 적용한다.
6. 성공 후 로비를 닫고 HUD와 입력을 복구한다.

GameMode의 `GetDefaultPawnClassForController`도 선택 프로필을 읽어 게임 재실행 시 마지막 캐릭터 클래스를 처음부터 생성한다.

## Data Asset 변경

각 `FRogue10mCharacterArchetype`은 다음을 추가로 소유한다.

- `CharacterClass`: 실제 플레이에 사용할 종족·성별 상속 Pawn 클래스
- `Retargeter`: Manny에서 해당 Skeleton으로 변환하는 IK Retargeter
- `RetargetAnimClass`: 해당 Skeleton용 Retarget Animation Blueprint

Preview는 전신 메시를 직접 설정한다. Runtime Pawn은 상속 클래스가 이미 소유한 전신 메시와 AnimBP를 유지하고 머티리얼·파츠만 적용한다.

## Ultrawork Packets

### Packet 1 — 상속 런타임 기반

- 목표: 다른 Skeleton 간 Leader Pose 제거
- 수정 위치: `Character`, 신규 `Rogue10mStylizedCharacter`, Customization Data Asset
- 완료 조건: 실제 전신은 `GetMesh()`이며 Hair·Facial만 동일 Skeleton Leader Pose 사용
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 신규 상속 클래스 및 ApplyAppearance API

### Packet 2 — 프로필 기반 Pawn 클래스 선택

- 목표: 선택 종족·성별에 맞는 Pawn 생성
- 수정 위치: `GameMode`, `PlayerController`
- 완료 조건: 선택 프로필 클래스와 현재 Pawn이 다르면 안전하게 교체 후 프로필 적용
- 검증 명령: Editor 빌드 및 class-resolution Validator
- 롤백 경계: GameMode 기본 Pawn 결정 및 Controller 교체 함수

### Packet 3 — Retargeter·AnimBP 6종

- 목표: Manny 애니메이션을 각 Stylized Skeleton에 안전하게 전달
- 수정 위치: `/Game/Character/Customization/Retargeting`
- 완료 조건: 6개 IKR이 올바른 source/target Rig를 가지며 6개 AnimBP가 `Retarget Pose From Mesh`를 출력
- 검증 명령: Unreal Python Validator
- 롤백 경계: 생성 에셋 폴더

### Packet 4 — 상속 Character Blueprint 6종

- 목표: 종족·성별별 전신 메시와 Retarget AnimBP를 클래스 기본값으로 고정
- 수정 위치: `/Game/Character/Customization/Characters`
- 완료 조건: 6개 Blueprint가 `BP_FirstPersonCharacter`를 상속하고 고유 메시·AnimBP를 사용
- 검증 명령: Unreal Python Validator
- 롤백 경계: 생성 Character Blueprint 폴더 및 Data Asset class 참조

### Packet 5 — 회귀 검증·문서화

- 목표: 잘못된 Leader Pose와 클래스 누락 재발 방지
- 수정 위치: Validator, Feature 문서, DevLog, SprintChangeLog
- 완료 조건: 6개 클래스·Skeleton·IKR·AnimBP·카탈로그 연결 검증, 빌드 성공
- 검증 명령: Validator, `CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 검증 및 문서 파일

## 안전 조건

- 기존 Pawn은 새 Pawn 생성과 프로필 적용 준비가 성공한 뒤 교체한다.
- 클래스나 Retarget 에셋이 누락되면 기존 Pawn과 로비를 유지한다.
- 기존 인벤토리·전투 데이터가 생기기 전 게임 시작 로비에서만 Pawn을 교체한다.
- Source Manny는 렌더하지 않지만 항상 포즈와 본을 갱신한다.
- Target Mesh는 Source Mesh의 Tick 이후 평가되도록 Tick prerequisite를 설정한다.
- `.uasset` 생성·변경은 Unreal Editor Python을 통해서만 수행한다.
