
#pragma once

#include "efk.ImageResource.h"
#include "efk.Window.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../3rdParty/imgui_platform/imgui_impl_glfw.h"
#include "../3rdParty/imgui_platform/imgui_impl_opengl3.h"

#ifdef _WIN32
#include "../3rdParty/imgui_platform/imgui_impl_dx11.h"
#endif

//#include "../3rdParty/imgui_glfw_gl3/imgui_impl_glfw_gl3.h"

#include "../../3rdParty/imgui_markdown/imgui_markdown.h"
#include <EditorCommon/GUI/FramerateController.h>
#include <EditorCommon/GUI/MainWindow.h>

class Native;

namespace efk
{
// Flags for ImGui::Begin()
enum class WindowFlags : int32_t
{
	None = 0,
	NoTitleBar = 1 << 0,		// Disable title-bar
	NoResize = 1 << 1,			// Disable user resizing with the lower-right grip
	NoMove = 1 << 2,			// Disable user moving the window
	NoScrollbar = 1 << 3,		// Disable scrollbars (window can still scroll with mouse or programatically)
	NoScrollWithMouse = 1 << 4, // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the
								// parent unless NoScrollbar is also set.
	NoCollapse = 1 << 5,		// Disable user collapsing window by double-clicking on it
	AlwaysAutoResize = 1 << 6,	// Resize every window to its content every frame
	// ShowBorders          = 1 << 7,   // Show borders around windows and items (OBSOLETE! Use e.g. style.FrameBorderSize=1.0f to enable
	// borders).
	NoSavedSettings = 1 << 8, // Never load/save settings in .ini file
	NoInputs = 1 << 9,		  // Disable catching mouse or keyboard inputs, hovering test with pass through.
	MenuBar = 1 << 10,		  // Has a menu-bar
	HorizontalScrollbar =
		1 << 11, // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior
	// to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
	NoFocusOnAppearing = 1 << 12, // Disable taking focus when transitioning from hidden to visible state
	NoBringToFrontOnFocus =
		1 << 13,						 // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
	AlwaysVerticalScrollbar = 1 << 14,	 // Always show vertical scrollbar (even if ContentSize.y < Size.y)
	AlwaysHorizontalScrollbar = 1 << 15, // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
	AlwaysUseWindowPadding = 1 << 16,	 // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered
										 // child windows, because more convenient)
	// ResizeFromAnySide = 1 << 17,  // (WIP) Enable resize from any corners and borders. Your back-end needs to honor the different values
	// of io.MouseCursor set by imgui.
	NoNavInputs = 1 << 18, // No gamepad/keyboard navigation within the window
	NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
};

// Enumeration for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum class ColorEditFlags : int32_t
{
	None = 0,
	NoAlpha =
		1 << 1,			//              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (read 3 components from the input pointer).
	NoPicker = 1 << 2,	//              // ColorEdit: disable picker when clicking on colored square.
	NoOptions = 1 << 3, //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
	NoSmallPreview =
		1 << 4,				//              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
	NoInputs = 1 << 5,		//              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview
							//              colored square).
	NoTooltip = 1 << 6,		//              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
	NoLabel = 1 << 7,		//              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the
							//              tooltip and picker).
	NoSidePreview = 1 << 8, //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored
							//              square preview instead.
	NoDragDrop = 1 << 9,	//              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

	// User Options (right-click on widget to change some of them). You can set application defaults using SetColorEditOptions(). The idea
	// is that you probably don't want to override them in most of your calls, let the user choose and/or call SetColorEditOptions() during
	// startup.
	AlphaBar = 1 << 16,		//              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
	AlphaPreview = 1 << 17, //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a
							//              checkerboard, instead of opaque.
	AlphaPreviewHalf =
		1 << 18,			  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
	HDR = 1 << 19,			  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to
							  //              use ImGuiColorEditFlags_Float flag as well).
	RGB = 1 << 20,			  // [Inputs]     // ColorEdit: choose one among RGB/HSV/HEX. ColorPicker: choose any combination using RGB/HSV/HEX.
	HSV = 1 << 21,			  // [Inputs]     // "
	HEX = 1 << 22,			  // [Inputs]     // "
	Uint8 = 1 << 23,		  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
	Float = 1 << 24,		  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of
							  // 0..255 integers. No round-trip of value via integers.
	PickerHueBar = 1 << 25,	  // [PickerMode] // ColorPicker: bar for Hue, rectangle for Sat/Value.
	PickerHueWheel = 1 << 26, // [PickerMode] // ColorPicker: wheel for Hue, triangle for Sat/Value.

};

enum class Cond : int32_t
{
	None = 0,
	Always = 1 << 0,	   // Set the variable
	Once = 1 << 1,		   // Set the variable once per runtime session (only the first call with succeed)
	FirstUseEver = 1 << 2, // Set the variable if the window has no saved data (if doesn't exist in the .ini file)
	Appearing = 1 << 3	   // Set the variable if the window is appearing after being hidden/inactive (or the first time)
};

enum class ComboFlags : int32_t
{
	None = 0,
	PopupAlignLeft = 1 << 0, // Align the popup toward the left by default
	HeightSmall = 1 << 1,	 // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use
							 // SetNextWindowSizeConstraints() prior to calling BeginCombo()
	HeightRegular = 1 << 2,	 // Max ~8 items visible (default)
	HeightLarge = 1 << 3,	 // Max ~20 items visible
	HeightLargest = 1 << 4,	 // As many fitting items as possible
	NoArrowButton = 1 << 5,	 // Display on the preview box without the square arrow button
	NoPreview = 1 << 6,		 // Display only a square arrow button
};

enum class SelectableFlags : int32_t
{
	None = 0,
	DontClosePopups = 1 << 0, // Clicking this don't close parent popup window
	SpanAllColumns = 1 << 1,  // Selectable frame can span all columns (text will still fit in current column)
	AllowDoubleClick = 1 << 2 // Generate press events on double clicks too
};

enum class TreeNodeFlags : int32_t
{
	None = 0,
	Selected = 1 << 0,				// Draw as selected
	Framed = 1 << 1,				// Full colored frame (e.g. for CollapsingHeader)
	AllowItemOverlap = 1 << 2,		// Hit testing to allow subsequent widgets to overlap this one
	NoTreePushOnOpen = 1 << 3,		// Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
	NoAutoOpenOnLog = 1 << 4,		// Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
	DefaultOpen = 1 << 5,			// Default node to be open
	OpenOnDoubleClick = 1 << 6,		// Need double-click to open node
	OpenOnArrow = 1 << 7,			// Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
	Leaf = 1 << 8,					// No collapsing, no arrow (use as a convenience for leaf nodes).
	Bullet = 1 << 9,				// Display a bullet instead of arrow
	FramePadding = 1 << 10,			// Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
	SpanAvailWidth = 1 << 11,		// Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
	SpanFullWidth = 1 << 12,		// Extend hit box to the left-most and right-most edges (bypass the indented area).
	NavLeftJumpsBackHere = 1 << 13, // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
	CollapsingHeader = Framed | NoTreePushOnOpen | NoAutoOpenOnLog
};

enum class MouseCursor : int32_t
{
	None = -1,
	Arrow = 0,
	TextInput,	// When hovering over InputText, etc.
	ResizeAll,	// Unused by imgui functions
	ResizeNS,	// When hovering over an horizontal border
	ResizeEW,	// When hovering over a vertical border or a column
	ResizeNESW, // When hovering over the bottom-left corner of a window
	ResizeNWSE, // When hovering over the bottom-right corner of a window
};

enum InputTextFlags
{
	None = 0,
	CharsDecimal = 1 << 0,		 // Allow 0123456789.+-*/
	CharsHexadecimal = 1 << 1,	 // Allow 0123456789ABCDEFabcdef
	CharsUppercase = 1 << 2,	 // Turn a..z into A..Z
	CharsNoBlank = 1 << 3,		 // Filter out spaces, tabs
	AutoSelectAll = 1 << 4,		 // Select entire text when first taking mouse focus
	EnterReturnsTrue = 1 << 5,	 // Return 'true' when Enter is pressed (as opposed to when the value was modified)
	CallbackCompletion = 1 << 6, // Call user function on pressing TAB (for completion handling)
	CallbackHistory = 1 << 7,	 // Call user function on pressing Up/Down arrows (for history handling)
	CallbackAlways = 1 << 8,	 // Call user function every time. User code may query cursor position, modify text buffer.
	CallbackCharFilter =
		1 << 9,					   // Call user function to filter character. Modify data->EventChar to replace/filter input, or return 1 to discard character.
	AllowTabInput = 1 << 10,	   // Pressing TAB input a '\t' character into the text field
	CtrlEnterForNewLine = 1 << 11, // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus
								   // with Ctrl+Enter, add line with Enter).
	NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
	AlwaysInsertMode = 1 << 13,	   // Insert mode
	ReadOnly = 1 << 14,			   // Read-only mode
	Password = 1 << 15,			   // Password mode, display all characters as '*'
	NoUndoRedo = 1 << 16,		   // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own
								   // undo/redo stack you need e.g. to call ClearActiveID().
	CharsScientific = 1 << 17,	   // Allow 0123456789.+-*/eE (Scientific notation input)
};

enum class ImGuiColFlags : int32_t
{
	Text,
	TextDisabled,
	WindowBg, // Background of normal windows
	ChildBg,  // Background of child windows
	PopupBg,  // Background of popups, menus, tooltips windows
	Border,
	BorderShadow,
	FrameBg, // Background of checkbox, radio button, plot, slider, text input
	FrameBgHovered,
	FrameBgActive,
	TitleBg,
	TitleBgActive,
	TitleBgCollapsed,
	MenuBarBg,
	ScrollbarBg,
	ScrollbarGrab,
	ScrollbarGrabHovered,
	ScrollbarGrabActive,
	CheckMark,
	SliderGrab,
	SliderGrabActive,
	Button,
	ButtonHovered,
	ButtonActive,
	Header,
	HeaderHovered,
	HeaderActive,
	Separator,
	SeparatorHovered,
	SeparatorActive,
	ResizeGrip,
	ResizeGripHovered,
	ResizeGripActive,
	PlotLines,
	PlotLinesHovered,
	PlotHistogram,
	PlotHistogramHovered,
	TextSelectedBg,
	DragDropTarget,
	NavHighlight,		   // Gamepad/keyboard: current highlighted item
	NavWindowingHighlight, // Highlight window when using CTRL+TAB
	NavWindowingDimBg,	   // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	ModalWindowDimBg,	   // Darken/colorize entire screen behind a modal window, when one is active
};

enum ImGuiStyleVarFlags : int32_t
{
	Alpha,			   // float     Alpha
	WindowPadding,	   // ImVec2    WindowPadding
	WindowRounding,	   // float     WindowRounding
	WindowBorderSize,  // float     WindowBorderSize
	WindowMinSize,	   // ImVec2    WindowMinSize
	WindowTitleAlign,  // ImVec2    WindowTitleAlign
	ChildRounding,	   // float     ChildRounding
	ChildBorderSize,   // float     ChildBorderSize
	PopupRounding,	   // float     PopupRounding
	PopupBorderSize,   // float     PopupBorderSize
	FramePadding,	   // ImVec2    FramePadding
	FrameRounding,	   // float     FrameRounding
	FrameBorderSize,   // float     FrameBorderSize
	ItemSpacing,	   // ImVec2    ItemSpacing
	ItemInnerSpacing,  // ImVec2    ItemInnerSpacing
	IndentSpacing,	   // float     IndentSpacing
	ScrollbarSize,	   // float     ScrollbarSize
	ScrollbarRounding, // float     ScrollbarRounding
	GrabMinSize,	   // float     GrabMinSize
	GrabRounding,	   // float     GrabRounding
	ButtonTextAlign,   // ImVec2    ButtonTextAlign
};

enum class FCurveInterporationType : int32_t
{
	Linear = 0,
	Cubic,
};

enum class FCurveEdgeType : int32_t
{
	Constant = 0,
	Loop = 1,
	LoopInversely = 2,
};

enum class DockSplitDir : int32_t
{
	Left = 0,
	Right,
	Top,
	Bottom,
};

enum class DockNodeFlags : int32_t
{
	None = 0,
	NoTabBar = (1 << 0),
	HiddenTabBar = (1 << 1),
	NoWindowMenuButton = (1 << 2),
	NoCloseButton = (1 << 3),
	NoDocking = (1 << 4),
};

enum class DialogStyle
{
	Info,
	Warning,
	Error,
	Question
};

enum class DialogButtons
{
	OK,
	OKCancel,
	YesNo,
	Quit
};

enum class DialogSelection
{
	OK,
	Cancel,
	Yes,
	No,
	Quit,
	None,
	Error
};

enum class Key
{
	Tab,
	LeftArrow,
	RightArrow,
	UpArrow,
	DownArrow,
	PageUp,
	PageDown,
	Home,
	End,
	Insert,
	Delete,
	Backspace,
	Space,
	Enter,
	Escape,
	NumEnter,
	A, // for text edit CTRL+A: select all
	C, // for text edit CTRL+C: copy
	V, // for text edit CTRL+V: paste
	X, // for text edit CTRL+X: cut
	Y, // for text edit CTRL+Y: redo
	Z, // for text edit CTRL+Z: undo
};

class GUIManagerCallback
{
	std::u16string path;

public:
	GUIManagerCallback()
	{
	}
	virtual ~GUIManagerCallback()
	{
	}
	virtual void Resized(int x, int y)
	{
	}
	virtual void Droped()
	{
	}
	virtual void Focused()
	{
	}
	virtual bool Closing()
	{
		return true;
	}
	virtual void Iconify(int f)
	{
	}
	virtual void DpiChanged(float scale)
	{
	}

	virtual bool ClickLink(const char16_t* path)
	{
		return false;
	}

	const char16_t* GetPath()
	{
		return path.c_str();
	}

	void SetPath(const char16_t* path)
	{
		this->path = path;
	}
};

class GUIManager
{
private:
	GUIManagerCallback* callback = nullptr;
	efk::Window* window = nullptr;
	std::shared_ptr<Effekseer::MainWindow> mainWindow_;
	efk::DeviceType deviceType;
	std::u16string clipboard;
	ImGuiID imguiWindowID;

	ImGui::MarkdownConfig markdownConfig_;

	Effekseer::FramerateController framerateController_;

	static void MarkdownLinkCallback(ImGui::MarkdownLinkCallbackData data);

public:
	GUIManager();

	virtual ~GUIManager();

	bool Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, efk::DeviceType deviceType);

	void InitializeGUI(Native* native);

	void ResetGUIStyle();

	void SetTitle(const char16_t* title);

	void SetWindowIcon(const char16_t* iconPath);

	Vec2 GetSize() const;

	void SetSize(int32_t width, int32_t height);

	void Terminate();

	bool DoEvents();

	float GetDeltaSecond();

	void Present();

	void Close();

	bool IsWindowMaximized();

	void SetWindowMaximized(bool maximized);

	bool IsWindowMinimized();

	void SetWindowMinimized(bool minimized);

	Vec2 GetMousePosition();

	int GetMouseButton(int32_t mouseButton);

	int GetMouseWheel();

	void SetCallback(GUIManagerCallback* callback);

	void InvalidateFont();

	void ResetGUI();

	void RenderGUI(bool isValid = true);

	void* GetNativeHandle();

	const char16_t* GetClipboardText();

	void SetClipboardText(const char16_t* text);

	bool Begin(const char16_t* name, bool* p_open);
	void End();

	bool BeginChild(const char* str_id, const Vec2& size_arg = Vec2(), bool border = false, WindowFlags extra_flags = WindowFlags::None);
	void EndChild();

	// Windows Utilities
	Vec2 GetWindowSize();
	Vec2 GetContentRegionAvail();

	void SetNextWindowPos(const Vec2& pos, Cond cond, const Vec2& pivot);

	void SetNextWindowSize(float size_x, float size_y, Cond cond);

	// Parameters stacks (shared)
	void PushStyleColor(ImGuiColFlags idx, uint32_t col);
	void PopStyleColor(int count = 1);

	void PushStyleVar(ImGuiStyleVarFlags idx, float val);
	void PushStyleVar(ImGuiStyleVarFlags idx, const Vec2& val);
	void PopStyleVar(int count = 1);

	// Parameters stacks (current window)
	void PushItemWidth(float item_width);

	void PopItemWidth();

	void Separator();

	void HiddenSeparator(float thicknessDraw, float thicknessItem);

	void Indent(float indent_w);
	void Spacing();
	void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);

	void BeginGroup();
	void EndGroup();

	void SetCursorPosX(float x);
	void SetCursorPosY(float y);
	float GetCursorPosX();
	float GetCursorPosY();
	float GetTextLineHeight();
	float GetTextLineHeightWithSpacing();
	float GetFrameHeight();
	float GetFrameHeightWithSpacing();
	float GetDpiScale() const;

	// Column
	void Columns(int count = 1, const char* id = nullptr, bool border = true);
	void NextColumn();

	float GetColumnWidth(int column_index = -1);
	void SetColumnWidth(int column_index, float width);

	float GetColumnOffset(int column_index = -1);
	void SetColumnOffset(int column_index, float offset_x);

	void Text(const char16_t* text);
	void TextWrapped(const char16_t* text);
	Vec2 CalcTextSize(const char16_t* text);

	// Main
	bool Button(const char16_t* label, float size_x = 0.0f, float size_y = 0.0f);

	void Image(ImageResource* user_texture_id, float x, float y);

	void Image(void* user_texture_id, float x, float y);

	bool ImageButton(ImageResource* user_texture_id, float x, float y);

	bool ImageButtonOriginal(ImageResource* user_texture_id, float x, float y);

	bool Checkbox(const char16_t* label, bool* v);

	bool RadioButton(const char16_t* label, bool active);

	bool ToggleButton(const char16_t* label, bool* p_checked);

	bool InputInt(const char16_t* label, int* v, int step = 1, int step_fast = 100);

	bool SliderInt(const char16_t* label, int* v, int v_min, int v_max);

	void ProgressBar(float fraction, const Vec2& size);

	// Widgets: Combo Box
	bool BeginCombo(const char16_t* label, const char16_t* preview_value, ComboFlags flags, ImageResource* user_texture_id = nullptr);
	void EndCombo(); // only call EndCombo() if BeginCombo() returns true!

	// Drags
	bool DragFloat(const char16_t* label,
				   float* v,
				   float v_speed = 1.0f,
				   float v_min = 0.0f,
				   float v_max = 0.0f,
				   const char* display_format = "%.3f",
				   float power = 1.0f); // If v_min >= v_max we have no bound
	bool DragFloat2(const char16_t* label,
					float* v,
					float v_speed = 1.0f,
					float v_min = 0.0f,
					float v_max = 0.0f,
					const char* display_format = "%.3f",
					float power = 1.0f);
	bool DragFloat3(const char16_t* label,
					float* v,
					float v_speed = 1.0f,
					float v_min = 0.0f,
					float v_max = 0.0f,
					const char* display_format = "%.3f",
					float power = 1.0f);
	bool DragFloat4(const char16_t* label,
					float* v,
					float v_speed = 1.0f,
					float v_min = 0.0f,
					float v_max = 0.0f,
					const char* display_format = "%.3f",
					float power = 1.0f);
	bool DragFloatRange2(const char16_t* label,
						 float* v_current_min,
						 float* v_current_max,
						 float v_speed = 1.0f,
						 float v_min = 0.0f,
						 float v_max = 0.0f,
						 const char* display_format = "%.3f",
						 const char* display_format_max = nullptr,
						 float power = 1.0f);
	bool DragInt(const char16_t* label,
				 int* v,
				 float v_speed = 1.0f,
				 int v_min = 0,
				 int v_max = 0,
				 const char* display_format = "%.0f"); // If v_min >= v_max we have no bound
	bool DragInt2(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
	bool DragInt3(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
	bool DragInt4(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
	bool DragIntRange2(const char16_t* label,
					   int* v_current_min,
					   int* v_current_max,
					   float v_speed = 1.0f,
					   int v_min = 0,
					   int v_max = 0,
					   const char* display_format = "%.0f",
					   const char* display_format_max = nullptr);

	// Drags(Ex)
	bool DragFloat1EfkEx(const char16_t* label,
						 float* v,
						 float v_speed = 1.0f,
						 float v_min = 0.0f,
						 float v_max = 0.0f,
						 const char16_t* display_format1 = u"%.3f",
						 float power = 1.0f);
	bool DragFloat2EfkEx(const char16_t* label,
						 float* v,
						 float v_speed = 1.0f,
						 float v_min1 = 0.0f,
						 float v_max1 = 0.0f,
						 float v_min2 = 0.0f,
						 float v_max2 = 0.0f,
						 const char16_t* display_format1 = u"%.3f",
						 const char16_t* display_format2 = u"%.3f",
						 float power = 1.0f);
	bool DragFloat3EfkEx(const char16_t* label,
						 float* v,
						 float v_speed = 1.0f,
						 float v_min1 = 0.0f,
						 float v_max1 = 0.0f,
						 float v_min2 = 0.0f,
						 float v_max2 = 0.0f,
						 float v_min3 = 0.0f,
						 float v_max3 = 0.0f,
						 const char16_t* display_format1 = u"%.3f",
						 const char16_t* display_format2 = u"%.3f",
						 const char16_t* display_format3 = u"%.3f",
						 float power = 1.0f);

	bool DragInt2EfkEx(const char16_t* label,
					   int* v,
					   int v_speed = 1.0f,
					   int v_min1 = 0.0f,
					   int v_max1 = 0.0f,
					   int v_min2 = 0.0f,
					   int v_max2 = 0.0f,
					   const char16_t* display_format1 = u"%d",
					   const char16_t* display_format2 = u"%d");

	// Input
	bool InputText(const char16_t* label, const char16_t* text, InputTextFlags flags = InputTextFlags::None);

	bool InputTextMultiline(const char16_t* label, const char16_t* text);

	const char16_t* GetInputTextResult();

	// Color
	bool ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags = ColorEditFlags::None);

	// Tree
	bool CollapsingHeader(const char16_t* label, TreeNodeFlags flags = TreeNodeFlags::None);

	bool CollapsingHeaderWithToggle(const char16_t* label, TreeNodeFlags flags, const char16_t* toggle_id, bool* p_checked);

	bool TreeNode(const char16_t* label);

	bool TreeNodeEx(const char16_t* label, TreeNodeFlags flags = TreeNodeFlags::None);

	void TreePop();

	void SetNextTreeNodeOpen(bool is_open, Cond cond = Cond::None);

	bool TreeNodeEx(const char16_t* label, bool* v, TreeNodeFlags flags = TreeNodeFlags::None);

	// Widgets: Selectable / Lists
	bool Selectable(const char16_t* label, bool selected = false, SelectableFlags flags = SelectableFlags::None);

	// Tooltips
	void SetTooltip(const char16_t* text);
	void BeginTooltip();
	void EndTooltip();

	// Menus
	bool BeginMainMenuBar();
	void EndMainMenuBar();
	bool BeginMenuBar();
	void EndMenuBar();
	bool BeginMenu(const char16_t* label, bool enabled = true);
	void EndMenu();
	bool MenuItem(const char16_t* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true);
	bool MenuItem(const char16_t* label, const char* shortcut, bool* p_selected, bool enabled = true);

	// Popups
	void OpenPopup(const char* str_id);
	bool BeginPopup(const char* str_id, WindowFlags extra_flags = WindowFlags::None);
	bool BeginPopupModal(const char16_t* name, bool* p_open = nullptr, WindowFlags extra_flags = WindowFlags::None);
	bool BeginPopupContextItem(const char* str_id = nullptr, int mouse_button = 1);
	void EndPopup();
	bool IsPopupOpen(const char* str_id);
	void CloseCurrentPopup();

	void SetItemDefaultFocus();
	void SetKeyboardFocusHere(int offset = 0);

	void ClearAllFonts();
	void AddFontFromFileTTF(const char16_t* fontFilepath, const char16_t* commonCharacterTablePath, const char16_t* characterTableName, float size_pixels);
	void AddFontFromAtlasImage(const char16_t* filename, uint16_t baseCode, int sizeX, int sizeY, int countX, int countY);

	// Utils
	bool BeginChildFrame(uint32_t id, const Vec2& size, WindowFlags flags = WindowFlags::None);
	void EndChildFrame();

	// Inputs
	int GetKeyIndex(Key key);
	bool IsKeyDown(int user_key_index);
	bool IsKeyPressed(int user_key_index);
	bool IsKeyReleased(int user_key_index);
	bool IsShiftKeyDown();
	bool IsCtrlKeyDown();
	bool IsAltKeyDown();
	bool IsMouseDown(int button);
	bool IsMouseClicked(int button, bool repeat);
	bool IsMouseReleased(int button);
	bool IsMouseDoubleClicked(int button);

	bool IsItemHovered();
	bool IsItemActive();
	bool IsItemFocused();
	bool IsItemClicked(int mouse_button);
	bool IsItemEdited();
	bool IsItemActivated();
	bool IsItemDeactivated();
	bool IsItemDeactivatedAfterEdit();
	bool IsAnyItemActive();
	bool IsWindowHovered();
	bool IsWindowFocused();
	bool IsAnyWindowHovered();
	bool IsAnyWindowFocused();
	MouseCursor GetMouseCursor();

	// Context
	float GetHoveredIDTimer();

	// Design
	void DrawLineBackground(float height, uint32_t col);

	// Dock
	bool BeginFullscreen(const char16_t* label);
	bool BeginDock(const char16_t* label, const char16_t* tabLabel, bool* p_open, bool allowClose, WindowFlags extra_flags);
	void EndDock();
	uint32_t BeginDockLayout();
	void EndDockLayout();
	void DockSplitNode(uint32_t nodeId, DockSplitDir dir, float sizeRatio, uint32_t* outId1, uint32_t* outId2);
	void DockSetNodeFlags(uint32_t nodeId, DockNodeFlags flags);
	void DockSetWindow(uint32_t nodeId, const char* windowName);
	bool IsDockFocused();
	bool IsDockVisibled();
	bool IsDockWindowed();
	void SetDockFocus(const char16_t* label);

	// Fcurve
	bool IsHoveredOnFCurve(float* keys, float* values, float* leftHandleKeys, float* leftHandleValues, float* rightHandleKeys, float* rightHandleValues, int* interporations, FCurveEdgeType startEdge, FCurveEdgeType endEdge, uint32_t col, int count);

	bool IsHoveredOnFCurvePoint(const float* keys, const float* values, int count, int* hovered);

	bool IsFCurvePanning();

	Vec2 GetCurrentFCurveFieldPosition();

	bool BeginFCurve(int id, const Vec2& size, float current, const Vec2& scale, const Vec2& min_kv, const Vec2& max_kv);
	void EndFCurve();
	bool FCurve(int fcurve_id,
				float* keys,
				float* values,
				float* leftHandleKeys,
				float* leftHandleValues,
				float* rightHandleKeys,
				float* rightHandleValues,
				int* interporations,
				FCurveEdgeType startEdge,
				FCurveEdgeType endEdge,
				uint8_t* kv_selected,
				int count,
				float defaultValue,
				bool isLocked,
				bool canControl,
				uint32_t col,
				bool selected,
				float v_min,
				float v_max,
				int* newCount,
				bool* newSelected,
				float* movedX,
				float* movedY,
				int* changedType);

	bool StartSelectingAreaFCurve();

	bool AddFCurvePoint(const Vec2& v,
						float* keys,
						float* values,
						float* leftHandleKeys,
						float* leftHandleValues,
						float* rightHandleKeys,
						float* rightHandleValues,
						int* interporations,
						uint8_t* kv_selected,
						int count,
						int* newCount);

	bool RemoveFCurvePoint(Vec2 v,
					   float* keys,
					   float* values,
					   float* leftHandleKeys,
					   float* leftHandleValues,
					   float* rightHandleKeys,
					   float* rightHandleValues,
					   int* interporations,
					   uint8_t* kv_selected,
					   int count,
					   int* newCount);

	// Drag
	bool BeginDragDropSource();
	bool SetDragDropPayload(const char* type, uint8_t* data, int size);
	void EndDragDropSource();

	bool BeginDragDropTarget();
	bool AcceptDragDropPayload(const char* type, uint8_t* data_output, int data_output_size, int* size);
	void EndDragDropTarget();

	// Dalog
	static DialogSelection show(const char16_t* message, const char16_t* title, DialogStyle style, DialogButtons buttons);

	static bool IsMacOSX();

	static void SetIniFilename(const char16_t* filename);

	// Markdown
	void Markdown(const char16_t* text);

	// NodeFrameTimeline
	bool BeginNodeFrameTimeline();
	void TimelineNode(const char16_t* title, int frameStart, int frameEnd);
	void EndNodeFrameTimeline(int* frameMin, int* frameMax, int* currentFrame, int* selectedEntry, int* firstFrame);
};
} // namespace efk
