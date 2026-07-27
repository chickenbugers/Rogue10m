# 몬스터 영역 스포너 개발 결과

## 결과

`ARogue10mMonsterSpawner`를 추가했다. 레벨에 배치한 Box 범위에서 지정한 몬스터 클래스를 최대 N마리까지 생성하며, 설정에 따라 처치된 몬스터를 일정 간격으로 보충하거나 처음 한 번만 생성할 수 있다.

## 주요 변경

- `UBoxComponent` 기반 생성 범위
- `TSubclassOf<ARogue10mBasicMonster>` 기반 특정 몬스터 Blueprint 지정
- 최대 생존 수 `MaxAliveMonsters`
- 시작 시 최대 수 즉시 충원 옵션
- 지속 인구 유지 또는 일회성 생성 선택
- Tick 없는 반복 타이머 기반 보충
- WorldStatic 지면 라인 트레이스
- 충돌 처리 실패 시 추적 수를 증가시키지 않는 안전 생성
- `TWeakObjectPtr` 배열과 `OnDestroyed` Delegate 기반 생존 수 추적
- Authority 전용 생성
- Blueprint용 수동 한 마리 생성, 최대 수 충원, 생존 수 조회 API

## 에디터 사용법

1. C++ 클래스 `Rogue10mMonsterSpawner`를 레벨에 배치하거나 Blueprint 자식을 만든다.
2. `Spawn Area` Box 크기를 원하는 범위로 조절한다.
3. `Monster Class`에 생성할 `ARogue10mBasicMonster` 자식 Blueprint를 지정한다.
4. `Max Alive Monsters`에 최대 동시 생존 수를 지정한다.
5. 시작 시 즉시 최대 수를 채우려면 `Fill To Maximum On Begin Play`를 켠다.
6. 처치 후 계속 보충하려면 `Maintain Population`을 켜고 `Spawn Interval Seconds`를 지정한다.
7. 한 번만 생성하려면 `Maintain Population`을 끈다.
8. 지면 배치가 필요하면 `Project Spawn Location To Ground`를 켜고 Box가 WorldStatic 지면을 포함하도록 배치한다.

## 동작 규칙

- `MaxAliveMonsters`가 0이면 스포너가 비활성화된다.
- 생성 충돌이나 지면 탐색 실패 시 해당 시도는 생존 수에 포함되지 않는다.
- `Maintain Population`이 켜져 있으면 최대 수 미만일 때 간격마다 한 마리씩 보충한다.
- 사망 후 Actor가 즉시 파괴되지 않는 몬스터도 `IsDead()` 기준으로 생존 목록에서 제외한다.
- 스포너가 종료될 때 타이머와 파괴 Delegate를 정리하지만 이미 생성된 몬스터를 강제로 제거하지 않는다.

## 검증

- UHT 통과
- `Rogue10mMonsterSpawner.cpp` UBT 단일 파일 컴파일 성공
- UI Preview Actor의 Unity 상수 재정의 수정
- `Rogue10mEditor` Win64 Development 전체 빌드 성공
- `git diff --check`
- `Scripts/CheckGeneratedChanges.ps1`

초기 전체 Unity 빌드에서는 `Rogue10mEquipmentPreviewActor.cpp`와 `Rogue10mCharacterCustomizationPreviewActor.cpp`의 익명 네임스페이스 상수 이름이 충돌했다. 값은 유지하고 각각 `EquipmentPreview*`, `CustomizationPreview*` 이름으로 분리했으며, 수정 후 스포너를 포함한 전체 모듈 컴파일과 링크가 성공했다.

## 수동 QA

- Box 범위 밖에 생성되지 않는지 확인
- 시작 시 최대 N마리 생성 확인
- N마리 상태에서 추가 생성이 차단되는지 확인
- 한 마리 처치 후 지정 간격에 한 마리만 보충되는지 확인
- `Maintain Population=false`에서 재소환되지 않는지 확인
- 지면 없음 또는 충돌 위치에서 실패 로그와 최대 수 보존 확인
