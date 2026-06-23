# Git Branch Strategy

## Branch Roles

- `main`: Stable project history. Keep this branch playable and suitable for backup or release points.
- `test`: Integration branch for playable testing. Merge completed feature branches here first.
- `feature/<name>`: Short-lived feature work branched from `test`.
- `fix/<name>`: Bug fixes branched from `test` unless the bug is already in `main`.
- `release/<version>`: Optional stabilization branch when preparing a build.

## Default Flow

1. Update local branches.
2. Create a feature branch from `test`.
3. Commit focused changes to the feature branch.
4. Merge or open a PR from `feature/<name>` into `test`.
5. Test the integrated build on `test`.
6. Merge `test` into `main` when the build is stable.

## Commands

Create a feature branch:

```powershell
git switch test
git pull
git switch -c feature/player-combat
```

Push a new feature branch:

```powershell
git push -u origin feature/player-combat
```

After a feature is validated:

```powershell
git switch test
git merge feature/player-combat
git push
```

Promote tested work to `main`:

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
- `feature/room-generation`
- `feature/upgrade-selection`
- `fix/timer-expiry`
