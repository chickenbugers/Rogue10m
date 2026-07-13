# Sprint 변경 이력

이 문서는 Rogue10m의 Sprint별 주요 변경 사항과 검증 결과를 요약한다.
세부 설계와 구현 결과는 Feature 문서, 일자별 작업 과정은 DevLog를 참조한다.

## 기록 규칙

각 작업은 다음 형식으로 누적한다.

- 작업 식별자: Sprint#스프린트번호-작업번호
- 브랜치: Sprint#스프린트번호-작업번호-간단한-영문-설명
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