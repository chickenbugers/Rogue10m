# Sprint#2-1 장비창 캐릭터 프리뷰 구현 결과

## 구현 결과

`WBP_EquipmentWindow`의 `UI_CharacterPreviewImage`에 현재 플레이어 캐릭터를 실시간으로 표시한다.
별도의 미리 제작한 Render Target 에셋 없이 위젯 인스턴스가 열릴 때 transient Render Target을 생성한다.

## 주요 구성

- `ARogue10mEquipmentPreviewActor`
  - 플레이어가 가진 전신/모듈러 Skeletal Mesh를 검색한다.
  - 1인칭 전용 팔 메시를 제외한다.
  - 원본 메시의 Skeletal Mesh, 머티리얼, 상대 Transform을 복제한다.
  - `SetLeaderPoseComponent`로 현재 플레이어 포즈를 따라간다.
  - Scene Capture 전용 표시 플래그로 게임 카메라에서는 숨긴다.
  - 3점 조명과 자동 카메라 프레이밍을 사용한다.
- `URogue10mEquipmentWindowWidget`
  - 장비창 최초 열림 시 프리뷰 액터와 512×768 Render Target을 생성한다.
  - Render Target을 `UI_CharacterPreviewImage` Brush Resource에 연결한다.
  - 장비 변경 시 메시/머티리얼 구성을 새로 읽는다.
  - 창이 닫히면 캡처·조명·프리뷰 메시를 비활성화한다.
  - 위젯 파괴 시 프리뷰 액터와 Brush 연결을 정리한다.

## 튜닝 위치

- Render Target 해상도: `WBP_EquipmentWindow` 클래스 기본값의
  `Rogue10m > Equipment > Preview > Character Preview Resolution`
- 기본값: `512 × 768`
- 카메라 FOV, 프레임 여백, 조명: `Rogue10mEquipmentPreviewActor.cpp`

## 검증

- `Rogue10mEditor Win64 Development`: 성공
- Widget Blueprint 컴파일/바인딩 자동 검증: 성공
- PIE 프리뷰 액터 수: 1
- PIE 프리뷰 Skeletal Mesh 수: 1
- PIE Render Target: 512×768
- Render Target 픽셀 표본: 384개 중 54개가 배경 임계값보다 밝아 캐릭터 렌더 확인
- 닫힘: `CaptureEveryFrame=false`
- 재열림: 액터 중복 없이 1개 유지, `CaptureEveryFrame=true`

## 제한과 다음 단계

현재 장비 Data Asset에는 착용 외형 메시 정보가 없다. 따라서 프리뷰는 플레이어 액터에 실제로 구성된 메시와
머티리얼을 정확히 보여주지만, 슬롯에 아이템을 장착하는 것만으로 새로운 갑옷 메시를 생성하지는 않는다.
장비 외형 변경 기능은 Item Data Asset에 착용 메시/소켓 정의를 추가하는 별도 작업으로 진행한다.

## 7부위 장비창 레이아웃 확장

- 실제 장비 데이터 배열을 무기, 투구, 갑옷, 장갑, 신발, 반지, 목걸이로 정리했다.
- 기존 직렬화 값을 보호하기 위해 `SecondaryWeapon`, `Legs`, `Earring` 등 구형 enum은 삭제하지 않았고 `Necklace`를 마지막 값으로 추가했다.
- `UI_EquipmentSlotContainer`를 `CanvasPanel`로 변경해 캐릭터 프리뷰 주변에 슬롯을 직접 배치했다.
- `UI_RingSlotIcon`, `UI_NecklaceSlotIcon`을 명시적으로 바인딩하고 구형 `Legs/Accessory` 통합 슬롯을 제거했다.
- 무기와 갑옷은 큰 세로 슬롯, 반지와 목걸이는 작은 정사각 슬롯을 사용한다.
- 장착 아이콘은 `ScaleBox(ScaleToFit)`와 흰색 Tint를 사용해 원본 Texture 종횡비와 색상을 유지한다.
- `RebuildEquipmentWindowLayout.py`가 장비창만 재생성하고 7개 CanvasSlot의 좌표와 크기를 회귀 검사한다.

### 추가 검증

- 7개 CanvasSlot 위치·크기 자동 검사: 성공
- 전체 메뉴 Widget Blueprint 컴파일 및 필수 바인딩 검사: 성공
- PIE 장비 슬롯: Left 4개 + Right 3개, 총 7개 확인
- PIE 장비창 표시: `EquipmentVisible=true`
- PIE 캐릭터 프리뷰 액터: 1개
- 스타터 무기/투구/갑옷/신발/반지 Texture Brush 연결 및 표시 상태 확인
- 빈 장갑/목걸이 아이콘은 `Collapsed`, 슬롯 프레임은 유지
