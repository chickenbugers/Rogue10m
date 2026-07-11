# 공격 공간 형태 및 피해 방식 분리 설계

## 목표

공격을 공간 판정과 피해 적용 방식의 두 축으로 분리한다. 플레이어와 몬스터가 같은 `AttackSkillData` 정의를 사용할 수 있도록 데이터 구조를 일반화한다.

## Ultrawork Packets

### Packet 1 — 데이터 스키마

- 목표: 공간 형태와 피해 방식을 독립 Enum으로 정의
- 수정 위치: `Rogue10mAttackSkillData`
- 완료 조건: Data Asset에서 형태별 수치와 피해 방식을 선택 가능
- 검증: UHT 및 Editor 빌드
- 롤백 경계: Data Asset 헤더

### Packet 2 — 공간 판정

- 목표: 직선 박스, 투사체 경로, 부채꼴, 원형 판정 구현
- 수정 위치: `Rogue10mCombatComponent`
- 완료 조건: 선택한 형태에 따라 후보 대상 수집 및 디버그 표시
- 검증: Editor 빌드 및 코드 리뷰
- 롤백 경계: CombatComponent 판정 함수

### Packet 3 — 피해 방식

- 목표: 단타, 연속 공격, 적중 후 다단히트의 실행 의미 분리
- 수정 위치: `Rogue10mCombatComponent`
- 완료 조건:
  - 단타: 최초 펄스에서 대상당 1회
  - 연속 공격: 매 펄스 공간을 재판정하고 매번 피해
  - 다단히트: 최초 적중 대상을 고정하고 정해진 횟수만큼 연속 피해
- 검증: 타이머 종료 및 약한 참조 안전성 리뷰
- 롤백 경계: 공격 실행 상태 구조체

### Packet 4 — 문서화

- 목표: 디자이너 설정 규칙과 마이그레이션 기록
- 수정 위치: `Feature/doc`, `Docs/AttackSkillDataAssetGuide.md`, `DevLog/20260712.txt`
- 완료 조건: 조합별 설정 예시 제공
- 검증: `CheckGeneratedChanges.ps1`, `git diff --check`

## 데이터 모델

### 공간 형태

- `LinearBox`: 카메라 전방으로 뻗는 박스
- `Projectile`: 공격 펄스마다 전진 구간을 판정하는 투사체 경로
- `Arc`: 전방 원뿔 각도 안의 대상
- `Circle`: 공격자 또는 지정 전방 오프셋 중심의 원형 범위

### 피해 방식

- `Single`: 공간 판정 1회, 대상당 피해 1회
- `Continuous`: `HitCount` 동안 공간을 반복 판정하며 현재 범위의 대상에게 매번 피해
- `MultiHit`: 첫 판정에서 맞은 대상을 고정하고 `HitCount`만큼 연속 피해

## 성능 원칙

- Tick을 사용하지 않고 기존 TimerManager를 유지한다.
- 실행 상태에는 `TWeakObjectPtr`만 저장한다.
- 펄스당 대상 수를 `MaxTargetsPerHit`로 제한한다.
- 한 펄스 내 동일 액터를 중복 적용하지 않는다.
- 투사체도 공격별 Actor를 생성하지 않는 경량 이동 판정으로 제공한다. 실제 비주얼 투사체가 필요한 스킬은 이후 전용 Projectile Actor로 교체할 수 있다.
