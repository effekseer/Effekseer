
#define GLEW_STATIC 1
#include <GL/glew.h>

#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "efk.GUIManager.h"

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

	bool GUIManager::Initialize(char16_t* title, int32_t width, int32_t height, bool isSRGBMode)
	{
		window = new efk::Window();

		if (!window->Initialize(title, width, height, isSRGBMode, true))
		{
			ES_SAFE_DELETE(window);
			return false;
		}

		window->MakeCurrent();

		glewInit();

		ImGui_ImplGlfwGL3_Init(window->GetGLFWWindows(), true);
		ImGui::StyleColorsClassic();

		return true;
	}

	void GUIManager::Terminate()
	{
		ImGui_ImplGlfwGL3_Shutdown();

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

	void GUIManager::ResetGUI()
	{
		ImGui_ImplGlfwGL3_NewFrame();
	}

	void GUIManager::RenderGUI()
	{
		ImGui::Render();

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

	bool GUIManager::Button(const char16_t* label)
	{
		return ImGui::Button(utf16_to_utf8(label).c_str());
	}

	bool GUIManager::Checkbox(const char16_t* label, bool* v)
	{
		return ImGui::Checkbox(utf16_to_utf8(label).c_str(), v);
	}

	bool GUIManager::DragInt(const char16_t* label, int* v, float v_speed, int v_min, int v_max)
	{
		return ImGui::DragInt(utf16_to_utf8(label).c_str(), v, v_speed, v_min, v_max);
	}

	bool GUIManager::InputInt(const char16_t* label, int* v, int step, int step_fast)
	{
		return ImGui::InputInt(utf16_to_utf8(label).c_str(), v, step, step_fast);
	}

	bool GUIManager::SliderInt(const char16_t* label, int* v, int v_min, int v_max)
	{
		return ImGui::SliderInt(utf16_to_utf8(label).c_str(), v, v_min, v_max);
	}

	bool GUIManager::DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
	{
		return ImGui::DragIntRange2(utf16_to_utf8(label).c_str(), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
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
}