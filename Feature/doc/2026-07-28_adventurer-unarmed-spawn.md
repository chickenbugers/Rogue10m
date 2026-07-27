# 모험가 기본 직업과 스폰 후 맨손 공격

## 결과

캐릭터 선택 후 종족별 Pawn이 스폰·Possess되면 기본 직업 `모험가`, 기본 무기 `Unarmed`, 좌클릭 Primary 주먹 공격이 적용되도록 구현했다. 초기화는 `BeginPlay`에만 의존하지 않고 PlayerState가 연결되는 `PossessedBy`와 `OnRep_PlayerState`에서 다시 수행한다.

## 주요 변경

- `URogue10mCharacterDataAsset`에 기본 직업 `JobName`을 추가하고 기본값을 `모험가`로 설정했다.
- `ARogue10mPlayerState`의 기본 직업 표시를 `미정`에서 `모험가`로 변경했다.
- 외형 프로필 적용 시 종족명이 직업명을 덮어쓰던 경로를 제거하고 CharacterData의 직업명을 반영했다.
- `URogue10mCombatComponent::InitializeSpawnedLoadout()`을 추가해 CharacterData, 활성 무기 Profile, GAS Ability를 한 진입점에서 적용한다.
- Character의 `PossessedBy`와 `OnRep_PlayerState`에서 스폰 로드아웃을 재적용한다.
- `DA_Character_Default`를 `모험가`/`Unarmed`로 설정하고 Unarmed Profile을 등록했다.
- `BP_FirstPersonCharacter`의 CombatComponent가 기본 CharacterData와 Profile 목록을 참조하도록 저장했다.
- Unarmed Profile의 Primary 슬롯이 `DA_Attack_Unarmed_Primary`를 가리키며 처음부터 해금된 상태인지 Editor Python으로 검증했다.
- UE Unity 빌드에서 충돌하던 두 Preview Actor의 익명 namespace 상수 이름을 기능별로 구분했다.

## 실행 흐름

```text
캐릭터 선택
-> 종족·성별 CharacterClass Pawn 스폰
-> Controller Possess
-> InitializeSpawnedLoadout
   -> 직업 모험가 적용
   -> 기본 무기 Unarmed 적용
   -> Primary 맨손 공격 및 Ability 부여
-> LeftMouseButton Press/Release
-> CombatComponent Primary 입력
-> DA_Attack_Unarmed_Primary 실행
```

## 검증

- UE 5.8 `Rogue10mEditor Win64 Development` 전체 빌드 성공
- Editor Python 설정/검증 성공: `[Rogue10mAdventurer] 모험가 / Unarmed / 좌클릭 Primary 주먹 공격 설정 및 검증 완료`
- Human·Dwarf·Orc 남녀 6개 상속 Character와 6개 Retarget AnimBP Validator 통과
- 좌클릭 Press/Release → Combat Primary → Unarmed Primary Data Asset 연결 정적 확인
- 실제 PIE 마우스 클릭 자동화는 Windows 샌드박스 ACL로 실행하지 못했으므로 에디터에서 최종 체감 QA가 필요하다.

## 수동 PIE 확인

1. 에디터를 재시작하고 게임을 실행한다.
2. 캐릭터 슬롯을 선택한 뒤 `게임 접속`으로 Pawn을 스폰한다.
3. HUD 또는 캐릭터 정보에서 직업이 `모험가`인지 확인한다.
4. 좌클릭 시 주먹 공격 Montage·판정·쿨다운이 실행되는지 확인한다.
5. 인간·드워프·오크 중 최소 한 캐릭터에서 이동 직후와 연속 클릭 상황을 확인한다.

## 관련 문서

- `Feature/architect/2026-07-28_adventurer-unarmed-spawn.md`
- `Docs/CharacterDataOwnership.md`
- `Scripts/Editor/ConfigureAdventurerUnarmedSpawn.py`
