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

The high-level game development pipeline is:

1. Pre-production: define the idea, scope, constraints, and design plan.
2. Production: implement features and run in-development testing.
3. Testing: run QA-style verification on integrated work.
4. Distribution: prepare final builds and release candidates.
5. Post-Production: track DLC, patches, and live fixes.

Codex feature work maps onto that production flow:

1. Architect: inspect code and write a plan in `Feature/architect/` (Pre-production).
2. Implementer: make scoped code/config/doc changes (Production).
3. Builder: run the relevant build command and fix local compile issues only (In-development testing).
4. Reviewer: review the diff for Unreal-specific risks (Testing / QA).
5. Doc Writer: write the feature result in `Feature/doc/` and update `DevLog/YYYYMMDD.txt`.

Small typo fixes and tiny mechanical edits can skip the full pipeline, but still follow the Unreal rules below.

## Required Per-Command Checklist

Check this list on every user command before finishing:

- Confirm whether the request changes code, config, docs, branch state, build settings, assets, or project structure.
- If anything changed, update the daily development log in `DevLog/YYYYMMDD.txt`.
- If the daily log file does not exist, create `DevLog/` and the date file first.
- Append to the existing daily file instead of replacing prior entries.
- Write DevLog entries in a Tistory-friendly format.
- When a feature is developed, add a clearly marked Notion summary candidate inside the DevLog.
- Before any GitHub commit or push summary, list the changed files and a short explanation.
- Do not commit unless the user explicitly confirms.
- Write every new DevLog entry in Korean.
- Always switch to an appropriate work branch before feature development.

## Unreal Engineering Rules

- Gameplay logic belongs in C++; Blueprint children should configure data and assets.
- Expose tuning data with `UPROPERTY(EditAnywhere/EditDefaultsOnly, BlueprintReadOnly, Category="...")`.
- All UObject member references that must be GC-tracked use `UPROPERTY()` and `TObjectPtr<T>`.
- Use `TWeakObjectPtr<T>` for non-owning cached references.
- Prefer Enhanced Input, Subsystems, StateTree, Niagara, MetaSounds, Gameplay Tags, and Data Assets when appropriate.
- Prefer `PlayerState` for persistent player-facing character state, and split growing feature logic into dedicated `ActorComponent` classes.
- Prefer `Data Asset` definitions for future gameplay data such as weapons, skills, items, monsters, balance values, and NPC setup.
- For Unreal-specific guidance, prefer the Epic community assistant URL: `https://dev.epicgames.com/community/assistant/unreal-engine/conversation`.
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
