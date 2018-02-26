
#define GLEW_STATIC 1
#include <GL/glew.h>

#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "efk.GUIManager.h"

namespace efk
{
	GUIManager::GUIManager()
	{}

	GUIManager::~GUIManager()
	{}

	bool GUIManager::Initialize()
	{
		auto window = new efk::Window();

		if (!window->Initialize(u"Effekseer", 960, 540, false, true))
		{
			delete window;
			window = nullptr;
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
}