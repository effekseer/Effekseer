
#pragma once

#include "efk.Window.h"

#include "../3rdParty\imgui\imgui.h"
#include "../3rdParty\imgui_glfw_gl3\imgui_impl_glfw_gl3.h"
#include "../3rdParty/imgui_addon/imguidock/imguidock.h"

namespace efk
{
	class GUIManager
	{
	private:
		efk::Window*	window = nullptr;

	public:
		GUIManager();

		virtual ~GUIManager();

		bool Initialize();

		void Terminate();

		bool DoEvents();

		void Present();
	};
}