
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.GLExtension.h"
#include "Effekseer.h"

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

typedef void(EFK_STDCALL* FP_glDeleteBuffers)(GLsizei n, const GLuint* buffers);
typedef GLuint(EFK_STDCALL* FP_glCreateShader)(GLenum type);
typedef void(EFK_STDCALL* FP_glBindBuffer)(GLenum target, GLuint buffer);
typedef void(EFK_STDCALL* FP_glGenBuffers)(GLsizei n, GLuint* buffers);
typedef void(EFK_STDCALL* FP_glBufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

typedef void(EFK_STDCALL* FP_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
typedef void(EFK_STDCALL* FP_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);

typedef void(EFK_STDCALL* FP_glBlendEquation)(GLenum mode);

typedef void(EFK_STDCALL* FP_glActiveTexture)(GLenum texture);

typedef void(EFK_STDCALL* FP_glUniform1i)(GLint location, GLint v0);

typedef void(EFK_STDCALL* FP_glShaderSource)(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);

typedef void(EFK_STDCALL* FP_glCompileShader)(GLuint shader);

typedef void(EFK_STDCALL* FP_glGetShaderiv)(GLuint shader, GLenum pname, GLint* param);

typedef GLuint(EFK_STDCALL* FP_glCreateProgram)(void);

typedef void(EFK_STDCALL* FP_glAttachShader)(GLuint program, GLuint shader);

typedef void(EFK_STDCALL* FP_glDeleteProgram)(GLuint program);
typedef void(EFK_STDCALL* FP_glDeleteShader)(GLuint shader);
typedef void(EFK_STDCALL* FP_glLinkProgram)(GLuint program);

typedef void(EFK_STDCALL* FP_glGetProgramiv)(GLuint program, GLenum pname, GLint* param);

typedef void(EFK_STDCALL* FP_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void(EFK_STDCALL* FP_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);

typedef GLint(EFK_STDCALL* FP_glGetAttribLocation)(GLuint program, const GLchar* name);

typedef GLint(EFK_STDCALL* FP_glGetUniformLocation)(GLuint program, const GLchar* name);

typedef void(EFK_STDCALL* FP_glUseProgram)(GLuint program);

typedef void(EFK_STDCALL* FP_glEnableVertexAttribArray)(GLuint index);
typedef void(EFK_STDCALL* FP_glDisableVertexAttribArray)(GLuint index);
typedef void(EFK_STDCALL* FP_glVertexAttribPointer)(
	GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
typedef void(EFK_STDCALL* FP_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void(EFK_STDCALL* FP_glUniform4fv)(GLint location, GLsizei count, const GLfloat* value);
typedef void(EFK_STDCALL* FP_glGenerateMipmap)(GLenum target);
typedef void(EFK_STDCALL* FP_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

typedef void(EFK_STDCALL* FP_glGenVertexArrays)(GLsizei n, GLuint* arrays);
typedef void(EFK_STDCALL* FP_glDeleteVertexArrays)(GLsizei n, const GLuint* arrays);
typedef void(EFK_STDCALL* FP_glBindVertexArray)(GLuint array);

typedef void(EFK_STDCALL* FP_glGenSamplers)(GLsizei n, GLuint* samplers);
typedef void(EFK_STDCALL* FP_glDeleteSamplers)(GLsizei n, const GLuint* samplers);
typedef void(EFK_STDCALL* FP_glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
typedef void(EFK_STDCALL* FP_glBindSampler)(GLuint unit, GLuint sampler);

typedef void*(EFK_STDCALL* FP_glMapBuffer)(GLenum target, GLenum access);
typedef void*(EFK_STDCALL* FP_glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean(EFK_STDCALL* FP_glUnmapBuffer)(GLenum target);

typedef void(EFK_STDCALL* FP_glDrawElementsInstanced)(GLenum mode,
													  GLsizei count,
													  GLenum type,
													  const void* indices,
													  GLsizei primcount);

typedef void(EFK_STDCALL* FP_glCompressedTexImage2D)(
	GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);

typedef void(EFK_STDCALL* FP_glGenFramebuffers)(GLsizei n, GLuint* ids);

typedef void(EFK_STDCALL* FP_glBindFramebuffer)(GLenum target, GLuint framebuffer);

typedef void(EFK_STDCALL* FP_glDeleteFramebuffers)(GLsizei n, GLuint* framebuffers);

typedef void(EFK_STDCALL* FP_glFramebufferTexture2D)(GLenum target,
													 GLenum attachment,
													 GLenum textarget,
													 GLuint texture,
													 GLint level);

typedef void(EFK_STDCALL* FP_glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);

typedef void(EFK_STDCALL* FP_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);

typedef void(EFK_STDCALL* FP_glDeleteRenderbuffers)(GLsizei n, GLuint* renderbuffers);

typedef void(EFK_STDCALL* FP_glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);

typedef void(EFK_STDCALL* FP_glDrawBuffers)(GLsizei n, const GLenum* bufs);

static FP_glDeleteBuffers g_glDeleteBuffers = nullptr;
static FP_glCreateShader g_glCreateShader = nullptr;
static FP_glBindBuffer g_glBindBuffer = nullptr;
static FP_glGenBuffers g_glGenBuffers = nullptr;
static FP_glBufferData g_glBufferData = nullptr;
static FP_glBlendEquationSeparate g_glBlendEquationSeparate = nullptr;
static FP_glBlendFuncSeparate g_glBlendFuncSeparate = nullptr;
static FP_glBlendEquation g_glBlendEquation = nullptr;
static FP_glActiveTexture g_glActiveTexture = nullptr;
static FP_glUniform1i g_glUniform1i = nullptr;
static FP_glShaderSource g_glShaderSource = nullptr;
static FP_glCompileShader g_glCompileShader = nullptr;
static FP_glGetShaderiv g_glGetShaderiv = nullptr;
static FP_glCreateProgram g_glCreateProgram = nullptr;
static FP_glAttachShader g_glAttachShader = nullptr;
static FP_glDeleteProgram g_glDeleteProgram = nullptr;
static FP_glDeleteShader g_glDeleteShader = nullptr;
static FP_glLinkProgram g_glLinkProgram = nullptr;
static FP_glGetProgramiv g_glGetProgramiv = nullptr;
static FP_glGetShaderInfoLog g_glGetShaderInfoLog = nullptr;
static FP_glGetProgramInfoLog g_glGetProgramInfoLog = nullptr;
static FP_glGetAttribLocation g_glGetAttribLocation = nullptr;
static FP_glGetUniformLocation g_glGetUniformLocation = nullptr;
static FP_glUseProgram g_glUseProgram = nullptr;
static FP_glEnableVertexAttribArray g_glEnableVertexAttribArray = nullptr;
static FP_glDisableVertexAttribArray g_glDisableVertexAttribArray = nullptr;
static FP_glVertexAttribPointer g_glVertexAttribPointer = nullptr;
static FP_glUniformMatrix4fv g_glUniformMatrix4fv = nullptr;
static FP_glUniform4fv g_glUniform4fv = nullptr;
static FP_glGenerateMipmap g_glGenerateMipmap = nullptr;
static FP_glBufferSubData g_glBufferSubData = nullptr;
static FP_glGenVertexArrays g_glGenVertexArrays = nullptr;
static FP_glDeleteVertexArrays g_glDeleteVertexArrays = nullptr;
static FP_glBindVertexArray g_glBindVertexArray = nullptr;
static FP_glGenSamplers g_glGenSamplers = nullptr;
static FP_glDeleteSamplers g_glDeleteSamplers = nullptr;
static FP_glSamplerParameteri g_glSamplerParameteri = nullptr;
static FP_glBindSampler g_glBindSampler = nullptr;

static FP_glMapBuffer g_glMapBuffer = nullptr;
static FP_glMapBufferRange g_glMapBufferRange = nullptr;
static FP_glUnmapBuffer g_glUnmapBuffer = nullptr;

static FP_glDrawElementsInstanced g_glDrawElementsInstanced = nullptr;

static FP_glCompressedTexImage2D g_glCompressedTexImage2D = nullptr;

static FP_glGenFramebuffers g_glGenFramebuffers = nullptr;

static FP_glBindFramebuffer g_glBindFramebuffer = nullptr;

static FP_glGenRenderbuffers g_glGenRenderbuffers = nullptr;

static FP_glBindRenderbuffer g_glBindRenderbuffer = nullptr;

static FP_glDeleteFramebuffers g_glDeleteFramebuffers = nullptr;

static FP_glFramebufferTexture2D g_glFramebufferTexture2D = nullptr;

static FP_glDeleteRenderbuffers g_glDeleteRenderbuffers = nullptr;

static FP_glRenderbufferStorageMultisample g_glRenderbufferStorageMultisample = nullptr;

static FP_glDrawBuffers g_glDrawBuffers = nullptr;

#elif defined(__EFFEKSEER_RENDERER_GLES2__)

typedef void (*FP_glGenVertexArraysOES)(GLsizei n, GLuint* arrays);
typedef void (*FP_glDeleteVertexArraysOES)(GLsizei n, const GLuint* arrays);
typedef void (*FP_glBindVertexArrayOES)(GLuint array);

typedef void* (*FP_glMapBufferOES)(GLenum target, GLenum access);
typedef void* (*FP_glMapBufferRangeEXT)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (*FP_glUnmapBufferOES)(GLenum target);

#ifdef EMSCRIPTEN
typedef void(EFK_STDCALL* FP_glDrawElementsInstancedANGLE)(GLenum mode,
														   GLsizei count,
														   GLenum type,
														   const void* indices,
														   GLsizei primcount);

#endif

static FP_glGenVertexArraysOES g_glGenVertexArraysOES = nullptr;
static FP_glDeleteVertexArraysOES g_glDeleteVertexArraysOES = nullptr;
static FP_glBindVertexArrayOES g_glBindVertexArrayOES = nullptr;

static FP_glMapBufferOES g_glMapBufferOES = nullptr;
static FP_glMapBufferRangeEXT g_glMapBufferRangeEXT = nullptr;
static FP_glUnmapBufferOES g_glUnmapBufferOES = nullptr;

#ifdef EMSCRIPTEN
static FP_glDrawElementsInstancedANGLE g_glDrawElementsInstancedANGLE = nullptr;
#endif

#endif

static bool g_isInitialized = false;
static bool g_isSupportedVertexArray = false;
static bool g_isSurrpotedBufferRange = false;
static bool g_isSurrpotedMapBuffer = false;
static OpenGLDeviceType g_deviceType = OpenGLDeviceType::OpenGL2;

#if _WIN32
#define GET_PROC_REQ(name)                                                                       \
	g_##name = (FP_##name)wglGetProcAddress(#name);                                              \
	if (g_##name == nullptr)                                                                     \
	{                                                                                            \
		Effekseer::Log(Effekseer::LogType::Error, "Failed to get proc : " + std::string(#name)); \
		return false;                                                                            \
	}
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GLES3__)
#define GET_PROC_REQ(name)                                                                       \
	g_##name = (FP_##name)eglGetProcAddress(#name);                                              \
	if (g_##name == nullptr)                                                                     \
	{                                                                                            \
		Effekseer::Log(Effekseer::LogType::Error, "Failed to get proc : " + std::string(#name)); \
		return false;                                                                            \
	}
#endif

#if _WIN32
#define GET_PROC(name)                                                                             \
	g_##name = (FP_##name)wglGetProcAddress(#name);                                                \
	if (g_##name == nullptr)                                                                       \
	{                                                                                              \
		Effekseer::Log(Effekseer::LogType::Warning, "Failed to get proc : " + std::string(#name)); \
	}
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GLES3__)
#define GET_PROC(name)                                                                             \
	g_##name = (FP_##name)eglGetProcAddress(#name);                                                \
	if (g_##name == nullptr)                                                                       \
	{                                                                                              \
		Effekseer::Log(Effekseer::LogType::Warning, "Failed to get proc : " + std::string(#name)); \
	}
#endif

OpenGLDeviceType GetDeviceType()
{
	return g_deviceType;
}

bool Initialize(OpenGLDeviceType deviceType, bool isExtensionsEnabled)
{
	if (g_isInitialized)
		return true;
	g_deviceType = deviceType;
#if _WIN32
	GET_PROC_REQ(glDeleteBuffers);
	GET_PROC_REQ(glCreateShader);
	GET_PROC_REQ(glBindBuffer);
	GET_PROC_REQ(glGenBuffers);
	GET_PROC_REQ(glBufferData);
	GET_PROC_REQ(glBlendEquationSeparate);
	GET_PROC_REQ(glBlendFuncSeparate);
	GET_PROC_REQ(glBlendEquation);
	GET_PROC_REQ(glActiveTexture);
	GET_PROC_REQ(glUniform1i);
	GET_PROC_REQ(glShaderSource);

	GET_PROC_REQ(glCompileShader);
	GET_PROC_REQ(glGetShaderiv);
	GET_PROC_REQ(glCreateProgram);

	GET_PROC_REQ(glAttachShader);
	GET_PROC_REQ(glDeleteProgram);
	GET_PROC_REQ(glDeleteShader);

	GET_PROC_REQ(glLinkProgram);
	GET_PROC_REQ(glGetProgramiv);
	GET_PROC_REQ(glGetShaderInfoLog);

	GET_PROC_REQ(glGetProgramInfoLog);
	GET_PROC_REQ(glGetAttribLocation);
	GET_PROC_REQ(glGetUniformLocation);

	GET_PROC_REQ(glUseProgram);
	GET_PROC_REQ(glEnableVertexAttribArray);
	GET_PROC_REQ(glDisableVertexAttribArray);

	GET_PROC_REQ(glVertexAttribPointer);
	GET_PROC_REQ(glUniformMatrix4fv);
	GET_PROC_REQ(glUniform4fv);

	GET_PROC_REQ(glGenerateMipmap);
	GET_PROC_REQ(glBufferSubData);

	GET_PROC_REQ(glGenVertexArrays);
	GET_PROC_REQ(glDeleteVertexArrays);
	GET_PROC_REQ(glBindVertexArray);

	GET_PROC_REQ(glGenSamplers);
	GET_PROC_REQ(glDeleteSamplers);
	GET_PROC_REQ(glSamplerParameteri);
	GET_PROC_REQ(glBindSampler);

	GET_PROC_REQ(glMapBuffer);
	GET_PROC_REQ(glMapBufferRange);
	GET_PROC_REQ(glUnmapBuffer);

	GET_PROC_REQ(glDrawElementsInstanced);

	GET_PROC_REQ(glCompressedTexImage2D);

	GET_PROC_REQ(glGenFramebuffers);

	GET_PROC_REQ(glBindFramebuffer);

	GET_PROC_REQ(glDeleteFramebuffers);

	GET_PROC(glFramebufferTexture2D);

	GET_PROC(glGenRenderbuffers);

	GET_PROC(glBindRenderbuffer);

	GET_PROC(glDeleteRenderbuffers);

	GET_PROC(glRenderbufferStorageMultisample);

	GET_PROC_REQ(glDrawBuffers);

	g_isSupportedVertexArray = (g_glGenVertexArrays && g_glDeleteVertexArrays && g_glBindVertexArray);
	g_isSurrpotedBufferRange = (g_glMapBufferRange && g_glUnmapBuffer);
	g_isSurrpotedMapBuffer = (g_glMapBuffer && g_glUnmapBuffer);

#endif

#if defined(__EFFEKSEER_RENDERER_GLES2__)

#if defined(__APPLE__)
	g_glGenVertexArraysOES = ::glGenVertexArraysOES;
	g_glDeleteVertexArraysOES = ::glDeleteVertexArraysOES;
	g_glBindVertexArrayOES = ::glBindVertexArrayOES;
	g_isSupportedVertexArray = true;

	g_glMapBufferRangeEXT = ::glMapBufferRangeEXT;
	g_glMapBufferOES = ::glMapBufferOES;
	g_glUnmapBufferOES = ::glUnmapBufferOES;
	g_isSurrpotedBufferRange = true;
	g_isSurrpotedMapBuffer = true;
#else
	char* glExtensions = (char*)glGetString(GL_EXTENSIONS);

	if (isExtensionsEnabled)
	{
		GET_PROC(glGenVertexArraysOES);
		GET_PROC(glDeleteVertexArraysOES);
		GET_PROC(glBindVertexArrayOES);
	}

#if defined(__EMSCRIPTEN__)
	if (isExtensionsEnabled)
	{
		GET_PROC(glDrawElementsInstancedANGLE);
	}

	g_isSupportedVertexArray = (g_glGenVertexArraysOES && g_glDeleteVertexArraysOES && g_glBindVertexArrayOES &&
								((glExtensions && strstr(glExtensions, "OES_vertex_array_object")) ? true : false));
#else
	g_isSupportedVertexArray = (g_glGenVertexArraysOES && g_glDeleteVertexArraysOES && g_glBindVertexArrayOES &&
								((glExtensions && strstr(glExtensions, "GL_OES_vertex_array_object")) ? true : false));
#endif

	// Some smartphone causes segmentation fault.
	// GET_PROC(glMapBufferRangeEXT);

#ifdef EMSCRIPTEN
	g_isSurrpotedBufferRange = false;
	g_isSurrpotedMapBuffer = false;
#else
	if (isExtensionsEnabled)
	{
		GET_PROC(glMapBufferOES);
		GET_PROC(glUnmapBufferOES);
	}
	g_isSurrpotedBufferRange = (g_glMapBufferRangeEXT && g_glUnmapBufferOES);
	g_isSurrpotedMapBuffer =
		(g_glMapBufferOES && g_glUnmapBufferOES && ((glExtensions && strstr(glExtensions, "GL_OES_mapbuffer")) ? true : false));
#endif

#endif

#else
	if (deviceType == OpenGLDeviceType::OpenGL3 || deviceType == OpenGLDeviceType::OpenGLES3)
	{
		g_isSupportedVertexArray = true;
		g_isSurrpotedBufferRange = true;
	}
	if (deviceType == OpenGLDeviceType::OpenGL3)
	{
		g_isSurrpotedMapBuffer = true;
	}

#endif

	g_isInitialized = true;
	return true;
}

bool IsSupportedVertexArray()
{
	return g_isSupportedVertexArray;
}

bool IsSupportedBufferRange()
{
	return g_isSurrpotedBufferRange;
}

bool IsSupportedMapBuffer()
{
	return g_isSurrpotedMapBuffer;
}

void MakeMapBufferInvalid()
{
	g_isSurrpotedMapBuffer = false;
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

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
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

void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
#if _WIN32
	g_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
#else
	::glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
#endif
}

void glBlendEquation(GLenum mode)
{
#if _WIN32
	g_glBlendEquation(mode);
#else
	::glBlendEquation(mode);
#endif
}

void glActiveTexture(GLenum texture)
{
#if _WIN32
	g_glActiveTexture(texture);
#else
	::glActiveTexture(texture);
#endif
}

void glUniform1i(GLint location, GLint v0)
{
#if _WIN32
	g_glUniform1i(location, v0);
#else
	::glUniform1i(location, v0);
#endif
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
#if _WIN32
	g_glShaderSource(shader, count, strings, lengths);
#else
	::glShaderSource(shader, count, strings, lengths);
#endif
}

void glCompileShader(GLuint shader)
{
#if _WIN32
	g_glCompileShader(shader);
#else
	::glCompileShader(shader);
#endif
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint* param)
{
#if _WIN32
	g_glGetShaderiv(shader, pname, param);
#else
	::glGetShaderiv(shader, pname, param);
#endif
}

GLuint glCreateProgram(void)
{
#if _WIN32
	return g_glCreateProgram();
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

void glLinkProgram(GLuint program)
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

void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
#if _WIN32
	g_glGetShaderInfoLog(shader, bufSize, length, infoLog);
#else
	::glGetShaderInfoLog(shader, bufSize, length, infoLog);
#endif
}

void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
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
	g_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
#else
	::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
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

void glGenVertexArrays(GLsizei n, GLuint* arrays)
{
#if _WIN32
	g_glGenVertexArrays(n, arrays);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	g_glGenVertexArraysOES(n, arrays);
#else
	::glGenVertexArrays(n, arrays);
#endif
}

void glDeleteVertexArrays(GLsizei n, const GLuint* arrays)
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

void glGenSamplers(GLsizei n, GLuint* samplers)
{
#if _WIN32
	g_glGenSamplers(n, samplers);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glGenSamplers(n, samplers);
#endif
}

void glDeleteSamplers(GLsizei n, const GLuint* samplers)
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

void* glMapBuffer(GLenum target, GLenum access)
{
#if _WIN32
	return g_glMapBuffer(target, access);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	return g_glMapBufferOES(target, access);
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
	return nullptr;
#else
	return ::glMapBuffer(target, access);
#endif
}

void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
#if _WIN32
	return g_glMapBufferRange(target, offset, length, access);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	return g_glMapBufferRangeEXT(target, offset, length, access);
#else

#if defined(__APPLE__)

#if defined(GL_ARB_map_buffer_range)
	return ::glMapBufferRange(target, offset, length, access);
#else
	return nullptr;
#endif

#else
	return ::glMapBufferRange(target, offset, length, access);
#endif

#endif
}

GLboolean glUnmapBuffer(GLenum target)
{
#if _WIN32
	return g_glUnmapBuffer(target);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
	return g_glUnmapBufferOES(target);
#else
	return ::glUnmapBuffer(target);
#endif
}

void glDrawElementsInstanced(GLenum mode,
							 GLsizei count,
							 GLenum type,
							 const void* indices,
							 GLsizei primcount)
{
#if _WIN32
	return g_glDrawElementsInstanced(mode, count, type, indices, primcount);
#elif defined(__EFFEKSEER_RENDERER_GLES2__)

#ifdef EMSCRIPTEN
	return g_glDrawElementsInstancedANGLE(mode, count, type, indices, primcount);
#endif
	return;
#else
	return ::glDrawElementsInstanced(mode, count, type, indices, primcount);
#endif
}

void glCompressedTexImage2D(
	GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#if _WIN32
	g_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#endif
}

void glGenFramebuffers(GLsizei n, GLuint* ids)
{
#if _WIN32
	g_glGenFramebuffers(n, ids);
#else
	::glGenFramebuffers(n, ids);
#endif
}

void glBindFramebuffer(GLenum target, GLuint framebuffer)
{
#if _WIN32
	g_glBindFramebuffer(target, framebuffer);
#else
	::glBindFramebuffer(target, framebuffer);
#endif
}

void glDeleteFramebuffers(GLsizei n, GLuint* framebuffers)
{
#if _WIN32
	g_glDeleteFramebuffers(n, framebuffers);
#else
	::glDeleteFramebuffers(n, framebuffers);
#endif
}

void glFramebufferTexture2D(GLenum target,
							GLenum attachment,
							GLenum textarget,
							GLuint texture,
							GLint level)
{
#if _WIN32
	g_glFramebufferTexture2D(target,
							 attachment,
							 textarget,
							 texture,
							 level);
#else
	::glFramebufferTexture2D(target,
							 attachment,
							 textarget,
							 texture,
							 level);
#endif
}

void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
#if _WIN32
	g_glGenRenderbuffers(n, renderbuffers);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glGenRenderbuffers(n, renderbuffers);
#endif
}

void glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#if _WIN32
	g_glBindRenderbuffer(target, renderbuffer);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glBindRenderbuffer(target, renderbuffer);
#endif
}

void glDeleteRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
#if _WIN32
	g_glDeleteRenderbuffers(n, renderbuffers);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glDeleteRenderbuffers(n, renderbuffers);
#endif
}

void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
#if _WIN32
	g_glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
#endif
}

void glDrawBuffers(GLsizei n, const GLenum* bufs)
{
#if _WIN32
	g_glDrawBuffers(n, bufs);
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GL2__)
#else
	::glDrawBuffers(n, bufs);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace GLExt
} // namespace EffekseerRendererGL
