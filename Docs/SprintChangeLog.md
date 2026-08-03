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

# Sprint#2-1 - 장비창 캐릭터 프리뷰 및 7부위 레이아웃

- 목표: 장비창에서 현재 플레이어 전신 외형과 7부위 장착 상태를 적절한 비율로 함께 확인한다.
- 주요 변경:
  - SceneCapture 전용 프리뷰 액터, transient 512×768 Render Target, Leader Pose 기반 메시 복제, 장비창 수명주기 연결
  - 장비 데이터와 UMG 바인딩을 무기·투구·갑옷·장갑·신발·반지·목걸이 7부위로 통일
  - 캐릭터 프리뷰 중심 CanvasPanel 직접 배치와 부위별 슬롯 크기 차등 적용
  - ScaleToFit 및 최대 1.0 아이콘 배율로 Texture 종횡비와 슬롯 경계 보존
  - WBP_EquipmentWindow 전용 재생성 및 CanvasSlot 좌표·크기 회귀 검사 추가
- 검증:
  - UE Editor 빌드 및 전체 메뉴 Widget Blueprint 컴파일 성공
  - 7개 슬롯 위치·크기 자동 검사 성공
  - PIE 장비 데이터 7개, 장비창 표시, 프리뷰 액터 1개 확인
  - 스타터 장비 5종 Texture Brush 연결 및 빈 슬롯 아이콘 숨김 확인
- 상태: 구현 및 런타임 검증 완료
- 관련 문서: `Feature/architect/2026-07-16_equipment-character-preview.md`, `Feature/doc/2026-07-16_equipment-character-preview.md`
# Sprint#2-1 - 인벤토리·장비창 동시 표시와 드래그 UI

- 목표: 인벤토리와 장비창을 동시에 표시하고 각 창을 타이틀 Drag로 이동하며, 장비창을 좌측 스탯·중앙 캐릭터 프리뷰·우측 장비 슬롯 구조로 정리한다.
- 주요 변경:
  - 인벤토리·장비창 전용 표시 그룹으로 두 창의 상호 배타 정책 제거
  - 두 전체화면 UserWidget·Canvas를 SelfHitTestInvisible로 설정해 실제 창 자식만 입력을 받고 투명 영역은 다른 창으로 입력 통과
  - 공통 `UI_WindowRoot`, `UI_WindowDragHandle`과 Canvas 이동·Viewport 경계 Clamp 추가
  - 인벤토리와 장비창에 44px 타이틀 Drag 영역 적용
  - 장비창을 980x620으로 확장하고 좌측 200x500 스탯, 중앙 340x500 프리뷰, 우측 350x500 7슬롯 Canvas 배치
  - 공격력·방어력·최대 체력·치명타 확률·공격 속도·이동 속도 Text 자리표시자 추가
  - 6개 스탯 Text를 C++ `BindWidgetOptional`로 노출해 향후 실제 수치 연결 준비
  - 슬롯을 `투구 | 목걸이`, `갑옷 | 장갑 | 무기`, `신발 | 반지` 3행으로 재정렬
  - 슬롯 크기를 갑옷 100x150, 무기 100x200, 나머지 5부위 100x100으로 통일
  - 각 장비 슬롯을 Frame > Canvas Layer > ScaleBox/Image + LocationText 계층으로 구성
  - Texture 원본 Brush 크기 복사를 끄고 긴 변 64px 정규화 및 사방 7px Stretch Anchor 적용
  - 무기·투구·갑옷·장갑·반지·신발·목걸이 위치 텍스트를 우측 상단 8px, Layer 1로 고정
  - 빈 장비 슬롯은 아이콘만 숨기고 위치 텍스트 유지
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 활성 Editor에서 두 WBP 재구성·컴파일·저장 성공
  - 7개 슬롯 계층·좌표·크기·폰트·우측 상단 Anchor·ZOrder 자동 검사 성공
  - PIE에서 인벤토리·장비창 동시 True, 양쪽 UserWidget·화면 Canvas 입력 투과와 실제 자식 Visible 확인
  - PIE에서 투구 64×64, 갑옷 42.67×64, 무기 21.33×64 정규화 Desired Size 확인
  - 새 UE5.8 commandlet 전체 메뉴 WBP 컴파일 성공, 0 errors / 0 warnings
  - PIE 마우스 상호작용은 Remote Execution 중단으로 수동 QA 필요
- 상태: 구현 및 정적·자산 검증 완료, PIE 수동 상호작용 QA 필요
- 관련 문서: `Feature/architect/2026-07-17_draggable-inventory-equipment-windows.md`, `Feature/doc/2026-07-17_draggable-inventory-equipment-windows.md`, `DevLog/20260717.txt`

# Sprint#2-1 - 장비 → 인벤토리 Drag & Drop

- 목표: 장비창의 장착 아이템을 Data Asset의 MxN 크기를 유지한 채 인벤토리의 원하는 빈 위치로 옮긴다.
- 주요 변경:
  - `Equipment` Item Drag Source와 장비 부위·Item Data·수량·클릭 셀 오프셋 Payload 추가
  - 장비창 7개 슬롯 Frame의 Drag 시작 입력 연결
  - Inventory Window DragOver/Drop의 Grid Item·Equipment Item 공통 처리
  - `TryUnequipItemToGrid()`의 위치 재검증, Grid Entry 생성, 장비 슬롯 초기화 원자적 처리
  - 성공 시 `OnInventoryGridChanged`, `OnEquipmentChanged` 동시 갱신
  - 주무기 해제 시 `Unarmed` 전환
  - 자동 장착 시작 장비의 Grid 중복 제거
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 전체 메뉴 Widget Blueprint compile/구조 검증 성공
  - commandlet 0 errors / 0 warnings
  - `git diff --check` 통과
  - 실제 마우스 Drop은 PIE 수동 QA 필요
- 상태: 구현·C++ 빌드·WBP 컴파일 검증 완료, PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-18_equipment-to-inventory-drag-drop.md`, `Feature/doc/2026-07-18_equipment-to-inventory-drag-drop.md`, `DevLog/20260718.txt`

# Sprint#2-1 - 메뉴 창 호출 순서 ZOrder 스택

- 목표: 인벤토리와 장비창이 겹칠 때 가장 늦게 호출하거나 클릭한 창을 최상단에 표시
- 주요 변경:
  - `MenuWindowStack`과 `BringMenuWindowToFront()`로 메뉴 창 호출 순서 관리
  - 스택 순서에 따라 ZOrder `50 + Index`를 제한된 범위에서 재배치
  - 공통 Preview Mouse 좌클릭으로 가려진 창 승격
  - 직접 `SetWindowOpen(true)` 호출 경로도 동일하게 승격
  - 현재 최상단 열린 창을 Game and UI 입력 Focus 대상으로 선택
  - 표시 상태가 변한 창만 갱신해 인벤토리·장비창 동시 표시 순서 보존
  - Viewport에 없는 메뉴 창만 `AddToPlayerScreen()`으로 최초 등록
  - 이미 등록된 메뉴 창은 `UGameViewportSubsystem`의 기존 Widget Slot ZOrder만 갱신
  - 아이템 이동·창 클릭 시 메뉴 위젯 중복 화면 등록 경고 제거
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 전체 메뉴 Widget Blueprint commandlet 컴파일 성공
  - `ValidateMenuWidgetAssets.py` 0 errors / 0 warnings
  - 중복 등록 방지 수정 후 Rogue10mEditor Win64 Development 재빌드 성공
  - PIE 실제 겹침·클릭·Drag & Drop 수동 QA 필요
- 상태: 구현·C++ 빌드·WBP 정적 검증 완료, PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-18_menu-window-z-order-stacking.md`, `Feature/doc/2026-07-18_menu-window-z-order-stacking.md`, `DevLog/20260718.txt`

# Sprint#2-2 - 인벤토리 아이템 Hover 툴팁

- 목표: 인벤토리 아이템 Hover 시 이름·아이템 정보·무게를 별도 UserWidget으로 표시
- 주요 변경:
  - `URogue10mInventoryItemTooltipWidget` C++ 부모와 `WBP_InventoryItemTooltip` 추가
  - `DisplayName`, `Description`, `UnitWeight` 기반 표시
  - 중첩 수량은 개당 무게와 총 무게를 함께 표시
  - Inventory Item의 `SetToolTip()` 연결 및 Drag Preview Tooltip 제거
  - `TSoftClassPtr` native 기본 경로로 활성 Editor의 기존 WBP 파일 잠금 회피
  - Menu Designer 빌더·좁은 Tooltip 생성 스크립트·전체 Validator 확장
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - WBP_InventoryItemTooltip 생성·저장 성공
  - 전체 메뉴 Widget Blueprint commandlet 0 errors / 0 warnings
  - 필수 Widget 3개 및 native Tooltip Soft Class 기본값 검증 성공
  - PIE 실제 Hover·화면 경계·Drag 전환 수동 QA 필요
- 상태: 구현·C++ 빌드·WBP 생성 및 정적 검증 완료, PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-18_inventory-item-hover-tooltip.md`, `Feature/doc/2026-07-18_inventory-item-hover-tooltip.md`, `DevLog/20260718.txt`

# Sprint#2-3 - 인벤토리 아이템 우클릭 사용·장착

- 목표: 소비 아이템은 우클릭으로 사용하고 장비 아이템은 우클릭으로 즉시 장착·교체한다.
- 주요 변경:
  - Item Data Asset에 소비 효과용 `RestoreHealth` 설정 추가
  - 체력 회복이 실제 적용될 때만 소비 아이템 수량 1 감소
  - 빈 장비 부위 즉시 장착 및 동일 부위 기존 장비 교체
  - 기존 장비의 MxN Grid 공간을 변경 전에 확보하는 원자적 교체
  - 새 장비 원래 위치 우선, 이후 전체 컨테이너 빈 위치 검색
  - 공간 부족 시 인벤토리·장비 상태 변경 취소
  - 인벤토리·장비 Delegate 및 전투 로그 결과 갱신
  - 좌클릭 Drag & Drop과 Hover 툴팁 입력 경로 유지
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 전체 메뉴 Widget Blueprint commandlet 0 errors / 0 warnings
  - 스타터 Item Asset Python 스크립트 문법 검사 성공
  - `git diff --check` 통과
  - PIE 실제 우클릭 사용·장착·교체 수동 QA 필요
- 상태: 구현·C++ 빌드·WBP 정적 검증 완료, PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-18_inventory-item-right-click-actions.md`, `Feature/doc/2026-07-18_inventory-item-right-click-actions.md`, `DevLog/20260718.txt`

# Sprint#2-4 - 장비 Hover 스탯 및 장착 장비 비교 Tooltip

- 목표: 인벤토리 장비 Hover 시 장착 증가량과 동일 부위 현재 장착 장비의 교체 차이를 함께 표시한다.
- 주요 변경:
  - `FRogue10mEquipmentStatModifiers` 6종 장비 증가량 데이터 추가
  - 부위별 현재 장착 Item Data 조회 API 추가
  - Hover 장비 아이콘·이름·설명·무게·장착 증가량 카드 구성
  - 동일 부위 장착 장비 아이콘·이름·설명·현재 증가량 오른쪽 카드 구성
  - `Hover - 현재 장착` 변화량의 증가·감소·동일 색상 비교
  - 한쪽에만 존재하는 스탯을 포함한 합집합 비교
  - 일반 300px·비교 620px Tooltip 동적 폭과 비교 패널 `Collapsed` 처리
  - 스타터 장비 5종 샘플 스탯 Data Asset 저장
  - Tooltip 계층·폭·가시성·장비 스탯 값 Validator 확장
  - 등급별 불투명 아이템 이름 색상 API 및 Tooltip 적용
  - Hover 대상·현재 장착 비교 대상·장착 해제 메뉴 이름 색상 통일
  - `장착 시 증가` 제목 런타임 및 레이아웃 생성 단계에서 숨김
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - WBP_InventoryItemTooltip 재구성·컴파일·저장 성공
  - 전체 메뉴 Widget Blueprint commandlet 0 errors / 0 warnings
  - 스타터 장비 5종 스탯 값 검증 성공
  - Python 문법 및 `git diff --check` 통과
  - 등급 이름 색상 보완 후 Editor UHT/C++ 빌드 및 전체 메뉴 commandlet 재검증 성공
- 상태: 구현·C++ 빌드·WBP/Data Asset 정적 검증 완료, PIE Hover 비교 수동 QA 대기
- 관련 문서: `Feature/architect/2026-07-18_equipment-tooltip-stat-comparison.md`, `Feature/doc/2026-07-18_equipment-tooltip-stat-comparison.md`, `DevLog/20260718.txt`
- 보완 문서: `Feature/architect/2026-07-18_equipment-tooltip-rarity-name.md`, `Feature/doc/2026-07-18_equipment-tooltip-rarity-name.md`

# Sprint#2-5 - ???? ??? ?? ??

- ??: ???? ???? MxN ?? ?? ??????????????? ??? ?? ??? ????.
- ?? ??:
  - ERogue10mItemRarity? ??? 5??? ?? ????? ??
  - Item Data Asset ??? Inventory Background Color ?? ?? ??
  - WBP_InventoryItem? MxN ??? ??? UI_InventoryItemRarityBackground Border ??
  - ?? ?? 0????/Drag Preview 1??? 2 ??? ??
  - ??? Preview Tint? ?? ???? ?? ??? ?? ??
  - Tooltip Soft Class? Widget Blueprint ???? ????? ??
  - ??? 6?? ???????????? ?? ?? ??
  - ?? WBP/Data Asset ?? ????? 5????????? Validator ??
- ??:
  - Rogue10mEditor Win64 Development ?? ??
  - ?? Unreal Editor?? WBP_InventoryItem ?????????? ??
  - ?? ?? Widget Blueprint ?? ??
  - 5?? RGBA ??? ?? ?? ??
  - ??? 6? ?? ?? ?? ??
  - Unreal Editor Python ?? ?? ??
- ??: ???C++ ???WBP/Data Asset ?? ?? ??, PIE ?? ? ???? ?? QA ??
- ?? ??: Feature/architect/2026-07-18_inventory-item-rarity-backgrounds.md, Feature/doc/2026-07-18_inventory-item-rarity-backgrounds.md, DevLog/20260718.txt

# Sprint#2-6 - 장착 장비 Hover 및 장착 해제 메뉴

- 목표: 장비창의 장착 아이템에 Hover 상세 정보와 우클릭 장착 해제 UserWidget을 제공한다.
- 주요 변경:
  - 7개 장비 슬롯 Frame에 기존 장비 상세 Tooltip 연결
  - 이름·설명·무게·6종 장착 증가량 표시
  - 빈 슬롯 Tooltip 제거 및 장비 변경 시 이벤트 기반 갱신
  - URogue10mEquipmentSlotActionWidget과 WBP_EquipmentSlotAction 추가
  - 아이템명·장착 해제 버튼·공간 부족 결과 문구 구성
  - 우클릭 커서 위치와 Viewport 경계 보정
  - 장비창 닫기·장비 변경·좌클릭·성공 시 메뉴 자동 정리
  - 모든 Inventory Container의 MxN 첫 빈 공간 장착 해제 API 추가
  - 공간 부족 시 장비·인벤토리 상태 원상 유지
  - 기존 장비 좌클릭 Drag & Drop 경로 유지
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - WBP_EquipmentSlotAction 생성·컴파일·저장 성공
  - Action 결과 기본 Collapsed 및 필수 계층 검증 성공
  - 장비창 Tooltip/Action Soft Class Reference 검증 성공
  - 첫 빈 Grid 장착 해제 API 노출 검증 성공
  - 새 Unreal commandlet 프로세스 0 errors / 0 warnings
  - Editor Python AST와 git diff --check 통과
- 상태: 구현·C++ 빌드·WBP/API 정적 검증 완료, PIE Hover·우클릭·공간 부족 수동 QA 대기
- 관련 문서: Feature/architect/2026-07-18_equipped-item-hover-unequip-menu.md, Feature/doc/2026-07-18_equipped-item-hover-unequip-menu.md, DevLog/20260718.txt

# Sprint#2-7 - 인벤토리·장비 UI 비율 및 Drag & Drop 보정

- 목표: MxN 아이콘 비율과 Tooltip 크기를 정규화하고 인벤토리→장비 슬롯 Drop 장착 및 커서 중심 장비 Payload를 제공한다.
- 주요 변경:
  - 인벤토리 아이콘 footprint 84% Fit 및 원본 종횡비 유지
  - Texture Match Size 비활성화와 스케일 0.75~1.0 제한
  - 스타터 아이템 6종 InventoryIconScale 1.0 정규화
  - Tooltip 기본 280px·비교 580px·아이콘 52×52px
  - 장비·소비·장착 상태별 우클릭 동작 안내
  - 장비창 GridInventory DragOver·Drop·DragLeave 경로 추가
  - 부위 일치 검사와 호환·비호환 슬롯 Preview
  - 기존 TryEquipGridItem 기반 원자적 장착·교체
  - 장비 Drag Visual CenterCenter Pivot 적용
  - UE 5.8 UMG의 0.15초 Drag Decorator 보간과 전체 화면 Drag Source Geometry를 좌측 상단 비행 원인으로 특정
  - 장비 슬롯 Canvas의 MouseDown Hit-Test 경로에 투명 Drag Source 프록시 7개 사전 배치
  - MouseDown 시 동일 프록시를 Payload MxN 크기의 커서 중심 Layout으로 이동
  - Drop·Drag Cancel·클릭 종료·창 닫기 홈 Layout 복원 및 장비 갱신·Destruct 제거 수명주기 연결
  - 기존 CenterCenter Pivot과 MxN Payload 크기 유지
  - Drop·이탈·창·장비 수명주기 Preview 정리
  - Tooltip WBP 빌더와 전체 메뉴 Validator 확장
- 검증:
  - Rogue10mEditor Win64 Development 빌드 성공
  - 장비 Drag Source 프록시 추가 후 UE 5.8 UHT 통과
  - 수정된 Rogue10mEditor Win64 Development 재빌드 성공
  - 새 UnrealEditor-Cmd 프로세스에서 전체 메뉴 Widget Validator 통과
  - git diff --check 및 CheckGeneratedChanges 통과
  - WBP_InventoryItemTooltip 재생성·컴파일·저장 성공
  - 전체 메뉴 Widget Blueprint 명령형 검증 성공
  - Tooltip 폭·안내 문구 기본 가시성 검증 성공
  - 스타터 아이템 6종 InventoryIconScale 1.0 검증 성공
  - Python 문법 및 git diff --check 통과
- 상태: 구현·빌드·WBP/Data Asset 정적 검증 완료, 에디터 재시작 후 PIE 수동 상호작용 QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-18_inventory-equipment-ui-polish.md`
  - `Feature/doc/2026-07-18_inventory-equipment-ui-polish.md`
  - `DevLog/20260718.txt`

## 2026-07-22 보완 - 장비 장착 해제 컨텍스트 메뉴

- 목표: 장착 장비 우클릭 시 Tooltip 중첩을 제거하고 메뉴 위치 및 외부 클릭 닫기 동작을 개선한다.
- 주요 변경:
  - 활성 Hover Tooltip 즉시 닫기 및 메뉴 종료 시 슬롯 Tooltip 복원
  - 커서 오른쪽 12px 메뉴 배치와 Viewport 경계 보정
  - 전체 화면 투명 dismiss 버튼으로 메뉴 외부 첫 클릭 소비 및 닫기
  - Action WBP 생성 스크립트와 Validator 필수 바인딩 갱신
- 검증: Rogue10mEditor 빌드 성공, 전체 메뉴 Widget Validator 0 errors / 0 warnings, Python 문법 및 diff 검사 통과
- 상태: 구현·빌드·WBP 정적 검증 완료, PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-22_equipment-context-menu-dismiss.md`, `Feature/doc/2026-07-22_equipment-context-menu-dismiss.md`, `DevLog/20260722.txt`

## 2026-07-23 보완 - 메뉴 Python wrapper 정리

- 목표: 메뉴 Widget 자동화의 중복 진입점을 제거하고 통합 빌더·Validator 중심으로 유지보수 경로를 단순화한다.
- 주요 변경:
  - `BuildMenuDesignerLayouts.py`로 대체된 부분 생성·재생성 wrapper 6개 삭제
  - 메뉴 생성은 `BuildMenuDesignerLayouts.py`, 회귀 검증은 `ValidateMenuWidgetAssets.py`로 통합
  - Data Asset·콘텐츠 이동·경고 검증·원격 실행용 재사용 스크립트는 유지
- 검증: 남은 Python 전체 문법 검사, 삭제 파일 참조 검색, `git diff --check`, `CheckGeneratedChanges.ps1`
- 상태: 스크립트 정리 및 정적 검증 완료
- 관련 문서: `DevLog/20260723.txt`

## 2026-07-23 보완 - HUD Quick Slot USTRUCT 초기화

- 목표: Editor Automation에서 반복된 `FRogue10mHudQuickSlotView::InputSlot` 미초기화 오류를 제거한다.
- 주요 변경: `InputSlot` 기본값을 `ERogue10mAttackInputSlot::Primary`로 지정
- 원인 분리:
  - `r.MotionVectorSimulation`: UE 5.8 엔진 CVar 플래그 문제로 프로젝트 변경 없음
  - 단독 `Condition failed` 15건: UE 5.8 엔진 테스트 로그로 프로젝트 오류와 무관
- 검증: Rogue10mEditor 빌드 성공, `UObject.Class AttemptToFindUninitializedScriptStructMembers` Automation 테스트 성공, 관련 LogClass 오류 0건
- 상태: 수정 및 자동 검증 완료
- 관련 문서: `DevLog/20260723.txt`

## 2026-07-23 보완 - 시작 인벤토리 추가 갑옷

- 목표: 시작 장비 자동 장착을 유지하면서 인벤토리에 가죽 갑옷 한 개를 추가로 제공한다.
- 주요 변경: `DA_Item_LeatherArmor` 시작 참조 추가, 동일 장비 부위의 후속 자동 장착 방지
- 검증: Rogue10mEditor 빌드 성공, `CheckGeneratedChanges.ps1` 및 `git diff --check` 통과
- 상태: 구현 및 자동 검증 완료, 새 PIE 세션 수동 확인 대기
- 관련 문서: `Feature/architect/2026-07-23_starter-inventory-extra-armor.md`, `Feature/doc/2026-07-23_starter-inventory-extra-armor.md`, `DevLog/20260723.txt`

## 2026-07-23 보완 - 에픽 수호자 투구

- 목표: 에픽 등급 Head 장비 Data Asset을 제작하고 시작 인벤토리에 추가한다.
- 주요 변경: `DA_Item_GuardianHelmet` 생성, 방어력 +10·최대 체력 +20 적용, 시작 아이템 참조 및 자동 검증 목록 추가
- 에셋 정책: 기존 철제 투구 아이콘 재사용, 불필요한 기존 스타터 에셋 재저장 변경 제거
- 검증: Rogue10mEditor 빌드 성공, UnrealEditor-Cmd 생성 검증 및 전체 메뉴·아이템 Validator 통과, 생성 파일·diff 검사 통과
- 상태: 구현 및 자동 검증 완료, 새 PIE 세션 수동 확인 대기
- 관련 문서: `Feature/architect/2026-07-23_epic-guardian-helmet.md`, `Feature/doc/2026-07-23_epic-guardian-helmet.md`, `DevLog/20260723.txt`

# Sprint#2-8 - 캐릭터 기본 스탯 및 장비창 표시

- 목표: 캐릭터 기본 스탯과 장비 보너스의 소유 구조를 확립하고 최종값을 전투·생존·이동 및 장비창에 일관되게 반영한다.
- 주요 변경:
  - Character Data Asset에 체력·자원·공격력·방어력·치명타·공격 속도·이동 기본값 정의
  - 기본·장비·최종값을 공유하는 Character Stat 구조체 추가
  - 장착 장비 전체의 6종 보너스 합산 API 추가
  - 장비 변경 성공 경로를 단일 스탯 갱신 진입점으로 통합
  - 공격력의 스킬 피해 반영과 방어력의 정액 피해 감소 적용
  - 최대 체력 변경 시 현재 체력 비율 보존
  - 이동 속도와 질주 속도를 CharacterMovement에 반영
  - 장비창 6종 스탯을 `최종 (기본 + 장비)` 형식으로 표시
  - 기본 Character Data 전용 설정 모드와 Validator 추가
- 검증:
  - Rogue10mEditor Win64 Development 전체 빌드 성공
  - 리뷰 보완 후 Rogue10mCharacter 증분 재빌드 성공
  - UnrealEditor-Cmd에서 기본 스탯 10종과 장비창 바인딩 6종 검증 성공
  - 기존 스타터 장비 6종 능력치 회귀 검증 성공
  - `UObject.Class AttemptToFindUninitializedScriptStructMembers` Automation 테스트 성공
  - Python 문법, `CheckGeneratedChanges.ps1`, `git diff --check` 검사
- 상태: 구현·빌드·에셋 정적 검증 완료, 새 PIE 세션 장착·해제·피해·이동 수동 QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-23_character-base-stats-equipment-window.md`
  - `Feature/doc/2026-07-23_character-base-stats-equipment-window.md`
  - `Docs/CharacterDataOwnership.md`
  - `DevLog/20260723.txt`

## 2026-07-25 보완 - 장비창 Scene Capture 중복 갱신 제거

- 목표: 실시간 캐릭터 프리뷰는 유지하면서 자동·수동 Scene Capture의 중복 렌더 경고를 제거한다.
- 원인: `bCaptureEveryFrame=true` 상태에서 `SetPreviewActive()`와 장비 변경 경로가 수동 `CaptureScene()`도 호출
- 주요 변경: 활성 프리뷰는 자동 캡처만 사용하고 수동 캡처는 `bCaptureEveryFrame=false`일 때만 허용
- 검증: Rogue10mEditor Win64 Development 빌드 성공, 수동 캡처 보호 조건, 생성물·diff 검사
- 상태: 코드 및 빌드 검증 완료, 에디터 재시작 후 PIE 경고 재발 여부 수동 확인 대기
- 관련 문서: `Feature/doc/2026-07-16_equipment-character-preview.md`, `DevLog/20260725.txt`

## 2026-07-25 보완 - 장비창 Preview Character 좌클릭 회전

- 목표: 장비창 프리뷰에서 좌클릭 가로 드래그로 캐릭터를 좌우 회전한다.
- 주요 변경: 메시 전용 Pivot, 프리뷰 영역 판정, 마우스 캡처 기반 드래그, 0.35°/px 감도 설정, 캡처 손실 정리
- 입력 호환: 타이틀 바 창 이동, 장비 슬롯 Drag & Drop, 우클릭 장착 해제 경로 유지
- 렌더 호환: 실시간 Scene Capture 유지, 카메라·조명 고정, 자동·수동 중복 캡처 보호 유지
- 검증: UE 5.8 UHT 및 Rogue10mEditor 빌드 성공, 생성물·diff 검사
- 상태: 구현·빌드·정적 검증 완료, 에디터 재시작 후 PIE 수동 상호작용 QA 대기
- 관련 문서: `Feature/architect/2026-07-25_equipment-preview-mouse-rotation.md`, `Feature/doc/2026-07-25_equipment-preview-mouse-rotation.md`, `DevLog/20260725.txt`

# Sprint#2-9 - 몬스터 경험치 보상 및 25종 로스터

- 목표: 몬스터 처치 경험치를 플레이어 성장에 연결하고 Data Asset 기반 25종 로스터를 준비한다.
- 주요 변경: 서버 권한 마지막 공격자 경험치 지급, 중복 지급 방지, 전투 로그, MonsterRank 및 공격 Fallback 수치 추가
- 콘텐츠: 일반 20종, 중간 보스 4종, 최종 보스 1종 Data Asset 생성
- 밸런스: 일반 경험치 18~140, 중간 보스 300~900, 최종 보스 3,000
- 검증: UE 5.8 UHT 및 Rogue10mEditor 빌드 성공, Unreal Python Validator 25종·20/4/1 통과, 생성물·diff 검사
- 상태: 구현·에셋 생성·빌드·정적 검증 완료, 에디터 재시작 후 PIE 처치 보상 수동 QA 대기
- 관련 문서: `Feature/architect/2026-07-25_monster-experience-roster.md`, `Feature/doc/2026-07-25_monster-experience-roster.md`, `DevLog/20260725.txt`

# Sprint#2-10 - 캐릭터 커스터마이징·생성·접속

- 목표: 게임 시작 시 3슬롯에서 캐릭터를 생성·선택하고 해당 외형으로 접속하는 로비 흐름을 제공한다.
- 주요 변경: 인간·드워프·오크 남녀 6개 아키타입 카탈로그, 이름·외형 SaveGame, 생성·선택·삭제·접속 UI, 좌클릭 회전 프리뷰 추가
- 런타임 적용: 선택 프로필을 플레이어 메시와 PlayerState 표시에 적용하고 기존 1인칭 팔·전투·장비창 프리뷰 흐름 유지
- 저장 경계: 이름·외형·선택 슬롯만 저장하며 인벤토리·경험치·스탯·월드 진행도는 후속 캐릭터 저장 기능으로 분리
- 검증: UE 5.8 UHT 및 Rogue10mEditor 빌드 성공, Unreal Python Validator에서 6개 아키타입·필수 에셋·위젯 바인딩 통과, 생성물·diff 검사
- 상태: 구현·에셋 생성·빌드·정적 검증 완료, 에디터 재시작 후 PIE 생성·저장·접속·애니메이션 수동 QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-25_character-customization-lobby.md`
  - `Feature/doc/2026-07-25_character-customization-lobby.md`
  - `Docs/CharacterDataOwnership.md`
  - `DevLog/20260725.txt`

## 2026-07-26 보완 - 캐릭터 선택 후 접속 실패

- 증상: 슬롯 선택 후 `게임 접속`을 눌러도 로비가 닫히지 않고 외형 적용 실패 로그가 반복됨
- 원인: `BP_FirstPersonCharacter` CDO의 `CustomizationCatalog` 참조 누락
- 주요 변경: Character Blueprint 카탈로그 기본값 저장, C++ 기본 경로 fallback, 로비 포커스 활성화
- 회귀 방지: 에셋 생성 스크립트에서 Character CDO 참조 설정, Validator에서 메시 컴포넌트와 카탈로그 CDO 검사
- 검증: 수정 전 Validator 오류 재현, 수정 후 6개 아키타입·로비·Player Character CDO 통과, UE 5.8 Editor 빌드 성공
- 상태: 원인 수정·에셋 저장·빌드·정적 검증 완료, 에디터 재시작 후 기존 슬롯 접속 PIE QA 대기
- 관련 문서: `Feature/doc/2026-07-25_character-customization-lobby.md`, `DevLog/20260726.txt`

## 2026-07-26 보완 - 상속 기반 캐릭터 외형 및 종족별 리타기팅

- 목표: 캐릭터 생성 후 입장 시 서로 다른 Skeleton의 Leader Pose 연결로 발생하는 머리·몸·팔 왜곡을 구조적으로 제거한다.
- 주요 변경:
  - `ARogue10mStylizedCharacter` 공통 외형 부모와 Human·Dwarf·Orc 남녀 6개 자식 Character Blueprint 추가
  - 숨김 Manny `AnimationSourceMesh`와 종족 전신 `Character Mesh`를 부모·자식으로 구성
  - 종족별 프로젝트 로컬 Target IK Rig, IK Retargeter, `Retarget Pose From Mesh` AnimBP 추가
  - 카탈로그 Archetype에 CharacterClass·Retargeter·RetargetAnimClass 연결
  - GameMode 선택 클래스 결정 및 실패 안전 Spawn/Possess 교체 흐름 추가
  - 서로 다른 Skeleton Leader Pose 제거, 동일 Skeleton Hair·Facial 파츠에만 Leader Pose 적용
  - 원본 Orc Male IK Rig의 무효 `Cape` 체인은 프로젝트 로컬 복제본에서 제거
- 검증:
  - Rogue10mEditor Win64 Development 전체 빌드 성공
  - 상속/IK/AnimGraph Validator 오류 0건, 경고 0건
  - 기존 캐릭터 로비/카탈로그 Validator 오류 0건, 경고 0건
  - `CheckGeneratedChanges.ps1` 통과
- 상태: 구현·에셋 생성·정적 검증 완료, 에디터 재시작 후 6조합 이동·공격·장비창 Preview 수동 PIE QA 필요
- 관련 문서:
  - `Feature/architect/2026-07-26_inherited-character-appearance.md`
  - `Feature/doc/2026-07-26_inherited-character-appearance.md`
  - `Docs/CharacterDataOwnership.md`
  - `DevLog/20260726.txt`
## 2026-07-26 보완 - 상속 CharacterClass 접속 실패 수정

- 목표: 선택 프로필의 자식 CharacterClass를 찾지 못해 게임 접속이 중단되는 문제 해결
- 주요 변경: 공통 외형 부모 BeginPlay 조기 적용 제거, 카탈로그 고정 경로 fallback, 종족·성별 자식 Blueprint class fallback 추가
- 검증: Editor 빌드 성공, 상속 Validator 통과, 실제 PIE에서 Human Male 자식 Pawn Spawn/Possess 및 EnterSelectedCharacter 후 로비 종료 확인, 최근 오류 로그 0건
- 상태: 수정 및 자동 PIE 검증 완료
- 관련 문서: `Feature/doc/2026-07-26_inherited-character-appearance.md`, `DevLog/20260726.txt`

# Sprint#3-2 - 몬스터 영역 스포너

- 브랜치: `Sprint#3-2-monster-area-spawner`
- 목표: 지정한 Box 범위 안에서 특정 몬스터를 최대 N마리까지 생성하고 선택적으로 생존 수를 유지한다.
- 주요 변경:
  - `ARogue10mMonsterSpawner` World Actor 추가
  - Monster Class, Box 범위, 최대 생존 수, 생성 간격 에디터 설정
  - 시작 즉시 최대 수 충원과 지속 보충·일회성 생성 선택
  - 지면 트레이스와 충돌 안전 생성
  - Tick 없는 타이머, 약한 참조, 파괴 Delegate 기반 생존 수 추적
  - Authority 전용 생성과 Blueprint 수동 제어 API
- 검증:
  - UHT 통과
  - `Rogue10mMonsterSpawner.cpp` UBT 단일 파일 컴파일 성공
  - UI Preview Actor의 Unity 상수 재정의를 기능별 접두사로 수정
  - `Rogue10mEditor` Win64 Development 전체 빌드 성공
  - `CheckGeneratedChanges.ps1`, `git diff --check`
- 상태: C++ 구현 및 전체 Editor 빌드 검증 완료, 레벨 배치 PIE QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-28_monster-area-spawner.md`
  - `Feature/doc/2026-07-28_monster-area-spawner.md`
  - `DevLog/20260728.txt`

## Sprint#3-3 - 모험가 기본 직업과 스폰 후 맨손 공격

- 목표: 캐릭터 선택 후 스폰·Possess된 Pawn에 기본 직업 모험가와 좌클릭 맨손 Primary 공격을 안정적으로 부여한다.
- 주요 변경: CharacterData 직업 필드, PlayerState 모험가 기본값, PossessedBy/OnRep 기반 로드아웃 재초기화, 종족명과 직업명 분리
- 에셋: `DA_Character_Default`를 Unarmed 기본 무기로 변경하고 Unarmed Profile·Primary 주먹 공격·BP CombatComponent 참조를 검증 및 저장
- 빌드 보완: UE Unity 빌드의 Preview Actor 익명 namespace 상수 재정의 충돌 제거
- 검증: UE 5.8 Rogue10mEditor 전체 빌드 성공, 모험가/Unarmed Editor Python Validator 통과, 6개 상속 Character 및 Retarget AnimBP Validator 통과
- 상태: 구현·에셋 설정·빌드·정적 검증 완료, 에디터 재시작 후 실제 좌클릭 PIE 체감 QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-28_adventurer-unarmed-spawn.md`
  - `Feature/doc/2026-07-28_adventurer-unarmed-spawn.md`
  - `Docs/CharacterDataOwnership.md`
  - `DevLog/20260728.txt`
## Sprint#3-4 - 3인 캐릭터 선택 무대

- 브랜치: `Sprint#3-4-character-selection-stage`
- 목표: 기존 캐릭터 로비 기능을 유지하면서 저장된 최대 3명의 캐릭터를 판타지 배경 앞에 동시에 표시한다.
- 주요 변경:
  - CharacterLobbyWidget의 Draft Preview 1개와 Slot Preview 3개 수명주기
  - 숨김 Manny Idle Source Mesh와 6개 종족·성별 Retarget AnimBP 적용
  - 선택 캐릭터 명도·슬롯 카드 강조와 각 프리뷰 좌클릭 회전
  - 오리지널 16:9 판타지 성채 배경과 1920×1080 WBP 무대형 레이아웃
  - 기존 생성·삭제·선택·게임 접속·외형 편집 BindWidget 유지
  - 빈 슬롯 캡처 비활성화와 로비 종료 시 Preview Actor 정리
- 검증:
  - UE 5.8 UHT 및 Rogue10mEditor Win64 Development 전체 빌드 성공
  - 기존 캐릭터 커스터마이징 Validator 통과
  - 신규 3인 선택 무대 Validator 통과
  - 최종 WBP 재로드 시 Blueprint/Python 오류 없음
- 상태: 구현·배경·WBP·빌드·에셋 정적 검증 완료, 에디터 PIE 육안 QA 대기
- 관련 문서:
  - `Feature/architect/2026-07-28_character-selection-stage.md`
  - `Feature/doc/2026-07-28_character-selection-stage.md`
  - `DevLog/20260728.txt`

### Sprint#3-4 보완 - Main Menu / Lobby / Component 경로 분리

- 목표: 캐릭터 로비 비율·상호작용을 보완하고 기본 메인 메뉴에서 로비와 인게임으로 이어지는 화면 흐름 구성
- 주요 변경:
  - `Content/Widget/Lobby/WBP_CharacterLobby`로 로비 이동
  - 기존 공용 메뉴 위젯을 `Content/Widget/Component`로 이동
  - `Content/Widget/Menu/WBP_MainMenu` 신규 생성 및 게임 시작·종료 연결
  - Slot Preview 폭 360px 제한, `UI_SelectedCharacterInfoText` 제거, 하단 선택/생성 버튼 배치
  - 캐릭터/슬롯 정보 더블클릭 접속과 빈 슬롯 캐릭터 생성 지원
  - PlayerController 흐름을 `Main Menu → Lobby → 현재 Start Map`으로 변경
- 검증: UE 5.8 Rogue10mEditor 빌드 성공, Widget Flow/Selection Stage/Customization/Inherited Character 검증 통과
- 상태: 구현·에셋 이동·자동 검증 완료, PIE 수동 체감 QA 대기
- 후속: `Start Map → World Partition Cell 오픈월드` 설계 및 구현
- 관련 문서:
  - `Feature/architect/2026-07-28_character-selection-stage.md`
  - `Feature/doc/2026-07-28_character-selection-stage.md`
  - `Docs/CurrentProjectArchitecture.md`
  - `DevLog/20260728.txt`
## Sprint#3-5 - 몬스터 Behavior Tree 전투 AI

- 브랜치: `Sprint#3-5-monster-behavior-tree-ai`
- 목표: 몬스터 이동을 홈·순찰·추격 범위로 제한하고 거리 감지 또는 피격 조건에서만 플레이어와 전투
- 주요 변경:
  - Tick 추적 제거, AIController + AI Perception + Blackboard + Behavior Tree + NavigationSystem 적용
  - Sight/피격 어그로, 기억 시간, 최대 추격 거리, 홈 복귀, 순찰 도착 대기 구현
  - 공통 `BB_Monster`, `BT_Monster`와 런타임 안전 대체 트리 추가
  - 25개 Monster Data Asset에 BT·거리 설정 연결
  - BeginPlay 데이터 적용 후 AI 재초기화로 UE Possess 순서 대응
  - 로비 `UI_StatusText` 제거, 3개 프리뷰 정면 배치, 16:9 배경 채움
- 검증:
  - UE 5.8 Rogue10mEditor Win64 Development 빌드 성공
  - Monster Behavior Tree AI Validator 통과(Blackboard 8키, 2 Task, 25 Data Asset)
  - Character Selection Stage Validator 통과
- 상태: 코드·에셋·자동 검증 완료, NavMesh 레벨에서 순찰·피격 어그로·추격 해제 수동 PIE QA 필요
- 관련 문서:
  - `Feature/architect/2026-07-28_monster-behavior-tree-ai.md`
  - `Feature/doc/2026-07-28_monster-behavior-tree-ai.md`
  - `Docs/CurrentProjectArchitecture.md`
  - `DevLog/20260728.txt`
### Sprint#3-5 보완 - Main Menu UIOnly 포커스 오류

- 문제: `FInputModeUIOnly`가 포커스 불가능한 Main Menu `SObjectWidget`을 대상으로 지정해 PlayerController 오류 로그 발생
- 수정: `URogue10mMainMenuWidget::NativeConstruct()`에서 Main Menu 루트를 포커스 가능하게 설정
- 검증: UE 5.8 Rogue10mEditor Win64 Development 빌드 성공
- 상태: 코드·빌드 검증 완료, PIE에서 오류 로그 미발생 확인 필요
- 관련 기록: `DevLog/20260729.txt`
### Sprint#3-5 보완 - 캐릭터 선택 슬롯 및 프리뷰 복구

- 문제: BottomBar가 버튼 입력을 차단하고, 일시정지 중 SceneCapture 갱신 실패로 Slot Preview가 흰색으로 표시됨
- 수정: 버튼 ZOrder 8, 장식/프리뷰 Hit Test 비활성, 조명·메시 활성화 후 수동 Capture, Pause 중 메시·Capture 갱신
- UI: Lobby Background Image 유지, 전체 화면 Stretch 앵커와 0 오프셋 적용
- 에디터 적용: 공식 Unreal Python Remote Execution으로 현재 PIE 종료 후 WBP 재구성
- 검증: UE 5.8 Editor 빌드 성공, 강화된 Character Selection Stage Validator 통과
- 상태: 코드·WBP·자동 검증 완료, 에디터 재시작 후 PIE에서 실제 프리뷰와 클릭 체감 확인 필요
- 관련 문서: `Feature/doc/2026-07-28_character-selection-stage.md`, `DevLog/20260729.txt`
### Sprint#3-5 보완 - 캐릭터 프리뷰 소스 로드 및 WASD 입력 복구

- 문제: Manny 프리뷰 메시의 불완전한 Soft Object Path로 프리뷰 초기화 실패
- 문제: Main Menu와 Lobby에서 이동·시점 입력 차단이 중복 누적되어 접속 후에도 WASD 비활성
- 수정: 메시 경로를 `.SKM_Manny_Simple`까지 포함한 전체 오브젝트 경로로 변경
- 수정: UI 진입 시 입력 차단 중복 방지, 접속 시 `ResetIgnoreMoveInput`/`ResetIgnoreLookInput` 적용
- 진단: 접속 직후 입력 차단 및 일시정지 상태 로그 추가
- 검증: UE 5.8 Rogue10mEditor Win64 Development 빌드 성공
- 상태: 코드·빌드 검증 완료, 에디터 재시작 후 PIE 프리뷰/WASD 체감 QA 필요
- 관련 문서: `Feature/doc/2026-07-28_character-selection-stage.md`, `DevLog/20260729.txt`
## Sprint#3-6 - Menu Map 기반 UI와 인게임 전환

- 브랜치: `Sprint#3-6-menu-map-flow`
- 목표: 메뉴·캐릭터 선택을 별도 맵으로 분리하고 선택 캐릭터로 인게임 맵에 진입
- 주요 변경:
  - `/Game/Rogue10m/Maps/L_Menu` 신규 생성
  - `Rogue10mMenuGameMode`, `Rogue10mMenuPlayerController` 추가
  - PlayerController의 Menu World / Gameplay World 초기화 분리
  - 선택 프로필 저장 후 `Lvl_FirstPerson?StartRun=1` OpenLevel
  - `EditorStartupMap`, `GameDefaultMap`을 `L_Menu`로 변경
  - Main Menu 전체 화면 Stretch 및 제목·버튼 확대
- 검증:
  - UE 5.8 Editor 빌드 성공
  - Menu Map Flow Validator 통과
  - Widget Flow Validator 통과
  - 실제 PIE에서 `L_Menu → Lobby → Lvl_FirstPerson` 이동 성공
  - 선택 Pawn Spawn/Possess, 이동·시점 입력 및 Pause 해제 확인
- 상태: 구현·맵 자산·설정·자동 PIE 검증 완료
- 관련 문서:
  - `Feature/architect/2026-07-29_menu-map-flow.md`
  - `Feature/doc/2026-07-29_menu-map-flow.md`
  - `DevLog/20260729.txt`

## Sprint#3-6 - 로비 캐릭터 프리뷰 정면·동일 크기·투명 배경

- 목표: 캐릭터 선택 로비에서 세 캐릭터를 동일한 크기의 정면 고정 프리뷰로 표시하고 배경 위에 캐릭터만 합성
- 주요 변경:
  - 프리뷰 Yaw -90도 고정
  - 세 Slot Preview Y=15, 360×720 통일
  - SceneColor HDR/Inverse Opacity + UI Translucent 합성
  - M_CharacterPreviewTransparent 추가
  - Lobby 배경 Shade 및 Slot Stage Glow 제거
  - Lobby 드래그 회전 제거, Equipment 회전 유지
- 검증:
  - UE 5.8 Rogue10mEditor 빌드 성공
  - Character Selection Stage Validator 오류 0건
  - Character Lobby Flow Suite 오류 0건
- 상태: 구현 및 자동 검증 완료
- 관련 문서:
  - Feature/architect/2026-07-28_character-selection-stage.md
  - Feature/doc/2026-07-28_character-selection-stage.md

- 추가 런타임 검증: Menu Map PIE에서 Lobby 표시 및 프리뷰 액터 4개 생성, 관련 로드 오류 0건

## Sprint#4-1 - 로비 프리뷰-슬롯 선택 동기화

- 브랜치: `Sprint#4-1-lobby-preview-slot-selection`
- 목표: 로비의 캐릭터 프리뷰 클릭과 하단 캐릭터 슬롯 선택 상태를 일치시킨다.
- 주요 변경:
  - 프리뷰 단일 좌클릭 입력 처리 추가
  - 프리뷰 화면 좌표를 슬롯 인덱스로 판정
  - 프리뷰와 하단 슬롯 버튼이 기존 `SelectSlot()` 경로 공유
  - 프리뷰 더블클릭 즉시 접속 기능 유지
  - 생성 화면에서는 슬롯 선택 입력 제외
- 검증:
  - UE 5.8 Rogue10mEditor Win64 Development 빌드 성공
  - `Scripts/CheckGeneratedChanges.ps1` 통과
  - `git diff --check` 통과
- 상태: 코드 및 자동 검증 완료, Menu Map PIE 수동 클릭 QA 대기
- 관련 문서:
  - `Feature/architect/2026-08-04_lobby-preview-slot-selection.md`
  - `Feature/doc/2026-08-04_lobby-preview-slot-selection.md`
  - `DevLog/20260804.txt`
