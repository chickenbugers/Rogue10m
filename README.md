# Rogue10m

`Rogue10m`은 Unreal Engine 5.8로 개발 중인 1인칭 싱글 플레이 로그라이크 액션 프로토타입입니다. 플레이어는 준비 허브에서 아이템·특성·난이도·스토리 정보를 확인한 뒤 전투 맵에 진입하고, 제한 시간 20분 안에 보스를 처치하는 것을 목표로 합니다.

## 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 엔진 | Unreal Engine 5.8 |
| 장르 | 1인칭 싱글 플레이 로그라이크 액션 |
| 런타임 모듈 | `Source/Rogue10m` |
| 핵심 목표 | 20분 안에 보스 처치 |
| 기본 전투 | 무기 없이 시작하는 맨손 공격 |
| 데이터 설계 | 공격·스킬·밸런스 값을 Data Asset으로 구성 |
| 상태 시스템 | Gameplay Ability System 기반 Attribute 관리 |

## 게임 흐름

1. 시작 허브에서 NPC를 통해 아이템, 특성, 난이도, 스토리를 확인합니다.
2. `ARogue10mRunPortal`을 통해 전투 맵으로 이동합니다.
3. 전투 맵 진입 시 20분 런 타이머가 시작됩니다.
4. 적을 처치하고 장비와 스킬을 성장시키며 보스를 준비합니다.
5. 제한 시간 안에 보스를 처치하면 런에 성공합니다.

권장 맵 에셋:

- 시작 허브: `Content/Rogue10m/Maps/StartHub.umap`
- 전투 맵: `Content/Rogue10m/Maps/OpenRunMap.umap`

## 현재 구현된 주요 기능

### 플레이어 전투

- 좌클릭·우클릭, 점프 공격, 차징 공격 입력 구분
- Data Asset 기반 공격 정의와 콤보 연결
- 공격 범위와 명중 결과 디버그 표시
- 플레이어와 몬스터가 공통 공격 데이터 구조 사용
- 별도 Projectile Actor나 매 프레임 Tick을 추가하지 않는 타이머 기반 처리

### 공격 형태와 히트 모드

`URogue10mAttackSkillData`에서 공격 공간 형태와 피해 적용 방식을 독립적으로 조합합니다.

- 공격 형태: `LinearBox`, `Projectile`, `Arc`, `Circle`
- 히트 모드: `Single`, `Continuous`, `MultiHit`

### 피해 범위와 치명타

- 기본 피해 변동 범위: 90~110%
- 공격 Data Asset에서 최소·최대 피해 비율 추가 보정
- 기본 치명타 확률 0%, 기본 치명타 피해 배율 150%
- 장비, 패시브, 버프·디버프는 GAS Attribute로 확장 가능
- 연속 공격과 다단 히트는 각 타격마다 피해와 치명타를 독립 판정

### 전투 피드백

- 화면 로그와 피격 피드백
- 몬스터 피격 시 월드 위치 기반 데미지 숫자 표시
- 일반 피해는 흰색/검은 외곽선, 치명타는 노란색/주황 외곽선
- 데미지 인디케이터 위젯 풀링으로 반복 생성 비용 절감

### HUD와 UI

- 체력 ProgressBar는 빨간색, 스태미나 ProgressBar는 노란색
- 체력·스태미나 텍스트는 정수 `현재 스탯 / 최대 스탯` 형식
- 동일한 Vital 값은 다시 적용하지 않는 캐시 기반 갱신
- 주요 HUD는 0.1초, 슬롯·로그는 0.25초 간격으로 분리 갱신
- 슬롯과 로그는 전체 재생성 대신 증분 추가·제거
- 장비창, 아이템창, 스킬 트리, 설정 UI 골격
- 런 타이머와 몬스터 정보 표시

### 메뉴 UI와 NxM 인벤토리

- Inventory, Equipment, SkillTree 메뉴를 기능별 Widget Blueprint 폴더로 분리
- 10×10 기본 인벤토리 하나로 고정하며 가방 탭은 표시하지 않음
- 아이템 Data Asset의 InventorySize를 이용한 1×1부터 NxM까지의 공간 점유
- Canvas 좌표 기반 드래그 앤 드롭, 경계·아이템 충돌 프리뷰
- 회전 없이 Item Data Asset의 `InventorySize`를 고정 점유 크기로 사용
- 44px 셀 배수의 정확한 footprint와 ScaleToFit 기반 아이콘 원본 종횡비 보존
- 장비 카테고리는 보유 개수 미표시, 중첩 가능한 비장비 아이템만 수량 표시
- 셀은 WBP_InventoryCell UserWidget으로 GridSize X×Y만큼 자동 생성
- 하단에 Gold와 현재 무게 / 최대 무게 표시
- 아이콘이 없는 초기 아이템은 InventoryTint 색상으로 구분

프로토타입 Data Asset은 Content/DataAsset/Item/Prototype에 있으며 1×1, 2×3, 4×3 크기를 제공합니다. 기본 인벤토리가 비어 있으면 게임 시작 시 한 번만 자동 배치됩니다. Inventory Component의 bAddPrototypeStartingItems와 PrototypeStartingItems에서 자동 배치 여부와 목록을 조정할 수 있습니다.
### 성장과 장비 방향

- 무기 종류: 권갑, 단검, 대검, 쌍검, 활, 지팡이
- 무기 종류별 숙련도와 스킬 해금 구조
- 인벤토리와 장비창 분리, 장비 슬롯과 아이템 툴팁 UI
- 외형 커스터마이징은 능력치에 영향을 주지 않는 방향

## 기술 구조

```text
Source/Rogue10m/
├─ Ability/       GAS Attribute와 능력치
├─ Character/     플레이어 캐릭터
├─ Components/    전투 등 기능 컴포넌트
├─ Core/          GameMode, PlayerController 등 핵심 프레임워크
├─ Data/          공격·스킬 Data Asset 정의
├─ Enemy/         몬스터 전투와 AI 연동
├─ UI/            HUD, 위젯, 데미지 인디케이터
└─ World/         맵 데이터와 런 포털
```

주요 기술은 Gameplay Ability System, Enhanced Input, StateTree, UMG/Slate이며 Niagara 확장을 고려합니다.

## 공격 Data Asset

공격 데이터의 기본 위치는 `Content/DataAsset/AttackSkill`입니다. 공격력, 범위, 공간 형태, 히트 모드, 타격 횟수와 간격, 쿨다운, 자원 비용, 콤보, 애니메이션, 이펙트, 피해·치명타 보정을 에디터에서 조정할 수 있습니다.

자세한 설정 방법은 [공격 스킬 Data Asset 가이드](Docs/AttackSkillDataAssetGuide.md)를 참고하세요.

## 개발 환경과 빌드

필수 환경은 Unreal Engine 5.8, Windows 64-bit, Visual Studio Unreal Engine용 C++ 도구 체인입니다.

```powershell
.\Scripts\BuildEditor.ps1
```

실행 정책으로 차단된 경우:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Scripts\BuildEditor.ps1
```

커밋 전 생성·캐시 파일 검사:

```powershell
.\Scripts\CheckGeneratedChanges.ps1
```

## 문서

- [프로젝트 개발 가이드](Docs/ProjectDevelopmentGuidelines.md)
- [월드 구조](Docs/WorldStructure.md)
- [애니메이션 연동 가이드](Docs/AnimationIntegrationGuide.md)
- [공격 스킬 Data Asset 가이드](Docs/AttackSkillDataAssetGuide.md)
- [Grid Inventory와 메뉴 창 가이드](Docs/GridInventoryAndMenuWindowsGuide.md)
- [달리기와 화면 피드백 설정 가이드](Docs/SprintAndFeedbackGuide.md)
- [캐릭터 데이터 소유권](Docs/CharacterDataOwnership.md)
- [브랜치 전략](Docs/GitBranchStrategy.md)
- [Harness 파이프라인](Docs/HarnessPipeline.md)
- [개발 로그](DevLog/)
- [기능 설계와 결과 문서](Feature/)

## 개발 원칙

- 핵심 게임플레이 로직은 C++에 구현하고 Blueprint는 에셋과 튜닝에 사용합니다.
- 공격, 무기, 스킬, 아이템, 몬스터 데이터는 Data Asset을 우선합니다.
- Tick은 기본적으로 피하고 이벤트, 델리게이트, 타이머, 캐시를 사용합니다.
- 기능 브랜치에서 개발하고 빌드·리뷰·문서화를 거쳐 `main`에 반영합니다.

## 저장소

- GitHub: https://github.com/chickenbugers/Rogue10m

- [Grid Inventory and Menu Windows Guide](Docs/GridInventoryAndMenuWindowsGuide.md)

- [Sprint 변경 이력](Docs/SprintChangeLog.md)

- [스킬트리 Loadout과 무기별 회피 설정](Docs/SkillTreeLoadoutGuide.md)
