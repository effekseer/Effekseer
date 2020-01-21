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