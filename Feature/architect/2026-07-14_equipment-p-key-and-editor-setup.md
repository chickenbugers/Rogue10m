# Equipment P Key and Editor Setup - Architecture Plan

## Feature Summary

장비창 토글 키를 B에서 P로 변경하고, 메뉴 Widget Class 경고와 무기별 회피 Data Asset 연결 위치를 문서화한다.

## Existing Context

- 메뉴 입력은 PlayerController의 직접 Key Binding으로 처리한다.
- 인벤토리/장비/스킬트리 창은 PlayerController Blueprint에 지정한 Widget Class로 BeginPlay에서 생성한다.
- 회피 설정은 무기별 Weapon Skill Profile의 DefaultDodgeSkill을 사용한다.

## Technical Approach

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | 장비창 입력 변경 | Rogue10mPlayerController | P 입력이 ToggleItemWindow를 호출 | Editor 빌드 |
| 2 | 에디터 연결 안내 | 메뉴/스킬 문서 | Widget 및 회피 DA 설정 위치 명시 | 속성명 대조 |

## C++ / Blueprint Boundary

- C++: P 입력과 창 생성/토글 로직을 담당한다.
- Blueprint: 실제 WBP 클래스와 Data Asset을 설정한다.

## Risks And Review

- PlayerController Blueprint에 올바른 C++ 부모의 WBP를 지정하지 않으면 생성 또는 Cast가 실패한다.
- 회피 Data Asset이 없어도 Character 기본값 100cm를 fallback으로 사용한다.