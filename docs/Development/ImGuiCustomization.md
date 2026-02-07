# ImGui Customization (Effekseer)

This document tracks Effekseer-specific modifications in the imgui submodule.

## Source of truth

- Submodule path: `Dev/Cpp/3rdParty/imgui`
- Remote: `https://github.com/effekseer/imgui.git`
- Branch: `origin/effekseer_18x`
- Current Effekseer patch commit: `3f8ac1fec58d8a018e6665f0eb330cc49b4d23d3`
- Base commit used by the patch: `4eb5cdc99c9e52dc85fa57e712ec94c0f757df8b`

## Current custom changes

Diff range used for confirmation:

```bash
git -C Dev/Cpp/3rdParty/imgui diff --name-status 4eb5cdc99c9e52dc85fa57e712ec94c0f757df8b..3f8ac1fec58d8a018e6665f0eb330cc49b4d23d3
```

Changed files and intent:

1. `imgui.cpp`
- Navigation activate key for keyboard changed from `Space` to `Enter`.
- `ImGuiWindow` constructor initializes `DockTabLabel`.

2. `imgui_internal.h`
- Added `char DockTabLabel[32];` to `ImGuiWindow`.

3. `imgui_tables.cpp`
- In `NextColumn()`, applies half column padding for the last column.

4. `imgui_widgets.cpp`
- `DragScalar()` supports single-click text input activation.
- Tab label source changed from `window->Name` to `window->DockTabLabel`.

## Update procedure when imgui is updated

1. Move submodule to the target upstream commit.
2. Reapply or port the four Effekseer changes above.
3. Re-check the diff:

```bash
git -C Dev/Cpp/3rdParty/imgui show --stat --patch --no-color HEAD
```

4. Update this file:
- patch commit hash
- base commit hash
- changed files and intent summary
