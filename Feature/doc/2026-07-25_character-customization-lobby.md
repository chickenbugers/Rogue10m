# 캐릭터 커스터마이징·생성·접속 결과

## 결과

게임 시작 시 메이플스토리 방식의 3슬롯 캐릭터 로비가 먼저 열리도록 구현했다.

- 저장된 캐릭터가 없으면 생성 화면을 자동으로 연다.
- 저장된 캐릭터가 있으면 슬롯에서 선택한 뒤 해당 캐릭터로 접속한다.
- 캐릭터 이름과 외형은 로컬 SaveGame에 저장한다.
- 캐릭터 슬롯은 최대 3개이며 빈 슬롯 생성, 선택, 삭제, 접속을 지원한다.
- 캐릭터 로비가 표시되는 동안 월드를 일시정지하고 기존 HUD 및 게임 입력을 차단한다.
- 프리뷰 캐릭터는 마우스 좌클릭 드래그로 좌우 회전할 수 있다.

## 지원 범위

실제 에셋 경로는 요청의 `Meshs`가 아닌 `/Game/StylizedCharacter/Meshes/Character`이다.

| 종족 | 성별 | 머리 | 피부색 | 헤어 | 얼굴 장식 | 눈 | 몸 장식 |
|---|---:|---:|---:|---:|---:|---:|---:|
| 인간 | 남성 | 5 | 4 | 5 | 수염 2 | 4 | 0 |
| 인간 | 여성 | 5 | 4 | 5 | 귀걸이·피어싱 12 | 4 | 0 |
| 드워프 | 남성 | 1 | 1 | 0 | 0 | 0 | 0 |
| 드워프 | 여성 | 1 | 1 | 0 | 0 | 0 | 0 |
| 오크 | 남성 | 5 | 3 | 0 | 엄니 1 | 9 | 문신 4 |
| 오크 | 여성 | 5 | 3 | 0 | 엄니 1 | 9 | 문신 4 |

`DA_CharacterCustomizationCatalog`가 6개 종족·성별 아키타입과 선택 가능한 메시 및 머티리얼을 소유한다. UI 인덱스는 아키타입 변경 시 유효 범위로 정규화된다.

## 런타임 구성

- `URogue10mCharacterProfileSubsystem`: 캐릭터 3슬롯, 선택 캐릭터, 생성·삭제·저장을 관리한다.
- `URogue10mCharacterProfileSaveGame`: 이름과 외형 프로필을 로컬 SaveGame에 저장한다.
- `URogue10mCharacterCustomizationDataAsset`: 종족·성별별 메시와 외형 선택지를 정의하고 런타임 메시를 구성한다.
- `URogue10mCharacterLobbyWidget`: 캐릭터 선택·생성·삭제·접속과 프리뷰 조작을 담당한다.
- `ARogue10mCharacterCustomizationPreviewActor`: SceneCapture 기반 온디맨드 프리뷰를 제공한다.
- `ARogue10mCharacter`: 선택 프로필을 적용하고 PlayerState의 이름·종족 표시를 갱신한다.

현재 플레이어의 기존 Manny 메시를 애니메이션 리더로 유지하고 StylizedCharacter의 본체·헤어·얼굴 파츠를 Leader Pose로 연결한다. 1인칭 팔, 카메라, 전투 입력은 기존 구성을 유지한다. 장비창 프리뷰는 숨겨진 Manny 대신 표시 중인 커스텀 메시를 복제한다.

## 저장 경계

이번 SaveGame 범위는 캐릭터 이름, 종족, 성별, 외형 선택값, 선택 슬롯이다.

인벤토리, 장비, 경험치, 스탯, 월드 진행도 저장은 이번 작업에 포함하지 않았다. 이후 통합 캐릭터 저장 기능에서 프로필 ID를 기준으로 별도 진행 데이터와 연결한다.

## 검증

- UE 5.8 Editor 타깃 빌드 성공
- 에셋 생성 스크립트로 카탈로그와 로비 Widget Blueprint 생성 성공
- 에셋 검증 스크립트 통과: 6개 아키타입, 종족·성별 중복 없음, 필수 메시·배열·위젯 바인딩·카탈로그 CDO 연결 확인
- `Scripts/CheckGeneratedChanges.ps1` 통과
- `git diff --check` 통과

## 수동 PIE 확인 항목

Hot Reload된 에디터는 종료하고 재시작한 뒤 확인한다.

1. 최초 실행 시 빈 3슬롯과 캐릭터 생성 화면이 표시되는지 확인한다.
2. 이름 유효성, 중복 이름, 세 슬롯 제한이 정상 동작하는지 확인한다.
3. 인간·드워프·오크 남녀 6개 조합의 프리뷰와 선택 옵션을 확인한다.
4. 프리뷰 좌클릭 드래그 회전과 랜덤 외형을 확인한다.
5. 캐릭터 생성 후 접속 시 HUD, 입력, 외형, 이름, 종족이 적용되는지 확인한다.
6. 게임 재실행 후 프로필과 선택 슬롯이 유지되는지 확인한다.
7. 삭제 버튼을 두 번 눌러야 삭제되는지 확인한다.
8. 이동·공격 애니메이션과 장비창 프리뷰에서 커스텀 메시가 정상 표시되는지 확인한다.

## 후속 개선

StylizedCharacter 6개 전신 메시가 서로 다른 Skeleton을 사용하고 전용 AnimBP가 제공되지 않는다. 현재 Leader Pose 구성은 기존 게임플레이를 보존하기 위한 1차 통합이며, 제품 수준의 종족별 변형과 애니메이션 품질을 위해 각 Skeleton용 IK Retargeter와 AnimBP를 제작하고 동작별 육안 검증을 진행해야 한다.

## 2026-07-26 캐릭터 선택 후 접속 실패 보완

### 원인

슬롯 클릭과 프로필 선택은 정상 동작했지만 `BP_FirstPersonCharacter` CDO의 `CustomizationCatalog`가 비어 있어 `ApplyCharacterProfile()`이 실패하고 접속을 중단했다. PIE 로그에는 `선택 캐릭터 외형을 적용하지 못해 게임 접속을 중단했습니다.`가 반복 기록됐으며, 로비 루트가 포커스 불가라 UIOnly 입력 모드 오류도 발생했다.

### 수정

- Player Character Blueprint 기본 카탈로그 참조 저장
- 에셋 생성 스크립트에 Player Character CDO 설정 단계 추가
- C++ 외형 적용 경로에 표준 카탈로그 fallback 추가
- 캐릭터 로비 Widget 포커스 활성화
- Validator에 Player Character 메시 컴포넌트와 카탈로그 CDO 검사 추가

수정 후 깨끗한 Unreal 프로세스에서 6개 아키타입, 로비 필수 바인딩, Player Character CDO 검증을 통과했으며 Rogue10mEditor Win64 Development 빌드에 성공했다.

## 2026-07-26 상속 외형 구조 전환

초기 구현의 “Manny Mesh를 애니메이션 Leader로 두고 서로 다른 StylizedCharacter Skeleton에 Leader Pose를 연결”하는 방식은 폐기했다. 서로 다른 Skeleton 사이의 Leader Pose가 게임 입장 후 체형 왜곡을 만들었기 때문이다.

현재 런타임은 `ARogue10mStylizedCharacter`를 공통 부모로 사용한다. 숨김 Manny는 애니메이션 포즈 소스만 담당하고, Human·Dwarf·Orc 남녀 자식 Character의 실제 전신 Mesh는 종족별 `Retarget Pose From Mesh` AnimBP로 포즈를 변환한다. Leader Pose는 동일 Skeleton의 Hair·Facial 파츠에만 사용한다.

상세 구현과 검증 결과는 `Feature/doc/2026-07-26_inherited-character-appearance.md`를 참조한다.