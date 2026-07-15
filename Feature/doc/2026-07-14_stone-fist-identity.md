# 권(Stone Fist) 아이덴티티 구현 결과

## 완료 항목

- 기존 `T_Identity_StoneFist`를 권 프로필 대표 이미지로 연결했다.
- `DA_SkillProfile_StoneFist`를 만들고 무기 타입을 `Knuckle`, 최대 점프 횟수를 2로 설정했다.
- 기본 바인딩은 Primary=왼손 잽, ChargedSpecial=권압, JumpPrimary=낙권만 사용한다.
- Special과 ChargedPrimary는 바인딩하지 않아 일반 우클릭과 좌클릭 차징을 비활성 상태로 유지한다.
- 왼손 잽의 `NextComboSkill`을 오른손 스트레이트로 연결했다.
- 권 전용 `DA_Dodge_StoneFist`를 만들고 기존 공중 회피 차단 로직과 연결했다.
- `DA_Character_Default.DefaultWeaponType`을 `Knuckle`로 설정하고 BP 캐릭터 CombatComponent에 CharacterData와 권 프로필을 연결했다.
- 활성 프로필의 `MaxJumpCount`가 `ACharacter::JumpMaxCount`에 적용되도록 했다.

## 생성 자산

### 공격

- `DA_Attack_StoneFist_Jab`: 왼손 잽, Primary 1타
- `DA_Attack_StoneFist_Straight`: 오른손 스트레이트, 잽의 2타 콤보
- `DA_Attack_StoneFist_ChargedShockwave`: 권압, ChargedSpecial
- `DA_Attack_StoneFist_JumpSlam`: 낙권, JumpPrimary

### 이동과 프로필

- `DA_Dodge_StoneFist`: 권보
- `DA_SkillProfile_StoneFist`: 권 아이덴티티 프로필
- `DA_Character_Default`: 기본 무기 타입과 프로필 소유

### 아이콘

- `T_Skill_StoneFist_Jab`
- `T_Skill_StoneFist_Straight`
- `T_Skill_StoneFist_ChargedShockwave`
- `T_Skill_StoneFist_Dodge`
- `T_Skill_StoneFist_JumpSlam`
- `T_Skill_StoneFist_DoubleJump`

## 이미지 생성 프롬프트 세트

모든 아이콘은 `T_Identity_StoneFist`의 회색 풍화 석재, 황금빛 균열과 기운, 어두운 액션 RPG UI 스타일을 공통으로 사용했다. 텍스트·테두리·워터마크 없이 밝은 녹색 크로마키 배경으로 생성한 뒤 투명 PNG로 후처리했다.

1. 왼손 잽: 왼쪽에서 오른쪽으로 빠르게 뻗는 돌주먹, 짧은 황금 속도선, 민첩한 1타.
2. 오른손 스트레이트: 정면으로 강하게 꽂히는 무거운 돌주먹, 집중된 황금 충격파, 콤보 마무리.
3. 권압: 뒤로 당겼다가 내지른 돌주먹과 전방으로 퍼지는 원형 풍압, 응축된 황금 기.
4. 권보: 돌 전사의 짧고 빠른 방향 대시 실루엣, 잔상과 황금 파편.
5. 낙권: 공중에서 두 돌주먹을 모아 아래로 내려찍으며 바닥에 원형 균열과 충격파가 퍼지는 장면.
6. 답공: 두 돌 부츠가 공중에서 두 번째 황금 도약 고리를 밟아 상승하는 장면.

## Data Asset과 GAS 판단

현재처럼 Data Asset이 스킬 표시와 밸런스, 입력 슬롯, 몽타주/VFX, Ability Class 선택을 소유하는 방향은 적절하다. 다만 공용 `URogue10mGameplayAbility_Attack`이 CombatComponent 실행을 호출한 직후 종료되므로 현재 GAS 연결은 얇은 어댑터 단계다.

다음 단계에서는 다음과 같이 분리한다.

- Data Asset: 이름, 설명, 아이콘, 피해/범위/차징/콤보 값, 몽타주와 Niagara 참조
- Gameplay Ability: 차징 유지와 해제, 몽타주 구간, Gameplay Event, 취소·예측·복제 흐름
- Gameplay Effect: 스테미나 비용, 쿨다운, 피해, 넉백·경직 같은 효과
- Gameplay Tag: 공격·차징·공중·회피·쿨다운 상태와 상호 배제

특히 권압과 낙권은 여러 프레임의 차징/공중 흐름과 전용 VFX가 필요하므로 전용 Ability로 분리할 우선순위가 높다. 아이콘·설명·수치를 GameplayAbility CDO에 중복 저장하지 않는다.

## 검증

- `Rogue10mEditor Win64 Development` 빌드 성공
- Editor Remote Execution으로 신규 자산 7종과 아이콘 참조 로드 성공
- 권 프로필: `Knuckle`, `MaxJumpCount=2`, 기본 바인딩 3종 확인
- 잽 → 스트레이트 `NextComboSkill` 확인
- CharacterData 기본 무기 `Knuckle` 및 BP CombatComponent 참조 확인
- 기존 `DoDodge()`의 `IsFalling()` 차단 확인

## 아직 필요한 결정

- 권압의 관통 여부, 넉백/경직 세기, 최대 대상 수
- 회피 무적 프레임 적용 여부와 정확한 구간
- 2단 점프 스테미나 소모와 공중 제어량
- 권 아이덴티티 자원의 생성·소모 규칙과 활성화 효과
- 최종 피해, 쿨다운, 스테미나 비용
- 공격 몽타주, Niagara, 사운드 자산
