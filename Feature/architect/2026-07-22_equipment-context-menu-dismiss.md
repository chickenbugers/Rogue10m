# 장비 장착 해제 컨텍스트 메뉴 동작 개선 설계

## 목표

- 장착 장비를 우클릭하면 기존 Hover Tooltip을 즉시 숨긴다.
- 장착 해제 메뉴를 마우스 커서의 오른쪽에 표시한다.
- 메뉴 영역을 제외한 화면의 다른 위치를 클릭하면 메뉴를 닫고 해당 클릭을 뒤 UI로 전달하지 않는다.

## Ultrawork Packets

### Packet 1 - Tooltip 및 메뉴 수명주기

- 목표: 우클릭 시 활성 Tooltip을 닫고 메뉴 종료 시 슬롯 Tooltip 연결을 복원한다.
- 수정 위치: `Rogue10mMenuWindowWidgets.h/.cpp`
- 완료 조건: 메뉴와 Hover Tooltip이 동시에 보이지 않고 장비 갱신·창 닫기 이후 참조가 남지 않는다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 장비창의 Action Menu 열기/닫기 함수와 Tooltip 복원 상태.

### Packet 2 - 외부 클릭 닫기와 커서 오른쪽 배치

- 목표: Action Widget을 전체 화면 dismiss surface와 실제 220x96 메뉴로 분리한다.
- 수정 위치: `BuildMenuDesignerLayouts.py`, `WBP_EquipmentSlotAction`
- 완료 조건: 메뉴 내부 클릭은 기존 버튼에 전달되고 외부 첫 클릭은 메뉴만 닫으며 소비된다.
- 검증 명령: `ValidateMenuWidgetAssets.py`와 PIE 수동 QA
- 롤백 경계: Action Widget Designer 트리와 메뉴 배치 API.

### Packet 3 - 빌드·리뷰·문서

- 목표: UE 5.8 컴파일, 생성물 검사, Unreal 입력·GC·수명주기 리뷰, 작업 기록을 완료한다.
- 수정 위치: `Feature/doc`, `Docs/SprintChangeLog.md`, `DevLog/20260722.txt`
- 완료 조건: Editor 빌드, 메뉴 Validator, `CheckGeneratedChanges.ps1`, `git diff --check` 통과.
- 롤백 경계: 검증 결과 및 문서 항목.

## 입력 및 배치 흐름

1. 장비 슬롯 우클릭에서 장착 아이템과 Hit Widget을 확인한다.
2. 이전 메뉴를 닫고 억제했던 Tooltip 연결을 복원한다.
3. Action Widget을 전체 Viewport에 올리고 실제 메뉴 패널을 커서 오른쪽 12px 위치에 배치한다.
4. Slate Tooltip을 즉시 닫고 우클릭한 슬롯의 Tooltip 연결을 메뉴가 열려 있는 동안 제거한다.
5. 전체 화면 투명 버튼을 메뉴보다 낮은 Z 순서에 배치해 외부 첫 클릭을 소비하고 메뉴를 닫는다.
6. Close 함수에서 Action Widget을 제거하고 슬롯 Tooltip을 최신 데이터로 다시 생성한다.

## 수동 QA

- Tooltip이 열린 장비를 우클릭하면 Tooltip이 즉시 사라지는지 확인한다.
- 메뉴가 커서 오른쪽에 표시되고 우측·하단에서는 Viewport 안으로 보정되는지 확인한다.
- 메뉴 내부 클릭이 외부 닫기로 처리되지 않는지 확인한다.
- 메뉴 외부를 좌·우클릭하면 메뉴만 닫히고 첫 클릭이 뒤 UI에 작동하지 않는지 확인한다.
- 메뉴 종료 후 장비 슬롯 Hover Tooltip이 다시 표시되는지 확인한다.
