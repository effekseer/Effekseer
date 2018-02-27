
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

		bool DragInt(const char16_t* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0);

		bool InputInt(const char16_t* label, int* v, int step = 1, int step_fast = 100);

		bool SliderInt(const char16_t* label, int* v, int v_min, int v_max);

		bool DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char16_t* display_format = u"%.0f", const char16_t* display_format_max = NULL);
	};
}