
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

	// Enumeration for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
	enum class ColorEditFlags : int32_t
	{
		None = 0,
		NoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (read 3 components from the input pointer).
		NoPicker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
		NoOptions = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
		NoSmallPreview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
		NoInputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
		NoTooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
		NoLabel = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
		NoSidePreview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
		// User Options (right-click on widget to change some of them). You can set application defaults using SetColorEditOptions(). The idea is that you probably don't want to override them in most of your calls, let the user choose and/or call SetColorEditOptions() during startup.
		AlphaBar = 1 << 9,   //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
		AlphaPreview = 1 << 10,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
		AlphaPreviewHalf = 1 << 11,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
		HDR = 1 << 12,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use ImGuiColorEditFlags_Float flag as well).
		RGB = 1 << 13,  // [Inputs]     // ColorEdit: choose one among RGB/HSV/HEX. ColorPicker: choose any combination using RGB/HSV/HEX.
		HSV = 1 << 14,  // [Inputs]     // "
		HEX = 1 << 15,  // [Inputs]     // "
		Uint8 = 1 << 16,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255. 
		Float = 1 << 17,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
		PickerHueBar = 1 << 18,  // [PickerMode] // ColorPicker: bar for Hue, rectangle for Sat/Value.
		PickerHueWheel = 1 << 19,  // [PickerMode] // ColorPicker: wheel for Hue, triangle for Sat/Value.
	};

	enum class Cond : int32_t
	{
		None = 0,
		Always = 1 << 0,   // Set the variable
		Once = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
		FirstUseEver = 1 << 2,   // Set the variable if the window has no saved data (if doesn't exist in the .ini file)
		Appearing = 1 << 3    // Set the variable if the window is appearing after being hidden/inactive (or the first time)
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

		void SetNextWindowSize(float size_x, float size_y, Cond cond);

		void PushItemWidth(float item_width);

		void PopItemWidth();

		void Separator();

		void SameLine();

		void Text(const char16_t* text);
		void TextWrapped(const char16_t* text);

		bool Button(const char16_t* label);

		bool Checkbox(const char16_t* label, bool* v);

		bool InputInt(const char16_t* label, int* v, int step = 1, int step_fast = 100);

		bool SliderInt(const char16_t* label, int* v, int v_min, int v_max);

		// Drags
		bool DragFloat(const char16_t* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
		bool DragFloat2(const char16_t* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
		bool DragFloat3(const char16_t* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
		bool DragFloat4(const char16_t* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
		bool DragFloatRange2(const char16_t* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = NULL, float power = 1.0f);
		bool DragInt(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");                                       // If v_min >= v_max we have no bound
		bool DragInt2(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
		bool DragInt3(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
		bool DragInt4(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
		bool DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = NULL);

		// Color
		bool ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags = ColorEditFlags::None);

		// Tree
		bool TreeNode(const char16_t* label);
		void TreePop();

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
		bool BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);
		void EndPopup();
		bool IsPopupOpen(const char* str_id);
		void CloseCurrentPopup();
	};
}