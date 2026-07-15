# 프로토타입 NxM 인벤토리 아이템 설계

## 목표

- 1x1, 2x3, 4x3 크기의 테스트 아이템을 Data Asset으로 제공한다.
- 기본 10x10 인벤토리가 비어 있을 때 테스트 아이템을 한 번만 자동 배치한다.
- 아이콘이 없는 초기 Data Asset도 인벤토리에서 서로 구분할 수 있게 한다.

## Ultrawork Packet

### Packet 1 - Data Asset 및 초기 배치

- 변경 영역: URogue10mItemDataAsset, URogue10mInventoryComponent, 에디터 자동화 스크립트
- 완료 조건: 세 Data Asset이 생성되고 게임 시작 시 겹침 없이 기본 인벤토리에 배치됨
- 검증: Editor 빌드, Python 에셋 검증, 생성 로그 확인
- 롤백 경계: 프로토타입 시작 아이템 배열과 세 Data Asset만 제거 가능

### Packet 2 - 아이콘 미지정 표시

- 변경 영역: URogue10mInventoryItemWidget
- 완료 조건: 아이콘이 없는 아이템이 Data Asset의 InventoryTint 색상으로 표시됨
- 검증: C++ 빌드 및 위젯 에셋 로드

## 배치 예상

행 우선 탐색 기준으로 1x1, 2x3, 4x3 순서로 첫 번째 유효 좌표에 배치한다. 저장 데이터나 기존 아이템이 있으면 자동 추가하지 않는다.