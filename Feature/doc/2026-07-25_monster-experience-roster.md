# 몬스터 경험치 보상 및 25종 Data Asset 결과

## 결과

- 몬스터 처치 시 마지막으로 피해를 준 플레이어에게 경험치를 1회 지급한다.
- 경험치 지급은 서버 권한에서만 수행하고, 기존 사망 중복 방지 상태를 함께 사용한다.
- 처치 시 전투 로그와 `LogRogue10m`에 몬스터 이름 및 획득 경험치를 표시한다.
- Data Asset이 연결되지 않은 기존 기본 몬스터는 10 경험치를 지급한다.
- Data Asset이 연결된 몬스터는 `ExperienceReward` 값을 우선 사용한다.

## Monster Data Asset 확장

- `ERogue10mMonsterRank`
  - `Normal`
  - `MidBoss`
  - `FinalBoss`
- 기존 `bIsBoss`는 에셋 호환을 위해 유지하고, 신규 등급 표시는 `MonsterRank`를 기준으로 한다.
- 별도 공격 스킬이 없는 몬스터도 Data Asset에서 조정할 수 있도록 아래 값을 추가했다.
  - `AttackRange`
  - `AttackDamage`
  - `AttackInterval`

## 생성된 로스터

| 등급 | 수량 | 레벨 | 경험치 범위 | HP 범위 |
| --- | ---: | ---: | ---: | ---: |
| 일반 | 20 | 1~20 | 18~140 | 70~500 |
| 중간 보스 | 4 | 8~20 | 300~900 | 1,200~3,200 |
| 최종 보스 | 1 | 25 | 3,000 | 10,000 |

생성 경로:

- `/Game/DataAsset/Monster/Normal`
- `/Game/DataAsset/Monster/MidBoss`
- `/Game/DataAsset/Monster/FinalBoss`

상세 이름, 수치, 참조 메시 목록은 `Feature/architect/2026-07-25_monster-experience-roster.md`에 기록했다.

## 제작 및 검증 도구

- `Scripts/Editor/MonsterRosterDefinitions.py`
  - 25종의 단일 원본 정의
- `Scripts/Editor/CreateMonsterRosterDataAssets.py`
  - Data Asset 생성 및 갱신
- `Scripts/Editor/ValidateMonsterRosterDataAssets.py`
  - 개수, 등급, 식별자, 수치, 메시 참조 검증

## 검증 결과

- UE 5.8 UHT 및 Rogue10mEditor Win64 Development 빌드 성공
- Unreal Python Validator 통과
  - 전체 25종
  - 일반 20종
  - 중간 보스 4종
  - 최종 보스 1종
- 25종 모두 양수 경험치, HP, 공격 수치 및 유효 Skeletal Mesh 참조 확인
- `Scripts/CheckGeneratedChanges.ps1` 및 `git diff --check` 통과

## 남은 수동 QA

- 에디터 재시작 후 일반 몬스터 처치 시 HUD 경험치 및 전투 로그 증가 확인
- 중간 보스와 최종 보스가 각각 Data Asset 경험치를 1회만 지급하는지 확인
- 환경 피해나 플레이어 외 공격으로 처치됐을 때 경험치 귀속 정책 확인
- 멀티플레이 도입 시 파티 경험치 분배 정책 별도 설계
