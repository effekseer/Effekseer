#pragma once

#include <Effekseer.Modules.h>
#include <Effekseer.h>

namespace Effekseer
{
namespace Tool
{

class PostProcess
{
private:
	struct Vertex
	{
		float x, y;
		float u, v;
	};

	Effekseer::Backend::ShaderRef shader_;
	Effekseer::Backend::VertexBufferRef vb_;
	Effekseer::Backend::IndexBufferRef ib_;
	Effekseer::Backend::VertexLayoutRef vertexLayout_;
	Backend::PipelineStateRef pip_;
	Backend::UniformBufferRef uniformBufferVS_;
	Backend::UniformBufferRef uniformBufferPS_;

public:
	PostProcess(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, Effekseer::Backend::ShaderRef shader, size_t uniformBufferVSSize, size_t uniformBufferPSSize);
	void Render(Backend::TextureRef dst, Backend::TextureRef src);
};

} // namespace Tool
} // namespace Effekseer