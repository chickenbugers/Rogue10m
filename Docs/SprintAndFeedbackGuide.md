# 달리기와 화면 피드백 설정 가이드

## 기본 동작

- 왼쪽 `Shift`를 누르면 달리기를 시작하고 떼면 종료합니다.
- 기본 보행 속도는 Character Movement의 `Max Walk Speed`를 사용합니다.
- 기본 달리기 속도는 `900 cm/s`입니다.
- 달리는 동안 스태미나를 초당 `1` 소모합니다.
- 스태미나가 부족하거나 캐릭터가 사망하거나 차단 UI가 열리면 달리기가 자동 종료됩니다.
- 달리기 중에는 카메라 Post Process의 색조, 비네트, 모션 블러, 색수차 효과가 적용됩니다.

## 달리기 수치 수정 위치

Unreal Editor에서 `BP_FirstPersonCharacter`를 열고 `Class Defaults`의 다음 카테고리를 수정합니다.

```text
Rogue10m | Movement | Sprint
```

- `Sprint Walk Speed`: 달리기 이동 속도. 기본값 `900 cm/s`.
- `Sprint Stamina Cost Per Second`: 초당 스태미나 소모량. 기본값 `1.0`.
- `Sprint Stamina Drain Interval`: 소모 타이머 간격. 기본값 `0.1초`.

기본 걷기 속도는 Character Movement 컴포넌트의 다음 항목에서 수정합니다.

```text
Character Movement > Walking > Max Walk Speed
```

## 달리기 Post Process 수정 위치

`BP_FirstPersonCharacter`에서 `Player Feedback Component`를 선택한 뒤 다음 카테고리를 수정합니다.

```text
Rogue10m | Feedback | Sprint
```

- `Sprint Tint`: 달리기 중 화면 색조.
- `Sprint Vignette Intensity`: 가장자리 비네트 강도.
- `Sprint Motion Blur Amount`: 모션 블러 강도.
- `Sprint Chromatic Aberration`: 색수차 강도.

별도 Post Process Volume이나 머티리얼 없이 런타임에 카메라 부착 `UPostProcessComponent`를 생성하며, 피격·저스태미나 효과와 같은 컴포넌트에서 합성합니다.

## 피격 효과 지속시간 수정 위치

피격 화면 효과는 두 경로가 있으며 기본값은 모두 `0.5초`입니다.

### 카메라 Post Process 피격 효과

`BP_FirstPersonCharacter`에서 `Player Feedback Component`를 선택합니다.

```text
Rogue10m | Feedback | Damage > Damage Pulse Duration
```

### HUD 피격 오버레이

`BP_FirstPersonPlayerController`의 `Class Defaults`에서 수정합니다.

```text
Rogue10m | HUD | Damage Feedback > Player Damage Feedback Duration
```

피격 숫자가 떠 있는 시간은 별도 설정입니다.

```text
Rogue10m | HUD | Damage Indicator > Damage Indicator Duration
```

이 값은 데미지 숫자 위젯의 표시 시간이며 화면 피격 효과 0.5초와는 독립적입니다.

## 방향 회피 설정

스페이스 바를 누르면 회피합니다. WASD 입력이 있으면 해당 방향, 입력이 없으면 전방으로 이동합니다.

`BP_FirstPersonCharacter > Class Defaults > Rogue10m | Movement | Dodge`

- `Dodge Speed`: 기본 1800cm/s
- `Dodge Duration`: 기본 0.16초
- `Dodge Cooldown`: 기본 0.45초
