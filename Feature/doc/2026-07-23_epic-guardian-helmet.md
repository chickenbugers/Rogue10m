# 에픽 수호자 투구 결과

## 구현 결과

에픽 등급 Head 장비 `DA_Item_GuardianHelmet`을 제작해 시작 인벤토리에 추가했다.

- 표시 이름: 수호자의 투구
- 분류: Equipment / Epic / Head
- 크기와 무게: 2×2 / 2.8kg
- 능력치: 방어력 +10, 최대 체력 +20
- 아이콘: 기존 `T_Item_IronHelmet` 재사용
- 시작 아이템 목록에서 기존 철제 투구 바로 뒤에 배치

## 시작 시 동작

1. 기존 레어 철제 투구가 Head 슬롯에 먼저 자동 장착된다.
2. 수호자의 투구는 이미 Head 슬롯이 사용 중이므로 자동 장착을 건너뛴다.
3. 수호자의 투구 한 개가 기본 인벤토리 그리드에 남는다.

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- UnrealEditor-Cmd에서 스타터 Data Asset 생성 및 2×2 아이콘 검증 성공
- 전체 메뉴·아이템 Validator 통과
- 실제 에셋의 Epic 등급, 방어력 +10, 최대 체력 +20, 아이콘 스케일 1.0 확인
- `Scripts/CheckGeneratedChanges.ps1` 및 `git diff --check` 통과

## 참고

시작 아이템은 기본 인벤토리가 비어 있을 때만 초기화되므로 새 PIE 세션 또는 신규 상태에서 확인해야 한다.
