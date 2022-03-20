
#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

class LineRenderer
{
private:
	struct Vertex
	{
		Effekseer::Vector3D Pos;
		Effekseer::Color VColor;
		Effekseer::Vector2D UV;
	};
	std::vector<Vertex> vertexies_;

	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::UniformLayoutRef uniformLayout_;
	Backend::ShaderRef shader_;
	Backend::PipelineStateRef pip_;
	Backend::VertexLayoutRef vertexLayout_;
	Backend::TextureRef dummyTexture_;

	Backend::VertexBufferRef vb_;
	Backend::IndexBufferRef ib_;
	Backend::UniformBufferRef uniformBufferVS_;

public:
	LineRenderer(const Backend::GraphicsDeviceRef graphicsDevice);

	void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c);

	void Render(const Effekseer::Matrix44& cameraMatrix, const Effekseer::Matrix44& projectionMatrix);

	void ClearCache();
};

} // namespace Effekseer::Tool