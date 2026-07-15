# 점프·구르기 입력 재배치 설계

## 목표

- Space 입력은 기본 점프를 수행한다.
- E 입력은 현재 방향 회피 로직을 사용하는 구르기를 수행한다.
- HUD 입력 안내가 실제 동작과 일치한다.

## 변경 영역

- `ARogue10mCharacter::SetupPlayerInputComponent`
- 점프 시작/종료 래퍼 함수
- 스킬 슬롯 다섯 번째 입력 안내

## 완료 조건

- Space Press: `Jump()`
- Space Release: `StopJumping()`
- E Press: `DoDodge()`
- 스킬 슬롯: `스페이스 / 점프`

## 검증

- Rogue10mEditor Win64 Development 빌드
- PIE에서 점프, 방향 구르기, 구르기 쿨타임 확인
