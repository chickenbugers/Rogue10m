# 플레이어 스탯·애니메이션·화면 피드백 설계

## 목표

- `Rogue10mCharacter`, `Rogue10mPlayerState`, GAS `AttributeSet`, 전용 컴포넌트의 책임 경계를 검증한다.
- 공격속도 스탯이 공격 몽타주와 공격 타이밍에 일관되게 반영되도록 한다.
- 피해와 스테미나 부족을 에셋 의존 없이 포스트프로세스로 전달한다.

## 소유권 검증 결과

- `ARogue10mPlayerState`: 사망 상태, 장비 상태, 성장/전투 수치의 지속 소유자 역할이 적절하다.
- `URogue10mAttributeSet`: 체력·스테미나·마나·아이덴티티·성장 수치의 단일 원본 역할이 적절하다.
- `URogue10mCombatComponent`: 입력 해석 이후 공격 실행, 자원 비용, 콤보, 쿨다운 소유가 적절하다.
- `ARogue10mCharacter`: 입력 전달, 컴포넌트 조립, Pawn 수명주기, 피해 진입점만 유지한다.
- 보완 필요: Attribute 복제 알림, 공격속도 수치, 화면 피드백 책임의 전용 컴포넌트가 없다.

## Ultrawork Packets

### Packet 1 — 스탯 소유권과 복제

- 목표: PlayerState의 AttributeSet을 단일 수치 원본으로 유지하고 GAS 복제를 완성한다.
- 수정 위치: `Source/Rogue10m/Ability/Rogue10mAttributeSet.*`
- 완료 조건: 모든 Attribute가 `ReplicatedUsing`과 GAS rep-notify를 가지며 공격속도 배율이 추가된다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: AttributeSet 변경만 되돌릴 수 있다.

### Packet 2 — 공격속도 기반 애니메이션/타이밍

- 목표: 공격속도 배율을 몽타주, 콤보 창, 공유 쿨다운에 동일 적용한다.
- 수정 위치: `Source/Rogue10m/Components/Rogue10mCombatComponent.*`
- 완료 조건: 1.0은 기존 동작, 2.0은 재생 및 타이밍이 2배 빠르게 동작한다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: CombatComponent의 배율 계산 변경만 되돌릴 수 있다.

### Packet 3 — 포스트프로세스 화면 피드백

- 목표: 피해 펄스, 낮은 스테미나 지속 효과, 스테미나 부족 펄스를 구현한다.
- 수정 위치: `Source/Rogue10m/Components/Rogue10mPlayerFeedbackComponent.*`, Character/Combat 연결부
- 완료 조건: 피해 시 적색 비네트/색수차, 낮은 스테미나 시 탈채도/비네트, 비용 부족 시 강화 펄스가 보인다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 피드백 컴포넌트와 두 연결 호출만 되돌릴 수 있다.

### Packet 4 — 리뷰와 문서화

- 목표: Unreal 규칙, 생성물 오염, 책임 경계를 재검토하고 결과를 기록한다.
- 수정 위치: `Feature/doc/`, `DevLog/20260711.txt`
- 완료 조건: 빌드/생성물 검사 결과와 에디터 조정 항목이 기록된다.
- 검증 명령: `Scripts/CheckGeneratedChanges.ps1`
- 롤백 경계: 문서 변경만 되돌릴 수 있다.

## 구현 원칙

- 새 포스트프로세스 컴포넌트는 상시 Tick을 사용하지 않는다.
- 수치 변경은 GAS 델리게이트로 받고, 펄스 감쇠 중에만 짧은 타이머를 사용한다.
- C++ 기본값만으로 동작하며 Blueprint에서는 강도·색·임계값만 조정한다.
- `.uasset`과 `.umap`은 수정하지 않는다.
