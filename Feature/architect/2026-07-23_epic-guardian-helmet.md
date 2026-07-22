# 에픽 수호자 투구 설계

## 목표

에픽 등급 Head 장비 Data Asset을 제작하고 시작 인벤토리에 한 개를 추가한다.

## 아이템 정의

- Asset: `DA_Item_GuardianHelmet`
- 표시 이름: 수호자의 투구
- 등급/슬롯: Epic / Head
- 인벤토리 크기: 2×2
- 무게: 2.8kg
- 능력치: 방어력 +10, 최대 체력 +20
- 아이콘: 기존 철제 투구 아이콘 재사용

## Ultrawork Packets

1. 스타터 아이템 생성 정의와 검증 기준을 추가한다.
2. Unreal Editor에서 Data Asset을 생성하고 저장한다.
3. 시작 인벤토리 참조를 연결한다.
4. Editor 빌드, 에셋 검증, 생성 파일 검사를 수행한다.
5. 결과 문서와 개발 로그를 갱신한다.

## 완료 조건

- `/Game/DataAsset/Item/Starter/DA_Item_GuardianHelmet`이 존재한다.
- 에픽 등급, Head 슬롯, 2×2 크기와 지정 능력치가 저장된다.
- 기존 철제 투구는 자동 장착되고 수호자의 투구는 시작 인벤토리에 남는다.

## 롤백 경계
