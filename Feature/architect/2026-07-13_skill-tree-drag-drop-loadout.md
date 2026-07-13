# 스킬트리 Drag and Drop과 무기별 기본 회피 설계

## 목표

- CombatComponent 생성자에 고정된 스킬 경로 의존성을 Data Asset 프로필 참조로 대체한다.
- 스킬트리에서 해금된 스킬을 공격 입력 슬롯으로 Drag and Drop 장착한다.
- 회피기는 무기/직업 프로필 변경 시 자동 바인딩한다.
- 기본 회피 이동 거리는 1m로 정의한다.

## Ultrawork Packets

### Packet 1 — Data Asset 스키마

- 목표: 무기별 스킬 프로필과 회피 Data Asset 정의
- 수정 위치: Source/Rogue10m/Data
- 완료 조건: 프로필에서 기본 공격, 스킬트리 후보, 기본 회피를 설정 가능
- 검증: UnrealHeaderTool 및 Editor 빌드
- 롤백 경계: 신규 Data Asset 타입 파일

### Packet 2 — Combat Loadout

- 목표: 입력 슬롯별 장착 스킬과 무기 프로필 적용
- 수정 위치: Rogue10mCombatComponent
- 완료 조건: 장착/해제/조회/실행이 런타임 Loadout을 참조
- 검증: Editor 빌드와 정적 코드 검토
- 롤백 경계: CombatComponent API와 런타임 배열

### Packet 3 — Skill Tree Drag and Drop

- 목표: 스킬트리 엔트리에서 HUD 스킬 슬롯으로 Drag and Drop
- 수정 위치: UI/Widgets
- 완료 조건: 해금된 스킬만 드래그되며 드롭 대상 입력 슬롯에 장착
- 검증: Editor 빌드 및 UMG 설정 체크리스트
- 롤백 경계: DragDropOperation과 Entry Widget 클래스

### Packet 4 — 무기별 Dodge

- 목표: E 회피가 현재 무기 프로필의 Dodge Data Asset을 자동 참조
- 수정 위치: Character, CombatComponent
- 완료 조건: 기본 100cm 이동, 쿨다운/지속시간/스테미나 비용 데이터화
- 검증: Editor 빌드 및 공중 회피 차단 유지
- 롤백 경계: Character DoDodge의 데이터 조회 구간

### Packet 5 — 문서와 Sprint 기록

- 목표: Data Asset 생성/참조 위치와 UMG 배치 방법 문서화
- 수정 위치: Docs, Feature/doc, DevLog, SprintChangeLog
- 완료 조건: 에디터 작업자가 추가 설정을 재현 가능
- 검증: CheckGeneratedChanges와 diff check
- 롤백 경계: 이번 작업 문서