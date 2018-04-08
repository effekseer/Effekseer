#define NOMINMAX
#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "efk.GUIManager.h"

#include "efk.JapaneseFont.h"

#include "../3rdParty/imgui_addon/fcurve/fcurve.h"

namespace efk
{
	// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
	static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

	static std::string utf16_to_utf8(const std::u16string& s)
	{
#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return conv.to_bytes(p, p + s.length());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.to_bytes(s);
#endif
	}

	static std::u16string utf8_to_utf16(const std::string& s)
	{

#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return std::u16string((const char16_t*)conv.from_bytes(s).c_str());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.from_bytes(s);
#endif
	}

	bool DragFloatN(const char* label, float* v, int components, float v_speed, float v_min, float v_max, 
		const char* display_format1, 
		const char* display_format2,
		const char* display_format3,
		float power)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		ImGui::BeginGroup();
		ImGui::PushID(label);
		ImGui::PushMultiItemsWidths(components);

		const char* display_formats[] = {
			display_format1,
			display_format2,
			display_format3
		};

		for (int i = 0; i < components; i++)
		{
			ImGui::PushID(i);
			value_changed |= ImGui::DragFloat("##v", &v[i], v_speed, v_min, v_max, display_formats[i], power);
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		ImGui::PopID();

		ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
		ImGui::EndGroup();

		return value_changed;
	}

	bool DragIntN(const char* label, int* v, int components, int v_speed, int v_min, int v_max,
		const char* display_format1,
		const char* display_format2,
		const char* display_format3)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		ImGui::BeginGroup();
		ImGui::PushID(label);
		ImGui::PushMultiItemsWidths(components);

		const char* display_formats[] = {
			display_format1,
			display_format2,
			display_format3
		};

		for (int i = 0; i < components; i++)
		{
			ImGui::PushID(i);
			value_changed |= ImGui::DragInt("##v", &v[i], v_speed, v_min, v_max, display_formats[i]);
			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		ImGui::PopID();

		ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
		ImGui::EndGroup();

		return value_changed;
	}

	GUIManager::GUIManager()
	{}

	GUIManager::~GUIManager()
	{}

	bool GUIManager::Initialize(const char16_t* title, int32_t width, int32_t height, bool isSRGBMode)
	{
		window = new efk::Window();

		if (!window->Initialize(title, width, height, isSRGBMode, true))
		{
			ES_SAFE_DELETE(window);
			return false;
		}

		window->Resized = [this](int x, int y) -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->Resized(x, y);
			}
		};

		window->Focused = [this]() -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->Focused();
			}
		};

		window->Droped = [this](const char* path) -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->SetPath(utf8_to_utf16(path).c_str());
				this->callback->Droped();
			}
		};
		window->MakeCurrent();

#ifdef _WIN32
		glewInit();
#endif
        
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window->GetGLFWWindows(), true);
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildRounding = 3.f;
		style.GrabRounding = 3.f;
		style.WindowRounding = 3.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 3.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		// mono tone

		for (int32_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto v = (style.Colors[i].x + style.Colors[i].y + style.Colors[i].z) / 3.0f;
			style.Colors[i].x = v;
			style.Colors[i].y = v;
			style.Colors[i].z = v;
		}

		style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);

		return true;
	}

	void GUIManager::SetTitle(const char16_t* title)
	{
		window->SetTitle(title);
	}

	void GUIManager::SetSize(int32_t width, int32_t height)
	{
		window->SetSize(width, height);
	}

	void GUIManager::Terminate()
	{
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();

		window->MakeNone();
		window->Terminate();
		ES_SAFE_DELETE(window);
	}

	bool GUIManager::DoEvents()
	{
		return window->DoEvents();
	}

	void GUIManager::Present()
	{
		window->Present();
	}

	void GUIManager::Close()
	{
		window->Close();
	}

	Vec2 GUIManager::GetMousePosition()
	{
		return window->GetMousePosition();
	}

	int GUIManager::GetMouseButton(int32_t mouseButton)
	{
		return window->GetMouseButton(mouseButton);
	}

	int GUIManager::GetMouseWheel()
	{
		return ImGui::GetIO().MouseWheel;
	}

	void GUIManager::SetCallback(GUIManagerCallback* callback)
	{
		this->callback = callback;
	}

	void GUIManager::ResetGUI()
	{
		ImGui_ImplGlfwGL3_NewFrame();
	}

	void GUIManager::RenderGUI()
	{
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		auto bit = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		glClearDepth(1.0f);
		glClear(bit);
	}

	void* GUIManager::GetNativeHandle()
	{
		return window->GetNativeHandle();
	}

	bool GUIManager::Begin(const char16_t* name, bool* p_open)
	{
		return ImGui::Begin(utf16_to_utf8(name).c_str(), p_open);
	}

	void GUIManager::End()
	{
		ImGui::End();
	}

	bool GUIManager::BeginChild(const char* str_id)
	{
		return ImGui::BeginChild(str_id);
	}

	void GUIManager::EndChild()
	{
		ImGui::EndChild();
	}

	void GUIManager::SetNextWindowSize(float size_x, float size_y, Cond cond)
	{
		ImVec2 size;
		size.x = size_x;
		size.y = size_y;

		ImGui::SetNextWindowSize(size, (int)cond);
	}

	void GUIManager::PushItemWidth(float item_width)
	{
		ImGui::PushItemWidth(item_width);
	}

	void GUIManager::PopItemWidth()
	{
		ImGui::PopItemWidth();
	}

	void GUIManager::Separator()
	{
		ImGui::Separator();
	}

	void GUIManager::SameLine()
	{
		ImGui::SameLine();
	}

	void GUIManager::BeginGroup()
	{
		ImGui::BeginGroup();
	}

	void GUIManager::EndGroup()
	{
		ImGui::EndGroup();
	}

	void GUIManager::Columns(int count, const char* id, bool border)
	{
		ImGui::Columns(count, id, border);
	}

	void GUIManager::NextColumn()
	{
		ImGui::NextColumn();
	}

	float GUIManager::GetColumnWidth(int column_index)
	{
		return ImGui::GetColumnWidth(column_index);
	}

	void GUIManager::SetColumnWidth(int column_index, float width)
	{
		ImGui::SetColumnWidth(column_index, width);
	}

	float GUIManager::GetColumnOffset(int column_index)
	{
		return ImGui::GetColumnOffset(column_index);
	}

	void GUIManager::SetColumnOffset(int column_index, float offset_x)
	{
		ImGui::SetColumnOffset(column_index, offset_x);
	}

	void GUIManager::Text(const char16_t* text)
	{
		ImGui::Text(utf16_to_utf8(text).c_str());
	}

	void GUIManager::TextWrapped(const char16_t* text)
	{
		ImGui::TextWrapped(utf16_to_utf8(text).c_str());
	}

	bool GUIManager::Button(const char16_t* label)
	{
		return ImGui::Button(utf16_to_utf8(label).c_str());
	}

	void GUIManager::Image(ImageResource* user_texture_id, float x, float y)
	{
		if (user_texture_id != nullptr && user_texture_id->GetTextureData() != nullptr)
		{
			if (user_texture_id->GetTextureData()->UserPtr != nullptr)
			{
				ImGui::Image((ImTextureID)user_texture_id->GetTextureData()->UserPtr, ImVec2(x, y));
			}
			else
			{
				ImGui::Image((ImTextureID)user_texture_id->GetTextureData()->UserID, ImVec2(x, y));
			}
		}
	}

	bool GUIManager::ImageButton(ImageResource* user_texture_id, float x, float y)
	{
		if (user_texture_id != nullptr && user_texture_id->GetTextureData() != nullptr)
		{
			if (user_texture_id->GetTextureData()->UserPtr != nullptr)
			{
				return ImGui::ImageButton((ImTextureID)user_texture_id->GetTextureData()->UserPtr, ImVec2(x, y));
			}
			else
			{
				return ImGui::ImageButton((ImTextureID)user_texture_id->GetTextureData()->UserID, ImVec2(x, y));
			}
		}

		return false;
	}

	bool GUIManager::Checkbox(const char16_t* label, bool* v)
	{
		return ImGui::Checkbox(utf16_to_utf8(label).c_str(), v);
	}

	bool GUIManager::RadioButton(const char16_t* label, bool active)
	{
		return ImGui::RadioButton(utf16_to_utf8(label).c_str(), active);
	}

	bool GUIManager::InputInt(const char16_t* label, int* v, int step, int step_fast)
	{
		return ImGui::InputInt(utf16_to_utf8(label).c_str(), v, step, step_fast);
	}

	bool GUIManager::SliderInt(const char16_t* label, int* v, int v_min, int v_max)
	{
		return ImGui::SliderInt(utf16_to_utf8(label).c_str(), v, v_min, v_max);
	}

	bool GUIManager::BeginCombo(const char16_t* label, const char16_t* preview_value, ComboFlags flags)
	{
		return ImGui::BeginCombo(
			utf16_to_utf8(label).c_str(),
			utf16_to_utf8(preview_value).c_str(),
			(int)flags);
	}

	void GUIManager::EndCombo()
	{
		ImGui::EndCombo();
	}

	bool GUIManager::DragFloat(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat2(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat2(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat3(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat3(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat4(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat4(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloatRange2(const char16_t* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power)
	{
		return ImGui::DragFloatRange2(utf16_to_utf8(label).c_str(), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
	}

	bool GUIManager::DragInt(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
	{
		return ImGui::DragInt(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragInt2(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
	{
		return ImGui::DragInt2(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragInt3(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
	{
		return ImGui::DragInt3(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragInt4(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
	{
		return ImGui::DragInt4(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
	{
		return ImGui::DragIntRange2(utf16_to_utf8(label).c_str(), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
	}

	bool GUIManager::DragFloat1EfkEx(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char16_t* display_format1, float power)
	{
		return DragFloatN(
			utf16_to_utf8(label).c_str(), v, 1, v_speed, v_min, v_max, 
			utf16_to_utf8(display_format1).c_str(), 
			nullptr, 
			nullptr,
			power);
	}

	bool GUIManager::DragFloat2EfkEx(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char16_t* display_format1, const char16_t* display_format2, float power)
	{
		return DragFloatN(
			utf16_to_utf8(label).c_str(), v, 2, v_speed, v_min, v_max,
			utf16_to_utf8(display_format1).c_str(),
			utf16_to_utf8(display_format2).c_str(),
			nullptr,
			power);
	}

	bool GUIManager::DragFloat3EfkEx(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char16_t* display_format1, const char16_t* display_format2, const char16_t* display_format3, float power)
	{
		return DragFloatN(
			utf16_to_utf8(label).c_str(), v, 3, v_speed, v_min, v_max,
			utf16_to_utf8(display_format1).c_str(),
			utf16_to_utf8(display_format2).c_str(),
			utf16_to_utf8(display_format3).c_str(),
			power);
	}

	bool GUIManager::DragInt2EfkEx(const char16_t* label, int* v, int v_speed, int v_min, int v_max, const char16_t* display_format1, const char16_t* display_format2)
	{
		return DragIntN(
			utf16_to_utf8(label).c_str(), v, 2, v_speed, v_min, v_max,
			utf16_to_utf8(display_format1).c_str(),
			utf16_to_utf8(display_format2).c_str(),
			nullptr);
	}

	static std::u16string inputTextResult;

	bool GUIManager::InputText(const char16_t* label, const char16_t* text)
	{
		auto text_ = utf16_to_utf8(text);

		char buf[260];
		memcpy(buf, text_.data(), std::min((int32_t)text_.size(), 250));
		buf[std::min((int32_t)text_.size(), 250)] = 0;

		auto ret = ImGui::InputText(utf16_to_utf8(label).c_str(), buf, 260);
	
		inputTextResult = utf8_to_utf16(buf);
	
		return ret;
	}

	const char16_t* GUIManager::GetInputTextResult()
	{
		return inputTextResult.c_str();
	}

	bool GUIManager::ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags)
	{
		return ImGui::ColorEdit4(utf16_to_utf8(label).c_str(), col, (int)flags);
	}

	bool GUIManager::TreeNode(const char16_t* label)
	{
		return ImGui::TreeNode(utf16_to_utf8(label).c_str());
	}

	bool GUIManager::TreeNodeEx(const char16_t* label, TreeNodeFlags flags)
	{
		return ImGui::TreeNodeEx(utf16_to_utf8(label).c_str(), (int)flags);
	}

	void GUIManager::TreePop()
	{
		ImGui::TreePop();
	}

	bool GUIManager::Selectable(const char16_t* label, bool selected, SelectableFlags flags)
	{
		return ImGui::Selectable(utf16_to_utf8(label).c_str(), selected, (int)flags);
	}

	void GUIManager::SetTooltip(const char16_t* text)
	{
		ImGui::SetTooltip(utf16_to_utf8(text).c_str());
	}

	bool GUIManager::BeginMainMenuBar()
	{
		return ImGui::BeginMainMenuBar();
	}

	void GUIManager::EndMainMenuBar()
	{
		ImGui::EndMainMenuBar();
	}

	bool GUIManager::BeginMenuBar()
	{
		return ImGui::BeginMenuBar();
	}

	void GUIManager::EndMenuBar()
	{
		return ImGui::EndMenuBar();
	}

	bool GUIManager::BeginMenu(const char16_t* label, bool enabled)
	{
		return ImGui::BeginMenu(utf16_to_utf8(label).c_str(), enabled);
	}

	void GUIManager::EndMenu()
	{
		ImGui::EndMenu();
	}

	bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool selected, bool enabled)
	{
		return ImGui::MenuItem(utf16_to_utf8(label).c_str(), shortcut, selected, enabled);
	}

	bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool* p_selected, bool enabled)
	{
		return ImGui::MenuItem(utf16_to_utf8(label).c_str(), shortcut, p_selected, enabled);
	}

	void GUIManager::OpenPopup(const char* str_id)
	{
		ImGui::OpenPopup(str_id);
	}

	bool GUIManager::BeginPopup(const char* str_id, WindowFlags extra_flags)
	{
		return ImGui::BeginPopup(str_id, (int)extra_flags);
	}

	bool GUIManager::BeginPopupModal(const char16_t* name, bool* p_open, WindowFlags extra_flags)
	{
		return ImGui::BeginPopupModal(utf16_to_utf8(name).c_str(), p_open, (int)extra_flags);
	}

	bool GUIManager::BeginPopupContextItem(const char* str_id, int mouse_button)
	{
		return ImGui::BeginPopupContextItem(str_id, mouse_button);
	}

	void GUIManager::EndPopup()
	{
		ImGui::EndPopup();
	}

	bool GUIManager::IsPopupOpen(const char* str_id)
	{
		return ImGui::IsPopupOpen(str_id);
	}

	void GUIManager::CloseCurrentPopup()
	{
		ImGui::CloseCurrentPopup();
	}

	void GUIManager::SetItemDefaultFocus()
	{
		ImGui::SetItemDefaultFocus();
	}

	void GUIManager::AddFontFromFileTTF(const char* filename, float size_pixels)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, glyphRangesJapanese);
	}

	bool GUIManager::BeginChildFrame(uint32_t id, const Vec2& size, WindowFlags flags)
	{
		return ImGui::BeginChildFrame(id, ImVec2(size.X, size.Y), (int32_t)flags);
	}

	void GUIManager::EndChildFrame()
	{
		ImGui::EndChildFrame();
	}

	bool GUIManager::IsKeyDown(int user_key_index)
	{
		return ImGui::IsKeyDown(user_key_index);
	}

	bool GUIManager::IsMouseDoubleClicked(int button)
	{
		return ImGui::IsMouseDoubleClicked(button);
	}

	bool GUIManager::IsItemHovered()
	{
		return ImGui::IsItemHovered();
	}

	bool GUIManager::IsItemActive()
	{
		return ImGui::IsItemActive();
	}

	bool GUIManager::IsItemFocused()
	{
		return ImGui::IsItemFocused();
	}

	bool GUIManager::IsItemClicked(int mouse_button)
	{
		return ImGui::IsItemClicked(mouse_button);
	}

	bool GUIManager::IsAnyWindowHovered()
	{
		return ImGui::IsAnyWindowHovered();
	}

	void GUIManager::BeginDockspace()
	{
		ImGui::BeginDockspace();
	}

	void GUIManager::EndDockspace()
	{
		ImGui::EndDockspace();
	}

	bool GUIManager::BeginDock(const char16_t* label)
	{
		return ImGui::BeginDock(utf16_to_utf8(label).c_str());
	}

	void GUIManager::EndDock()
	{
		ImGui::EndDock();
	}

	bool GUIManager::BeginFCurve(int id)
	{
		return ImGui::BeginFCurve(id);
	}

	void GUIManager::EndFCurve()
	{
		ImGui::EndFCurve();
	}

	bool GUIManager::FCurve(
		int fcurve_id,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		uint8_t* kv_selected,
		int count,
		bool isLocked,
		bool canControl,
		uint32_t col,
		bool selected,
		int* newCount,
		bool* newSelected,
		float* movedX,
		float* movedY,
		int* changedType)
	{
		return ImGui::FCurve(
			fcurve_id,
			keys,
			values,
			leftHandleKeys,
			leftHandleValues,
			rightHandleKeys,
			rightHandleValues,
			(bool*)kv_selected,
			count,
			isLocked,
			canControl,
			col,
			selected,
			newCount,
			newSelected,
			movedX,
			movedY,
			changedType);
	}
}
