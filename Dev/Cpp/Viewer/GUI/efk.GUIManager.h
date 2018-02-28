
#pragma once

#include "efk.Window.h"

#include "../3rdParty\imgui\imgui.h"
#include "../3rdParty\imgui_glfw_gl3\imgui_impl_glfw_gl3.h"
#include "../3rdParty/imgui_addon/imguidock/imguidock.h"

namespace efk
{
	// Flags for ImGui::Begin()
	enum class WindowFlags : int32_t
	{
		None = 0,
		NoTitleBar = 1 << 0,   // Disable title-bar
		NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
		NoMove = 1 << 2,   // Disable user moving the window
		NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programatically)
		NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
		NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it
		AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
		ShowBorders          = 1 << 7,   // Show borders around windows and items (OBSOLETE! Use e.g. style.FrameBorderSize=1.0f to enable borders).
		NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
		NoInputs = 1 << 9,   // Disable catching mouse or keyboard inputs, hovering test with pass through.
		MenuBar = 1 << 10,  // Has a menu-bar
		HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
		NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
		NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
		AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
		AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
		AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
		ResizeFromAnySide = 1 << 17,  // (WIP) Enable resize from any corners and borders. Your back-end needs to honor the different values of io.MouseCursor set by imgui.

		// [Internal]
		ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
		Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
		Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
		Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
		ChildMenu = 1 << 28   // Don't use! For internal use by BeginMenu()
	};

	class GUIManager
	{
	private:
		efk::Window*	window = nullptr;

	public:
		GUIManager();

		virtual ~GUIManager();

		bool Initialize(char16_t* title, int32_t width, int32_t height, bool isSRGBMode);

		void Terminate();

		bool DoEvents();

		void Present();

		void ResetGUI();

		void RenderGUI();

		void* GetNativeHandle();

		bool Begin(const char16_t* name, bool* p_open);

		void End();

		void PushItemWidth(float item_width);

		void PopItemWidth();

		void Separator();

		void SameLine();

		void Text(const char16_t* text);

		bool Button(const char16_t* label);

		bool Checkbox(const char16_t* label, bool* v);

		bool DragInt(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0);

		bool InputInt(const char16_t* label, int* v, int step = 1, int step_fast = 100);

		bool SliderInt(const char16_t* label, int* v, int v_min, int v_max);

		bool DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = NULL);

		// Menus
		bool BeginMainMenuBar();                                                
		void EndMainMenuBar();
		bool BeginMenuBar();                                                    
		void EndMenuBar();
		bool BeginMenu(const char16_t* label, bool enabled = true);
		void EndMenu();
		bool MenuItem(const char16_t* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);
		bool MenuItem(const char16_t* label, const char* shortcut, bool* p_selected, bool enabled = true);

		// Popups
		void OpenPopup(const char* str_id);
		bool BeginPopupModal(const char16_t* name, bool* p_open = NULL, WindowFlags extra_flags = WindowFlags::None);
		void EndPopup();
		bool IsPopupOpen(const char* str_id);
		void CloseCurrentPopup();
	};
}