
#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "efk.GUIManager.h"

#include "efk.JapaneseFont.h"

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
		if (user_texture_id != nullptr)
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
		if (user_texture_id != nullptr)
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

	bool GUIManager::ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags)
	{
		return ImGui::ColorEdit4(utf16_to_utf8(label).c_str(), col, (int)flags);
	}

	bool GUIManager::TreeNode(const char16_t* label)
	{
		return ImGui::TreeNode(utf16_to_utf8(label).c_str());
	}

	void GUIManager::TreePop()
	{
		ImGui::TreePop();
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

	void GUIManager::AddFontFromFileTTF(const char* filename, float size_pixels)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, glyphRangesJapanese);
	}
}
