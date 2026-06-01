#pragma once

#include "StaticMeshRenderer.h"

namespace Effekseer
{
namespace ToolRuntime
{

struct BackgroundPlaneParameter
{
	Effekseer::Color Color = {0, 0, 0, 0};
	float ClipDepth = 1.0f - 0.00001f;
};

void CreateBackgroundPlaneMeshData(
	const BackgroundPlaneParameter& parameter,
	Effekseer::CustomVector<StaticMeshVertex>& vertexes,
	Effekseer::CustomVector<int32_t>& indexes);

std::shared_ptr<StaticMesh> CreateBackgroundPlaneMesh(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	const BackgroundPlaneParameter& parameter);

class BackgroundPlaneRenderer
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
	std::shared_ptr<StaticMeshRenderer> meshRenderer_;
	std::shared_ptr<StaticMesh> mesh_;
	Effekseer::Backend::TextureRef texture_;
	Effekseer::Color meshColor_ = {};

	bool Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);

public:
	static std::shared_ptr<BackgroundPlaneRenderer> Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);

	bool UpdateMesh(const Effekseer::Color& color);

	void SetTexture(Effekseer::Backend::TextureRef texture);

	void Render();
};

} // namespace ToolRuntime
} // namespace Effekseer
