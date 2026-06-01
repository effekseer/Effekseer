#pragma once

#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffectPlatformGLFW.h"
#include <EffekseerToolRuntime/GroundRendering.h>

class EffectPlatformGL final : public EffectPlatformGLFW
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	Effekseer::Backend::TextureRef checkedTexture_ = nullptr;
	Effekseer::Backend::TextureRef groundDepthTextureForEffekseer_ = nullptr;
	GLuint frameBuffer_ = 0;
	GLuint groundDepthFrameBuffer_ = 0;
	GLuint groundDepthColorTexture_ = 0;
	GLuint groundDepthTexture_ = 0;
	GLuint groundProgram_ = 0;
	GLuint groundDepthProgram_ = 0;
	GLuint groundVertexBuffer_ = 0;
	GLuint groundIndexBuffer_ = 0;
	bool usesGpuGroundDepth_ = false;

	bool CreateGroundResources();
	void ReleaseGroundResources();
	void UpdateGroundVertexBuffer();
	void DrawGround(Effekseer::ToolRuntime::GroundRenderPass pass);

protected:
	void UpdateBackgroundTexture() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformGL()
		: EffectPlatformGLFW(true)
	{
		isBackgroundFlipped_ = true;
	}

	~EffectPlatformGL();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	bool TakeScreenshot(const char* path) override;
	void ResetBackgroundPattern() override;
	void GenerateGroundDepth() override;
};
