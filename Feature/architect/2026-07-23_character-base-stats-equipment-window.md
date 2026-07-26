# 캐릭터 기본 스탯 및 장비창 표시 설계

## 목표

캐릭터 기본 스탯의 데이터 소유권과 계산 공식을 확정하고, 장비 보너스를 실제 런타임 수치와 장비창 표시에 연결한다.

## 데이터 소유권

- `URogue10mCharacterDataAsset`: 캐릭터 원형의 기본 스탯과 밸런스 값
- `URogue10mInventoryComponent`: 현재 장착 아이템의 능력치 보너스 합계
- `URogue10mAttributeSet`: 기본값과 장비 보너스를 합산한 최종 런타임 수치
- `ARogue10mCharacter`: Data Asset, 장비 합계, 이동 컴포넌트를 연결하는 재계산 진입점
- `URogue10mEquipmentWindowWidget`: 읽기 전용 스냅샷을 장비창에 표시

## 기본 스탯

| 분류 | 스탯 | 기본값 | 최종 계산 |
| --- | --- | ---: | --- |
| 생존 | 최대 체력 | 100 | 기본 + 장비 |
| 자원 | 최대 스태미나 | 100 | 기본 |
| 자원 | 최대 마나 | 100 | 기본 |
| 전투 | 공격력 | 10 | 기본 + 장비 |
| 전투 | 방어력 | 0 | 기본 + 장비 |
| 전투 | 치명타 확률 | 5% | 기본 + 장비 |
| 전투 | 치명타 피해 | 150% | 기본 |
| 전투 | 공격 속도 | 100% | 기본 + 장비 |
| 이동 | 이동 속도 | 600 cm/s | 기본 + 장비 |
| 이동 | 질주 속도 | 900 cm/s | 기본 + 이동 장비 보너스 |

## 전투 공식

- 일반 피해: `(스킬 기본 피해 + 최종 공격력) × 피해 변동 비율`
- 치명타: 일반 피해 × 최종 치명타 피해 배율
- 피격 피해: `Max(1, 원본 피해 - 최종 방어력)`
- 최대 체력 변경 시 현재 체력 비율을 보존한다.

## 장비창 표시

장비창의 기존 6개 텍스트를 사용해 공격력, 방어력, 최대 체력, 치명타 확률, 공격 속도, 이동 속도를 표시한다. 각 행은 `최종값 (기본값 + 장비 보너스)` 구조로 갱신한다.

## Ultrawork Packets

### Packet 1 — 공통 스탯 타입과 Data Asset

- 목표: 장비 보너스와 최종 스냅샷의 공통 구조를 정의하고 Character Data Asset 기본값을 확장한다.
- 수정 위치: `Source/Rogue10m/Data`, `Source/Rogue10m/Character`
- 완료 조건: 기본 스탯과 장비 보너스를 한 스냅샷으로 계산할 수 있다.
- 검증: Editor Target 컴파일
- 롤백 경계: 공통 타입과 Character Data 필드

### Packet 2 — 런타임 적용

- 목표: 장비 변경 때 AttributeSet과 이동 속도를 재계산한다.
- 수정 위치: `Rogue10mAttributeSet`, `Rogue10mInventoryComponent`, `Rogue10mCharacter`, 공격 피해 계산
- 완료 조건: 장착·교체·해제 후 6개 최종 수치가 실제 동작에 반영된다.
- 검증: Editor Target 컴파일, PIE 장비 교체
- 롤백 경계: 재계산 진입점과 장비 변경 알림

### Packet 3 — 장비창 표시

- 목표: 장비창에 기본·장비·최종 스탯을 표시한다.
- 수정 위치: `Rogue10mMenuWindowWidgets`, 메뉴 Designer 생성 스크립트
- 완료 조건: 장비 변경 직후 6개 텍스트가 갱신된다.
- 검증: Widget Validator, PIE 장비창 확인
- 롤백 경계: 장비창 스탯 포맷팅

### Packet 4 — 콘텐츠와 문서

- 목표: 기본 Character Data Asset 값을 저장하고 개발 기록을 남긴다.
- 수정 위치: `/Game/DataAsset/Character`, Editor Python, Feature/Docs/DevLog
- 완료 조건: Data Asset 값과 문서가 코드 기본값과 일치한다.
- 검증: UnrealEditor-Cmd Data Asset 검사, `CheckGeneratedChanges.ps1`
- 롤백 경계: Data Asset 및 문서 변경

## 제외 범위

- 캐릭터 커스터마이징과 캐릭터 저장
- 인벤토리 저장·불러오기
- 스탯 포인트 투자와 레벨 성장 곡선
- 스킬트리 효과에 의한 스탯 보정
