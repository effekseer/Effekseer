
#ifndef	__EFFEKSEERRENDERER_BASE_PRE_H__
#define	__EFFEKSEERRENDERER_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#if defined(__EFFEKSEER_RENDERER_GL_GLEW__)

#if _WIN32
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GL_GLEW_S__)
#define GLEW_STATIC
#include <GL/glew.h>

#elif defined(__EFFEKSEER_RENDERER_GLES2__)

#if defined(__APPLE__)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#else

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_BASE_PRE_H__
