# Git Branch Strategy

## Branch Roles

- `main`: Stable milestone branch. Only merge builds that are playable, backed up, and safe to show.
- `develop`: Daily integration branch. Code, Blueprint, content, and level work come together here first.
- `test`: Playtest branch. Promote from `develop` when a build should be tested as a whole.
- `release/<version>`: Temporary stabilization branch for packaged builds, demos, or milestones.
- `hotfix/<name>`: Urgent fixes branched from `main` when the stable build itself is broken.

## Work Branches

Create short-lived branches from `develop` using `Sprint#<sprint>-<work>-<short-description>`.

- `Sprint#<sprint>-<work>-<short-description>`: Short-lived work branch for every feature, content, level, balance, and fix task.
- Keep the Sprint number fixed while work is promoted through `develop` and `test`.
- Increment only the work number for independent work branches in the current Sprint.
- Increment the Sprint number only after that Sprint is successfully merged into `main`; then reset the work number to `1`.
- Use a short English kebab-case description.
- Examples: `Sprint#1-1-item-quick-slot`, `Sprint#1-2-sprint-postprocess`, `Sprint#2-1-monster-ai`.

## Default Flow

1. Start new work from `develop`.
2. Merge completed work back into `develop`.
3. Promote `develop` into `test` for playable test builds.
4. Test and fix on branches made from `develop`.
5. Merge `test` into `main` only when the build is stable.
6. After the successful `main` merge, close the current Sprint; the next work branch uses the next Sprint number and work number `1`.

## Promotion Rules

- `develop` can be messy but should compile.
- `test` should launch, load the default map, and support a real playtest pass.
- `main` should be a known-good checkpoint.
- Large binary asset changes should stay focused in `content/*` or `level/*` branches to avoid painful conflicts.
- Do not merge generated folders such as `Binaries`, `Intermediate`, `Saved`, or `DerivedDataCache`.

## Commands

Create a work branch:

```powershell
git switch develop
git pull
git switch -c "Sprint#1-1-item-quick-slot"
```

Push a work branch:

```powershell
git push -u origin "Sprint#1-1-item-quick-slot"
```

## Naming Examples

- `Sprint#1-1-item-quick-slot`
- `Sprint#1-2-sprint-postprocess`
- `Sprint#2-1-monster-ai`
- `Sprint#2-2-boss-balance`

## Sprint Change Records

Update Docs/SprintChangeLog.md whenever a Sprint work item is completed or receives a material scope change.

Each entry records:

- Sprint/work identifier and branch name
- Status
- Goal
- Major player-facing and architectural changes
- Build or QA verification
- Related architecture and result documents

Daily implementation details remain in DevLog/. The Sprint changelog is the milestone-level summary.
