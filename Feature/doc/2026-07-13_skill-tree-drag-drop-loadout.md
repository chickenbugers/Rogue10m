# 스킬트리 Drag and Drop Loadout 구현 결과

## 구현 결과

- CombatComponent 생성자의 FObjectFinder 기반 하드코딩 스킬 경로를 제거했다.
- URogue10mWeaponSkillProfileDataAsset을 추가해 무기별 기본 입력 스킬, 스킬트리 후보, 초기 해금 스킬, 기본 회피를 묶었다.
- URogue10mDodgeSkillDataAsset을 추가해 회피 거리, 시간, 쿨다운, 스테미나 비용, 아이콘을 데이터화했다.
- 기본 회피 거리는 100cm로 설정했다.
- 무기 타입 변경 시 대응 프로필과 회피기가 자동 적용된다.
- 스킬트리 Entry Widget에서 해금된 AttackSkill Data Asset을 Drag할 수 있다.
- QuickSlot Widget이 Drop을 받아 좌/우 클릭과 차징 입력 Loadout을 변경한다.
- E 회피 슬롯은 자동 바인딩 슬롯이라 Drop을 거부한다.
- 숫자키 소비 아이템 슬롯은 기존 기능을 유지한다.

## 검증

- UE 5.8 UnrealHeaderTool 성공
- Rogue10mEditor Win64 Development 빌드 성공
- 공중 회피 차단 코드 유지
- Tick 추가 없음

## 에디터 설정

- Docs/SkillTreeLoadoutGuide.md 참고
- 실제 Weapon Skill Profile과 Dodge Data Asset은 Unreal Editor에서 생성한다.
- BP_FirstPersonCharacter의 Combat Component에 Weapon Skill Profiles를 등록한다.