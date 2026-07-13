# 메뉴 Widget 레이아웃 재구성 결과

## 구현 결과

`WBP_InventoryWindow`, `WBP_EquipmentWindow`, `WBP_SkillTreeEntry`, `WBP_SkillTreeWindow`는 빈 Designer 트리와 네이티브 `RebuildWidget()` fallback을 사용하는 구조다. Parts 위젯의 어두운 반투명 패널, 금색 강조, 밝은 본문/회색 보조 텍스트 색을 공통 스타일로 사용해 네이티브 트리를 재구성하고 WBP를 다시 컴파일했다.

### Inventory

- 중앙 Canvas 창과 어두운 Frame
- `UI_BagTabContainer`: 기본 가방과 확장 가방 탭 영역
- `UI_InventoryCapacityText`: 사용 공간 표시
- `UI_InventoryGrid`: 44px 기준 10×10 셀 배경
- `UI_InventoryItemCanvas`: Grid 위에서 NxM 아이템을 실제 크기로 배치할 레이어
- 이동/회전/월드 드롭 안내

UniformGrid는 좌표와 셀 배경을 담당한다. NxM 아이템은 `UI_InventoryItemCanvas`의 CanvasSlot에 `Position = GridPosition * CellSize`, `Size = ItemGridSize * CellSize`로 배치해야 한다.

### Equipment

- 캐릭터 프리뷰 Image와 SceneCapture/RenderTarget 안내 영역
- 무기, 머리, 상의, 장갑, 하의, 신발, 장신구 슬롯 Frame
- 공격력/방어력 능력치 영역
- 인벤토리 Drag & Drop 안내

### Skill Tree Entry

- 64px 스킬 아이콘
- 스킬 이름과 Data Asset 설명
- 잠금 상태 텍스트
- 기존 해금 여부에 따른 입력 차단, 투명도, 툴팁, Drag 시작 유지

### Skill Tree Window

- 전체/해금/잠김 필터 표시 영역
- ScrollBox 안 WrapBox 카드 목록
- 사용 가능 스킬 포인트 표시 영역
- 기존 SkillTreeEntry 동적 생성과 HUD Drag & Drop 유지

## 추가 구현이 필요한 요소

### Inventory

- `WBP_InventoryCell`: 셀 선택/드롭 하이라이트
- `WBP_InventoryItem`: 아이콘, 스택, NxM 크기, 회전, 툴팁, DragVisual
- `WBP_BagTab`: 가방별 크기/이름/활성 탭
- InventoryComponent 변경 delegate와 UI 갱신 함수
- Canvas 좌표 변환, 배치 가능 프리뷰, 충돌 셀 표시
- 스크롤/줌 정책과 월드 드롭 확인 창

### Equipment

- 공통 `WBP_EquipmentSlot`과 장비 타입/GameplayTag 제한
- 장착/해제 Drop 처리와 InventoryComponent 연동
- SceneCapture2D + RenderTarget 또는 별도 캐릭터 프리뷰 씬
- 장착 전후 능력치 비교와 세트 효과 표시

### Skill Tree

- 스킬 포인트 보유/소비 데이터와 해금 요청
- 선행 스킬, 요구 레벨, 분기 연결선
- 실제 필터 버튼과 검색
- 입력 슬롯별 Loadout Drop Zone과 교체 확인
- 스킬 레벨, 비용, 쿨다운, 자원 소모 표시

### 공통

- 닫기 버튼, Escape 처리, Gamepad Focus/Navigation
- 열림/닫힘 애니메이션과 사운드
- DPI/해상도별 Safe Zone과 최소 크기
- 접근성 설명과 색약 대응

## 검증

- Rogue10mEditor Win64 Development 빌드 성공
- 네 WBP 재컴파일 및 저장 성공
- WBP 4종 Generated Class 재로드 성공
- SkillTreeWindow → WBP_SkillTreeEntry Class 연결 확인
- BP_FirstPersonPlayerController → Inventory/Equipment/SkillTree Window Class 연결 확인
- Tick 추가 없음

## 주의

현재 WBP Designer 트리가 비어 있으므로 표시 트리는 네이티브 부모가 런타임에 생성한다. 아트 작업자가 Designer에서 직접 배치하는 단계로 전환할 때는 이 문서의 이름을 그대로 사용해 BindWidget을 연결하고, 해당 WBP에 RootWidget이 생기면 네이티브 fallback은 자동으로 실행되지 않는다.
