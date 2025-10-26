#pragma once

#include <cstdint>
#include <type_traits>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

//! after include windows.h
#include <GL/gl.h>

#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <OpenGL/gl3.h>
#else
#include <OpenGLES/ES3/gl.h>
#endif
#else
#include <GL/gl.h>
#endif

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#define OGL_EXT_STDCALL __stdcall
#else
#define OGL_EXT_STDCALL
#endif

#if defined(_WIN32) || defined(__EMSCRIPTEN__) || defined(__ANDROID__) || (defined(__APPLE__))
typedef char GLchar;
#endif

#ifndef GL_READ_FRAMEBUFFER
#define GL_READ_FRAMEBUFFER 0x8CA8
#endif

#ifndef GL_DRAW_FRAMEBUFFER
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#endif

#ifndef GL_DEBUG_TYPE_PORTABILITY
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#endif

#ifndef GL_DEBUG_TYPE_OTHER
#define GL_DEBUG_TYPE_OTHER 0x8251
#endif

#ifndef GL_CONTEXT_FLAGS
#define GL_CONTEXT_FLAGS 0x821E
#endif

#ifndef GL_CONTEXT_FLAG_DEBUG_BIT
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#endif

#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#endif

#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#endif

#ifndef GL_MAP_READ_BIT
#define GL_MAP_READ_BIT 0x0001
#endif

#ifndef GL_MAP_INVALIDATE_RANGE_BIT
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#endif

#ifndef GL_MAP_INVALIDATE_BUFFER_BIT
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#endif

#ifndef GL_MAP_FLUSH_EXPLICIT_BIT
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#endif

#ifndef GL_R32F
#define GL_R32F 0x822E
#endif

#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24 0x81A6
#endif

#ifndef GL_COLOR_ATTACHMENT4
#define GL_COLOR_ATTACHMENT4 0x8CE4
#endif

#ifndef GL_COLOR_ATTACHMENT5
#define GL_COLOR_ATTACHMENT5 0x8CE5
#endif

#ifndef GL_COLOR_ATTACHMENT6
#define GL_COLOR_ATTACHMENT6 0x8CE6
#endif

#ifndef GL_COLOR_ATTACHMENT7
#define GL_COLOR_ATTACHMENT7 0x8CE7
#endif

#ifndef GL_COLOR_ATTACHMENT8
#define GL_COLOR_ATTACHMENT8 0x8CE8
#endif

#ifndef GL_COLOR_ATTACHMENT9
#define GL_COLOR_ATTACHMENT9 0x8CE9
#endif

#ifndef GL_COLOR_ATTACHMENT10
#define GL_COLOR_ATTACHMENT10 0x8CEA
#endif

#ifndef GL_COLOR_ATTACHMENT11
#define GL_COLOR_ATTACHMENT11 0x8CEB
#endif

#ifndef GL_COLOR_ATTACHMENT12
#define GL_COLOR_ATTACHMENT12 0x8CEC
#endif

#ifndef GL_COLOR_ATTACHMENT13
#define GL_COLOR_ATTACHMENT13 0x8CED
#endif

#ifndef GL_COLOR_ATTACHMENT14
#define GL_COLOR_ATTACHMENT14 0x8CEE
#endif

#ifndef GL_COLOR_ATTACHMENT15
#define GL_COLOR_ATTACHMENT15 0x8CEF
#endif

#ifndef GL_STENCIL_ATTACHMENT
#define GL_STENCIL_ATTACHMENT 0x8D20
#endif

#ifndef GL_WRITE_ONLY
#define GL_WRITE_ONLY 0x88B9
#endif

#ifndef GL_QUERY_RESULT_AVAILABLE
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#endif

namespace Effekseer
{
namespace OpenGLHelper
{

bool Initialize();
bool IsSupportedDebugOutput();

void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value);
void glCopyTexSubImage2D(GLenum target,
						 GLint level,
						 GLint xoffset,
						 GLint yoffset,
						 GLint x,
						 GLint y,
						 GLsizei width,
						 GLsizei height);
void glBlitFramebuffer(GLint srcX0,
					   GLint srcY0,
					   GLint srcX1,
					   GLint srcY1,
					   GLint dstX0,
					   GLint dstY0,
					   GLint dstX1,
					   GLint dstY1,
					   GLbitfield mask,
					   GLenum filter);
void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);

using DebugCallback = void(OGL_EXT_STDCALL*)(GLenum source,
											 GLenum type,
											 GLuint id,
											 GLenum severity,
											 GLsizei length,
											 const GLchar* message,
											 const void* userParam);

void glDebugMessageCallback(DebugCallback callback, const void* userParam);
void glDebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);

} // namespace OpenGLHelper
} // namespace Effekseer
