
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <assert.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include "../Effekseer/Effekseer.h"
#include "../EffekseerRendererGL/EffekseerRendererGL.h"
#include "graphics.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "zlib.lib")

#if _DEBUG
#pragma comment(lib, "EffekseerRendererGL.Debug.lib" )
#else
#pragma comment(lib, "EffekseerRendererGL.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static void WaitFrame();

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HDC					g_hDC = NULL;
static HGLRC				g_hGLRC = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;
extern ::Effekseer::Manager*			g_manager;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitGraphics( HWND handle, int width, int height )
{
	timeBeginPeriod(1);

	g_hDC = GetDC( handle );

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

	wglMakeCurrent( g_hDC, g_hGLRC );

	GLenum glewIniterr = glewInit();
    assert ( glewIniterr == GLEW_OK );

	wglSwapIntervalEXT(1);
	glViewport( 0, 0, width, height );

	g_renderer = ::EffekseerRendererGL::Renderer::Create( 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)width / (float)height, 1.0f, 50.0f ) );
	
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );

	g_manager->SetCoordinateSystem( ::Effekseer::COORDINATE_SYSTEM_RH );

	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermGraphics()
{
	g_renderer->Destory();
	wglMakeCurrent( 0, 0 );
	wglDeleteContext( g_hGLRC );

	timeEndPeriod(1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Rendering()
{
	wglMakeCurrent( g_hDC, g_hGLRC );
	wglSwapIntervalEXT( 1 );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	g_renderer->BeginRendering();
	g_manager->Draw();
	g_renderer->EndRendering();

	glFlush();
	wglMakeCurrent( 0, 0 );

	WaitFrame();
	SwapBuffers( g_hDC );
}

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
static void WaitFrame()
{
	static DWORD beforeTime = timeGetTime();
	DWORD currentTime = timeGetTime();
	
	DWORD elapsedTime = currentTime - beforeTime;
	if (elapsedTime < 16) {
		Sleep(16 - elapsedTime);
	}
	beforeTime = timeGetTime();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------