# 메뉴 Widget 런타임 레이아웃 재구성

## 목표

`Content/Widget/Parts`의 어두운 반투명 패널과 금색 포인트 스타일을 기준으로 Inventory, Equipment, SkillTreeEntry, SkillTreeWindow를 재구성한다. 네 WBP는 Designer 트리가 비어 있고 네이티브 부모의 `RebuildWidget()`으로 생성되므로 `.uasset` 바이너리를 직접 편집하지 않고 C++ 런타임 WidgetTree와 WBP 부모 연결을 갱신한다.

## Ultrawork Packets

### Packet 1 — 요구사항 및 트리 감사

- 대상: Menu WBP 4종, Parts WBP, `Rogue10mMenuWindowWidgets.*`
- 완료 조건: BindWidget 이름과 빈 Designer 트리 구조를 확인한다.
- 검증: Editor Remote Execution 자산/부모 클래스 검사

### Packet 2 — 런타임 레이아웃 구현

- 대상: `Rogue10mMenuWindowWidgets.h/.cpp`
- 완료 조건: Canvas/Frame/Content 계층, Inventory UniformGrid+ItemCanvas, Equipment 슬롯/미리보기, SkillTree Scroll/Wrap, Entry 아이콘/설명/잠금 표시를 만든다.
- 검증: Rogue10mEditor 빌드

### Packet 3 — WBP 연결 및 자산 검증

- 대상: `Content/Widget/Menu/*.uasset`, Editor 구성 스크립트
- 완료 조건: 네 WBP가 올바른 C++ 부모를 유지하고 SkillTreeEntry Class와 PlayerController Class 참조가 저장된다.
- 검증: Editor Remote Execution 재로드

### Packet 4 — 문서, 리뷰, Git

- 대상: Feature/doc, DevLog, SprintChangeLog
- 완료 조건: 추가로 필요한 Designer/데이터 요소와 후속 기능을 기록하고 검사 후 명시된 파일만 커밋·push한다.
- 검증: CheckGeneratedChanges, git diff --check, push readback

## 인벤토리 레이아웃

```text
UI_InventoryCanvas (Canvas)
└─ UI_InventoryWindowFrame (Border)
   └─ UI_InventoryContent (VerticalBox)
      ├─ UI_InventoryTitleText
      ├─ UI_BagTabContainer (HorizontalBox)
      ├─ UI_InventoryCapacityText
      └─ UI_InventoryGridOverlay (Overlay)
         ├─ UI_InventoryGrid (UniformGrid, 10x10 배경/셀)
         └─ UI_InventoryItemCanvas (Canvas, NxM 아이템 시각 배치)
```

`UniformGrid`는 셀 배경과 좌표 기준을 담당하고, NxM 아이템 위젯은 `Canvas`에서 `CellSize * Position/Size`로 배치한다. 아이템이 Grid 자식으로만 들어가면 여러 셀을 차지하는 NxM 표현이 어렵다.

## 추가로 필요한 기능 요소

- Inventory: 가방 탭 버튼 클래스, 셀 배경 위젯, NxM 아이템 위젯, 선택/회전/드롭 프리뷰, 스크롤 또는 줌 정책, 툴팁 위젯.
- Equipment: 실제 장비 슬롯 위젯, 허용 장비 태그/타입, 캐릭터 프리뷰용 SceneCapture 또는 RenderTarget, 능력치 비교 패널.
- Skill Tree: 분기/선행 조건선, 스킬 포인트 표시, 해금 버튼, 잠금 사유, 검색/필터, 현재 로드아웃 슬롯 드롭 영역.
- Skill Entry: 레벨/최대 레벨, 비용, 선행 스킬, 쿨다운/자원 비용, 잠금 아이콘과 선택 상태.
- 공통: 닫기 버튼, 키보드/게임패드 포커스, 해상도별 크기 조정, 접근성 텍스트, 열림/닫힘 애니메이션.
