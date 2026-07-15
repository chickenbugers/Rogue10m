# lazy codex harness - Architecture Plan

## Feature Summary

LazyCodex 글을 참고해 Rogue10m의 기존 Codex Harness에 작업 분해, 반복 검증, 역할 라우팅, 비용 가드 규칙을 추가한다.

외부 LazyCodex 패키지를 설치하지 않고, 프로젝트 문서와 기능 템플릿에 로컬 하네스 규칙으로 장착한다.

## Existing Context

- `AGENTS.md`는 이미 Architect, Implementer, Builder, Reviewer, Doc Writer 파이프라인을 정의한다.
- `Docs/HarnessPipeline.md`는 게임 개발 파이프라인과 Codex 역할을 연결한다.
- `Scripts/NewHarnessFeature.ps1`는 기능별 `Feature/architect`, `Feature/doc` 문서를 생성한다.
- 매 변경은 `DevLog/YYYYMMDD.txt`에 한국어로 기록해야 한다.

## Technical Approach

- LazyCodex를 Codex 위에 얹는 얇은 자동화/운용 계층으로 해석한다.
- `ultrawork`는 큰 기능을 검증 가능한 Work Packet으로 나누는 규칙으로 적용한다.
- `ulw-loop`는 Scope Gate부터 Exit Gate까지 반복 검증하는 규칙으로 적용한다.
- 멀티모델 라우팅은 실제 모델 선택 기능이 아니라, Rogue10m 하네스의 역할 전환 규칙으로 적용한다.

## Lazy Codex Ultrawork Packets

| Packet | Goal | Touched Area | Completion Condition | Validation |
| --- | --- | --- | --- | --- |
| 1 | LazyCodex 개념 문서화 | `Docs/LazyCodexHarness.md` | Rogue10m용 overlay 규칙 문서가 추가됨 | 문서 검토 |
| 2 | 기존 하네스에 overlay 연결 | `AGENTS.md`, `Docs/HarnessPipeline.md`, `Feature/README.md` | 매 작업 체크리스트와 파이프라인에서 Lazy overlay를 참조함 | `rg "Lazy Codex"` |
| 3 | 기능 템플릿 보강 | `Scripts/NewHarnessFeature.ps1` | 새 기능 문서에 Ultrawork/ULW Exit Gate 항목이 생성됨 | 스크립트 실행 |
| 4 | 작업 결과 기록 | `Feature/doc`, `DevLog` | 개발 요약과 오늘 로그가 한국어로 갱신됨 | `CheckGeneratedChanges.ps1` |

## Affected Files

- `AGENTS.md`
- `Docs/HarnessPipeline.md`
- `Docs/LazyCodexHarness.md`
- `Feature/README.md`
- `Scripts/NewHarnessFeature.ps1`
- `Feature/architect/2026-07-01_lazy-codex-harness.md`
- `Feature/doc/2026-07-01_lazy-codex-harness.md`
- `DevLog/20260701.txt`

## C++ / Blueprint Boundary

해당 작업은 하네스 문서와 스크립트 보강이다.

- C++ 변경 없음
- Blueprint 변경 없음
- Unreal Editor 에셋 변경 없음

## Implementation Steps

1. 참고 글에서 LazyCodex 핵심 개념을 확인한다.
2. Rogue10m에 맞는 Lazy Codex Overlay 문서를 추가한다.
3. 기존 하네스 문서와 AGENTS 규칙에서 overlay를 참조하게 한다.
4. 기능 생성 스크립트 템플릿에 Ultrawork Packet과 ULW Exit Gate를 추가한다.
5. Feature 문서와 DevLog를 갱신한다.
6. 하네스 체크를 실행한다.

## Build And Validation Plan

- 문서/스크립트 중심 변경이므로 Unreal Editor 빌드는 필수 아님.
- `Scripts/NewHarnessFeature.ps1 -Name "lazy codex harness"` 실행으로 템플릿 검증.
- `Scripts/CheckGeneratedChanges.ps1` 실행으로 하네스 경로 규칙 검증.

## ULW Loop Exit Gate

- [x] Scope is clear.
- [x] Packets are small enough to validate.
- [x] Build/check command is selected.
- [x] Review risks are listed.
- [x] DevLog update is planned.

## Risks And Questions

- 외부 LazyCodex 도구를 설치하지 않았기 때문에 실제 LazyCodex CLI 기능은 제공하지 않는다.
- 현재 적용 범위는 문서화된 로컬 하네스 규칙과 기능 템플릿 보강이다.
- 추후 사용자가 원하면 별도 작업으로 LazyCodex 설치/검증을 진행한다.
