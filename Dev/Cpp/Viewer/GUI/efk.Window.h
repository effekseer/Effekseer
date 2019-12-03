
#pragma once

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1

#endif

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <EditorCommon/GUI/MainWindow.h>

#include <codecvt>
#include <locale>
#include <functional>

#include "efk.Vec2.h"
#include "../efk.Base.h"

namespace efk
{
	void GLFLW_ResizeCallback(GLFWwindow* w, int x, int y);
	
	/**
		@note !!Important!! it is migrating into EditorCommon/GUI/MainWindow
	*/
	class Window
	{
	private:
		std::shared_ptr<Effekseer::MainWindow> mainWindow_;

		GLFWwindow*	window = nullptr;
		bool		isOpenGLMode = false;
		DeviceType	deviceType = DeviceType::OpenGL;

#ifndef _WIN32
        GLuint      vao;
#endif
        
	public:
		Window();
		virtual ~Window();

		bool Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, DeviceType deviceType);

		void Terminate();

		bool DoEvents();

		void SetTitle(const char16_t* title);

		void SetWindowIcon(const char16_t* iconPath);

		Vec2 GetSize() const;

		void SetSize(int32_t width, int32_t height);

		void Present();

		void Close();

		Vec2 GetMousePosition();

		int GetMouseButton(int32_t mouseButton);

		void MakeCurrent();

		void MakeNone();

		GLFWwindow*	GetGLFWWindows() const { return window; }

		void* GetNativeHandle() const;

		std::function<void(int, int)> Resized;

		std::function<void()> Focused;

		std::function<void(const char* path)> Droped;

		std::function<bool()> Closing;

		std::function<void(int)> Iconify;

		std::function<void(float)> DpiChanged;
	};

}
