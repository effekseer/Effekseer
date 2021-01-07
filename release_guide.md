# Check

- Numeric keypad enter is enabled on Windows

Imgui is revised on Effekseer.

- Text boxes can be editted with single click

Imgui is revised on Effekseer.

- Save with Ctrl + S in EffekseerEditor.

Imgui is revised on EffekseerEditor.

# Package

## Windows 

Execute releae.bat


## imgui customization

- Navigate(editor and material)

```
NAV_MAP_KEY(ImGuiKey_Space,     ImGuiNavInput_Activate );
```

->

```
NAV_MAP_KEY(ImGuiKey_Enter,     ImGuiNavInput_Activate );
```


- Numkey Enter

add 

```
io.KeyMap[ImGuiKey_NumEnter] = GLFW_KEY_KP_ENTER;
```

```
else if (IsKeyPressedMap(ImGuiKey_Enter))
```

->

```
else if (IsKeyPressedMap(ImGuiKey_Enter)) || IsKeyPressedMap(ImGuiKey_NumEnter))
```

- Single click

```

// Tabbing or CTRL-clicking on Drag turns it into an input box
const bool hovered = ItemHoverable(frame_bb, id);
bool temp_input_is_active = TempInputTextIsActive(id);
bool temp_input_start = false;
if (!temp_input_is_active)
{
    const bool focus_requested = FocusableItemRegister(window, id);
    const bool clicked = (hovered && g.IO.MouseClicked[0]);
    const bool double_clicked = (hovered && g.IO.MouseDoubleClicked[0]);
    if (focus_requested || clicked || double_clicked || g.NavActivateId == id || g.NavInputId == id)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdAllowNavDirFlags = (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        if (focus_requested || (clicked && g.IO.KeyCtrl) || double_clicked || g.NavInputId == id)
        {
            temp_input_start = true;
            FocusableItemUnregister(window);
        }
    }
}

// ======== Add ========
if (!temp_input_is_active && hovered && g.IO.MouseReleased[0] && g.IO.MouseClickedPos[0].x == g.IO.MousePos.x &&
		 g.IO.MouseClickedPos[0].y == g.IO.MousePos.y)
{
	SetActiveID(id, window);
	SetFocusID(id, window);
	FocusWindow(window);

	// HACK
	g.IO.MouseClicked[0] = 1;

	g.ActiveIdAllowNavDirFlags = (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
	{
		temp_input_start = true;
		FocusableItemUnregister(window);
	}
}
// ================

```

- void ImGui::NextColumn()

```diff
    PopItemWidth();
    PopClipRect();

    const float column_padding = g.Style.ItemSpacing.x;
    float column_padding = g.Style.ItemSpacing.x;
    columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
    if (++columns->Current < columns->Count)
    {
        // FIXME-COLUMNS: Unnecessary, could be locked?
        window->DC.ColumnsOffset.x = GetColumnOffset(columns->Current) - window->DC.Indent.x + column_padding;
        columns->Splitter.SetCurrentChannel(window->DrawList, columns->Current + 1);

+       if (columns->Current + 1 == columns->Count)
+       {
+           column_padding /= 2;
+       }
    }
    else
    {
```

### docking window tabs

- imgui_internal.h

```diff
struct IMGUI_API ImGuiWindow
    ...
    bool                    DockTabWantClose    :1;
+   char                    DockTabLabel[32];

public:
```

```diff
    const char*         GetTabName(const ImGuiTabItem* tab) const
    {
        if (tab->Window)
-           return tab->Window->Name;
+           return tab->Window->DockTabLabel;
        IM_ASSERT(tab->NameOffset != -1 && tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
```

- imgui.cpp

```diff
ImGuiWindow::ImGuiWindow(ImGuiContext* context, const char* name)
    : DrawListInst(&context->DrawListSharedData)
{
    ...
+    memset(DockTabLabel, 0, sizeof(DockTabLabel));
}
```

- imgui_widgets.cpp

```diff
bool    ImGui::TabItemEx(ImGuiTabBar* tab_bar, const char* label, bool* p_open, ImGuiTabItemFlags flags, ImGuiWindow* docked_window)
{
    ...
    // Render tab label, process close button
    const ImGuiID close_button_id = p_open ? window->GetID((void*)((intptr_t)id + 1)) : 0;
-   bool just_closed = TabItemLabelAndCloseButton(display_draw_list, bb, flags, tab_bar->FramePadding, label, id, close_button_id);
+   bool just_closed = TabItemLabelAndCloseButton(display_draw_list, bb, flags, tab_bar->FramePadding, tab_bar->GetTabName(tab), id, close_button_id);
    if (just_closed && p_open != NULL)
    {
        *p_open = false;
```
