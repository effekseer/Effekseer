#include "PostProcess.h"

namespace Effekseer
{
namespace Tool
{

PostProcess::PostProcess(Backend::GraphicsDeviceRef graphicsDevice, Backend::ShaderRef shader, size_t uniformBufferVSSize, size_t uniformBufferPSSize)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
{
	std::array<Vertex, 4> vdata;
	vdata[0] = Vertex{-1.0f, 1.0f, 0.0f, 0.0f};
	vdata[1] = Vertex{1.0f, 1.0f, 1.0f, 0.0f};
	vdata[2] = Vertex{1.0f, -1.0f, 1.0f, 1.0f};
	vdata[3] = Vertex{-1.0f, -1.0f, 0.0f, 1.0f};
	vb_ = graphicsDevice->CreateVertexBuffer(sizeof(Vertex) * 4, vdata.data(), false);

	std::array<int32_t, 6> idata;
	idata[0] = 0;
	idata[1] = 1;
	idata[2] = 2;
	idata[3] = 0;
	idata[4] = 2;
	idata[5] = 3;
	ib_ = graphicsDevice->CreateIndexBuffer(6, idata.data(), Backend::IndexBufferStrideType::Stride4);

	std::vector<Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(2);
	vertexLayoutElements[0].Format = Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[0].Name = "Input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "Input_UV";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "TEXCOORD";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));

	Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsDepthWriteEnabled = false;
	pipParam.IsBlendEnabled = false;

	pip_ = graphicsDevice->CreatePipelineState(pipParam);

	if (uniformBufferVSSize > 0)
	{
		uniformBufferVS_ = graphicsDevice->CreateUniformBuffer(uniformBufferVSSize, nullptr);
	}

	if (uniformBufferVSSize > 0)
	{
		uniformBufferPS_ = graphicsDevice->CreateUniformBuffer(uniformBufferPSSize, nullptr);
	}

	drawParam_.TextureCount = 0;
	drawParam_.TextureSamplingTypes[0] = Backend::TextureSamplingType::Linear;
	drawParam_.TextureWrapTypes[0] = Backend::TextureWrapType::Repeat;

	drawParam_.VertexBufferPtr = vb_;
	drawParam_.IndexBufferPtr = ib_;
	drawParam_.PipelineStatePtr = pip_;
	drawParam_.VertexUniformBufferPtr = uniformBufferVS_;
	drawParam_.PixelUniformBufferPtr = uniformBufferPS_;
	drawParam_.PrimitiveCount = 2;
	drawParam_.InstanceCount = 1;
}

void PostProcess::Render()
{
	graphicsDevice_->Draw(drawParam_);
}

} // namespace Tool
} // namespace Effekseer