# 장비창 Preview Character 좌클릭 회전 설계

## 목표

장비창 캐릭터 프리뷰 영역에서 마우스 좌클릭 드래그로 캐릭터를 좌우 회전한다. 장비 슬롯 Drag & Drop, 창 타이틀 이동, 우클릭 장착 해제 메뉴와 입력 충돌이 없어야 한다.

## 범위

- `ARogue10mEquipmentPreviewActor`
  - 카메라·조명과 분리된 프리뷰 메시 전용 Pivot 추가
  - 가로 드래그 입력을 Yaw 회전으로 적용하는 API 추가
- `URogue10mEquipmentWindowWidget`
  - 프리뷰 이미지 영역 좌클릭 시작 판정
  - 마우스 캡처 중 가로 이동량 계산
  - 좌클릭 해제·캡처 손실·창 닫기 시 회전 상태 정리
  - Blueprint 기본값으로 조절 가능한 회전 감도 제공

## 입력 규칙

1. 좌클릭 시작 위치가 `UI_CharacterPreviewImage` 내부일 때만 회전을 시작한다.
2. 회전 중에는 마우스를 캡처해 프리뷰 영역 밖으로 이동해도 드래그를 유지한다.
3. 화면 X 이동량에 `CharacterPreviewRotationDegreesPerPixel`을 곱해 Yaw에 누적한다.
4. 좌클릭 해제, 마우스 캡처 손실, 장비창 닫기 또는 Widget 파괴 시 회전을 종료한다.
5. 타이틀 바 좌클릭은 기존 창 이동, 장비 슬롯 좌클릭은 기존 아이템 Drag & Drop을 유지한다.

## 렌더 구조

```text
SceneRoot
├─ SceneCapture
├─ Key/Fill/Rim Light
└─ PreviewMeshRoot (Yaw 회전)
   └─ Preview Skeletal Meshes
```

카메라와 조명을 회전시키지 않고 메시 Pivot만 회전해 화면 구도와 조명 방향을 유지한다. 장비 변경으로 메시를 다시 생성할 때도 Pivot 회전값은 보존한다.

## 완료 조건

- 프리뷰 내부 좌클릭 가로 드래그로 캐릭터가 좌우 회전한다.
- 클릭만 하고 움직이지 않으면 회전하지 않는다.
- 프리뷰 밖에서 시작한 드래그는 캐릭터를 회전시키지 않는다.
- 창 이동, 장비 Drag & Drop, 우클릭 메뉴 동작이 유지된다.
- Rogue10mEditor 빌드와 생성물·diff 검사가 통과한다.

## 수동 QA

- 프리뷰 중앙에서 좌우로 여러 차례 드래그
- 프리뷰 밖으로 커서를 이동한 상태에서 좌클릭 해제
- 회전 도중 Alt+Tab 또는 포커스 손실
- 타이틀 바 창 이동과 7개 장비 슬롯 Drag & Drop 회귀
- 장비 교체 후 기존 회전 각도 유지 확인
