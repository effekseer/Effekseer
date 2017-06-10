
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.GLExtension.h"

#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
#include <EGL/egl.h>
#endif

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
#if _WIN32

typedef void (EFK_STDCALL * FP_glDeleteBuffers) (GLsizei n, const GLuint* buffers);
typedef GLuint (EFK_STDCALL * FP_glCreateShader) (GLenum type);
typedef void (EFK_STDCALL * FP_glBindBuffer) (GLenum target, GLuint buffer);
typedef void (EFK_STDCALL * FP_glGenBuffers) (GLsizei n, GLuint* buffers);
typedef void (EFK_STDCALL * FP_glBufferData) (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

typedef void (EFK_STDCALL * FP_glBlendEquationSeparate) (GLenum modeRGB, GLenum modeAlpha);
typedef void (EFK_STDCALL * FP_glBlendFuncSeparate) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

typedef void (EFK_STDCALL * FP_glBlendEquation) (GLenum mode);

typedef void (EFK_STDCALL * FP_glActiveTexture) (GLenum texture);

typedef void (EFK_STDCALL * FP_glUniform1i) (GLint location, GLint v0);

typedef void (EFK_STDCALL * FP_glShaderSource) (GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);

typedef void (EFK_STDCALL * FP_glCompileShader) (GLuint shader);

typedef void (EFK_STDCALL * FP_glGetShaderiv) (GLuint shader, GLenum pname, GLint* param);

typedef GLuint (EFK_STDCALL * FP_glCreateProgram) (void);

typedef void (EFK_STDCALL * FP_glAttachShader) (GLuint program, GLuint shader);

typedef void (EFK_STDCALL * FP_glDeleteProgram) (GLuint program);
typedef void (EFK_STDCALL * FP_glDeleteShader) (GLuint shader);
typedef void (EFK_STDCALL * FP_glLinkProgram) (GLuint program);

typedef void (EFK_STDCALL * FP_glGetProgramiv) (GLuint program, GLenum pname, GLint* param);

typedef void (EFK_STDCALL * FP_glGetShaderInfoLog) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (EFK_STDCALL * FP_glGetProgramInfoLog) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

typedef GLint (EFK_STDCALL * FP_glGetAttribLocation) (GLuint program, const GLchar* name);

typedef GLint (EFK_STDCALL * FP_glGetUniformLocation) (GLuint program, const GLchar* name);

typedef void (EFK_STDCALL * FP_glUseProgram) (GLuint program);

typedef void (EFK_STDCALL * FP_glEnableVertexAttribArray) (GLuint index);
typedef void (EFK_STDCALL * FP_glDisableVertexAttribArray) (GLuint index);
typedef void (EFK_STDCALL * FP_glVertexAttribPointer) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
typedef void (EFK_STDCALL * FP_glUniformMatrix4fv) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (EFK_STDCALL * FP_glUniform4fv) (GLint location, GLsizei count, const GLfloat* value);
typedef void (EFK_STDCALL * FP_glGenerateMipmap) (GLenum target);
typedef void (EFK_STDCALL * FP_glBufferSubData) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

typedef void (EFK_STDCALL * FP_glGenVertexArrays) (GLsizei n, GLuint *arrays);
typedef void (EFK_STDCALL * FP_glDeleteVertexArrays) (GLsizei n, const GLuint *arrays);
typedef void (EFK_STDCALL * FP_glBindVertexArray) (GLuint array);

typedef void (EFK_STDCALL * FP_glGenSamplers) (GLsizei n, GLuint *samplers);
typedef void (EFK_STDCALL * FP_glDeleteSamplers) (GLsizei n, const GLuint * samplers);
typedef void (EFK_STDCALL * FP_glSamplerParameteri) (GLuint sampler, GLenum pname, GLint param);
typedef void (EFK_STDCALL * FP_glBindSampler) (GLuint unit, GLuint sampler);

typedef void (EFK_STDCALL * FP_glCompressedTexImage2D) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);

static FP_glDeleteBuffers g_glDeleteBuffers = NULL;
static FP_glCreateShader g_glCreateShader = NULL;
static FP_glBindBuffer g_glBindBuffer = NULL;
static FP_glGenBuffers g_glGenBuffers = NULL;
static FP_glBufferData g_glBufferData = NULL;
static FP_glBlendEquationSeparate g_glBlendEquationSeparate = NULL;
static FP_glBlendFuncSeparate g_glBlendFuncSeparate = NULL;
static FP_glBlendEquation g_glBlendEquation = NULL;
static FP_glActiveTexture g_glActiveTexture = NULL;
static FP_glUniform1i g_glUniform1i = NULL;
static FP_glShaderSource g_glShaderSource = NULL;
static FP_glCompileShader g_glCompileShader = NULL;
static FP_glGetShaderiv g_glGetShaderiv = NULL;
static FP_glCreateProgram g_glCreateProgram = NULL;
static FP_glAttachShader g_glAttachShader = NULL;
static FP_glDeleteProgram g_glDeleteProgram = NULL;
static FP_glDeleteShader g_glDeleteShader = NULL;
static FP_glLinkProgram g_glLinkProgram = NULL;
static FP_glGetProgramiv g_glGetProgramiv = NULL;
static FP_glGetShaderInfoLog g_glGetShaderInfoLog = NULL;
static FP_glGetProgramInfoLog g_glGetProgramInfoLog = NULL;
static FP_glGetAttribLocation g_glGetAttribLocation = NULL;
static FP_glGetUniformLocation g_glGetUniformLocation = NULL;
static FP_glUseProgram g_glUseProgram = NULL;
static FP_glEnableVertexAttribArray g_glEnableVertexAttribArray = NULL;
static FP_glDisableVertexAttribArray g_glDisableVertexAttribArray = NULL;
static FP_glVertexAttribPointer g_glVertexAttribPointer = NULL;
static FP_glUniformMatrix4fv g_glUniformMatrix4fv = NULL;
static FP_glUniform4fv g_glUniform4fv = NULL;
static FP_glGenerateMipmap g_glGenerateMipmap = NULL;
static FP_glBufferSubData g_glBufferSubData = NULL;
static FP_glGenVertexArrays g_glGenVertexArrays = NULL;
static FP_glDeleteVertexArrays g_glDeleteVertexArrays = NULL;
static FP_glBindVertexArray g_glBindVertexArray = NULL;
static FP_glGenSamplers g_glGenSamplers = nullptr;
static FP_glDeleteSamplers g_glDeleteSamplers = nullptr;
static FP_glSamplerParameteri g_glSamplerParameteri = nullptr;
static FP_glBindSampler g_glBindSampler = nullptr;

static FP_glCompressedTexImage2D g_glCompressedTexImage2D = nullptr;

#elif defined(__EFFEKSEER_RENDERER_GLES2__)

typedef void (* FP_glGenVertexArraysOES) (GLsizei n, GLuint *arrays);
typedef void (* FP_glDeleteVertexArraysOES) (GLsizei n, const GLuint *arrays);
typedef void (* FP_glBindVertexArrayOES) (GLuint array);

static FP_glGenVertexArraysOES g_glGenVertexArraysOES = NULL;
static FP_glDeleteVertexArraysOES g_glDeleteVertexArraysOES = NULL;
static FP_glBindVertexArrayOES g_glBindVertexArrayOES = NULL;

#endif

static bool g_isInitialized = false;
static bool g_isSupportedVertexArray = false;

#if _WIN32
#define GET_PROC(name)	g_##name = (FP_##name)wglGetProcAddress( #name ); if(g_##name==NULL) return false;
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GLES3__)
#define GET_PROC(name)	g_##name = (FP_##name)eglGetProcAddress( #name ); if(g_##name==NULL) return false;
#endif

bool Initialize(OpenGLDeviceType deviceType)
{
	if(g_isInitialized) return true;

#if _WIN32
	GET_PROC(glDeleteBuffers);
	GET_PROC(glCreateShader);
	GET_PROC(glBindBuffer);
	GET_PROC(glGenBuffers);
	GET_PROC(glBufferData);
	GET_PROC(glBlendEquationSeparate);
	GET_PROC(glBlendFuncSeparate);
	GET_PROC(glBlendEquation);
	GET_PROC(glActiveTexture);
	GET_PROC(glUniform1i);
	GET_PROC(glShaderSource);

	GET_PROC(glCompileShader);
	GET_PROC(glGetShaderiv);
	GET_PROC(glCreateProgram);

	GET_PROC(glAttachShader);
	GET_PROC(glDeleteProgram);
	GET_PROC(glDeleteShader);

	GET_PROC(glLinkProgram);
	GET_PROC(glGetProgramiv);
	GET_PROC(glGetShaderInfoLog);

	GET_PROC(glGetProgramInfoLog);
	GET_PROC(glGetAttribLocation);
	GET_PROC(glGetUniformLocation);

	GET_PROC(glUseProgram);
	GET_PROC(glEnableVertexAttribArray);
	GET_PROC(glDisableVertexAttribArray);

	GET_PROC(glVertexAttribPointer);
	GET_PROC(glUniformMatrix4fv);
	GET_PROC(glUniform4fv);

	GET_PROC(glGenerateMipmap);
	GET_PROC(glBufferSubData);

	GET_PROC(glGenVertexArrays);
	GET_PROC(glDeleteVertexArrays);
	GET_PROC(glBindVertexArray);

	GET_PROC(glGenSamplers);
	GET_PROC(glDeleteSamplers);
	GET_PROC(glSamplerParameteri);
	GET_PROC(glBindSampler);

	GET_PROC(glCompressedTexImage2D);

	g_isSupportedVertexArray = (g_glGenVertexArrays && g_glDeleteVertexArrays && g_glBindVertexArray);
#endif

#if defined(__EFFEKSEER_RENDERER_GLES2__)

#if defined(__APPLE__)
	g_glGenVertexArraysOES = ::glGenVertexArraysOES;
	g_glDeleteVertexArraysOES = ::glDeleteVertexArraysOES;
	g_glBindVertexArrayOES = ::glBindVertexArrayOES;
	g_isSupportedVertexArray = true;
#else
	GET_PROC(glGenVertexArraysOES);
	GET_PROC(glDeleteVertexArraysOES);
	GET_PROC(glBindVertexArrayOES);
	g_isSupportedVertexArray = (g_glGenVertexArraysOES && g_glDeleteVertexArraysOES && g_glBindVertexArrayOES);
#endif

#else
	if (deviceType == OpenGLDeviceType::OpenGL3 ||
		deviceType == OpenGLDeviceType::OpenGLES3)
	{
		g_isSupportedVertexArray = true;
	}
#endif

	g_isInitialized = true;
	return true;
}

bool IsSupportedVertexArray()
{
	return g_isSupportedVertexArray;
}

void glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
#if _WIN32
	g_glDeleteBuffers(n, buffers);
#else
	::glDeleteBuffers(n, buffers);
#endif
}

GLuint glCreateShader(GLenum type)
{
#if _WIN32
	return g_glCreateShader(type);
#else
	return ::glCreateShader(type);
#endif
}

void glBindBuffer(GLenum target, GLuint buffer)
{
#if _WIN32
	return g_glBindBuffer(target, buffer);
#else
	return ::glBindBuffer(target, buffer);
#endif
}

void glGenBuffers(GLsizei n, GLuint* buffers)
{
#if _WIN32
	g_glGenBuffers(n, buffers);
#else
	::glGenBuffers(n, buffers);
#endif
}

void glBufferData (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
#if _WIN32
	g_glBufferData(target, size, data, usage);
#else
	::glBufferData(target, size, data, usage);
#endif
}

void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
#if _WIN32
	g_glBlendEquationSeparate(modeRGB, modeAlpha);
#else
	::glBlendEquationSeparate(modeRGB, modeAlpha);
#endif
}

void glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
#if _WIN32
	g_glBlendFuncSeparate (sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
#else
	::glBlendFuncSeparate (sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
#endif
}

void glBlendEquation (GLenum mode)
{
#if _WIN32
	g_glBlendEquation (mode);
#else
	::glBlendEquation (mode);
#endif
}

void glActiveTexture (GLenum texture)
{
#if _WIN32
	g_glActiveTexture (texture);
#else
	::glActiveTexture (texture);
#endif
}

void glUniform1i(GLint location, GLint v0)
{
#if _WIN32
	g_glUniform1i (location, v0);
#else
	::glUniform1i (location, v0);
#endif
}

void glShaderSource (GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
#if _WIN32
	g_glShaderSource (shader, count, strings, lengths);
#else
	::glShaderSource (shader, count, strings, lengths);
#endif
}

void glCompileShader(GLuint shader)
{
#if _WIN32
	g_glCompileShader (shader);
#else
	::glCompileShader (shader);
#endif
}

void glGetShaderiv (GLuint shader, GLenum pname, GLint* param)
{
#if _WIN32
	g_glGetShaderiv (shader, pname, param);
#else
	::glGetShaderiv (shader, pname, param);
#endif
}

GLuint glCreateProgram (void)
{
#if _WIN32
	return g_glCreateProgram ();
#else
	return ::glCreateProgram();
#endif
}

void glAttachShader(GLuint program, GLuint shader)
{
#if _WIN32
	g_glAttachShader(program, shader);
#else
	::glAttachShader(program, shader);
#endif
}

void glDeleteProgram(GLuint program)
{
#if _WIN32
	g_glDeleteProgram(program);
#else
	::glDeleteProgram(program);
#endif
}

void glDeleteShader(GLuint shader)
{
#if _WIN32
	g_glDeleteShader(shader);
#else
	::glDeleteShader(shader);
#endif
}

void glLinkProgram (GLuint program)
{
#if _WIN32
	g_glLinkProgram(program);
#else
	::glLinkProgram(program);
#endif
}

void glGetProgramiv(GLuint program, GLenum pname, GLint* param)
{
#if _WIN32
	g_glGetProgramiv(program, pname, param);
#else
	::glGetProgramiv(program, pname, param);
#endif
}

void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
#if _WIN32
	g_glGetShaderInfoLog(shader, bufSize, length, infoLog);
#else
	::glGetShaderInfoLog(shader, bufSize, length, infoLog);
#endif
}

void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
#if _WIN32
	g_glGetProgramInfoLog(program, bufSize, length, infoLog);
#else
	::glGetProgramInfoLog(program, bufSize, length, infoLog);
#endif
}

GLint glGetAttribLocation(GLuint program, const GLchar* name)
{
#if _WIN32
	return g_glGetAttribLocation(program, name);
#else
	return ::glGetAttribLocation(program, name);
#endif
}

GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
#if _WIN32
	return g_glGetUniformLocation(program, name);
#else
	return ::glGetUniformLocation(program, name);
#endif
}

void glUseProgram(GLuint program)
{
#if _WIN32
	g_glUseProgram(program);
#else
	::glUseProgram(program);
#endif
}

void glEnableVertexAttribArray(GLuint index)
{
#if _WIN32
	g_glEnableVertexAttribArray(index);
#else
	::glEnableVertexAttribArray(index);
#endif
}

void glDisableVertexAttribArray(GLuint index)
{
#if _WIN32
	g_glDisableVertexAttribArray(index);
#else
	::glDisableVertexAttribArray(index);
#endif
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
#if _WIN32
	g_glVertexAttribPointer(index, size, type, normalized ,stride, pointer);
#else
	::glVertexAttribPointer(index, size, type, normalized ,stride, pointer);
#endif
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if _WIN32
	g_glUniformMatrix4fv(location, count, transpose, value);
#else
	::glUniformMatrix4fv(location, count, transpose, value);
#endif
}

void glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
#if _WIN32
	g_glUniform4fv(location, count, value);
#else
	::glUniform4fv(location, count, value);
#endif
}

void glGenerateMipmap(GLenum target)
{
#if _WIN32
	g_glGenerateMipmap(target);
#else
	::glGenerateMipmap(target);
#endif
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
#if _WIN32
	g_glBufferSubData(target, offset, size, data);
#else
	::glBufferSubData(target, offset, size, data);
#endif
}

void glGenVertexArrays(GLsizei n, GLuint *arrays)
{
#if _WIN32
	g_glGenVertexArrays(n, arrays);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	g_glGenVertexArraysOES(n, arrays);
#else
	::glGenVertexArrays(n, arrays);
#endif
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
#if _WIN32
	g_glDeleteVertexArrays(n, arrays);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	g_glDeleteVertexArraysOES(n, arrays);
#else
	::glDeleteVertexArrays(n, arrays);
#endif
}

void glBindVertexArray(GLuint array)
{
#if _WIN32
	g_glBindVertexArray(array);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	g_glBindVertexArrayOES(array);
#else
	::glBindVertexArray(array);
#endif
}

void glGenSamplers(GLsizei n, GLuint *samplers)
{
#if _WIN32
	g_glGenSamplers(n, samplers);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glGenSamplers(n, samplers);
#endif
}

void glDeleteSamplers(GLsizei n, const GLuint * samplers)
{
#if _WIN32
	g_glDeleteSamplers(n, samplers);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glDeleteSamplers(n, samplers);
#endif
}

void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
#if _WIN32
	g_glSamplerParameteri(sampler, pname, param);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glSamplerParameteri(sampler, pname, param);
#endif
}

void glBindSampler(GLuint unit, GLuint sampler)
{
#if _WIN32
	g_glBindSampler(unit, sampler);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glBindSampler(unit, sampler);
#endif
}

void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
{
#if _WIN32
	g_glCompressedTexImage2D(target, level,internalformat, width, height, border,imageSize, data);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	glCompressedTexImage2D(target, level,internalformat, width, height, border,imageSize, data);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
}
