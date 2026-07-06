# Rogue10m Profiling Records

이 폴더는 Rogue10m의 성능 측정 결과를 날짜별로 기록하기 위한 공간이다.

## 기록 원칙

- 프로파일링은 같은 조건에서 반복 측정한 값을 남긴다.
- 측정 환경, 맵, 플레이 상황, 해상도, 그래픽 품질, 플랫폼을 함께 기록한다.
- 수치만 남기지 않고 체감 문제와 다음 조치도 함께 적는다.
- Unreal Insights, Stat 명령, GPU Visualizer, packaged build 결과를 구분해서 기록한다.

## 권장 파일명

```text
YYYYMMDD_profiling.md
```

예시:

```text
20260706_profiling.md
```

## 기본 측정 명령

Unreal Editor 콘솔 또는 게임 실행 중 콘솔에서 사용한다.

```text
stat fps
stat unit
stat unitgraph
stat game
stat slate
stat gpu
stat rhi
stat memory
```

Unreal Insights용 Trace는 상황에 맞게 사용한다.

```text
trace.start cpu,gpu,frame,bookmark,file
trace.bookmark Rogue10m_Profile_Start
trace.stop
```

GPU 세부 분석이 필요하면 GPU Visualizer를 사용한다.

```text
profilegpu
```

## 측정 우선순위

1. 메인 HUD 표시 상태의 기본 프레임
2. 인벤토리, 장비창, 스킬트리, 설정창을 연 상태의 UI 비용
3. 전투 입력과 스킬 쿨타임 UI 갱신 비용
4. 몬스터 1마리, 10마리, 30마리 기준 AI/애니메이션 비용
5. 시작 맵과 플레이 맵 이동 후 20분 타이머/HUD 비용
6. Windows 패키징 빌드 기준 실측
7. Android 모바일 모드 기준 실측
