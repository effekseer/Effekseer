
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "common.h"

#include "../EffekseerRendererGL/EffekseerRendererGL.h"
#include "graphics.h"
#include "window.h"

#define _GLFW 1

#if _GLFW
#include <GLFW/glfw3.h>

#ifndef __EFFEKSEER_TEST_BUILD_AS_CMAKE__
#if _WIN32
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")
#if _DEBUG
#pragma comment(lib, "Debug/glfw3.lib")
#pragma comment(lib, "x86/EffekseerRendererGL.Debug.lib")
#else
#pragma comment(lib, "Release/glfw3.lib")
#pragma comment(lib, "x86/EffekseerRendererGL.Release.lib")
#endif
#endif

#else

#endif

#else

#if _WIN32
#include <windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")

#if _DEBUG
#pragma comment(lib, "EffekseerRendererGL.Debug.lib")
#else
#pragma comment(lib, "EffekseerRendererGL.Release.lib")
#endif

#else
#include <GL/glx.h>
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static int64_t GetTime()
{
#ifdef _WIN32
	int64_t count, freq;
	if (QueryPerformanceCounter((LARGE_INTEGER*)&count))
	{
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
		{
			return count * 1000000 / freq;
		}
	}
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
static void Sleep_(int32_t ms) { ::Sleep(ms); }
#else
static void Sleep_(int32_t ms) { usleep(1000 * ms); }
#endif

#if _GLFW

static GLFWwindow* window = nullptr;

#else

#ifdef _WIN32
static HDC g_hDC = NULL;
static HGLRC g_hGLRC = NULL;
#else
static GLXContext g_glx;
static Display* g_display;
static ::Window g_window;
#endif

#endif

static ::EffekseerRenderer::Renderer* g_renderer = NULL;
static uint32_t* g_clearing_image = NULL;
static int g_width = 0, g_height = 0;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
	::EffekseerRendererGL::Renderer* renderer = nullptr;
	GLuint texture = 0;

public:
	DistortingCallback(::EffekseerRendererGL::Renderer* renderer, int texWidth, int texHeight) : renderer(renderer)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_width, g_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual ~DistortingCallback() { glDeleteTextures(1, &texture); }

	virtual bool OnDistorting() override
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_width, g_height);
		glBindTexture(GL_TEXTURE_2D, 0);

		renderer->SetBackground(texture);

		return true;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#if _GLFW

#else

#ifdef _WIN32
bool InitGLWindow(void* handle1, void* handle2)
{
	timeBeginPeriod(1);

	g_hDC = GetDC((HWND)handle1);

	PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
								 1,
								 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
								 PFD_TYPE_RGBA,
								 24,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 32,
								 0,
								 0,
								 PFD_MAIN_PLANE,
								 0,
								 0,
								 0,
								 0};

	int pxfm = ChoosePixelFormat(g_hDC, &pfd);

	SetPixelFormat(g_hDC, pxfm, &pfd);

	g_hGLRC = wglCreateContext(g_hDC);
	return true;
}

#else
bool InitGLWindow(void* handle1, void* handle2)
{
	Display* display_ = (Display*)handle1;
	::Window window_ = *((::Window*)handle2);

	GLint attribute[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
	XVisualInfo* vi = glXChooseVisual(display_, DefaultScreen(display_), attribute);

	if (vi == NULL)
	{
		return false;
	}

	g_glx = glXCreateContext(display_, vi, 0, GL_TRUE);
	g_display = display_;
	g_window = window_;

	XFree(vi);

	return true;
}
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

void MakeContextCurrent() { glfwMakeContextCurrent(window); }

#else

#if _WIN32
void MakeContextCurrent() { wglMakeCurrent(g_hDC, g_hGLRC); }

#else
void MakeContextCurrent() { glXMakeCurrent(g_display, g_window, g_glx); }

#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

void MakeContextNone() { glfwMakeContextCurrent(nullptr); }

#else

#if _WIN32
void MakeContextNone() { wglMakeCurrent(0, 0); }

#else
void MakeContextNone() { glXMakeCurrent(g_display, 0, NULL); }

#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

#else

#if _WIN32
void DestroyContext()
{
	wglDeleteContext(g_hGLRC);
	timeEndPeriod(1);
}

#else
void DestroyContext() { glXDestroyContext(g_display, g_glx); }

#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

void SwapBuffers()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

#else

#if _WIN32
void SwapBuffers() { SwapBuffers(g_hDC); }
#else
void SwapBuffers() { glXSwapBuffers(g_display, g_window); }
#endif

#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static void WaitFrame()
{
	static uint64_t beforeTime = GetTime() / 1000;
	uint64_t currentTime = GetTime() / 1000;

	uint64_t elapsedTime = currentTime - beforeTime;
	if (elapsedTime < 16)
	{
		Sleep_(16 - elapsedTime);
	}
	beforeTime = GetTime() / 1000;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitGraphics(int width, int height)
{
#if _GLFW
	if (!glfwInit())
		return;

#if !_WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	window = glfwCreateWindow(width, height, "Effekseer(GLFW)", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return;
	}
#else
	InitWindow(width, height);

#if _WIN32
	InitGLWindow(GetHandle(), nullptr);
#else
	InitGLWindow(GetDisplay(), GetWindow());
#endif

#endif

	MakeContextCurrent();

	g_width = width;
	g_height = height;
	
	g_clearing_image = new uint32_t[width * height];
	CreateCheckeredPattern(width, height, g_clearing_image);
	
	glViewport(0, 0, width, height);

	g_renderer = ::EffekseerRendererGL::Renderer::Create( 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)width /
	 (float)height, 1.0f, 50.0f ) ); g_renderer->SetDistortingCallback( new DistortingCallback( 	(EffekseerRendererGL::Renderer*)g_renderer,
	width, height ) );
	
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );
	
	g_manager->SetCoordinateSystem( ::Effekseer::CoordinateSystem::RH );
	
	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermGraphics()
{
	g_renderer->Destroy();
	MakeContextNone();

#if _GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
#elif
	DestroyContext();
	ExitWindow();
#endif

	delete[] g_clearing_image;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Rendering()
{
	MakeContextCurrent();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(g_width, g_height, GL_RGBA, GL_UNSIGNED_BYTE, g_clearing_image);

	g_renderer->BeginRendering();
	g_manager->Draw();
	g_renderer->EndRendering();

	glFlush();
	MakeContextNone();

	WaitFrame();
	SwapBuffers();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

bool DoEvent()
{
	if (!glfwWindowShouldClose(window))
	{
		static bool pressed = true;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			if (!pressed)
			{
				PlayEffect();
			}
			pressed = true;
		}
		else
		{
			pressed = false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

#else

bool DoEvent() { return DoWindowEvent(); }

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SetCameraMatrix(const ::Effekseer::Matrix44& matrix) { g_renderer->SetCameraMatrix(matrix); }

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
