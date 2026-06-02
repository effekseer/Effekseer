#pragma once

#include <Effekseer.Modules.h>
#include <Effekseer.h>

namespace Effekseer
{
namespace ToolRuntime
{

enum class PostProcessBlendType
{
	None,
	Blend,
	Add,
};

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
	PostProcess(Backend::GraphicsDeviceRef graphicsDevice, Backend::ShaderRef shader, size_t uniformBufferVSSize, size_t uniformBufferPSSize, PostProcessBlendType blendType = PostProcessBlendType::None);
	void Render();

	bool GetIsValid() const
	{
		return graphicsDevice_ != nullptr && shader_ != nullptr && vb_ != nullptr && ib_ != nullptr && vertexLayout_ != nullptr && pip_ != nullptr;
	}

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

} // namespace ToolRuntime
} // namespace Effekseer
