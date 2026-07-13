# 몬스터 데이터·회복·HUD 정리 설계

## Scope Gate

- 코드: Monster Data Asset, 공용 회복 Component, Monster 초기화/보상, HUD View/Widget.
- 에셋: 기존 Widget Blueprint는 삭제된 자식 이름을 존중하고 Feed 부모 변경만 Unreal Editor API로 수행한다.
- 문서: 현재 프로젝트 구조와 기능 흐름을 Mermaid로 시각화한다.

## Packet 1: Monster Data Asset

- 목표: 몬스터 정적 설정을 Data Asset으로 이동한다.
- 필드: ID, 이름, 레벨, 보스, 경험치, 체력/스태미나/마나, 초당 회복량, Mesh, AnimBP, 이동 속도, 탐지/정지 거리, 공격 스킬.
- 완료 조건: BasicMonster가 BeginPlay에서 Data Asset을 적용한다.
- 검증: UHT/Editor 빌드.

## Packet 2: 공용 Vitals 회복

- 목표: 플레이어와 몬스터에 동일한 회복 로직을 제공한다.
- 방식: Tick 대신 1초 반복 Timer를 사용하는 ActorComponent.
- 기본값: 체력/스태미나/마나 각각 초당 0.1.
- 완료 조건: 최대값 Clamp, 사망 시 회복 중지, 서버 권한 실행.

## Packet 3: MonsterInfo 정리

- 목표: UI_MonsterPropertyText 없이 이름과 레벨을 한 줄에 표시한다.
- 형식: `LV 10 : 몬스터 이름`.
- 유지: 체력 ProgressBar와 `현재 / 최대` 수치.

## Packet 4: MainHUD 미니맵 일시 제거

- 목표: 미니맵 마커 조회/갱신 및 프로토타입 패널 생성을 중지한다.
- 롤백: MainHUD의 미니맵 호출만 복원 가능.

## Packet 5: Item Acquisition Feed

- 목표: UI_FeedStackBox에 이미지와 수량 라인을 동적으로 생성한다.
- 제거 허용: UI_FeedGuideText, UI_FeedLineMockup, UI_FeedLineText.
- 라인 표시: 아이콘과 `xN`; 아이템 이름은 화면에서 숨기고 툴팁으로 제공.

## Packet 6: 구조 시각화

- 목표: Runtime, Data Asset, GAS, UI, 입력/인벤토리 흐름을 한 문서에서 파악한다.
- 출력: `Docs/CurrentProjectArchitecture.md` Mermaid 다이어그램.
