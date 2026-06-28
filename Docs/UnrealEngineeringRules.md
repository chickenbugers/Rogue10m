# Unreal Engineering Rules

These are the project rules for Rogue10m Unreal Engine 5.7 work.

## C++ And Blueprint Boundary

- C++ owns gameplay logic.
- Blueprint children configure assets, data, and tuning values.
- Avoid Blueprint-only gameplay logic for systems that need review, testing, or reuse.
- Use Data Assets or Data Tables for larger tuning sets.

## Reflection And GC

- UObject member references that must stay alive need `UPROPERTY()`.
- Prefer `TObjectPtr<T>` for owned or tracked UObject members.
- Prefer `TWeakObjectPtr<T>` for cached non-owning references.
- Containers of UObject references also need `UPROPERTY()`.
- Never manually `delete` a UObject.

## Class Design

- `UObject` classes use `U`.
- `AActor` classes use `A`.
- structs use `F`.
- enum classes use `E`.
- interfaces use `I`.
- bools use `b`.
- Functions and variables use PascalCase, following the existing template style.

## Initialization

- Constructor: default values and `CreateDefaultSubobject`.
- `BeginPlay` or subsystem `Initialize`: world access, timers, bindings, actor lookups.
- Do not use `GetWorld()` or player lookups in constructors.
- Clear timers and dynamic bindings when the owning object ends play or is destroyed.

## Includes

- Use IWYU.
- Forward declare in headers when possible.
- Include concrete headers in `.cpp`.
- `*.generated.h` must be the last include in its header.

## Performance

- Tick is off by default.
- Prefer events, timers, delegates, Enhanced Input callbacks, and StateTree transitions.
- Cache repeated lookups.
- Avoid per-frame `GetAllActorsOfClass`, `FindComponentByClass`, `FString::Printf`, and spam logs.
- Use pooling for projectiles, short-lived enemies, VFX emitters, damage numbers, or frequent spawns.

## Assets And Content

- Project-owned assets should live under `Content/Rogue10m/` once production content begins.
- Keep marketplace or template folders isolated.
- Use prefixes consistently: `BP_`, `WBP_`, `DA_`, `DT_`, `IA_`, `IMC_`, `SM_`, `SK_`, `M_`, `MI_`, `T_`, `NS_`, `SW_`.
- Fix redirectors after moving assets in Unreal Editor.
- Prefer `UPROPERTY(EditDefaultsOnly)` references, soft references, Data Assets, or config over hard-coded asset paths.

## Forbidden Direct Edits

Do not directly edit or create:

- `Intermediate/`
- `Binaries/`
- `Saved/`
- `DerivedDataCache/`
- `*.generated.h`
- `*.gen.cpp`
- `.uasset`
- `.umap`

Binary asset and map changes must be made through Unreal Editor.
