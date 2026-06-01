#pragma once

#include "StaticMeshRenderer.h"

#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

namespace Effekseer
{
namespace ToolRuntime
{

class GroundPlaneRenderer
{
private:
	std::shared_ptr<StaticMeshRenderer> groundMeshRenderer_;
	bool Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);
	int32_t GroundExtent = 10;

public:
	float GroundHeight = 0.0f;

	void SetExtent(int32_t extent);

	static std::shared_ptr<GroundPlaneRenderer> Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);

	void UpdateGround();

	void Render(EffekseerRenderer::RendererRef renderer);
};

} // namespace ToolRuntime
} // namespace Effekseer
