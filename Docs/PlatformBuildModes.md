# 플랫폼 빌드 모드 설계

Rogue10m은 같은 게임 로직을 유지하면서 Windows 데스크탑과 Android 모바일을 분리해서 준비한다.

## 지원 모드

- Windows 데스크탑
  - 대상 플랫폼: `Win64`
  - 기본 입력: 키보드, 마우스, 게임패드
  - 기본 품질: 데스크탑 고품질 렌더링
  - 설정 파일: `Config/Windows/WindowsEngine.ini`, `Config/Windows/WindowsInput.ini`
  - 패키징 명령: `.\Scripts\PackageWindows.ps1`

- Android 모바일
  - 대상 플랫폼: `Android`
  - 기본 입력: 터치 인터페이스, 가상 조이스틱, 모바일 버튼 UI
  - 기본 품질: 모바일 확장성 우선 렌더링
  - 설정 파일: `Config/Android/AndroidEngine.ini`, `Config/Android/AndroidInput.ini`
  - 패키징 명령: `.\Scripts\PackageAndroid.ps1`

## 런타임 플랫폼 판별

`URogue10mPlatformModeLibrary`를 사용해 현재 실행 모드를 확인한다.

- `GetCurrentPlatformMode()`: 현재 플랫폼 모드를 enum으로 반환한다.
- `IsDesktopMode()`: Windows 데스크탑이면 true를 반환한다.
- `IsMobileMode()`: Android 모바일이면 true를 반환한다.
- `GetCurrentPlatformModeText()`: UI 표시용 텍스트를 반환한다.

이 함수들은 Blueprint에서도 호출할 수 있으므로 HUD, 설정창, 입력 프롬프트, 모바일 전용 조작 UI에서 공통으로 사용할 수 있다.

## 앞으로 연결할 항목

- 모바일 전용 HUD: 이동 조이스틱, 시점 조작 패드, 공격/회피/점프 버튼
- 데스크탑 전용 HUD: 키보드 단축키, 마우스 조준, 커서 잠금 방식
- 플랫폼별 성능 옵션: 그림자, 포스트 프로세스, 이펙트 수량, 몬스터 동시 수
- 플랫폼별 저장 위치와 해상도 정책
- Android 서명 키, 패키지명, 버전 코드, 스토어 업로드 설정

## 주의 사항

- `Config/Android/AndroidEngine.ini`의 SDK 버전과 패키지명은 실제 배포 계정과 Android SDK 설치 상태에 맞춰 최종 확정해야 한다.
- Android 패키징은 로컬에 Android SDK, NDK, JDK 설정이 완료되어 있어야 성공한다.
- Windows 패키징은 현재 에디터 빌드가 성공하는 상태에서 먼저 검증하고, Android는 별도 환경 검증 후 진행한다.
