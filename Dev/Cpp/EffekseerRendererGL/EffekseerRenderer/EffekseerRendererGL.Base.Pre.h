
#ifndef	__EFFEKSEERRENDERER_GL_BASE_PRE_H__
#define	__EFFEKSEERRENDERER_GL_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#if defined(__EFFEKSEER_RENDERER_GL_GLEW__)

#if _WIN32
#include <GL/gl.h>
#else
#include <GL/glew.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GL_GLEW_S__)

#if _WIN32
#include <GL/gl.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif


#elif defined(__EFFEKSEER_RENDERER_GLES2__)

#if defined(__APPLE__)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GLES3__)

#if defined(__APPLE__)
#include <OpenGLES/ES3/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GL2__)

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#else

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#endif

#if _WIN32
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "opengl32.lib")
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

enum class OpenGLDeviceType
{
	OpenGL2,
	OpenGL3,
	OpenGLES2,
	OpenGLES3,
	Emscripten,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_BASE_PRE_H__
