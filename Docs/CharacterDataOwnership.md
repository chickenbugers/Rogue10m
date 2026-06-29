# 캐릭터 데이터 소유 구조

## 목적

`ARogue10mCharacter`에 누적되던 상태와 전투 데이터를 기능별 클래스로 분산한다.

## 현재 분리 기준

### PlayerState

`ARogue10mPlayerState`는 플레이어에게 붙는 지속 상태를 관리한다.

- 캐릭터 표시 이름
- 캐릭터 직업 이름
- 현재 장착 무기 타입
- 캐릭터 사망 상태

캐릭터는 기존 외부 호출을 유지하기 위해 `GetCharacterDisplayName`, `GetCharacterJobName`, `GetEquippedWeaponType`, `IsDead` 래퍼를 제공한다.

### CombatComponent

`URogue10mCombatComponent`는 공격 입력과 전투 런타임 상태를 관리한다.

- 공격 Data Asset 참조
- 차징 입력 시작 시간
- 콤보 입력 가능 시간
- 콤보 루트 스킬
- 공유 공격 쿨타임
- HUD에 표시할 현재 공격 스킬

캐릭터는 입력을 받고 실제 카메라 트레이스와 데미지 적용을 수행한다. 공격 데이터 해석, 콤보 판단, 쿨타임 상태는 CombatComponent에 위임한다.

### Character

`ARogue10mCharacter`는 다음 역할을 유지한다.

- Enhanced Input과 키 입력 바인딩
- 이동, 점프, 시점 회전
- 카메라 기준 공격 트레이스
- 피격 처리와 사망 연출
- HUD, Inventory, Vitals, CombatComponent 연결 지점

## 다음 분리 후보

- 스킬트리 해금 상태는 HUD 임시 값에서 PlayerState 또는 SkillTreeComponent로 이동한다.
- 자원 비용 결제는 캐릭터에서 VitalsComponent 또는 CombatComponent로 더 분리할 수 있다.
- 실제 공격 타이밍은 Animation Montage Notify 기반 CombatComponent 함수로 이동하는 것이 좋다.
