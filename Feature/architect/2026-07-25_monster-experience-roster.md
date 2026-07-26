# 몬스터 경험치 보상 및 25종 Data Asset 설계

## Scope Gate

### 목표

- 몬스터 처치 시 마지막 피해를 준 플레이어에게 Monster Data Asset의 경험치를 한 번 지급한다.
- Monster Data Asset에서 일반·중간 보스·최종 보스를 명확히 구분한다.
- 일반 20종, 중간 보스 4종, 최종 보스 1종을 실제 Data Asset으로 준비한다.

### 제외

- 몬스터별 전용 Animation Blueprint와 공격 스킬 제작
- 스폰 테이블과 웨이브/던전 배치
- 드롭 아이템과 골드 보상
- 보스 전용 페이즈 AI와 연출
- 경험치 저장·불러오기

## Ultrawork Packets

| 패킷 | 목표 | 완료 조건 | 검증 |
| --- | --- | --- | --- |
| ULW-1 | 보상·등급 데이터 구조 | Rank와 기본 공격값이 Data Asset에 존재 | UHT/C++ 빌드 |
| ULW-2 | 경험치 지급 경로 | 서버에서 사망 1회당 마지막 플레이어에게 지급 | 코드 검토·빌드 |
| ULW-3 | 25종 콘텐츠 | Normal 20, MidBoss 4, FinalBoss 1 생성 | Unreal Python Validator |
| ULW-4 | 문서·회귀 | 로스터와 생성 결과 기록 | diff/generated 검사 |

## 데이터 구조

### Monster Rank

`ERogue10mMonsterRank`

- `Normal`
- `MidBoss`
- `FinalBoss`

기존 `bIsBoss`는 에셋 호환을 위해 유지하고 보스 두 등급에서 `true`로 동기화한다. 런타임 표시와 신규 로직은 `MonsterRank`를 기준으로 사용한다.

### 전투 Fallback

Monster Data Asset에 다음 값을 추가한다.

- `AttackRange`
- `AttackDamage`
- `AttackInterval`

전용 `AttackSkill`이 지정되면 스킬 데이터를 우선하고, 비어 있으면 위 기본 공격값을 사용한다.

## 경험치 흐름

```text
Player Attack
  -> BasicMonster::TakeDamage(EventInstigator 저장)
  -> 체력 0
  -> Die()의 bIsDead 중복 방지
  -> Authority 확인
  -> MonsterData.ExperienceReward
  -> PlayerState::AddExperience
  -> HUD Experience Attribute 갱신
```

`AddExperience`는 서버 권한에서만 실행하며 초과 경험치는 기존 레벨업 반복문으로 다음 레벨에 이월한다.

## 로스터

### 일반 몬스터 20종

| # | Asset ID | 표시 이름 | Lv | HP | 피해 | 경험치 | Mesh |
| ---: | --- | --- | ---: | ---: | ---: | ---: | --- |
| 1 | VerdantSlime | 초록 점액체 | 1 | 70 | 6 | 18 | Polyart Slime |
| 2 | CrimsonSlime | 붉은 점액체 | 2 | 90 | 7 | 22 | PBR Slime |
| 3 | FrostSlime | 서리 점액체 | 3 | 110 | 8 | 26 | Polyart Slime |
| 4 | ToxicSlime | 맹독 점액체 | 4 | 130 | 9 | 30 | PBR Slime |
| 5 | Sporeling | 어린 포자 버섯 | 5 | 150 | 10 | 35 | Polyart Mushroom |
| 6 | EmberMushroom | 불씨 버섯 | 6 | 175 | 11 | 40 | PBR Mushroom |
| 7 | ThornMushroom | 가시 버섯 | 7 | 195 | 12 | 45 | Polyart Mushroom |
| 8 | CactusScout | 선인장 정찰병 | 8 | 220 | 13 | 50 | Polyart Cactus |
| 9 | CactusRaider | 선인장 약탈자 | 9 | 245 | 14 | 55 | PBR Cactus |
| 10 | MossShell | 이끼 등껍질 | 10 | 280 | 14 | 60 | Polyart TurtleShell |
| 11 | IronShell | 철갑 등껍질 | 11 | 330 | 15 | 66 | PBR TurtleShell |
| 12 | CaveBeholder | 동굴 감시자 | 12 | 260 | 17 | 72 | Polyart Beholder |
| 13 | ArcaneBeholder | 비전 감시자 | 13 | 290 | 18 | 78 | PBR Beholder |
| 14 | SwarmDroneEight | 군집 드론 8호 | 14 | 220 | 19 | 84 | Polyart Swarm08 |
| 15 | SwarmStingerEight | 군집 독침 8호 | 15 | 250 | 20 | 90 | PBR Swarm08 |
| 16 | SwarmDroneNine | 군집 드론 9호 | 16 | 280 | 21 | 98 | Polyart Swarm09 |
| 17 | SwarmRipperNine | 군집 절단자 9호 | 17 | 310 | 22 | 106 | PBR Swarm09 |
| 18 | YoungMimic | 어린 미믹 | 18 | 380 | 24 | 116 | Polyart ChestMonster |
| 19 | HungryMimic | 굶주린 미믹 | 19 | 430 | 26 | 128 | PBR ChestMonster |
| 20 | DuneShell | 사막 철갑수 | 20 | 500 | 28 | 140 | PBR TurtleShell |

### 중간 보스 4종

| # | Asset ID | 표시 이름 | Lv | HP | 피해 | 경험치 | Mesh |
| ---: | --- | --- | ---: | ---: | ---: | ---: | --- |
| 1 | SporeMatriarch | 포자 군락의 어미 | 8 | 1,200 | 22 | 300 | PBR Mushroom |
| 2 | ThornWarden | 가시 성채의 수문장 | 12 | 1,800 | 30 | 450 | PBR Cactus |
| 3 | AbyssEye | 심연을 보는 눈 | 16 | 2,400 | 38 | 650 | PBR Beholder |
| 4 | GoldenMimic | 황금 포식자 | 20 | 3,200 | 48 | 900 | PBR ChestMonster |

### 최종 보스 1종

| Asset ID | 표시 이름 | Lv | HP | 피해 | 경험치 | Mesh |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| AbyssOverlordAzathor | 심연의 군주 아자토르 | 25 | 10,000 | 70 | 3,000 | PBR Beholder |

## 에셋 경로

- `/Game/DataAsset/Monster/Normal`
- `/Game/DataAsset/Monster/MidBoss`
- `/Game/DataAsset/Monster/FinalBoss`

## 에셋 정책

- 기존 `MonsterForSurvivalGame`의 Skeletal Mesh를 참조하고 원본 에셋은 수정하지 않는다.
- Polyart/PBR과 스케일 차이를 조합해 시각 변형을 제공한다.
- 전용 AnimBP가 없는 현재 상태에서는 `BP_BaseMonster`의 기존 AnimClass를 유지한다.
- 모든 생성 스크립트는 기존 에셋을 갱신하는 멱등 방식으로 작성한다.

## 검증 기준

- 정확히 25개 Monster Data Asset 존재
- Rank별 수량 20/4/1
- MonsterId와 경로 중복 없음
- 경험치가 모두 양수이며 일반 < 중간 보스 < 최종 보스 구간 유지
- 필수 Skeletal Mesh 참조 유효
- HP, 공격 피해, 공격 간격, 이동 속도 Clamp 범위 충족
- Rogue10mEditor 빌드와 UnrealEditor-Cmd Validator 통과
