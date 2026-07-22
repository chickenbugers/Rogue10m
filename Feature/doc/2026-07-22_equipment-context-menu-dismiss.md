# 장비 장착 해제 컨텍스트 메뉴 동작 개선 결과

## 구현 결과

- 장착 슬롯 우클릭 시 `FSlateApplication::CloseToolTip()`으로 현재 Hover Tooltip을 즉시 닫는다.
- 메뉴가 열린 동안 해당 슬롯의 Tooltip 연결을 제거하고 메뉴가 닫히면 최신 장비 데이터로 다시 연결한다.
- 장착 해제 메뉴를 커서의 오른쪽 12px에 배치하며 Viewport 우측·하단 경계를 넘지 않게 보정한다.
- `WBP_EquipmentSlotAction`을 전체 화면 Canvas 구조로 변경했다.
  - Z 0: 투명 `UI_ActionDismissButton`
  - Z 1: 220x96 `UI_EquipmentSlotActionSize`
- 메뉴 외부의 첫 클릭은 dismiss 버튼이 소비한 뒤 메뉴를 닫으므로 뒤쪽 장비창·인벤토리·게임 입력에 동시에 전달되지 않는다.
- 메뉴 닫기, 장비 변경, 창 닫기, Destruct 경로에서 Action Widget과 Tooltip 억제 상태를 정리한다.

## 변경 파일

- `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.h/.cpp`
- `Scripts/Editor/BuildMenuDesignerLayouts.py`
- `Scripts/Editor/ValidateMenuWidgetAssets.py`
- `Content/Widget/Menu/Equipment/WBP_EquipmentSlotAction.uasset`

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- Python 스크립트 문법 검사 성공
- 새 프로세스 전체 메뉴 Widget Validator: 0 errors / 0 warnings
- `git diff --check` 통과
- `Scripts/CheckGeneratedChanges.ps1` 통과

## 남은 수동 QA

- PIE에서 Tooltip이 열린 상태의 우클릭 전환 확인
- 화면 가장자리 메뉴 위치 보정 확인
- 메뉴 내부와 외부 클릭 입력 분리 확인
- 메뉴 종료 후 Hover Tooltip 복원 확인
