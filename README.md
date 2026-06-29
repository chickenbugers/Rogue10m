# Rogue10m

Unreal Engine 5.7 기반 1인칭 싱글 플레이 로그라이크 프로토타입입니다.

플레이어는 시작맵에서 NPC를 통해 아이템, 특성, 난이도, 스토리 정보를 확인한 뒤 포탈을 통해 플레이맵으로 진입합니다. 플레이맵에 들어가면 20분 런 타이머가 시작되고, 제한 시간 안에 보스를 처치하는 것을 목표로 합니다.

## 프로젝트 개요

- 엔진: Unreal Engine 5.7
- 장르: 1인칭 싱글 플레이 로그라이크
- 핵심 목표: 20분 안에 보스 처치
- 기본 상태: 무기 없이 시작하며 맨손 공격 가능
- 무기 타입: 단검, 대검, 쌍검, 활, 지팡이, 너클
- 성장 방향: 무기 타입별 숙련도, 스킬, 아이템, 보스 보상
- 커스터마이징: 외형 전용, 스탯 영향 없음

## 현재 구현된 주요 기능

### 캐릭터와 전투

- `BP_FirstPersonCharacter` 기반 1인칭 캐릭터
- WASD 이동, 점프, 마우스 시점 조작
- 좌클릭/우클릭 공격 입력
- 점프 공격, 차징 공격 입력 구분
- 공격 판정 디버그 표시
  - 공격 방향 선
  - 공격 범위 구체
  - 명중 지점 표시
- 플레이어 피격 시 화면 피드백
- 몬스터 피격 시 피해 숫자 표시

### 공격 스킬 Data Asset

공격 설정은 `URogue10mAttackSkillData` Data Asset으로 관리합니다.

설정 가능한 값:

- 스킬 이름과 설명
- 입력 슬롯
- 데미지
- 공격 범위
- 공격 판정 반지름
- 쿨타임
- 차징 시간
- 공격 Montage
- 공격 이펙트
- 디버그 색상

Data Asset이 지정되지 않은 공격 입력은 잠긴 것으로 처리됩니다.

관련 문서:

- `Docs/AttackSkillDataAssetGuide.md`

### 인벤토리와 장비창

- 아이템 창과 장비창 분리
- 드래그 앤 드롭 이동
- 우클릭 장착
- 장비창에서 우클릭 해제
- 아이템 hover 툴팁
- 한국어 아이템 이름과 설명
- 장비 슬롯 구조
  - 왼쪽: 주무기, 머리, 갑옷, 신발
  - 오른쪽: 보조무기, 반지, 귀걸이

### HUD와 UI

- 체력, 스테미나 기본 표시
- 마나는 활성화 변수에 따라 표시
- 캐릭터 정보 패널
- 1번부터 5번까지 퀵 슬롯
- 스킬트리 창
- 설정 패널
  - 음량
  - X/Y 감도
  - 60/120/140 FPS 제한
- 런 시작 전에는 20분 타이머 UI 숨김
- 플레이맵 진입 후 런 타이머 표시

### 맵 구조와 포탈

- 시작맵: NPC와 준비 시스템이 있는 허브
- 플레이맵: 20분 제한 로그라이크 전투 맵
- `ARogue10mRunPortal`로 시작맵에서 플레이맵 이동
- 포탈 이동 시 `StartRun=1` 옵션을 전달해 플레이맵에서 타이머 시작

권장 맵 에셋 위치:

- 시작맵: `Content/Rogue10m/Maps/StartHub.umap`
- 플레이맵: `Content/Rogue10m/Maps/OpenRunMap.umap`

맵 메타데이터 위치:

- `Source/Rogue10m/World/Rogue10mMapDataLibrary.cpp`

## 개발 문서

- 하네스 규칙: `AGENTS.md`
- 개발 기준: `Docs/ProjectDevelopmentGuidelines.md`
- 브랜치 전략: `Docs/GitBranchStrategy.md`
- 월드 구조: `Docs/WorldStructure.md`
- 애니메이션 연결 가이드: `Docs/AnimationIntegrationGuide.md`
- 공격 스킬 Data Asset 가이드: `Docs/AttackSkillDataAssetGuide.md`
- 기능별 슈도 코드: `Docs/Pseudocode/`
- 개발 로그: `DevLog/`

## 빌드

Editor target 빌드:

```powershell
.\Scripts\BuildEditor.ps1
```

PowerShell 실행 정책으로 막히는 경우:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Scripts\BuildEditor.ps1
```

커밋 전 생성 파일 확인:

```powershell
.\Scripts\CheckGeneratedChanges.ps1
```

## 브랜치 전략

기본 흐름:

```text
feature/*, content/*, level/*, balance/*, fix/* -> develop -> test -> main
```

현재 Codex 작업 규칙:

- 기능 개발은 항상 별도 브랜치에서 진행
- 변경이 있으면 `DevLog/YYYYMMDD.txt`에 한국어로 기록
- 커밋 전 변경 파일과 요약 확인
- 핵심 게임플레이 로직은 C++ 우선
- 데이터 확장은 Data Asset 우선
- 캐릭터 상태가 커지면 PlayerState와 Component로 분리

## 저장소

- GitHub: https://github.com/chickenbugers/Rogue10m
