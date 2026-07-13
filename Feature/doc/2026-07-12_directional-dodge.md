# Directional Dodge Result

- Space/Jump input now starts a dodge instead of jumping.
- Held WASD direction determines dodge direction; no input dodges forward.
- Default dodge speed: `1800 cm/s`.
- Default dodge duration: `0.16 s`.
- Default dodge cooldown: `0.45 s`.
- Sprint stops before dodge and normal move input is ignored during the dodge.
- Dodge uses CharacterMovement velocity and a one-shot timer; Character Tick remains disabled.
- `Rogue10mEditor Win64 Development` build succeeded.
