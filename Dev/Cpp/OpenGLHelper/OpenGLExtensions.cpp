#include "OpenGLExtensions.h"

#include <cstdio>

namespace Effekseer
{
namespace OpenGLHelper
{
namespace
{

using PFN_glClearBufferfv = void(OGL_EXT_STDCALL*)(GLenum buffer, GLint drawbuffer, const GLfloat* value);
using PFN_glCopyTexSubImage2D =
	void(OGL_EXT_STDCALL*)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
using PFN_glBlitFramebuffer = void(OGL_EXT_STDCALL*)(GLint srcX0,
													 GLint srcY0,
													 GLint srcX1,
													 GLint srcY1,
													 GLint dstX0,
													 GLint dstY0,
													 GLint dstX1,
													 GLint dstY1,
													 GLbitfield mask,
													 GLenum filter);
using PFN_glFramebufferRenderbuffer = void(OGL_EXT_STDCALL*)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
using PFN_glGetTexImage = void(OGL_EXT_STDCALL*)(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
using PFN_glDebugMessageCallback = void(OGL_EXT_STDCALL*)(DebugCallback callback, const void* userParam);
using PFN_glDebugMessageControl =
	void(OGL_EXT_STDCALL*)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);

template <class T>
T LoadProc(const char* name)
{
	return reinterpret_cast<T>(glfwGetProcAddress(name));
}

PFN_glClearBufferfv g_glClearBufferfv = nullptr;
PFN_glCopyTexSubImage2D g_glCopyTexSubImage2D = nullptr;
PFN_glBlitFramebuffer g_glBlitFramebuffer = nullptr;
PFN_glFramebufferRenderbuffer g_glFramebufferRenderbuffer = nullptr;
PFN_glGetTexImage g_glGetTexImage = nullptr;
PFN_glDebugMessageCallback g_glDebugMessageCallback = nullptr;
PFN_glDebugMessageControl g_glDebugMessageControl = nullptr;

bool g_initialized = false;
bool g_initializationResult = false;
bool g_debugOutputSupported = false;

} // namespace

bool Initialize()
{
	if (g_initialized)
	{
		return g_initializationResult;
	}

	auto logFailure = [](const char* name)
	{
		std::fprintf(stderr, "[OpenGLExtensions] Failed to load OpenGL function: %s\n", name);
	};

	auto loadRequired = [&](auto& dst, const char* name) -> bool
	{
		using ProcType = typename std::remove_reference<decltype(dst)>::type;
		dst = LoadProc<ProcType>(name);
		if (dst == nullptr)
		{
			logFailure(name);
			return false;
		}
		return true;
	};

	bool success = true;
	success &= loadRequired(g_glClearBufferfv, "glClearBufferfv");
	success &= loadRequired(g_glCopyTexSubImage2D, "glCopyTexSubImage2D");
	success &= loadRequired(g_glBlitFramebuffer, "glBlitFramebuffer");
	success &= loadRequired(g_glFramebufferRenderbuffer, "glFramebufferRenderbuffer");
	success &= loadRequired(g_glGetTexImage, "glGetTexImage");

	g_glDebugMessageCallback = LoadProc<PFN_glDebugMessageCallback>("glDebugMessageCallback");
	g_glDebugMessageControl = LoadProc<PFN_glDebugMessageControl>("glDebugMessageControl");
	g_debugOutputSupported = (g_glDebugMessageCallback != nullptr) && (g_glDebugMessageControl != nullptr);

	g_initialized = true;
	g_initializationResult = success;
	return success;
}

namespace
{
bool EnsureInitialized()
{
	if (!g_initialized)
	{
		return Initialize();
	}
	return g_initializationResult;
}
} // namespace

bool IsSupportedDebugOutput()
{
	EnsureInitialized();
	return g_debugOutputSupported;
}

void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
	if (!EnsureInitialized() || g_glClearBufferfv == nullptr)
	{
		return;
	}
	g_glClearBufferfv(buffer, drawbuffer, value);
}

void glCopyTexSubImage2D(
	GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (!EnsureInitialized() || g_glCopyTexSubImage2D == nullptr)
	{
		return;
	}
	g_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

void glBlitFramebuffer(GLint srcX0,
					   GLint srcY0,
					   GLint srcX1,
					   GLint srcY1,
					   GLint dstX0,
					   GLint dstY0,
					   GLint dstX1,
					   GLint dstY1,
					   GLbitfield mask,
					   GLenum filter)
{
	if (!EnsureInitialized() || g_glBlitFramebuffer == nullptr)
	{
		return;
	}
	g_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	if (!EnsureInitialized() || g_glFramebufferRenderbuffer == nullptr)
	{
		return;
	}
	g_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels)
{
	if (!EnsureInitialized() || g_glGetTexImage == nullptr)
	{
		return;
	}
	g_glGetTexImage(target, level, format, type, pixels);
}

void glDebugMessageCallback(DebugCallback callback, const void* userParam)
{
	if (!EnsureInitialized() || g_glDebugMessageCallback == nullptr)
	{
		return;
	}
	g_glDebugMessageCallback(callback, userParam);
}

void glDebugMessageControl(
	GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled)
{
	if (!EnsureInitialized() || g_glDebugMessageControl == nullptr)
	{
		return;
	}
	g_glDebugMessageControl(source, type, severity, count, ids, enabled);
}

} // namespace OpenGLHelper
} // namespace Effekseer
