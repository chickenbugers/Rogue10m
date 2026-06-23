# Rogue10m Codex Harness

This file defines the working rules for Codex in this Unreal Engine 5.7 project.
It adapts the harness engineering ideas from `BaeJongHo/Unreal_Harness` to this repository.

## Project Facts

- Project name: `Rogue10m`
- Unreal version: UE 5.7
- Engine root: `D:\Program Files\UE_5.7`
- Project root: `D:\Project\Rogue10m`
- Project file: `D:\Project\Rogue10m\Rogue10m.uproject`
- Runtime module: `Source/Rogue10m`
- Module API macro: `ROGUE10M_API`
- Primary branch flow: `feature/*`, `content/*`, `level/*`, `balance/*`, `fix/*` -> `develop` -> `test` -> `main`

## Harness Pipeline

Use this pipeline for meaningful gameplay, tool, UI, AI, content, level, or balance work:

1. Architect: inspect code and write a plan in `Feature/architect/`.
2. Implementer: make scoped code/config/doc changes.
3. Builder: run the relevant build command and fix local compile issues only.
4. Reviewer: review the diff for Unreal-specific risks.
5. Doc Writer: write the feature result in `Feature/doc/`.

Small typo fixes and tiny mechanical edits can skip the full pipeline, but still follow the Unreal rules below.

## Unreal Engineering Rules

- Gameplay logic belongs in C++; Blueprint children should configure data and assets.
- Expose tuning data with `UPROPERTY(EditAnywhere/EditDefaultsOnly, BlueprintReadOnly, Category="...")`.
- All UObject member references that must be GC-tracked use `UPROPERTY()` and `TObjectPtr<T>`.
- Use `TWeakObjectPtr<T>` for non-owning cached references.
- Prefer Enhanced Input, Subsystems, StateTree, Niagara, MetaSounds, Gameplay Tags, and Data Assets when appropriate.
- Keep Tick disabled by default. Prefer events, delegates, timers, and cached references.
- Do not call `GetWorld()`, player lookups, or subsystem access from constructors.
- Dynamic delegate callbacks must be declared with `UFUNCTION()`.
- Use IWYU includes. Header `*.generated.h` includes must be last.
- Use `UE_LOG(LogRogue10m, ...)`; avoid `LogTemp` for project code.
- Never edit generated or cached outputs: `Intermediate/`, `Binaries/`, `Saved/`, `DerivedDataCache/`, `*.generated.h`, `*.gen.cpp`.
- Do not edit `.uasset` or `.umap` binaries directly. Use Unreal Editor and then commit the resulting assets.

## Build Commands

Editor target build:

```powershell
.\Scripts\BuildEditor.ps1
```

Project file generation after module or `.Build.cs` changes:

```powershell
& "D:\Program Files\UE_5.7\Engine\Build\BatchFiles\Build.bat" -projectfiles -project="D:\Project\Rogue10m\Rogue10m.uproject" -game -engine
```

Before committing, run:

```powershell
.\Scripts\CheckGeneratedChanges.ps1
```

## Feature Artifact Locations

- Architecture plans: `Feature/architect/YYYY-MM-DD_feature-name.md`
- Development summaries: `Feature/doc/YYYY-MM-DD_feature-name.md`
- Harness documentation: `Docs/HarnessPipeline.md`, `Docs/UnrealEngineeringRules.md`

Create a paired feature document set:

```powershell
.\Scripts\NewHarnessFeature.ps1 -Name "player combat"
```
