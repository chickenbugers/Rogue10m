# 장비창 Preview Character 좌클릭 회전 개발 결과

## 결과

장비창 캐릭터 프리뷰 내부에서 마우스 좌클릭을 누른 채 좌우로 드래그하면 캐릭터가 수평 회전한다. 카메라와 조명은 고정되며 프리뷰 Skeletal Mesh 묶음만 회전한다.

## 구현

- `ARogue10mEquipmentPreviewActor`
  - `PreviewMeshRoot` Pivot을 Scene Root 아래에 추가했다.
  - 모든 프리뷰 Skeletal Mesh를 Pivot 아래에 생성한다.
  - `AddPreviewYaw()`가 드래그 가로 이동량을 Pivot의 Local Yaw에 누적한다.
  - 장비 변경으로 메시를 다시 구성해도 Pivot 회전값을 유지한다.
- `URogue10mEquipmentWindowWidget`
  - `UI_CharacterPreviewImage` 내부 좌클릭만 회전 입력으로 판정한다.
  - 드래그 시작 시 마우스를 캡처해 영역 밖 이동도 이어서 처리한다.
  - 좌클릭 해제, 버튼 상태 손실, 캡처 손실, 창 닫기, Widget 파괴 시 회전 상태를 정리한다.
  - 기존 타이틀 바 창 이동과 장비 슬롯 Drag & Drop 경로는 유지한다.

## 튜닝

`WBP_EquipmentWindow` 클래스 기본값의 다음 항목으로 회전 감도를 조절할 수 있다.

- Category: `Rogue10m > Equipment > Preview`
- Property: `Character Preview Rotation Degrees Per Pixel`
- 기본값: `0.35°/px`
- 허용 범위: `0.01~2.0°/px`

## Scene Capture 연동

장비창이 열린 동안에는 기존 실시간 `bCaptureEveryFrame` 갱신을 사용한다. 마우스 회전 API가 호출하는 보조 캡처 함수는 자동 캡처가 꺼진 경우에만 실행되므로, 이전에 수정한 자동·수동 Scene Capture 중복 경고를 다시 발생시키지 않는다.

## 검증

- UE 5.8 UHT 통과
- Rogue10mEditor Win64 Development 빌드 성공
- 프리뷰 Pivot과 카메라·조명 Attach 계층 분리 확인
- 회전 시작 영역, 좌클릭 해제, 버튼 상태 손실, 마우스 캡처 손실 정리 경로 확인
- 생성물 변경 검사 및 `git diff --check`

## 남은 수동 QA

실행 중 에디터가 핫리로드 모듈을 사용 중이므로 에디터 재시작 후 PIE에서 다음을 확인한다.

- 좌우 드래그 방향과 기본 감도
- 프리뷰 밖에서 좌클릭 시작 시 미회전
- 프리뷰 밖으로 드래그한 뒤 버튼 해제 시 정상 종료
- 타이틀 바 창 이동 및 7개 장비 슬롯 Drag & Drop 회귀
- 장비 교체 후 회전 각도 유지
- Scene Capture 중복 갱신 경고 미발생
