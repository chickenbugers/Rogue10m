# 플레이어 스탯·애니메이션·화면 피드백 구현 결과

## 구조 검증 결론

현재 리팩터링 방향은 적절합니다.

- `ARogue10mPlayerState`가 ASC와 AttributeSet을 소유해 Pawn 교체와 무관한 플레이어 수치를 유지합니다.
- `URogue10mAttributeSet`이 체력, 스테미나, 마나, 아이덴티티, 성장, 공격속도의 단일 수치 원본입니다.
- `URogue10mCombatComponent`가 공격 입력 해석 이후 자원 비용, 공격 실행, 콤보, 쿨다운을 담당합니다.
- `URogue10mPlayerFeedbackComponent`가 로컬 플레이어의 화면 피드백만 담당합니다.
- `ARogue10mCharacter`는 입력 전달, 컴포넌트 조립, 피해 진입점, Pawn 수명주기만 연결합니다.

검증 과정에서 기존 Attribute에 GAS 복제 알림이 없던 점을 보완했습니다. 모든 Attribute는 `ReplicatedUsing`과 `GAMEPLAYATTRIBUTE_REPNOTIFY`를 사용합니다.

## 공격속도 연동

`AttackSpeedMultiplier` Attribute를 추가했습니다.

- 기본값: `1.0`
- 허용 범위: `0.1` ~ `5.0`
- 공격 몽타주 `Montage_Play` 재생률에 적용
- 공유 공격 쿨다운에 역비례 적용
- 콤보 입력 창 시작/종료 시간에 역비례 적용

따라서 `2.0`이면 몽타주와 관련 공격 타이밍이 모두 2배 빠르고, `0.5`이면 모두 절반 속도로 동작합니다.

## 포스트프로세스 피드백

`URogue10mPlayerFeedbackComponent`는 로컬 Pawn에 런타임 `UPostProcessComponent`를 생성합니다. 별도 Material이나 바이너리 에셋은 필요하지 않습니다.

- 피해: 피해량에 비례한 적색 틴트, 비네트, 색수차 펄스
- 낮은 스테미나: 25% 아래에서 점진적인 탈채도, 청회색 틴트, 비네트
- 스테미나 비용 부족: 강한 단기 스테미나 펄스
- 성능: 상시 Tick 없음, GAS Attribute 변경 델리게이트 사용, 펄스 감쇠 중에만 약 30Hz 타이머 사용

Blueprint의 Character 기본값에서 `Player Feedback Component`를 선택하면 임계값, 색상, 지속시간, 비네트와 색수차 강도를 조정할 수 있습니다.

## 검증

- `Scripts/BuildEditor.ps1`: 성공
- UHT: 성공
- `Rogue10mEditor Win64 Development`: 컴파일 및 링크 성공
- 첫 빌드에서 UE 5.8 `UPostProcessComponent` 외부 상속 링크 제약을 확인했고, 상속 대신 컴포넌트 조합 방식으로 변경해 해결했습니다.

## 에디터 확인 항목

- PIE에서 피해량 10/50에 따른 펄스 강도 차이
- 스테미나 25% 경계 진입/회복 시 효과 전환
- 스테미나 부족 공격 시 펄스 발생
- `AttackSpeedMultiplier` 0.5/1.0/2.0에서 몽타주와 콤보/쿨다운 동기화
