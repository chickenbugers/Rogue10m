# 프레임워크 구조 단일화 계획

## 목표

Rogue10m의 임시 이중 구조를 제거하고 GAS, 전투 컴포넌트, UMG를 중심으로 기능 소유권을 단일화한다.

## Ultrawork 패킷

### 패킷 1: GAS AttributeSet 단일화

- 목표: 플레이어와 몬스터의 체력/자원 및 플레이어 성장 수치를 `URogue10mAttributeSet`에서만 관리한다.
- 수정 위치: `Ability`, `Core/Rogue10mPlayerState`, `Character`, `Enemy`, `UI/Rogue10mRunHUD`
- 완료 조건: `URogue10mVitalsComponent` 참조가 런타임 코드에서 제거되고 모든 수치 Getter가 AttributeSet을 사용한다.
- 검증: `rg "VitalsComponent" Source/Rogue10m`, Editor Target 빌드
- 롤백 경계: AttributeSet과 Vitals 교체 변경 전체

### 패킷 2: 전투 로직 CombatComponent 이전

- 목표: 공격 입력 상태, Data Asset 선택, 자원 검사/소비, GAS Ability 실행, 판정, 콤보, 쿨타임, 전투 로그를 CombatComponent가 담당한다.
- 수정 위치: `Components/Rogue10mCombatComponent`, `Character/Rogue10mCharacter`, `Ability/Rogue10mGameplayAbility_Attack`
- 완료 조건: Character는 입력 전달과 생명주기만 담당하고 공격 실행 세부 구현을 갖지 않는다.
- 검증: Character의 공격 구현 검색, Editor Target 빌드
- 롤백 경계: CombatComponent 공개 API와 Character 공격 위임 변경

### 패킷 3: Canvas HUD 제거

- 목표: `ARogue10mHUD`의 Canvas 렌더링과 상태 소유를 제거하고 PlayerController와 `URogue10mRunHUD`로 통합한다.
- 수정 위치: `Core/Rogue10mPlayerController`, `Core/Rogue10mGameMode`, `UI`, `Character`, `Enemy`
- 완료 조건: `Canvas`, `DrawHUD`, `ARogue10mHUD` 런타임 참조가 없고 `UW_Rogue10mMainWidget`이 HUD 데이터를 직접 조회한다.
- 검증: `rg "ARogue10mHUD|DrawHUD|Engine/Canvas" Source/Rogue10m`, Editor Target 빌드
- 롤백 경계: Canvas HUD 삭제와 UMG 상태 서비스 이전

### 패킷 4: UTF-8 복구

- 목표: 손상된 C++ 주석/기본 문자열과 핵심 개발 문서를 UTF-8 한국어로 복구한다.
- 수정 위치: `Source/Rogue10m`, 핵심 `Docs`
- 완료 조건: 알려진 모지바케 패턴이 소스와 갱신 문서에서 검출되지 않는다.
- 검증: 모지바케 패턴 검색, `git diff --check`
- 롤백 경계: 파일별 문자열 및 문서 변경

## 최종 소유권

- `PlayerState`: ASC, AttributeSet, 캐릭터 메타 정보, 장착 무기 타입, 아이덴티티 타입
- `AttributeSet`: 체력, 스테미나, 마나, 아이덴티티 수치, 레벨, 경험치
- `Character`: 이동, 카메라, 입력 전달, 사망 생명주기
- `CombatComponent`: 공격 실행 전체
- `InventoryComponent`: 아이템과 장비 상태
- `PlayerController`: 패널 상태, 설정, 로그, HUD 생성과 입력 모드
- `RunHUD/MainHUDWidget`: View 변환과 Widget Blueprint 바인딩
