
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
}