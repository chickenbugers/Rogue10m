# Sprint, Stamina Drain, and Post Process Plan

## Goal

- Hold Left Shift to sprint.
- Increase movement speed while sprinting and consume 1 stamina per second.
- Stop sprinting when Shift is released, stamina reaches zero, the player dies, or a blocking UI opens.
- Add a sprint Post Process effect without conflicting with damage and low-stamina feedback.
- Set both damage feedback paths to a tunable 0.5-second duration.

## Touched Areas

- `ARogue10mCharacter`: Shift input, speed state, stamina timer, tuning properties.
- `URogue10mPlayerFeedbackComponent`: sprint state blended into the existing camera Post Process component.
- `ARogue10mPlayerController`: expose HUD damage feedback duration and default it to 0.5 seconds.
- Documentation and daily DevLog.

## Design

- Bind Left Shift press/release in C++ to avoid direct binary edits to Input Mapping Context assets.
- Keep Character Tick disabled; use a repeating timer with a configurable interval.
- Consume fractional stamina based on `SprintStaminaCostPerSecond * interval`, so the default is exactly 1 per second.
- Use `MaxWalkSpeed` for movement and cache the normal walk speed.
- Blend sprint Motion Blur, chromatic aberration, vignette, and tint in the existing Post Process component.
- Keep damage, stamina, and sprint effects composable in one Post Process pass.

## Completion Conditions

- Shift press starts sprint only with stamina available.
- Shift release and zero stamina restore normal speed and Post Process.
- Default stamina cost is 1 per second.
- Damage feedback duration defaults to 0.5 seconds and is editable in Blueprint class defaults.
- `Rogue10mEditor Win64 Development` builds successfully.

## Rollback Boundary

- Character sprint changes, feedback component changes, PlayerController duration property, and documentation can be reverted independently of existing user-edited Widget assets.
