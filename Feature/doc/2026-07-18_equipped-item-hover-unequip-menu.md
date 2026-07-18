# 장착 장비 Hover 및 장착 해제 메뉴 구현 결과

## 결과

장비창의 투구·목걸이·갑옷·장갑·무기·신발·반지 슬롯에 장착된 아이템을 Hover하면 기존 인벤토리 상세 Tooltip이 표시된다. 우클릭하면 마우스 근처에 별도 WBP_EquipmentSlotAction 메뉴가 열리고 장착 해제 버튼으로 MxN 인벤토리의 첫 빈 공간에 장비를 이동한다.

## Hover 정보

- 아이템 아이콘
- 아이템 이름
- 아이템 설명
- 무게
- 공격력·방어력·최대 체력·치명타 확률·공격 속도·이동 속도 증가량

빈 슬롯은 Tooltip을 만들지 않으며 장비 상태가 바뀌면 슬롯 Tooltip을 다시 구성한다.

## 우클릭 Action UserWidget

- UI_EquipmentItemNameText
- UI_UnequipButton
- UI_UnequipButtonText
- UI_ActionResultText

메뉴는 220x96 크기로 커서에서 12px 떨어진 위치에 표시하며 Viewport 경계 안으로 보정한다. 장비창 닫기, 다른 장비 변경, 좌클릭 시작, 장착 해제 성공 시 메뉴를 제거한다.

## 장착 해제 정책

- InventoryContainers를 앞에서부터 검사한다.
- 장비의 MxN 크기가 들어가는 첫 좌표를 찾는다.
- 배치 가능한 경우에만 장비 슬롯에서 인벤토리 Grid로 이동한다.
- 공간이 없으면 장비를 그대로 유지하고 인벤토리 공간이 부족합니다. 문구를 표시한다.
- 이동 성공 시 Inventory Grid와 Equipment Delegate를 모두 갱신한다.

## 변경 파일

- Rogue10mInventoryComponent: 첫 빈 MxN 공간 장착 해제 API
- Rogue10mMenuWindowWidgets: 장비 Hover, 우클릭 입력, Action Widget 부모와 생명주기
- WBP_EquipmentSlotAction: 장착 해제 메뉴 UI
- BuildMenuDesignerLayouts.py: WBP 생성 및 장비창 Soft Class 저장
- ValidateMenuWidgetAssets.py: 계층·가시성·API·Class Reference 검증

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- WBP_EquipmentSlotAction 생성·컴파일·저장 성공
- WBP_EquipmentWindow Tooltip/Action Soft Class 저장 성공
- 활성 Editor 전체 메뉴 검증 성공
- 새 Unreal commandlet 프로세스 0 errors / 0 warnings
- Editor Python 3종 AST 문법 검증 성공
- git diff --check 통과

## 남은 수동 QA

- 각 장착 부위 Hover Tooltip 내용과 위치
- 우클릭 메뉴 클릭 및 실제 MxN 장착 해제
- 인벤토리 공간 부족 피드백과 장비 보존
- 좌클릭 Drag & Drop 및 두 메뉴 동시 상호작용 회귀
