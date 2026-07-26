# 캐릭터 커스터마이징·생성·접속 시스템 설계

## Scope Gate

### 목표

- 게임 시작 시 캐릭터 로비를 먼저 표시한다.
- 빈 슬롯에서는 캐릭터 이름, 종족, 성별, 외형 세부 옵션을 설정해 캐릭터를 생성한다.
- 생성된 캐릭터를 선택하고 `게임 접속`을 누르면 해당 이름과 외형으로 현재 게임 월드에 진입한다.
- 캐릭터 프로필은 로컬 SaveGame에 저장해 다음 실행에도 선택할 수 있게 한다.
- 메이플스토리식 흐름을 참고해 `캐릭터 슬롯 → 생성/선택 → 접속`의 3단계 구조로 구성한다.

### 변경 범위

- C++: 커스터마이징 타입, Data Asset, SaveGame, GameInstanceSubsystem, 프리뷰 Actor, 로비 Widget, Character·PlayerState·PlayerController 연결
- 에셋: 커스터마이징 카탈로그 Data Asset, 캐릭터 로비 Widget Blueprint
- 설정: 별도 시작 맵을 추가하지 않고 기존 게임 맵 진입 직후 로비를 모달로 표시
- 문서: 아키텍처, 결과 문서, DevLog, Sprint 변경 이력

### 제외

- 인벤토리·장비·스킬·퀘스트 진행도 저장
- 온라인 계정 및 서버 캐릭터 DB
- 캐릭터 삭제 복구와 이름 중복 서버 검사
- StylizedCharacter 전용 이동·전투 Anim Blueprint 제작 및 리타기팅
- 종족별 능력치 보너스

## 참조 에셋 조사

사용자 요청의 `Content/StylizedCharacter/Meshs/Character`는 실제 프로젝트에서 아래 경로에 존재한다.

`/Game/StylizedCharacter/Meshes/Character`

### 지원 종족과 성별

| 종족 | 남성 | 여성 | 세부 파츠 |
| --- | --- | --- | --- |
| 인간 | 지원 | 지원 | 헤어 각 5종, 남성 수염 2종, 여성 귀걸이·피어싱 12종 |
| 드워프 | 지원 | 지원 | 기본 전신 외형 |
| 오크 | 지원 | 지원 | 엄니 각 1종, 눈 색상, 문신·얼굴 표식 |

### 스켈레톤 확인

- 6개 종족·성별 전신 메시는 각각 별도 Skeleton을 사용한다.
- 현재 플레이어는 Manny Skeleton과 `ABP_Unarmed`, `ABP_FirstPerson`을 사용한다.
- StylizedCharacter 폴더에는 전용 Anim Blueprint가 없다.
- 1차 구현은 현재 Manny 메시를 보이지 않는 애니메이션 리더로 유지하고, Stylized 전신·파츠 메시를 Leader Pose로 따라가게 한다.
- 1인칭 팔과 카메라는 기존 Manny 구성을 유지해 전투·이동 입력 회귀를 방지한다.
- 전용 리타기팅 Anim Blueprint 제작은 후속 애니메이션 작업으로 분리한다.

## Ultrawork Packets

| Packet | 목표 | 수정 위치 | 완료 조건 | 검증 |
| --- | --- | --- | --- | --- |
| ULW-1 | 프로필·외형 데이터 구조 | Character, Data, Core | 종족·성별·세부 옵션과 3슬롯 SaveGame API 제공 | UHT·단위 경계 검토 |
| ULW-2 | 런타임 외형 적용 | Character, UI Preview | 선택 외형이 프리뷰와 플레이어 월드 메시 모두에 적용 | Editor 빌드·에셋 로드 |
| ULW-3 | 시작 로비 흐름 | PlayerController, Widget | 게임 시작 시 로비, 생성·선택·접속, 입력 차단 | Widget Validator·PIE |
| ULW-4 | 카탈로그·Widget 에셋 | DataAsset, Widget, Editor Python | 6 Archetype과 필수 Widget 바인딩 생성 | Unreal Python Validator |
| ULW-5 | 리뷰·문서 | Feature/doc, DevLog, SprintChangeLog | 검증 결과와 남은 QA 기록 | CheckGeneratedChanges·diff |

## 데이터 구조

### `FRogue10mCharacterAppearance`

- `Race`: Human / Dwarf / Orc
- `Gender`: Male / Female
- `HeadStyleIndex`
- `SkinToneIndex`
- `HairStyleIndex`
- `HairColorIndex`
- `FacialStyleIndex`
- `EyeColorIndex`
- `BodyDetailIndex`

모든 인덱스는 카탈로그의 해당 Archetype 옵션 수에 맞게 정규화한다. 지원하지 않는 파츠는 `INDEX_NONE`을 사용한다.

### `FRogue10mCharacterProfile`

- 영구 `FGuid`
- 캐릭터 이름
- 외형 구조체
- 생성 시각

### `URogue10mCharacterProfileSubsystem`

- Save Slot: `Rogue10mCharacterProfiles`
- 최대 캐릭터 슬롯: 3
- 프로필 생성, 삭제, 선택, 조회, 저장
- 선택 프로필을 현재 Character와 PlayerState에 적용

## 접속 흐름

```text
PlayerController::BeginPlay
  -> HUD·메뉴 초기화
  -> CharacterProfileSubsystem Load
  -> Character Lobby 표시 + 게임 일시정지 + UI 입력

빈 슬롯 선택
  -> 이름/종족/성별/외형 Draft
  -> 프리뷰 Actor 즉시 갱신
  -> 생성
  -> SaveGame 저장

기존 슬롯 선택
  -> 저장된 외형 프리뷰
  -> 게임 접속
  -> 선택 프로필 저장
  -> PlayerState 이름 적용
  -> Character Stylized 외형 적용
  -> 로비 닫기 + 게임 재개 + Game 입력
```

## UI 구성

- 좌측: 3개 캐릭터 슬롯과 새 캐릭터 생성
- 중앙: 3D 캐릭터 프리뷰
- 우측: 이름, 종족, 성별, 얼굴, 피부, 헤어, 헤어 색, 얼굴 장식, 눈, 신체 디테일
- 하단: 랜덤, 생성/취소, 삭제, 게임 접속, 상태 메시지
- 프리뷰는 좌클릭 가로 드래그 회전을 지원한다.

## 안전 규칙

- 이름은 앞뒤 공백을 제거하고 2~12자로 제한한다.
- 프로필 수는 최대 3개다.
- 빈 슬롯은 접속할 수 없다.
- 선택한 프로필이 삭제되면 선택 GUID를 초기화한다.
- SaveGame 실패 시 생성·삭제 결과를 성공으로 표시하지 않는다.
- 로비가 열린 동안 이동·시점 입력과 HUD를 차단한다.
- UObject 참조는 `UPROPERTY`와 `TObjectPtr`로 추적한다.

## 검증 기준

- UHT 및 Rogue10mEditor 빌드 성공
- 6개 Archetype의 전신 메시가 모두 유효
- 종족·성별별 옵션 수와 필수 Material·Mesh 참조 유효
- 캐릭터 로비 Widget의 필수 바인딩 존재
- 프로필 최대 3개, 이름 길이, 선택/삭제 경계 검증
- 생성 → 선택 → 접속 후 PlayerState 이름 및 Character 외형 적용
- `Scripts/CheckGeneratedChanges.ps1`, Python 문법, `git diff --check` 통과
