# 공격 스킬 Data Asset 설정 가이드

## 목적

좌클릭, 우클릭, 점프 공격, 차징 공격을 코드에 고정하지 않고 Data Asset으로 조정할 수 있게 한다.

## Data Asset 클래스

- C++ 클래스: `URogue10mAttackSkillData`
- 소스 위치: `Source/Rogue10m/Data/Rogue10mAttackSkillData.*`
- 에디터 생성 권장 위치: `Content/Rogue10m/Data/Skills/Attacks`

## 생성 방법

1. Unreal Editor Content Browser에서 `Miscellaneous > Data Asset`을 선택한다.
2. Data Asset 클래스 목록에서 `Rogue10mAttackSkillData`를 선택한다.
3. 아래 예시처럼 입력별 에셋을 만든다.

권장 에셋 이름:

- `DA_Unarmed_Primary`
- `DA_Unarmed_Special`
- `DA_Unarmed_JumpPrimary`
- `DA_Unarmed_JumpSpecial`
- `DA_Unarmed_ChargedPrimary`
- `DA_Unarmed_ChargedSpecial`

## 캐릭터에 지정하는 위치

`BP_FirstPersonCharacter`에서 아래 항목에 Data Asset을 지정한다.

- `Primary Attack Skill`: 좌클릭 공격
- `Special Attack Skill`: 우클릭 공격
- `Jump Primary Attack Skill`: 점프 중 좌클릭 공격
- `Jump Special Attack Skill`: 점프 중 우클릭 공격
- `Charged Primary Attack Skill`: 차징 좌클릭 공격
- `Charged Special Attack Skill`: 차징 우클릭 공격

해당 칸에 Data Asset이 없으면 그 입력은 잠긴 것으로 처리되고, 화면 전투 로그에 잠김 메시지가 표시된다.

## 조정 가능한 값

- `Skill Name`: 화면 로그에 표시될 스킬 이름
- `Skill Description`: 스킬 설명
- `Input Slot`: 해당 Data Asset이 의도한 입력 슬롯
- `Damage`: 공격 데미지
- `Attack Range`: 공격이 닿는 거리
- `Attack Trace Radius`: 공격 판정 두께
- `Attack Cooldown`: 재사용 대기 시간
- `Charge Seconds`: 차징 공격으로 인정되는 최소 누름 시간
- `Attack Montage`: 공격 애니메이션 몽타주
- `Attack Effect`: Niagara 또는 Particle 효과 에셋
- `Draw Debug Attack`: 공격 판정 디버그 표시 여부
- `Debug Color`: 디버그 선과 판정 구체 색상

## 화면 디버그 표시

공격이 실행되면 다음 디버그가 월드에 표시된다.

- 카메라 앞 방향 공격 선
- 공격 끝 지점의 판정 구체
- 몬스터 명중 시 빨간 명중 구체

디버그 표시는 `BP_FirstPersonCharacter`의 `Draw Attack Debug` 또는 개별 Data Asset의 `Draw Debug Attack`으로 제어한다.

## 확장 방향

- 실제 피해 타이밍은 이후 Animation Montage Notify로 옮기는 것이 좋다.
- 무기 타입별로 Data Asset을 나눠 장비 무기와 연결한다.
- `Attack Effect`는 추후 Niagara 모듈 연결 후 공격 위치 또는 명중 위치에 스폰한다.
