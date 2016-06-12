
#pragma once

#ifdef EMSCRIPTEN
#define		GL_GLEXT_PROTOTYPES
#define		EGL_EGLEXT_PROTOTYPES
#endif	// EMSCRIPTEN

#include <Effekseer.h>
#include <EffekseerRendererGL.h>

namespace EffekseerPlugin
{
	class DistortingCallbackGL
		: public EffekseerRenderer::DistortingCallback
	{
		GLuint framebufferForCopy = 0;
		GLuint backGroundTexture = 0;
		uint32_t backGroundTextureWidth = 0;
		uint32_t backGroundTextureHeight = 0;
		GLuint backGroundTextureInternalFormat = 0;

		EffekseerRendererGL::Renderer*	renderer = nullptr;

	public:
		DistortingCallbackGL(EffekseerRendererGL::Renderer* renderer);

		virtual ~DistortingCallbackGL();

		void ReleaseTexture();

		// コピー先のテクスチャを準備
		void PrepareTexture(uint32_t width, uint32_t height, GLint internalFormat);

		virtual void OnDistorting();
	};
}