# Sprint 변경 이력

이 문서는 Rogue10m의 Sprint별 주요 변경 사항과 검증 결과를 요약한다.
세부 설계와 구현 결과는 Feature 문서, 일자별 작업 과정은 DevLog를 참조한다.

## 기록 규칙

각 작업은 다음 형식으로 누적한다.

- 작업 식별자: Sprint#스프린트번호-작업번호
- 브랜치: Sprint#스프린트번호-작업번호-간단한-영문-설명
- Sprint 번호: `test`가 `main`에 성공적으로 병합되어 현재 Sprint가 종료된 시점에만 증가
- 작업 번호: 같은 Sprint 안에서 독립 작업이 시작될 때 증가하며, 다음 Sprint 시작 시 1로 초기화
- 상태: 계획 / 개발 중 / 빌드 완료 / QA 완료 / main 반영
- 목표: 작업이 해결하려는 문제
- 주요 변경: 플레이어가 체감하거나 구조적으로 중요한 변경
- 검증: 빌드, 테스트 또는 확인한 체크리스트
- 관련 문서: Feature/architect 및 Feature/doc 문서

작은 수정이 같은 Sprint 작업 범위에 포함되면 기존 항목에 추가한다.
별도 기능이나 독립적인 롤백 경계를 가진 작업은 다음 작업번호를 사용한다.

---

## Sprint 0 — 플레이어블 프로토타입 기반

### 주요 목표

- 10분 로그라이크 전투의 기본 플레이 루프 구성
- 전투, 장비, 스킬, HUD, 몬스터와 런 진입 구조 확보
- UE 5.8 프로젝트 Harness와 문서 흐름 도입

### 주요 변경

- 시작 허브와 전투 맵 이동 흐름
- 기본 몬스터와 플레이어 전투
- GAS 기반 Attribute 및 스킬 구조
- 공격 Data Asset과 콤보/쿨다운
- 장비, 인벤토리 초기 구조
- MainHUD와 Widget Part 브리지
- 다단 히트, 피해 숫자, 플레이어 피격 피드백
- Lazy Codex Harness와 Feature/DevLog 문서 체계

### 상태

- main 반영

### 관련 문서

- Feature/doc/2026-06-23_prototype-run-flow.md
- Feature/doc/2026-06-29_equipment-damage-feedback.md
- Feature/doc/2026-07-02_gas-skill-system.md
- Feature/doc/2026-07-11_multihit-damageable-targets.md
- Feature/doc/2026-07-12_attack-patterns-and-hit-modes.md
- Feature/doc/2026-07-12_hud-refresh-optimization.md

---

## Sprint 1 — 조작, 성장, 인벤토리, 몬스터 데이터 확장

### Sprint#1-1 — sprint-postprocess-stamina

- 상태: 빌드 완료, 통합 대기
- 목표: 달리기와 회피 조작 및 플레이어 전투 피드백 개선
- 주요 변경:
  - Shift 달리기와 스테미나 소비
  - Post Process 기반 달리기 효과
  - 방향성 Dodge와 입력 잠금
  - Space 점프, E 구르기 입력 재배치
  - 공중 구르기 차단
  - 피격 효과 지속 시간 0.5초 설정
- 검증:
  - Rogue10mEditor 빌드
  - 이동 상태와 입력 충돌 코드 검토
- 관련 문서:
  - Feature/doc/2026-07-12_sprint-postprocess-stamina.md
  - Feature/doc/2026-07-12_directional-dodge.md
  - Feature/doc/2026-07-13_jump-roll-input-remap.md

### Sprint#1-2 — progression-skill-slots

- 상태: 빌드 완료, 통합 대기
- 목표: 레벨/경험치 표시와 스킬 슬롯 상태 표현 개선
- 주요 변경:
  - 경험치 Hover 상세 정보
  - 현재 경험치 / 필요 경험치 소수점 3자리 표시
  - 정수 레벨 표시
  - WBP_Progression을 LevelExperience UI로 정리
  - 좌/우 공격, 차징, Space 스킬 슬롯 배치
  - 해금 및 콤보 진행 상태에 따른 슬롯 비활성화
- 검증:
  - Rogue10mEditor 빌드
  - Widget Blueprint 변수명 확인
- 관련 문서:
  - Feature/doc/2026-07-12_experience-hover-level.md
  - Feature/doc/2026-07-12_level-experience-skill-slots.md

### Sprint#1-3 — grid-inventory-windows

- 상태: 빌드 완료, 통합 대기
- 목표: 확장 가능한 NxM 인벤토리와 메뉴 창 기반 마련
- 주요 변경:
  - 기본 10x10 인벤토리
  - NxM 아이템 배치 및 충돌 검사
  - 가방 아이템 기반 추가 컨테이너
  - 아이템 Data Asset과 월드 드롭 Mesh
  - 인벤토리, 장비, 스킬트리 UserWidget 부모 클래스
  - 메뉴 창 토글과 입력 모드 처리
- 검증:
  - Rogue10mEditor 빌드
  - 그리드 배치/이동/제거 API 검토
- 관련 문서:
  - Feature/doc/2026-07-13_grid-inventory-and-windows.md
  - Docs/GridInventoryAndMenuWindowsGuide.md

### Sprint#1-4 — monster-data-regen-ui-cleanup

- 상태: 빌드 완료, 통합 대기
- 목표: 몬스터 데이터화, 공용 자원 회복, HUD 단순화
- 주요 변경:
  - Monster Data Asset 타입 추가
  - 이름, 레벨, 경험치, Vitals, Mesh, AnimBP, AI/공격 설정 데이터화
  - 플레이어/몬스터 공용 초당 자원 회복 컴포넌트
  - 기본 체력/스테미나/마나 회복량 각각 초당 0.1
  - MonsterInfo를 LV N : 이름 + HP 형태로 변경
  - MainWidget MiniMap 임시 비활성화
  - 아이템 획득 피드를 이미지 | 수량으로 변경
  - 현재 프로젝트 구조 Mermaid 문서 추가
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - CheckGeneratedChanges 통과
  - git diff --check 통과
- 관련 문서:
  - Feature/doc/2026-07-13_monster-data-regen-and-hud-cleanup.md
  - Docs/CurrentProjectArchitecture.md

---

## 다음 Sprint 작성 템플릿

### Sprint#N-M — short-description

- 상태: 계획
- 목표:
- 주요 변경:
  -
- 검증:
  -
- 관련 문서:
  - Feature/architect/YYYY-MM-DD_feature-name.md
  - Feature/doc/YYYY-MM-DD_feature-name.md
### Sprint#1-5 — skill-tree-drag-drop

- 상태: 빌드 완료, 에디터 에셋 설정 대기
- 목표: 하드코딩 스킬 경로 제거와 스킬트리 기반 Loadout 구성
- 주요 변경:
  - 무기별 Weapon Skill Profile Data Asset
  - 기본 1m Dodge Skill Data Asset
  - 무기 변경 시 기본 회피 자동 바인딩
  - 스킬트리 해금 스킬 Drag 시작
  - 좌/우 클릭 및 차징 슬롯 Drop 장착
  - E 회피 슬롯 자동 표시 및 Drop 차단
  - 기존 숫자키 소비 아이템 슬롯 유지
- 검증:
  - UnrealHeaderTool 성공
  - Rogue10mEditor Win64 Development 빌드 성공
  - Tick 추가 없음
- 관련 문서:
  - Feature/architect/2026-07-13_skill-tree-drag-drop-loadout.md
  - Feature/doc/2026-07-13_skill-tree-drag-drop-loadout.md
  - Docs/SkillTreeLoadoutGuide.md
### Sprint#1-6 - equipment-p-key

- 상태: 구현 및 Editor 빌드 완료
- 목표: 장비창 입력을 P로 변경하고 메뉴 Widget 및 회피 Data Asset 설정 위치 명확화
- 주요 변경:
  - 장비창 토글 키 B에서 P로 변경
  - 메뉴 창 가이드 및 입력 의사 코드 갱신
  - InventoryWindowWidgetClass 및 SkillTreeWindowWidgetClass 설정 절차 정리
  - 무기별 Weapon Skill Profile의 DefaultDodgeSkill 참조 경로 정리
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - CheckGeneratedChanges Harness 경로 검사 통과
- 관련 문서:
  - Feature/architect/2026-07-14_equipment-p-key-and-editor-setup.md
  - Feature/doc/2026-07-14_equipment-p-key-and-editor-setup.md
  - Docs/GridInventoryAndMenuWindowsGuide.md
  - Docs/SkillTreeLoadoutGuide.md
### Sprint#1-7 - character-assets-menu-widgets

- 상태: C++ 빌드 완료, Editor 에셋 생성 대기
- 목표: 캐릭터 기본 전투 Data Asset과 필수 메뉴 UserWidget 에셋 구성
- 주요 변경:
  - 맨손 기본 회피 Data Asset 생성 정의(100cm / 0.16초 / 쿨타임 0.45초 / 스테미나 10)
  - 기존 맨손 공격 6종을 묶는 Weapon Skill Profile 생성 및 Combat Component 연결 자동화
  - 인벤토리 10x10 그리드/용량 표시 골격
  - 장비 슬롯 컨테이너 골격
  - 스킬 트리 엔트리/목록/Drag & Drop 안내 골격
  - 메뉴 WBP 생성과 PlayerController 클래스 연결 자동화
- 검증:
  - Unreal Python `py_compile` 성공
  - Rogue10mEditor Win64 Development 빌드 성공
  - 열린 Editor Remote Execution 활성화 후 `.uasset` 생성 검증 예정
- 관련 문서:
  - Feature/architect/2026-07-14_character-assets-and-menu-widgets.md
  - Feature/doc/2026-07-14_character-assets-and-menu-widgets.md
  - Scripts/Editor/CreateCharacterAssetsAndMenuWidgets.py
#### Sprint#1-7 추가 문서 - AI 개발 세팅 참조

- Unreal Python, Remote Execution, AI 플러그인 설정 정리
- Editor 스크립트 실행 및 외부 연결 예시 추가
- Hot Reload, 에셋 저장, 동시 Editor 실행 관련 안전 규칙 추가
- 관련 문서: `Ai 개발 세팅 참조/README.md`
### Sprint#1-8 - plugin-cpp-cleanup

- 상태: 구현 및 Editor 빌드 완료
- 목표: 사용하지 않는 Editor 플러그인 자동 활성화와 C++ 설정·모듈 의존성 정리
- 주요 변경:
  - ModelingToolsEditorMode, StateTree, GameplayStateTree 프로젝트 자동 활성화 제거
  - StateTreeModule, GameplayStateTreeModule 의존성 제거
  - Main HUD 비활성 미니맵 바인딩·클래스 설정·갱신 함수 제거
  - Build.cs 빈 설정과 샘플 주석 제거
  - Python __pycache__ 및 bytecode Git 무시 규칙 추가
  - 사용 중인 GAS, AI, UMG/Slate, AI Assistant/MCP 플러그인 유지
- 검증:
  - Unreal 프로젝트 파일 생성 성공
  - Rogue10mEditor Win64 Development 빌드 성공
  - CheckGeneratedChanges 및 git diff --check 통과
- 관련 문서:
  - Feature/architect/2026-07-14_plugin-cpp-cleanup.md
  - Feature/doc/2026-07-14_plugin-cpp-cleanup.md
### Sprint#1-9 - menu-window-widget-setup

- 상태: 구현, Editor 에셋 설정 및 빌드 완료
- 목표: InventoryWindowWidget과 SkillTreeWindowWidget 누락 경고 해결 및 실제 WBP 연결
- 주요 변경:
  - WBP_InventoryWindow, WBP_EquipmentWindow, WBP_SkillTreeEntry, WBP_SkillTreeWindow 검증·컴파일
  - BP_FirstPersonPlayerController에 세 Menu Window Widget Class 지정
  - SkillTreeWindow에 WBP_SkillTreeEntry 지정
  - 명시 Class 누락 시 기본 WBP를 불러오는 Soft Class fallback 추가
  - 메뉴 WBP 생성·복구용 Editor Python 스크립트 추가
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - Editor CDO ClassProperty 재조회 성공
  - 저장 후 dirty package 0건
  - CheckGeneratedChanges 및 git diff --check 통과
- 관련 문서:
  - Feature/architect/2026-07-14_menu-window-widget-setup.md
  - Feature/doc/2026-07-14_menu-window-widget-setup.md
  - Docs/GridInventoryAndMenuWindowsGuide.md
### Sprint#1-10 - content-asset-naming

- 상태: 구현, Editor 에셋 참조 정리 및 빌드 완료
- 목표: Widget, DataAsset, FirstPerson 콘텐츠의 UE 5.8 명명 규칙 위반 정리
- 주요 변경:
  - `UW_Rogue10mMainWidget` → `WBP_Rogue10mMainHUD`
  - `ABP_FP_Copy` → `ABP_FirstPerson`
  - `CtrlRig_FPWarp` → `Rig_FirstPersonWarp`
  - Main HUD C++ Soft Class 및 유지보수 스크립트·가이드 경로 갱신
  - 재실행 가능한 Editor Python 이름 변경·구 소스 정리 절차 추가
  - DataAsset의 `DA_` 접두사와 World Partition `Lvl_FirstPerson`은 검토 후 유지
- 검증:
  - Widget 21개, DataAsset 10개, FirstPerson 7개 전수 감사 결과 위반 0건
  - 구 경로 3개 부재, 신규 경로 3개 존재
  - Character/Anim Blueprint/Control Rig 및 PlayerController/Main HUD 참조 확인
  - Object Redirector 0건, dirty package 0건
  - Rogue10mEditor Win64 Development 빌드 성공
- 관련 문서:
  - Feature/architect/2026-07-14_content-asset-naming.md
  - Feature/doc/2026-07-14_content-asset-naming.md
  - Docs/WidgetBlueprintHUDGuide.md
### Sprint#1-11 - stone-fist-identity

- 상태: 데이터 구성, Editor 자산 연결 및 C++ 빌드 완료
- 목표: 권 아이덴티티의 기본 전투·회피·2단 점프를 데이터 주도 구조로 설계하고 플레이어 기본 프로필로 연결
- 주요 변경:
  - `T_Identity_StoneFist` 대표 이미지와 권 스킬 아이콘 6종 반영
  - 잽 → 스트레이트 2타 콤보, 우클릭 차징 권압, 점프 내려찍기 Data Asset 생성
  - 일반 우클릭과 좌클릭 차징 미바인딩
  - 권보 Dodge Data Asset과 `Knuckle` Skill Profile 생성
  - CharacterData 기본 무기 타입 및 Profile 최대 점프 횟수 추가
  - BP_FirstPersonCharacter CombatComponent에 CharacterData/권 프로필 연결
  - Data Asset과 GAS의 책임 경계 및 후속 전용 Ability 마이그레이션 정의
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - Editor Remote Execution 자산 로드·속성·바인딩·콤보 참조 확인
  - 공중 회피 차단 및 `MaxJumpCount=2` 코드 경로 확인
- 관련 문서:
  - Feature/architect/2026-07-14_stone-fist-identity.md
  - Feature/doc/2026-07-14_stone-fist-identity.md
### Sprint#1-12 - menu-widget-layout

- 상태: 구현, Editor 자산 재컴파일 및 빌드 완료
- 목표: Parts UI 스타일을 기준으로 Inventory, Equipment, SkillTree 메뉴를 실제 확장 가능한 레이아웃으로 재구성
- 주요 변경:
  - 공통 Canvas/Frame/Content 메뉴 계층과 다크 패널/금색 강조 스타일
  - Inventory 10x10 UniformGrid, NxM ItemCanvas, BagTab, Capacity 영역
  - Equipment 캐릭터 프리뷰, 7개 장비 슬롯, 능력치 영역
  - SkillTreeEntry 아이콘/이름/설명/잠금 상태 카드
  - SkillTree ScrollBox/WrapBox, 필터, 스킬 포인트 영역
  - WBP 4종과 PlayerController/SkillTreeEntry 클래스 참조 재컴파일·저장
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - Editor Remote Execution 클래스 재로드와 참조 확인
  - Tick 추가 없음
- 관련 문서:
  - Feature/architect/2026-07-14_menu-widget-layout.md
  - Feature/doc/2026-07-14_menu-widget-layout.md

### Sprint#1-13 - menu-designer-layout

- 상태: 구현, Editor 자산 직접 배치 및 빌드 완료
- 목표: 메뉴 UI를 C++ Slate fallback이 아닌 Widget Blueprint Designer 소유 구조로 전환
- 주요 변경:
  - Inventory Canvas/Frame/Grid/NxM ItemCanvas/하단 돈·무게 직접 배치
  - GridSize X×Y에 맞춘 WBP_InventoryCell UserWidget 자동 생성
  - BagTab/Capacity/Hint 제거 및 Item Data Asset UnitWeight 기반 총 무게 계산
  - Equipment 프리뷰/장비 슬롯 7종/능력치 영역 직접 배치
  - SkillTreeEntry 아이콘/이름/설명/잠금 상태 UserWidget 구성
  - SkillTreeWindow 필터/ScrollBox/WrapBox/스킬 포인트 영역 직접 배치
  - Overlay 제거, Root Canvas 형제 배치와 ZOrder 사용
  - 네이티브 `RebuildWidget()` 제거 및 필수 `BindWidget` 계약 적용
  - Editor Python 필수 이름·Overlay 0개 검증 추가
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 메뉴 WBP 4종과 WBP_InventoryCell Compile/Save 성공
  - InventoryCellWidgetClass 연결 및 제거 대상 위젯 0개 확인
  - 필수 위젯 누락 0개, Overlay 0개
- 관련 문서:
  - Feature/architect/2026-07-15_menu-designer-layout.md
  - Feature/doc/2026-07-15_menu-designer-layout.md
  - Docs/GridInventoryAndMenuWindowsGuide.md

### Sprint#1-13 - NxM Inventory Cell/Item/BagTab 상호작용

- 목표: 인벤토리 좌표 셀, NxM 아이템, 가방 탭을 분리된 UserWidget으로 구현하고 회전·충돌 프리뷰를 제공한다.
- 주요 변경: 회전 상태 저장, 회전 footprint 기반 경계/AABB 충돌, Canvas 좌표 스냅, 잡은 셀 오프셋, R키 90도 회전, 녹색/적색 프리뷰, 가방 탭 전환.
- 자산: `WBP_InventoryCell`, `WBP_InventoryItem`, `WBP_BagTab`, `WBP_InventoryWindow`.
- 검증: Rogue10mEditor Development 빌드 성공, 열린 Editor WBP 컴파일/저장 및 클래스 할당 확인.
- 상태: 구현 및 로컬 검증 완료. PIE에서 아이템 Data Asset별 NxM 이동/회전 체감 QA 필요.
- 관련 문서: `Feature/architect/2026-07-15_menu-designer-layout.md`, `Feature/doc/2026-07-15_menu-designer-layout.md`, `Docs/GridInventoryAndMenuWindowsGuide.md`.
### Sprint#1-13 - menu-widget-contract-and-folders

- 상태: 구현 및 로컬 검증 완료
- 목표: BagTab 바인딩 오류와 UserInterfaceSettings EditCondition 오류를 해소하고 메뉴 자산 구조 및 Inventory Cell 시인성을 정리한다.
- 주요 변경:
  - `UI_BagSizeText` C++ 필수 바인딩 및 BagTab GridSize 인자 제거
  - `WBP_InventoryCell`에 0.5 패딩 기반 어두운 외곽선 적용
  - 메뉴 자산을 `Inventory`, `Equipment`, `SkillTree` 기능별 폴더로 이동
  - PlayerController SoftClassPath, 내부 WidgetClass, Editor 자동화 경로 갱신
  - 이동 후 누락된 `SkillTreeEntryWidgetClass` 재지정
  - UserInterfaceSettings Font DPI 기본값 명시 및 전체 Editor 모듈 재빌드
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - UE5.8 commandlet 메뉴 WBP 7종 컴파일 및 내부 클래스 참조 검사 성공
  - `UI_BagSizeText`, `LogEditCondition`, `bUseCustomFontDPI` 오류 재발 없음
- 관련 문서:
  - `Feature/architect/2026-07-15_menu-designer-layout.md`
  - `Feature/doc/2026-07-15_menu-designer-layout.md`
  - `Docs/GridInventoryAndMenuWindowsGuide.md`
### Sprint#1-13 - inventory-grid-visual-balance

- 상태: 구현 및 로컬 WBP 검증 완료, PIE 시각 재확인 필요
- 목표: Inventory Cell과 창 내부 구획의 시각적 균형을 마비노기/Diablo II식 NxM Grid 기준으로 개선한다.
- 주요 변경:
  - 44×44 Cell에 1px 다크 경계와 차콜 Fill 적용
  - 448×448 `UI_InventoryGridFrame` 추가
  - Grid/ItemCanvas 중심 `(0, 16)` 통일
  - BagTab 136×36, 14pt 및 어두운 배경 적용
  - Title/Tab/Grid/BottomInfo 중첩 제거와 대칭 여백 확보
- 검증:
  - UE5.8 메뉴 WBP 7종 commandlet 컴파일 성공
  - `UI_InventoryGridFrame` 필수 Designer 위젯 확인
  - Inventory/SkillTree 내부 WidgetClass 참조 확인
  - Python 오류 및 Overlay 0개
- 관련 문서:
  - `Feature/architect/2026-07-15_menu-designer-layout.md`
  - `Feature/doc/2026-07-15_menu-designer-layout.md`
  - `Docs/GridInventoryAndMenuWindowsGuide.md`

### Sprint#1-13 - prototype-inventory-items

- 상태: 구현, Editor Data Asset 생성 및 C++ 빌드 검증 완료
- 목표: NxM 인벤토리 배치와 회전 QA에 사용할 1x1, 2x3, 4x3 프로토타입 아이템 준비
- 주요 변경:
  - 프로토타입 Item Data Asset 3종 생성
  - 아이콘 미지정 시 사용하는 InventoryTint 속성 추가
  - 빈 기본 인벤토리에 시작 아이템을 한 번만 자동 배치
  - bAddPrototypeStartingItems, PrototypeStartingItems로 에디터 조정 가능
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - Unreal Python 에셋 생성 및 InventorySize 재검증 성공
- 관련 문서:
  - Feature/architect/2026-07-15_prototype-inventory-items.md
  - Feature/doc/2026-07-15_prototype-inventory-items.md

### Sprint#1-14 - project-warning-cleanup

- 상태: 프로젝트 측 경고 수정 및 독립 commandlet 검증 완료
- 목표: 삭제된 몬스터 컴포넌트 직렬화 경고와 GameplayCue 전체 콘텐츠 검색 경고를 제거하고 엔진 자체 경고를 분리한다.
- 주요 변경:
  - `BP_BaseMonster`와 배치 External Actor에서 삭제된 `Rogue10mVitalsComponent` 참조 제거
  - 현재 GAS Ability System 및 Vital Regeneration 컴포넌트 유지 확인
  - `GameplayCueNotifyPaths=/Game/GameplayCues` 설정 추가
  - `ValidateWarningFixes.py` 회귀 검증 추가
  - `r.MotionVectorSimulation`은 UE 5.8 엔진 측 Render Thread Safe 플래그 문제로 판정하고 프로젝트 렌더 설정은 유지
- 검증:
  - 구형 Vitals 문자열 참조 0건
  - 새 UE5.8 commandlet 검증 성공, 0 errors / 0 warnings
- 관련 문서:
  - `Feature/architect/2026-07-16_project-warning-cleanup.md`
  - `Feature/doc/2026-07-16_project-warning-cleanup.md`
  - `DevLog/20260716.txt`

### Sprint#1-14 - inventory-item-footprint

- 상태: 구현, C++ 빌드, WBP 재구성 및 독립 commandlet 검증 완료. PIE 시각 QA 필요.
- 목표: 단일 10×10 인벤토리에서 1×1·2×3·4×3 아이템의 셀 점유 크기와 아이콘 원본 종횡비를 정확히 유지한다.
- 주요 변경:
  - Grid Entry와 DragDrop Operation의 회전 상태 및 R키 회전 처리 제거
  - Item Data Asset의 원본 `InventorySize`만 배치·충돌 footprint로 사용
  - Designer SizeBox 기본값을 44×44로 명시하고 Canvas 슬롯과 런타임 `UI_InventoryItemSize`를 `Width×44`, `Height×44`로 동기화
  - Preview Border를 전체 footprint에 Fill하고 아이콘은 4px inset `ScaleBox(ScaleToFit)`로 분리
  - `SetBrushFromTexture(Icon, true)`로 텍스처 실제 크기를 반영해 세로형·가로형 아이콘 종횡비 보존
  - 아이콘 있는 아이템은 크기 문자열을 숨기고 비장비 스택 수량만 우측 상단 표시
  - 아이콘 없는 프로토타입은 `InventoryTint`와 `W×H` fallback 표시
  - Inventory Window의 BagTab 컨테이너와 클래스 참조를 제거하고 컨테이너 0으로 고정
  - 제거된 탭 공간에 맞춰 Grid Frame/Grid/ItemCanvas 중심을 `(0,-12)`로 통일
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - `WBP_InventoryItem`, `WBP_InventoryWindow` 재구성·컴파일·저장 성공
  - 새 commandlet에서 메뉴 WBP 7종 컴파일 및 클래스 참조 검증 성공
  - 최종 `WBP_InventoryItem`에 IconScale 포함, `WBP_InventoryWindow`에 BagTabContainer 없음
  - 최신 독립 검증 로그의 Blueprint/Python 오류 0건
  - 런타임 회전 코드 검색 0건
- 관련 문서:
  - `Feature/architect/2026-07-15_inventory-item-footprint.md`
  - `Feature/doc/2026-07-15_inventory-item-footprint.md`
  - `Docs/GridInventoryAndMenuWindowsGuide.md`

### Sprint#1-14 - inventory-item-icon-scale

- 상태: 구현 및 C++ 빌드 검증 완료
- 목표: Item Data Asset마다 인벤토리 아이콘의 표시 크기를 원본 비율을 유지한 채 조절한다.
- 주요 변경:
  - `InventoryIconScale` Data Asset 속성 추가, 기본값 `1.0`, 범위 `0.1~2.0`
  - `ScaleToFit` 이후 중앙 Pivot 기준 균일 Render Scale 적용
  - 확대 아이콘이 인접 셀을 침범하지 않도록 footprint 경계 클리핑
  - 점유 크기·배치·충돌은 기존 `InventorySize` 기준 유지
- 검증: Rogue10mEditor Win64 Development 빌드 성공
- 관련 문서:
  - `Feature/architect/2026-07-15_inventory-item-footprint.md`
  - `Feature/doc/2026-07-15_inventory-item-footprint.md`

### Sprint#1-14 - starter-item-icons

- 상태: 아이콘 생성, Data Asset/Texture Import, C++ 빌드, WBP 재구성 및 독립 commandlet 검증 완료. PIE 시각 QA 필요.
- 목표: 포션과 기본 장비를 실제 이미지로 제작해 M×N 인벤토리와 장비창에서 데이터 기반으로 표시한다.
- 주요 변경:
  - 다크 판타지 스타일 스타터 아이콘 6종 제작 및 투명 배경 처리
  - 1칸당 256px 기준으로 1×1, 1×3, 2×2, 2×3 원본 캔버스 구성
  - UI Texture 6개와 Item Data Asset 6개 생성
  - Data Asset 기반 `FRogue10mItemStack` 변환과 시작 아이템 배치
  - 포션 시작 수량 5개 및 장비 5종 프로토타입 자동 장착
  - `OnEquipmentChanged` Delegate와 장비창 슬롯 아이콘 7개 추가
  - 장비 슬롯에도 `ScaleBox(ScaleToFit)`와 `InventoryIconScale` 적용
- 검증:
  - PNG 투명 모서리와 M×N 해상도 검사 성공
  - Unreal Python Import 시 Texture/Data Asset 크기·참조 검증 성공
  - Rogue10mEditor Win64 Development 빌드 성공
  - UE5.8 commandlet 메뉴 WBP 전체 컴파일 성공, 0 errors / 0 warnings
  - 열린 Editor에서 Data Asset 6개, PrototypeStartingItems 9개, 장비 아이콘 7개와 누락 0개 확인
- 관련 문서:
  - `Feature/architect/2026-07-15_starter-item-icons.md`
  - `Feature/doc/2026-07-15_starter-item-icons.md`
  - `Feature/architect/2026-07-15_inventory-item-footprint.md`
  - `Feature/doc/2026-07-15_inventory-item-footprint.md`

### Sprint#1-14 - inventory-item-tint-layer-order

- 상태: 수정 및 Editor/commandlet 검증 완료
- 목표: InventoryTint Border가 실제 아이콘과 수량을 덮지 않도록 렌더 계층을 고정한다.
- 주요 변경:
  - `UI_InventoryItemPreviewBorder` GridSlot Layer 0
  - `UI_InventoryItemIconScale` GridSlot Layer 1
  - `UI_InventoryItemQuantityText` GridSlot Layer 2
  - `ValidateMenuWidgetAssets.py`에 Layer 회귀 검사 추가
- 검증:
  - 열린 Editor에서 Layer 0/1/2 확인 및 WBP 컴파일·저장 성공
  - UE5.8 commandlet 전체 메뉴 WBP 컴파일 성공, 0 errors / 0 warnings
- 관련 문서:
  - `Feature/doc/2026-07-15_starter-item-icons.md`
  - `DevLog/20260716.txt`

### Sprint#1-14 - inventory-item-icon-runtime-paint

- 상태: 구현, C++ 빌드, WBP 자동 검증, PIE 시각 검증 완료
- 목표: Texture가 연결됐지만 Tint만 보이던 NxM 인벤토리 아이콘을 런타임에 정상 표시한다.
- 주요 변경:
  - WBP_InventoryItem에서 Inventory Icon ScaleBox를 제거하고 Tint Border의 Image 콘텐츠로 직접 배치
  - Texture 종횡비와 NxM footprint를 이용해 44px 셀 기준 Fit 크기 계산
  - 생성 전에는 적용되지 않는 SetDesiredSizeOverride() 대신 FSlateBrush::SetImageSize()로 Brush에 크기 저장
  - 아이콘이 있는 일반 상태 Tint Alpha를 최대 0.18로 제한
  - 스타터 Texture Import를 TC_DEFAULT, TEXTUREGROUP_UI, TMGS_FROM_TEXTURE_GROUP으로 통일
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - ValidateMenuWidgetAssets.py 전체 통과
  - PIE UI 포함 캡처에서 포션, 장검, 투구, 갑옷, 부츠, 반지 아이콘 정상 표시
- 관련 문서:
  - Feature/architect/2026-07-15_starter-item-icons.md
  - Feature/doc/2026-07-15_starter-item-icons.md
  - Docs/GridInventoryAndMenuWindowsGuide.md
  - DevLog/20260716.txt
"@
Append-Utf8 'D:\Project\Rogue10m\Feature\architect\2026-07-15_starter-item-icons.md' @"

## Runtime Paint Correction (2026-07-16)

PIE에서 Brush 리소스는 유효하지만 아이콘이 보이지 않는 문제를 별도 패킷으로 진단했다. InitializeGridItem()은 위젯이 Canvas에 추가되기 전에 실행되므로, 내부 SImage가 존재할 때만 동작하는 SetDesiredSizeOverride()를 사용할 수 없다. 최종 구조는 SizeBox -> Grid -> Border -> Image이며, C++가 Texture 종횡비와 NxM footprint로 계산한 크기를 FSlateBrush::ImageSize에 저장한다. 완료 조건은 Editor 빌드, WBP 자동 검증, PIE 캡처에서 스타터 아이콘 6종 표시다.