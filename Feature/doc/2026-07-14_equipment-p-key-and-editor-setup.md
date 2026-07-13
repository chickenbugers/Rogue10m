# 장비창 P 키 및 에디터 설정 - Development Summary

## Summary

- 장비창 토글 키를 B에서 P로 변경했다.
- 인벤토리는 I, 스킬트리는 K, 장비창은 P를 사용한다.
- 메뉴 Widget Class 경고 해결 위치와 회피 Data Asset 참조 경로를 정리했다.

## Editor Or Blueprint Setup

PlayerController Blueprint의 `Rogue10m | UI | Menu Windows`에서 다음을 지정한다.

- InventoryWindowWidgetClass: URogue10mInventoryWindowWidget 자식 WBP
- EquipmentWindowWidgetClass: URogue10mEquipmentWindowWidget 자식 WBP
- SkillTreeWindowWidgetClass: URogue10mSkillTreeWindowWidget 자식 WBP

회피기는 URogue10mDodgeSkillDataAsset을 만든 뒤 무기별 Weapon Skill Profile의 DefaultDodgeSkill에 연결한다. 그 Weapon Skill Profile은 BP_FirstPersonCharacter의 Combat Component `WeaponSkillProfiles` 배열에 등록한다.

## Validation

- Rogue10mEditor Win64 Development 빌드 성공
- CheckGeneratedChanges Harness 경로 검사 통과
- git diff --check