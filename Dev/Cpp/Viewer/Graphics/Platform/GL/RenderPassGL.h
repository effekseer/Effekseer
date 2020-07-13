#pragma once

#include "../../RenderPass.h"
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

namespace Effekseer
{
namespace Tool
{

class RenderPassGL : public RenderPass
{
protected:
	GLuint frameBuffer_ = 0;

	GLuint frameBufferSrc_ = 0;
	GLuint frameBufferDst_ = 0;

public:
	RenderPassGL();
	virtual ~RenderPassGL();

	bool Initialize(std::shared_ptr<efk::RenderTexture> colorTexture, std::shared_ptr<efk::DepthTexture> depthTexture, std::shared_ptr<efk::RenderTexture> resolvedColorTexture);

	void Resolve();
};

} // namespace Tool
} // namespace Effekseer
