
#ifndef __EFFEKSEERRENDERER_GL_GLEXTENSION_H__
#define __EFFEKSEERRENDERER_GL_GLEXTENSION_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(__EFFEKSEER_RENDERER_GLES2__)

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

#if _WIN32
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#else

#ifdef EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif // EMSCRIPTEN

#if defined(_WIN32)
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#endif

#include "EffekseerRendererGL.Base.Pre.h"
#include <stddef.h>

#if defined(_WIN32) || defined(EMSCRIPTEN)
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
#endif

#if defined(_WIN32) || defined(EMSCRIPTEN) || defined(__ANDROID__) || (defined(__APPLE__) && (defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GLES3__)))
#define GL_BGRA 0x80E1
#define GL_DEPTH_COMPONENT32 0x81A7
#endif

#if defined(_WIN32) || defined(EMSCRIPTEN) || defined(__ANDROID__) || (defined(__APPLE__) && defined(__EFFEKSEER_RENDERER_GLES2__))
typedef char GLchar;

#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STREAM_DRAW 0x88E0
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_FUNC_ADD 0x8006
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE0 0x84C0
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_EQUATION 0x8009
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_CURRENT_PROGRAM 0x8B8D

#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_SRGB8_ALPHA8 0x8C43

#define GL_FRAMEBUFFER_SRGB 0x8DB9

#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020

#define GL_R8 0x8229
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG 0x8227
#define GL_RG16F 0x822f

#define GL_HALF_FLOAT 0x140b

#define GL_RGBA16F 0x881a
#define GL_RGBA32F 0x8814

#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_DEPTH_COMPONENT24 0x81A6

#define GL_DEPTH_STENCIL 0x84F9

#define GL_FRAMEBUFFER 0x8D40
#define GL_FRAMEBUFFER_BINDING 0x8CA6

#define GL_RENDERBUFFER 0x8D41
#define GL_RENDERBUFFER_BINDING 0x8CA7

#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20

#define GL_RED 0x1903

#define GL_MAX_VARYING_VECTORS 0x8DFC

#endif

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F

// TODO why redifinition
//#ifndef GL_WRITE_ONLY
#define GL_WRITE_ONLY 0x000088b9
//#endif


namespace EffekseerRendererGL
{
namespace GLExt
{

OpenGLDeviceType GetDeviceType();
bool Initialize(OpenGLDeviceType deviceType, bool isExtensionsEnabled);
bool IsSupportedVertexArray();
bool IsSupportedBufferRange();
bool IsSupportedMapBuffer();

//! for some devices to avoid a bug
void MakeMapBufferInvalid();

void glDeleteBuffers(GLsizei n, const GLuint* buffers);
GLuint glCreateShader(GLenum type);
void glBindBuffer(GLenum target, GLuint buffer);
void glGenBuffers(GLsizei n, GLuint* buffers);
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void glBlendEquation(GLenum mode);
void glActiveTexture(GLenum texture);
void glUniform1i(GLint location, GLint v0);
void glShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
void glCompileShader(GLuint shader);
void glGetShaderiv(GLuint shader, GLenum pname, GLint* param);
GLuint glCreateProgram(void);
void glAttachShader(GLuint program, GLuint shader);
void glDeleteProgram(GLuint program);
void glDeleteShader(GLuint shader);
void glLinkProgram(GLuint program);
void glGetProgramiv(GLuint program, GLenum pname, GLint* param);
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
GLint glGetAttribLocation(GLuint program, const GLchar* name);
GLint glGetUniformLocation(GLuint program, const GLchar* name);
void glUseProgram(GLuint program);
void glEnableVertexAttribArray(GLuint index);
void glDisableVertexAttribArray(GLuint index);
void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
void glUniform4fv(GLint location, GLsizei count, const GLfloat* value);
void glGenerateMipmap(GLenum target);
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

void glGenVertexArrays(GLsizei n, GLuint* arrays);
void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
void glBindVertexArray(GLuint array);

void glGenSamplers(GLsizei n, GLuint* samplers);
void glDeleteSamplers(GLsizei n, const GLuint* samplers);
void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param);
void glBindSampler(GLuint unit, GLuint sampler);

void* glMapBuffer(GLenum target, GLenum access);
void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
GLboolean glUnmapBuffer(GLenum target);

void glDrawElementsInstanced(GLenum mode,
							 GLsizei count,
							 GLenum type,
							 const void* indices,
							 GLsizei primcount);

void glCompressedTexImage2D(
	GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);

void glGenFramebuffers(GLsizei n, GLuint* ids);

void glBindFramebuffer(GLenum target, GLuint framebuffer);

void glDeleteFramebuffers(GLsizei n, GLuint* framebuffers);

void glFramebufferTexture2D(GLenum target,
							GLenum attachment,
							GLenum textarget,
							GLuint texture,
							GLint level);

void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);

void glBindRenderbuffer(GLenum target, GLuint renderbuffer);

void glDeleteRenderbuffers(GLsizei n, GLuint* renderbuffers);

void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

void glDrawBuffers(GLsizei n, const GLenum* bufs);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace GLExt
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_GLEXTENSION_H__
