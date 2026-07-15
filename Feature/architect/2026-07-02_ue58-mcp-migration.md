# UE 5.8 MCP 전환 설계

## 목표

Rogue10m 프로젝트를 UE 5.7 기반 설정에서 UE 5.8 기반 설정으로 전환하고, UE 5.8에 포함된 MCP 실험 플러그인을 사용할 수 있도록 프로젝트에 장착한다.

## 범위

- `Rogue10m.uproject` 엔진 연결을 5.8로 변경
- `ModelContextProtocol`, `MCPClientToolset` 플러그인 활성화
- 빌드/패키징 스크립트 엔진 경로를 `D:\Program Files\UE_5.8`로 변경
- Target 설정을 UE 5.8 빌드 기본값에 맞게 조정
- 이전 Variant 템플릿 정리로 남은 include path 경고 제거
- MCP 사용 가이드 문서 추가

## 제외 범위

- `.uasset`, `.umap`의 UE 5.8 저장 변환은 에디터에서 별도 확인한다.
- Android SDK 설치와 패키징 검증은 이번 작업에서 제외한다.
- MCP 서버 자동 시작 설정은 사용자 에디터 환경 설정으로 남긴다.

## 검증 계획

- UE 5.8 Build.bat로 프로젝트 파일 재생성
- `Scripts/BuildEditor.ps1` 실행
- `Scripts/CheckGeneratedChanges.ps1` 실행

## 위험 요소

- UE 5.8에서 저장한 에셋은 UE 5.7로 되돌리기 어려울 수 있다.
- MCP는 실험 기능이므로 main 병합 전 에디터에서 플러그인 UI와 서버 동작을 직접 확인해야 한다.
- Android SDK가 현재 설정되어 있지 않아 Android 패키징은 별도 환경 세팅이 필요하다.
