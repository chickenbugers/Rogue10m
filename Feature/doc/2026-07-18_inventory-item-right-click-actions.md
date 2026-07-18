# 인벤토리 아이템 우클릭 사용·장착 결과

## 결과

- 인벤토리 Grid 아이템의 우클릭 입력을 소비 아이템과 장비 아이템으로 구분했다.
- 소비 아이템은 `URogue10mItemDataAsset::RestoreHealth`만큼 체력을 회복한 뒤 수량을 1 감소시킨다.
- 이미 최대 체력이거나 회복량이 0인 경우 아이템을 소비하지 않는다.
- 장비 아이템은 지정된 장비 부위에 즉시 장착된다.
- 같은 부위에 기존 장비가 있으면 새 장비가 있던 위치를 우선 사용해 기존 장비를 인벤토리로 돌려보낸다.
- 기존 장비가 해당 위치에 맞지 않으면 모든 인벤토리 컨테이너에서 MxN 크기에 맞는 빈 위치를 검색한다.
- 기존 장비를 돌려놓을 공간이 없으면 장착·인벤토리 상태를 변경하지 않는다.
- 성공 시 인벤토리와 장비창 갱신 Delegate를 호출하고 전투 로그에 사용·장착·교체 결과를 표시한다.
- 좌클릭 Drag & Drop과 Hover 툴팁 동작은 기존 입력 경로를 유지한다.

## 변경 파일

- `Source/Rogue10m/Data/Rogue10mItemDataAsset.h`
- `Source/Rogue10m/Components/Rogue10mInventoryComponent.h`
- `Source/Rogue10m/Components/Rogue10mInventoryComponent.cpp`
- `Source/Rogue10m/UI/Widgets/Rogue10mMenuWindowWidgets.cpp`
- `Scripts/Editor/CreateStarterItemAssets.py`

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공
- 전체 메뉴 Widget Blueprint commandlet 컴파일 성공
- `ValidateMenuWidgetAssets.py`: 0 errors / 0 warnings
- `CreateStarterItemAssets.py` Python 문법 검사 성공
- `git diff --check` 성공

## PIE 수동 확인 항목

1. 체력이 감소한 상태에서 포션 우클릭 시 체력이 설정값만큼 회복되고 수량이 1 감소하는지 확인한다.
2. 최대 체력에서 포션을 우클릭해도 수량이 감소하지 않는지 확인한다.
3. 빈 장비 부위에 장비를 우클릭하면 즉시 장착되고 인벤토리에서 제거되는지 확인한다.
4. 같은 부위의 다른 장비를 우클릭하면 새 장비가 장착되고 기존 장비가 MxN 크기로 인벤토리에 돌아오는지 확인한다.
5. 기존 장비를 돌려놓을 빈 공간이 없을 때 두 아이템의 상태가 바뀌지 않는지 확인한다.
6. 우클릭 기능 추가 후에도 좌클릭 Drag & Drop과 Hover 툴팁이 정상 동작하는지 확인한다.
