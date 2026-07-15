# 권(Stone Fist) 아이덴티티 설계

## 목표

`T_Identity_StoneFist`를 권 아이덴티티 대표 이미지로 사용하고, 권 전용 기본 공격·차징 공격·점프 공격·회피·2단 점프를 데이터 주도 방식으로 구성한다.

## Ultrawork Packets

### Packet 1 — 구조 및 자산 감사

- 수정 위치: `Source/Rogue10m/Data`, `Source/Rogue10m/Components`, `/Game/DataAsset`
- 완료 조건: 기존 Attack/Dodge/Profile Data Asset과 GAS 연결 경계를 확인한다.
- 검증: 관련 헤더·CombatComponent 참조 검색, Editor 자산 로드 검사
- 롤백 경계: 읽기 전용 감사 결과

### Packet 2 — 권 프로필 스키마와 이동 규칙

- 수정 위치: `Rogue10mSkillLoadoutDataAsset.*`, `Rogue10mCombatComponent.*`
- 완료 조건: 프로필 표시 정보, 아이덴티티 이미지, 2단 점프 수를 데이터로 지정하고 활성 프로필 적용 시 캐릭터에 반영한다.
- 검증: Editor Target 빌드
- 롤백 경계: 위 두 C++ 파일

### Packet 3 — 스킬 아이콘과 Data Asset

- 수정 위치: `Content/UI/Icons/StoneFist`, `Content/DataAsset/AttackSkill/StoneFist`, `Content/DataAsset/DodgeSkill`, `Content/DataAsset/SkillProfile`
- 완료 조건: 잽, 스트레이트, 차징 풍압, 회피, 점프 내려찍기, 2단 점프 아이콘과 설명을 저장하고 권 프로필 기본 바인딩을 완성한다.
- 검증: Unreal Editor Python으로 자산 재로드 및 속성 확인
- 롤백 경계: StoneFist 신규 자산과 권 프로필 변경

### Packet 4 — 검증 및 문서화

- 수정 위치: `Feature/doc`, `DevLog/20260714.txt`, `Docs/SprintChangeLog.md`
- 완료 조건: 빌드·생성물 검사·Unreal 위험 리뷰 결과와 미결정 밸런스 항목을 기록한다.
- 검증: `Scripts/BuildEditor.ps1`, `Scripts/CheckGeneratedChanges.ps1`
- 롤백 경계: 문서 변경

## 입력 설계

| 입력 | 권 기본 동작 | Data Asset |
| --- | --- | --- |
| 좌클릭 | 1타 왼손 잽 → 콤보 2타 오른손 스트레이트 | Primary 2단 체인 |
| 좌클릭 차징 | 사용하지 않음 | 미바인딩 |
| 우클릭 | 사용하지 않음 | 미바인딩 |
| 우클릭 차징 | 뒤로 당겨 기를 모은 뒤 풍압과 함께 전방 타격 | ChargedSpecial |
| E | 짧은 거리 방향 회피, 공중 사용 불가 | DodgeSkill |
| 점프 중 좌클릭 | 양손 내려찍기 | JumpPrimary |
| Space | 최대 2회 점프 | Profile.MaxJumpCount = 2 |

## 임시 밸런스 기본값

최종 수치가 정해지기 전까지 기능 검증용으로 아래 값을 사용한다.

- 잽: 피해 8, 사거리 140 cm, 재사용 0.22초, 콤보 입력 0.10~0.42초
- 스트레이트: 피해 14, 사거리 165 cm, 재사용 0.38초
- 차징 풍압: 피해 30, 충전 0.8초, 사거리 600 cm, 폭 100 cm, 재사용 4초, 스테미나 12
- 점프 내려찍기: 피해 24, 반경 220 cm, 재사용 1.2초, 스테미나 8
- 회피: 거리 250 cm, 시간 0.16초, 재사용 0.55초, 스테미나 6
- 2단 점프: 착지 시 횟수 초기화, 별도 자원 소모 없음

## Data Asset / GAS 책임

- Data Asset: 이름, 설명, 아이콘, 입력 슬롯, 피해/범위/차징 수치, 콤보 연결, 몽타주, VFX와 선택할 Ability Class.
- Gameplay Ability: 입력 이후 실행 순서, 차징 유지/해제, 몽타주 Task, Gameplay Event 처리, 네트워크 예측과 취소 규칙.
- Gameplay Effect: 자원 비용, 쿨다운, 피해, 경직·버프·디버프.
- Gameplay Tag: `Ability.StoneFist.*`, `State.Attacking`, `State.Charging`, `State.Airborne`, `Cooldown.StoneFist.*` 등의 상호 배제와 상태 판정.

현재 공용 Attack Ability가 CombatComponent 실행을 호출하고 즉시 끝나는 구조는 프로토타입으로는 동작하지만, 차징 풍압과 점프 내려찍기는 전용 Ability로 분리하는 편이 적합하다. 이번 단계에서는 Data Asset과 프로필 연결을 먼저 완성하고, 전용 몽타주·Niagara·GameplayEffect가 준비될 때 Ability 실행을 확장한다.

## 미결정 항목

- 각 스킬 최종 피해/쿨다운/스테미나 수치
- 차징 풍압의 관통, 넉백, 경직, 최대 사거리
- 회피 무적 프레임 적용 여부
- 2단 점프의 스테미나 소모와 공중 제어량
- 권 아이덴티티 자원의 생성/소모 규칙과 활성화 효과
