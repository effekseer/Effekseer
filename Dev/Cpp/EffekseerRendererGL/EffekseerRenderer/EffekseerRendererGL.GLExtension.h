
#ifndef	__EFFEKSEERRENDERER_GL_GLEXTENSION_H__
#define	__EFFEKSEERRENDERER_GL_GLEXTENSION_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#if _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef EMSCRIPTEN
#define		GL_GLEXT_PROTOTYPES
#define		EGL_EGLEXT_PROTOTYPES
#endif	// EMSCRIPTEN

#include <stddef.h>
#include "EffekseerRendererGL.Base.Pre.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
namespace GLExt
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STREAM_DRAW 0x88E0
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE0 0x84C0
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_EQUATION 0x8009
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

bool Initialize(OpenGLDeviceType deviceType);
bool IsSupportedVertexArray();

void glDeleteBuffers(GLsizei n, const GLuint* buffers);
GLuint glCreateShader(GLenum type);
void glBindBuffer(GLenum target, GLuint buffer);
void glGenBuffers(GLsizei n, GLuint* buffers);
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
void glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
void glBlendEquation (GLenum mode);
void glActiveTexture (GLenum texture);
void glUniform1i(GLint location, GLint v0);
void glShaderSource (GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
void glCompileShader(GLuint shader);
void glGetShaderiv (GLuint shader, GLenum pname, GLint* param);
GLuint glCreateProgram (void);
void glAttachShader(GLuint program, GLuint shader);
void glDeleteProgram(GLuint program);
void glDeleteShader(GLuint shader);
void glLinkProgram (GLuint program);
void glGetProgramiv(GLuint program, GLenum pname, GLint* param);
void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
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

void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glBindVertexArray(GLuint array);

void glGenSamplers(GLsizei n, GLuint *samplers);
void glDeleteSamplers(GLsizei n, const GLuint * samplers);
void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param);
void glBindSampler(GLuint unit, GLuint sampler);

void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_GLEXTENSION_H__