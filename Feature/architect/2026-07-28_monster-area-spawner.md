# 몬스터 영역 스포너 설계

## 기능 요약

레벨에 배치한 Box 범위 안에서 지정한 몬스터 클래스를 최대 N마리까지 생성하고, 처치되어 파괴된 몬스터 자리를 일정 간격으로 보충하는 `ARogue10mMonsterSpawner`를 추가한다.

## 소유 경로

- 1차 경로: `Source/Rogue10m/World/`
- 연관 경로: `Source/Rogue10m/Enemy/`
- 에디터 설정: 스포너 Blueprint 또는 레벨에 직접 배치한 C++ Actor 인스턴스

## C++ / Blueprint 경계

- C++:
  - 생존 몬스터 수 추적
  - 최대 수 제한
  - 타이머 기반 보충
  - 영역 내 임의 위치 선정
  - 지면 투영과 충돌 안전 생성
  - 파괴 이벤트 수신과 참조 정리
- Blueprint·레벨:
  - Monster Class 지정
  - Box 범위 조절
  - 최대 생존 수, 재소환 간격, 시작 즉시 충원 여부 튜닝

## 공개 설정

- `MonsterClass`: 생성할 `ARogue10mBasicMonster` 자식 클래스
- `MaxAliveMonsters`: 동시에 살아 있을 수 있는 최대 수
- `SpawnIntervalSeconds`: 최대 수 미만일 때 다음 생성 시도 간격
- `bFillToMaximumOnBeginPlay`: BeginPlay에서 최대 수까지 즉시 채울지 여부
- `bMaintainPopulation`: 처치 후 최대 수까지 계속 보충할지 여부
- `bProjectSpawnLocationToGround`: 임의 위치를 지면으로 투영할지 여부
- `GroundTraceDistance`: Box 위·아래 지면 탐색 거리
- `SpawnCollisionHandlingMethod`: 생성 충돌 처리 방식

## Ultrawork Packet

### Packet 1: 런타임 스포너

- 목표: 영역 내 최대 N마리 유지
- 입력: Monster Class, Box 범위, 최대 수, 생성 간격
- 수정 위치: `Source/Rogue10m/World/Rogue10mMonsterSpawner.h/.cpp`
- 완료 조건: Tick 없이 시작 생성, 제한, 파괴 후 보충 동작 구현
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 신규 C++ 클래스 두 파일 제거

### Packet 2: 안전성과 리뷰

- 목표: 잘못된 설정과 생성 실패가 최대 수 계산을 깨뜨리지 않게 보호
- 입력: 빈 클래스, Max 0, 지면 없음, 충돌 생성 실패, EndPlay
- 수정 위치: 신규 스포너 클래스
- 완료 조건: 경고 로그, 약한 참조 정리, 타이머와 Delegate 정리
- 검증 명령: Editor 빌드, diff 리뷰
- 롤백 경계: 안전 조건별 독립 변경

### Packet 3: 문서와 에디터 사용법

- 목표: 레벨 디자이너가 Blueprint 또는 배치 Actor에서 설정 가능
- 수정 위치: `Feature/doc/`, `DevLog/20260728.txt`, `Docs/SprintChangeLog.md`
- 완료 조건: 배치·설정·수동 QA 절차 기록
- 검증 명령: `Scripts/CheckGeneratedChanges.ps1`, `git diff --check`
- 롤백 경계: 이번 기능 기록만 제거

## 위험과 대응

- 생성 실패: 반환 Actor가 유효할 때만 추적 목록에 추가한다.
- 최대 수 초과: 생성 직전에도 유효 참조를 정리하고 생존 수를 다시 확인한다.
- 파괴되지 않는 사망 몬스터: 현재 몬스터의 `bDestroyOnDeath`가 `false`이면 Actor가 남으므로 생존 슬롯도 유지된다. 반복 스폰을 원하면 해당 몬스터 설정을 `true`로 둔다.
- 네트워크 확장: 생성과 타이머는 Authority에서만 실행한다.
- 성능: Tick을 사용하지 않고 반복 타이머 한 개만 사용한다.
