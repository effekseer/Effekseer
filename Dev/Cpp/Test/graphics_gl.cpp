
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

#if _WIN32

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")
#if _DEBUG
#pragma comment(lib, "Debug/glfw3.lib")
#pragma comment(lib, "x86/EffekseerRendererGL.Debug.lib" )
#else
#pragma comment(lib, "Release/glfw3.lib")
#pragma comment(lib, "x86/EffekseerRendererGL.Release.lib" )
#endif

#else

#endif

#else

#if _WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")

#if _DEBUG
#pragma comment(lib, "EffekseerRendererGL.Debug.lib" )
#else
#pragma comment(lib, "EffekseerRendererGL.Release.lib" )
#endif

#else
#include<GL/glx.h>
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
static void Sleep_( int32_t ms )
{
	::Sleep( ms );
}
#else
static void Sleep_( int32_t ms )
{
	usleep( 1000 * ms );
}
#endif

#if _GLFW

static GLFWwindow* window = nullptr;

#else

#ifdef _WIN32
static HDC					g_hDC = NULL;
static HGLRC				g_hGLRC = NULL;
#else
static GLXContext			g_glx;
static Display*				g_display;
static ::Window				g_window;
#endif

#endif

static ::EffekseerRenderer::Renderer*	g_renderer = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#if _GLFW

#else

#ifdef _WIN32
bool InitGLWindow(void* handle1, void* handle2)
{
	timeBeginPeriod(1);

	g_hDC = GetDC( (HWND)handle1 );

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
		0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pxfm = ChoosePixelFormat( g_hDC, &pfd );

	SetPixelFormat( g_hDC, pxfm, &pfd );

	g_hGLRC = wglCreateContext( g_hDC );
	return true;
}

#else
bool InitGLWindow(void* handle1, void* handle2)
{
	Display* display_ = (Display*)handle1;
	::Window window_ = *((::Window*)handle2);

	GLint attribute[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* vi = glXChooseVisual(display_, DefaultScreen(display_), attribute);

	if( vi == NULL )
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

void MakeContextCurrent()
{
	glfwMakeContextCurrent(window);
}

#else

#if _WIN32
void MakeContextCurrent()
{
	wglMakeCurrent( g_hDC, g_hGLRC );
}

#else
void MakeContextCurrent()
{
	glXMakeCurrent(g_display, g_window, g_glx);
}

#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#if _GLFW

void MakeContextNone()
{
	glfwMakeContextCurrent(nullptr);
}

#else

#if _WIN32
void MakeContextNone()
{
	wglMakeCurrent( 0, 0 );
}

#else
void MakeContextNone()
{
	glXMakeCurrent(g_display, 0, NULL);
}

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
	wglDeleteContext( g_hGLRC );
	timeEndPeriod(1);
}

#else
void DestroyContext()
{
	glXDestroyContext(g_display, g_glx);
}

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
void SwapBuffers()
{
	SwapBuffers( g_hDC );
}
#else
void SwapBuffers()
{
	glXSwapBuffers(g_display, g_window);
}
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
	if (elapsedTime < 16) {
		Sleep_(16 - elapsedTime);
	}
	beforeTime = GetTime() / 1000;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitGraphics(int width, int height )
{
#if _GLFW
	if (!glfwInit()) return;

	window = glfwCreateWindow(width, height, "Effekseer(GLFW)", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return;
	}
#else
	InitWindow(width, height);

#if _WIN32
	InitGLWindow( GetHandle(), nullptr );
#else
	InitGLWindow( GetDisplay(), GetWindow() );
#endif

#endif

	MakeContextCurrent();

#if !_WIN32
	if( glewInit() != GLEW_OK )
	{
		assert(0);
	}
#endif

	glViewport( 0, 0, width, height );

	g_renderer = ::EffekseerRendererGL::Renderer::Create( 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)width / (float)height, 1.0f, 50.0f ) );
	
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
	g_renderer->Destory();
	MakeContextNone();

#if _GLFW
	glfwTerminate();
#elif
	DestroyContext();
	ExitWindow();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Rendering()
{
	MakeContextCurrent();

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

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

bool DoEvent()
{
	return DoWindowEvent();
}

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SetCameraMatrix( const ::Effekseer::Matrix44& matrix )
{
	g_renderer->SetCameraMatrix( matrix );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
