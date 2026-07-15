# Consumable Item Quick Slots Architecture

## Scope Gate

- Code: inventory state, item use, cooldown, HUD view model, input routing, drag/drop.
- Assets: preserve the user-edited `WBP_ItemSlotPanel`; bind its existing `UI_ItemSlotFrame1..5` names from C++.
- Documentation: architecture, result, Widget integration guide, DevLog.
- Validation: UHT, Editor build, generated-file check, binary asset preservation.

## Ultrawork Packets

### Packet 1: Inventory Quick-Slot Model

- Goal: own five consumable assignments and per-slot cooldowns in `URogue10mInventoryComponent`.
- Touched area: inventory component and item stack data.
- Completion: assign, unassign, move, use, quantity decrement, cooldown query.
- Validation: Editor compile.
- Rollback: inventory source files only.

### Packet 2: Input and HUD View

- Goal: keys 1–5 use the matching consumable slot and expose complete HUD state.
- Touched area: PlayerController and RunHUD view model.
- Completion: empty/cooldown slots reject use; name, description, quantity and cooldown are available.
- Validation: UHT and Editor compile.
- Rollback: controller and HUD files only.

### Packet 3: WBP Item Slot Binding

- Goal: populate `UI_ItemSlotFrame1..5` with `WBP_QuickSlot` instances.
- Touched area: MainHUD and QuickSlot widget native class.
- Completion: five frames display key, item, quantity, enabled state and cooldown.
- Validation: named widget discovery and Editor compile.
- Rollback: HUD widget source only; no binary asset rewrite.

### Packet 4: Drag and Drop

- Goal: inventory→quick slot, quick slot→inventory, quick slot→quick slot, and invalid-drop unassignment.
- Touched area: typed drag operation, inventory public API, quick-slot widget.
- Completion: only consumables can be assigned; quick-slot drag cancellation returns the assignment to inventory state.
- Validation: compile and runtime Blueprint API documentation.
- Rollback: drag operation and widget source.

## Data Ownership

- Inventory stacks remain in the inventory array; quick slots store inventory indices as non-owning assignments.
- Moving inventory slots remaps quick-slot indices so assignments remain correct.
- Removing or consuming the last stack clears every assignment that points to it.
- Cooldown state is owned by InventoryComponent, not by the widget.

## UI Contract

- `ItemSlotPanelWidget` contains `UI_ItemSlotFrame1` through `UI_ItemSlotFrame5`.
- Each frame must be a `UPanelWidget` so C++ can insert `QuickSlotWidgetClass`.
- Hover tooltip fields: display name, description, quantity, cooldown.
- Invalid/external drop from a quick slot unassigns it; the inventory item itself is never destroyed or duplicated.

## Risk Review

- Preserve current dirty Widget assets and do not stage or overwrite them.
- Avoid Tick for item logic. Existing slow HUD refresh updates cooldown display.
- Reject non-consumables and locked/empty stacks at the component boundary.
