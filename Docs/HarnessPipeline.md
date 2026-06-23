# Harness Pipeline

This project uses a lightweight harness for Unreal Engine 5.7 development.
It is based on the role-separated pipeline described in `BaeJongHo/Unreal_Harness`, adapted for Codex and this repository.

## Purpose

Unreal work mixes C++, Blueprints, binary assets, generated code, editor-only state, and long builds.
The harness keeps those concerns separated so feature work is easier to review, test, and recover.

## Pipeline Stages

| Stage | Role | Output |
| --- | --- | --- |
| 1 | Architect | `Feature/architect/<date>_<feature>.md` plan |
| 2 | Implementer | Focused source/config/doc changes |
| 3 | Builder | Build result and compile-only fixes |
| 4 | Reviewer | Unreal-specific review findings |
| 5 | Doc Writer | `Feature/doc/<date>_<feature>.md` result summary |

## Stage 1: Architect

Architect work is read-first and design-focused.

- Inspect existing classes, modules, config, assets referenced by code, and branch context.
- Ask questions when the request changes core design, player experience, or technical ownership.
- Prefer UE 5.7 systems over legacy approaches.
- Decide the C++ / Blueprint boundary.
- Write a plan before implementation.

The plan must include:

- feature summary
- touched modules and files
- new classes and responsibilities
- UPROPERTY/UFUNCTION exposure
- build dependencies
- validation plan
- risks and open questions

## Stage 2: Implementer

Implementation follows the plan.

- Keep changes scoped.
- Put gameplay behavior in C++.
- Use BP subclasses for data, asset references, and tuning.
- Avoid direct binary asset edits.
- Update `.Build.cs` only when a dependency is truly required.

## Stage 3: Builder

Builder work proves the project compiles.

Default command:

```powershell
.\Scripts\BuildEditor.ps1
```

If `.Build.cs` or module layout changed, regenerate project files first.
Compile fixes should be mechanical: includes, module dependencies, generated header order, export macros, or local const/type corrections.
Design changes go back to the Architect/Implementer stage.

## Stage 4: Reviewer

Review starts from the diff and focuses on Unreal-specific failure modes:

- GC tracking with `UPROPERTY` and `TObjectPtr`
- constructor vs `BeginPlay` responsibilities
- dynamic delegate `UFUNCTION` requirements
- Enhanced Input mapping and binding flow
- Tick usage and expensive per-frame lookups
- module boundaries and IWYU
- hard-coded asset paths
- generated/cache/binary file churn

Findings should identify file and line, severity, impact, and a concrete fix.

## Stage 5: Doc Writer

Write the result to `Feature/doc/`.

Include:

- what changed
- why it changed
- files touched
- build result
- review result
- editor or Blueprint setup still required
- test notes for the next playtest

## When To Use The Full Pipeline

Use the full pipeline for:

- new gameplay systems
- player controls and combat
- AI, spawning, StateTree, or behavior logic
- HUD and menu flows
- save/progression systems
- room generation and level flow
- asset organization changes
- packaging, build, or automation changes

Tiny fixes can be direct, but still run the relevant build/check before calling the work done.
