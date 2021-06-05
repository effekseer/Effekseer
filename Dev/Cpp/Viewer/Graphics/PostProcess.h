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

	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::VertexBufferRef vb_;
	Backend::IndexBufferRef ib_;
	Backend::VertexLayoutRef vertexLayout_;
	Backend::PipelineStateRef pip_;
	Backend::UniformBufferRef uniformBufferVS_;
	Backend::UniformBufferRef uniformBufferPS_;
	Backend::DrawParameter drawParam_;

public:
	PostProcess(Backend::GraphicsDeviceRef graphicsDevice, Backend::ShaderRef shader, size_t uniformBufferVSSize, size_t uniformBufferPSSize);
	void Render();

	Backend::UniformBufferRef GetUniformBufferVS()
	{
		return uniformBufferVS_;
	}

	Backend::UniformBufferRef GetUniformBufferPS()
	{
		return uniformBufferPS_;
	}

	//! for reading and writing
	Backend::DrawParameter& GetDrawParameter()
	{
		return drawParam_;
	}
};

} // namespace Tool
} // namespace Effekseer