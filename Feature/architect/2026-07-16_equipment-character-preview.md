# Sprint#2-1 장비창 캐릭터 프리뷰 설계

## 목표

`WBP_EquipmentWindow`의 `UI_CharacterPreviewImage`에 현재 플레이어의 전신 메시와 머티리얼을 실시간으로 표시한다.
프리뷰는 장비창이 열려 있을 때만 캡처하고, 게임 월드의 일반 카메라에는 노출하지 않는다.

## Ultrawork Packets

### Packet 1 - 프리뷰 렌더 액터

- 목표: 플레이어 메시를 복제해 전용 `SceneCaptureComponent2D`로 촬영한다.
- 수정 위치: `Source/Rogue10m/UI/Rogue10mEquipmentPreviewActor.*`
- 완료 조건: 캡처 전용 메시, 조명, 투명 Render Target을 런타임에 구성한다.
- 검증: `Scripts/BuildEditor.ps1`
- 롤백 경계: 신규 프리뷰 액터 파일만 제거할 수 있다.

### Packet 2 - 장비창 연결

- 목표: 장비창 열기/닫기와 프리뷰 생성·활성화·정리를 연결한다.
- 수정 위치: `Rogue10mMenuWindowWidgets.*`
- 완료 조건: 장비창이 열리면 프리뷰가 표시되고 닫히면 캡처가 중지된다.
- 검증: Editor 빌드 및 PIE 장비창 확인
- 롤백 경계: 장비창 클래스의 프리뷰 관련 멤버와 함수만 제거할 수 있다.

### Packet 3 - 위젯 에셋 및 문서

- 목표: 프리뷰 Image 배치/검증 규칙과 운영 문서를 보강한다.
- 수정 위치: `Scripts/Editor/*`, `Feature/doc`, `Docs/SprintChangeLog.md`, `DevLog/20260716.txt`
- 완료 조건: WBP 컴파일·필수 위젯 검증과 사용 가이드가 완료된다.
- 검증: `CheckGeneratedChanges.ps1`, 에디터 검증 스크립트
- 롤백 경계: 스크립트와 문서 변경만 독립적으로 되돌릴 수 있다.

## 런타임 구조

```text
URogue10mEquipmentWindowWidget
  -> ARogue10mEquipmentPreviewActor 생성
     -> 플레이어 GetMesh()의 SkeletalMesh/Material/Pose 복제
     -> SceneCapture2D -> UTextureRenderTarget2D
  -> UI_CharacterPreviewImage Brush Resource로 연결
```

## 성능 및 수명주기

- 장비창이 닫혀 있으면 `CaptureEveryFrame`과 `CaptureOnMovement`를 끈다.
- 프리뷰 메시에는 충돌·그림자·메인 패스 렌더링을 사용하지 않는다.
- 프리뷰 액터는 위젯 `NativeDestruct`에서 제거한다.
- 프리뷰 Render Target은 위젯 인스턴스 수명에 맞춘 transient 객체로 관리한다.

## 향후 확장

현재 아이템 Data Asset에는 장착 외형 메시 필드가 없으므로 이번 작업은 실제 플레이어 메시/머티리얼을 표시한다.
향후 장비별 외형을 지원할 때 `URogue10mItemDataAsset`에 슬롯별 Skeletal/Static Mesh 정의를 추가하고,
`OnEquipmentChanged`에서 프리뷰 컴포넌트를 교체하도록 확장한다.

## Packet 4 - 레퍼런스형 7부위 장비 슬롯

- 목표: 캐릭터 프리뷰를 중심으로 투구, 갑옷, 장갑, 무기, 신발, 반지, 목걸이를 부위별 비율로 배치한다.
- 수정 위치: `Rogue10mInventoryComponent.*`, `Rogue10mMenuWindowWidgets.*`, `BuildMenuDesignerLayouts.py`, `WBP_EquipmentWindow`
- 완료 조건: 장비 데이터와 BindWidget이 정확히 7부위로 일치하고, 장비 아이콘은 슬롯 내부에서 원본 종횡비를 유지한다.
- 검증: Editor 빌드, WBP 컴파일, CanvasSlot 위치·크기 자동 검사, PIE 장비 데이터 및 Brush 리소스 검사
- 롤백 경계: 기존 enum 직렬화 호환을 위해 구형 타입은 유지하고, 신규 `Necklace`는 enum 마지막에 추가한다.

### 배치 기준

| 부위 | 슬롯 크기(px) | 배치 의도 |
|---|---:|---|
| 투구 | 88×94 | 머리 위 중앙 |
| 목걸이 | 76×76 | 목 주변 우측 |
| 무기 | 94×174 | 긴 장비 비율을 반영한 좌측 세로 슬롯 |
| 갑옷 | 120×150 | 몸통 중앙의 가장 큰 방어구 슬롯 |
| 장갑 | 94×132 | 팔 위치에 맞춘 우측 세로 슬롯 |
| 반지 | 74×74 | 좌측 하단 소형 장신구 슬롯 |
| 신발 | 88×90 | 우측 하단 발 위치 슬롯 |

각 슬롯은 `CanvasPanel -> Border -> GridPanel -> ScaleBox -> Image` 구조를 사용한다. `ScaleBox`는 `ScaleToFit`으로 원본 종횡비를 보존하며, `InventoryIconScale`은 장비창에서 최대 1.0으로 제한해 인접 슬롯 침범을 막는다.
