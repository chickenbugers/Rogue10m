# Sprint, Stamina Drain, and Post Process Result

## Result

- Left Shift press/release controls sprinting.
- Sprint speed defaults to `900 cm/s` and is editable in Character Blueprint class defaults.
- Sprint consumes stamina at `1.0 per second` using a 0.1-second timer; Character Tick remains unused.
- Sprint ends on release, insufficient stamina, death, or blocking UI.
- Sprint tint, vignette, motion blur, and chromatic aberration are blended into the existing player Post Process component.
- Camera damage Post Process and HUD damage overlay durations both default to 0.5 seconds.
- Tuning locations are documented in `Docs/SprintAndFeedbackGuide.md`.

## Validation

- UnrealHeaderTool passed after removing an unsupported custom units metadata string.
- `Rogue10mEditor Win64 Development` build succeeded.
- Existing user-edited Widget assets were preserved and excluded from feature implementation.
