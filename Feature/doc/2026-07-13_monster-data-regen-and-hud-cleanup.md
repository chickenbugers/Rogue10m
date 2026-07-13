# 몬스터 데이터, 자원 회복, HUD 정리 결과

## 구현 결과

- URogue10mMonsterDataAsset을 추가해 몬스터 이름/레벨/경험치 보상/체력/스테미나/마나/초당 회복/외형/AnimBP/AI/공격 설정을 데이터화했다.
- 플레이어와 몬스터에 URogue10mVitalRegenerationComponent를 부착했다.
- 회복은 Tick 대신 서버의 1초 타이머를 사용하며 기본값은 체력, 스테미나, 마나 각각 초당 0.1이다.
- 몬스터 사망 시 마지막 공격자의 PlayerState에 Data Asset의 경험치 보상을 지급한다.
- MonsterInfo 이름은 LV N : 몬스터 이름으로 표시하고 체력 게이지/정수 체력을 직접 바인딩했다.
- Monster Property 텍스트는 네이티브 바인딩 대상에서 제외했다.
- MainHUD의 MiniMap 갱신 및 프로토타입 생성을 임시 중지했다.
- 인벤토리 아이템 추가 성공 시 획득 피드에 아이콘과 수량을 전달한다.
- 획득 라인은 이름을 숨기고 이미지 | x수량만 표시한다.

## 검증

- Rogue10mEditor Win64 Development 빌드 성공.
- 상세 구조는 Docs/CurrentProjectArchitecture.md 참고.

## 에디터 후속 설정

- 몬스터 Blueprint에 생성한 Monster Data Asset을 지정한다.
- WBP 위젯 변수 이름은 구조 문서의 에디터 설정 절을 따른다.
- UI_FeedGuideText, UI_FeedLineMockup, UI_FeedLineText는 C++ 런타임 의존성이 없어 제거 가능하다.