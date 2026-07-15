# UE 5.8 MCP Setup

## 적용 상태

- 프로젝트 엔진 연결을 UE 5.8로 변경했다.
- 엔진 경로 기준: `D:\Program Files\UE_5.8`
- `Rogue10m.uproject`에서 다음 실험 플러그인을 활성화했다.
  - `ModelContextProtocol`
  - `MCPClientToolset`

## 플러그인 위치

- Unreal MCP: `D:\Program Files\UE_5.8\Engine\Plugins\Experimental\ModelContextProtocol\ModelContextProtocol.uplugin`
- MCP Client Toolset: `D:\Program Files\UE_5.8\Engine\Plugins\Experimental\Toolsets\MCPClientToolset\MCPClientToolset.uplugin`

## 기본 서버 설정 메모

`ModelContextProtocol` 플러그인의 서버 설정은 `EditorPerProjectUserSettings` 성격이다.
따라서 저장소 기본 설정에는 플러그인 활성화만 반영하고, 서버 자동 시작과 포트는 에디터 환경에서 조정한다.

- 기본 URL 경로: `/mcp`
- 기본 포트: `8000`
- 자동 시작 기본값: 꺼짐
- 도구 검색 기본값: 켜짐

에디터에서 MCP 서버를 자동 시작하려면 `Project Settings > Model Context Protocol`에서 서버 설정을 확인한다.
외부 MCP 클라이언트를 연결할 때는 로컬 개발 환경에서만 포트를 열고, 필요한 경우 승인된 도구만 노출한다.

## Rogue10m에서 우선 활용할 작업

- C++ 클래스와 Widget Blueprint 바인딩 상태 점검
- Data Asset 참조 누락과 미사용 에셋 탐색
- 공격/아이템/몬스터 데이터 초안 생성 보조
- 빌드 로그와 PIE 오류 분석
- DevLog와 Feature 문서 요약 자동화
- 에셋 생성 요청 시 이미지 `png`, 음성 `wav` 기본 형식 확인

## 검증 기록

- 프로젝트 파일 재생성: 성공
- UE 5.8 `Rogue10mEditor Win64 Development` 빌드: 성공
- `CheckGeneratedChanges.ps1`: 통과
- 참고 경고: Android SDK 설정이 없어 프로젝트 파일 생성 중 Android 플랫폼이 스킵되었다.
- 참고 경고: Variant 템플릿 삭제분에 대한 바이너리 에셋 경고가 표시되었으나, 이전 최적화 작업의 의도된 삭제 항목이다.
