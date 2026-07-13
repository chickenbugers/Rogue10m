# Directional Dodge Plan

- Space/Jump input starts a short ground dodge instead of jumping.
- Cache the latest WASD vector; use it for dodge direction, otherwise use actor forward.
- Stop sprint before dodging and ignore movement input during the dodge window.
- Use CharacterMovement velocity plus a timer; keep Character Tick disabled.
- Expose speed, duration and cooldown in Character Blueprint class defaults.
- Reject dodge while dead, blocking UI is open, already dodging, or cooldown is active.
