# Git Branch Strategy

## Branch Roles

- `main`: Stable milestone branch. Only merge builds that are playable, backed up, and safe to show.
- `develop`: Daily integration branch. Code, Blueprint, content, and level work come together here first.
- `test`: Playtest branch. Promote from `develop` when a build should be tested as a whole.
- `release/<version>`: Temporary stabilization branch for packaged builds, demos, or milestones.
- `hotfix/<name>`: Urgent fixes branched from `main` when the stable build itself is broken.

## Work Branches

Create short-lived branches from `develop`.

- `feature/<name>`: Gameplay systems, UI, AI, combat, progression, tools.
- `content/<name>`: Art, audio, animation, VFX, materials, imported assets.
- `level/<name>`: Maps, rooms, blockouts, lighting, encounter layout.
- `balance/<name>`: Tuning values, enemy stats, item tables, spawn pacing.
- `fix/<name>`: Bugs found during development or playtesting.

## Default Flow

1. Start new work from `develop`.
2. Merge completed work back into `develop`.
3. Promote `develop` into `test` for playable test builds.
4. Test and fix on branches made from `develop`.
5. Merge `test` into `main` only when the build is stable.

## Promotion Rules

- `develop` can be messy but should compile.
- `test` should launch, load the default map, and support a real playtest pass.
- `main` should be a known-good checkpoint.
- Large binary asset changes should stay focused in `content/*` or `level/*` branches to avoid painful conflicts.
- Do not merge generated folders such as `Binaries`, `Intermediate`, `Saved`, or `DerivedDataCache`.

## Commands

Create a gameplay feature branch:

```powershell
git switch develop
git pull
git switch -c feature/player-combat
```

Create a content branch:

```powershell
git switch develop
git pull
git switch -c content/weapon-placeholders
```

Create a level branch:

```powershell
git switch develop
git pull
git switch -c level/prototype-arena
```

Push a new work branch:

```powershell
git push -u origin feature/player-combat
```

Promote daily integration to playtest:

```powershell
git switch test
git pull
git merge develop
git push
```

Promote a tested build to stable:

```powershell
git switch main
git pull
git merge test
git push
```

## Naming Examples

- `feature/run-hud`
- `feature/player-combat`
- `feature/enemy-spawner`
- `feature/upgrade-selection`
- `content/first-weapon-set`
- `content/temp-audio`
- `level/prototype-arena`
- `level/room-blockouts`
- `balance/early-run-pacing`
- `fix/timer-expiry`
