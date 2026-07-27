# 스폰 후 모험가·주먹 기본 공격 부여 설계

## 목표

캐릭터 선택 후 종족별 Pawn이 스폰·Possess되면 기본 직업 `모험가`, 기본 무기 `Unarmed`, 좌클릭 Primary 주먹 공격을 안정적으로 부여한다.

## 현재 원인

- 좌클릭 Press/Release 입력은 이미 `ARogue10mCharacter`에서 CombatComponent로 전달된다.
- 선택 캐릭터 Pawn은 `BeginPlay`가 먼저 실행되고 이후 Controller가 Possess한다.
- CombatComponent의 초기 로드아웃 적용은 `BeginPlay`에만 있어 PlayerState가 없는 시점에 기본 무기 설정이 유실될 수 있다.
- `DA_Character_Default`는 기존 돌주먹 작업으로 기본 무기 `Knuckle`이 저장돼 있다.
- PlayerState 기본 직업은 `미정`이며 프로필 적용 시 종족명이 직업 필드로 전달된다.

## 소유 구조

- `URogue10mCharacterDataAsset`
  - 기본 직업명
  - 기본 무기 타입
  - 무기별 Skill Profile 목록
- `ARogue10mPlayerState`
  - 스폰된 캐릭터에 실제 적용된 직업과 무기 상태
- `URogue10mCombatComponent`
  - Possess 이후 Character Data와 무기 Skill Profile 재적용
  - GAS 공격 Ability 중복 없는 부여
- `ARogue10mCharacter`
  - `PossessedBy`, `OnRep_PlayerState`에서 스폰 기능 초기화 진입점 호출
  - 선택 프로필 이름과 기본 직업을 PlayerState에 반영

## 실행 순서

```text
캐릭터 선택
-> 종족별 Pawn 스폰
-> 외형 프로필 적용
-> Controller Possess
-> Ability Actor Info 초기화
-> Spawned Character Features 초기화
   -> CharacterData.DefaultWeaponType 적용
   -> Unarmed Skill Profile 적용
   -> Primary = DA_Attack_Unarmed_Primary
   -> 기본 공격 Gameplay Ability 부여
   -> 직업 = 모험가
-> 입력 활성화
-> 좌클릭 Press/Release
-> 주먹 Primary 공격 실행
```

## Ultrawork Packet

### Packet 1: 스폰 후 C++ 초기화

- 목표: PlayerState 연결 이후 로드아웃을 다시 적용한다.
- 수정 위치: `Character`, `Components`, `Core`, `Data`
- 완료 조건: Possess 후 기본 직업·무기·Primary Skill·Ability가 설정된다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 신규 초기화 API와 호출부

### Packet 2: 기본 에셋 설정

- 목표: 기본 Character Data와 Unarmed Profile을 실제 에셋에 저장한다.
- 수정 위치: `Content/DataAsset/Character`, `Content/DataAsset/SkillProfile`, Character Blueprint
- 완료 조건: 기본 무기 Unarmed, Primary 주먹 스킬, CharacterData CDO 연결 검증
- 검증 명령: UE 5.8 Editor Python 설정·Validator
- 롤백 경계: 이번 실행에서 저장한 에셋

### Packet 3: 회귀 검증과 기록

- 목표: 선택→스폰→입력→공격 경로와 기존 빌드를 검증한다.
- 수정 위치: 기능 문서, DevLog, SprintChangeLog
- 완료 조건: Editor 빌드와 정적 Validator 성공, PIE 수동 항목 기록
- 검증 명령: `CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 이번 Sprint#3-3 문서 항목

## 안전 조건

- 초기화 API는 여러 번 호출돼도 Ability와 슬롯 바인딩이 중복되지 않아야 한다.
- 프로필에 Primary Skill이 없으면 전투 로그에 기존 잠김 메시지를 유지한다.
- 기존 StoneFist Profile은 삭제하지 않고 기본 선택만 Unarmed로 바꾼다.
- 캐릭터 이름과 외형은 기존 저장 프로필을 유지하고 직업만 기본 모험가로 부여한다.
