# 시작 인벤토리 추가 갑옷 결과

## 구현 결과

기존 가죽 갑옷 Data Asset을 재사용해 시작 인벤토리에 갑옷 한 개를 추가했다.

- `PrototypeStartingItems`에 `DA_Item_LeatherArmor` 참조를 한 번 더 등록했다.
- 시작 장비 자동 장착을 동일 부위당 첫 장비로 제한했다.
- 별도 `.uasset`이나 아이콘 복제 없이 기존 아이템 정의를 유지했다.

## 시작 시 동작

1. 첫 번째 가죽 갑옷은 기존처럼 Armor 슬롯에 자동 장착된다.
2. 두 번째 가죽 갑옷은 이미 Armor 슬롯이 사용 중이므로 자동 장착을 건너뛴다.
3. 자동 장착되지 않은 두 번째 갑옷은 기본 인벤토리 그리드에 남는다.

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- `Scripts/CheckGeneratedChanges.ps1` 통과
- `git diff --check` 통과

## 참고

시작 아이템은 기본 인벤토리가 비어 있을 때만 초기화되므로 새 PIE 세션 또는 신규 상태에서 확인해야 한다.
