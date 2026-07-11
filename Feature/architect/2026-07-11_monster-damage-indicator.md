# 몬스터 데미지 인디케이터 설계

## 문제 확인

`ARogue10mBasicMonster::TakeDamage()`는 이미 `ARogue10mPlayerController::AddFloatingDamageNumber()`를 호출하고 있었습니다. 그러나 PlayerController는 피해 엔트리를 배열에 저장할 뿐 이를 화면에 표시하는 Widget 또는 렌더링 경로가 없었습니다.

## Ultrawork Packets

### Packet 1 — 코드 기반 피해 숫자 위젯

- 목표: 별도 Widget Blueprint 없이 동작하는 상승·페이드 피해 숫자를 만든다.
- 수정 위치: `Source/Rogue10m/UI/Widgets/Rogue10mDamageIndicatorWidget.*`
- 완료 조건: 피해량 텍스트가 생성되고 약 1.2초 동안 상승한 뒤 제거된다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: 신규 위젯 파일만 제거할 수 있다.

### Packet 2 — 월드 위치 표시와 네트워크 전달

- 목표: 몬스터 머리 위 위치에 Screen Space WidgetComponent를 생성한다.
- 수정 위치: `Source/Rogue10m/Core/Rogue10mPlayerController.*`
- 완료 조건: 로컬 공격은 즉시 표시되고, 원격 클라이언트는 Client RPC로 동일한 월드 위치를 전달받는다.
- 검증 명령: `Scripts/BuildEditor.ps1`
- 롤백 경계: PlayerController의 표시 생성부만 되돌릴 수 있다.

### Packet 3 — 검증과 문서화

- 목표: 빌드, 생성물 검사, 수명주기 리뷰, DevLog 기록을 완료한다.
- 수정 위치: `Feature/doc/`, `DevLog/20260711.txt`
- 완료 조건: Editor 타깃 빌드와 `CheckGeneratedChanges.ps1`이 통과한다.
- 검증 명령: `Scripts/CheckGeneratedChanges.ps1`
- 롤백 경계: 문서 변경만 되돌릴 수 있다.

## 구현 원칙

- 몬스터 클래스는 피해 적용과 공격자 Controller 통지만 담당한다.
- 표시 생성과 로컬/원격 플레이어 분기는 PlayerController가 담당한다.
- 위젯은 자신의 애니메이션과 WidgetComponent 제거를 담당한다.
- 상시 Actor Tick이나 바이너리 에셋 변경을 추가하지 않는다.
