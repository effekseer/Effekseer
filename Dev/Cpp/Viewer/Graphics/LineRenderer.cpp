#include "LineRenderer.h"

namespace
{
#ifdef _WIN32

using BYTE = uint8_t;

namespace VS
{
#include "../Shaders/HLSL_DX11_Header/line_vs.h"
}

namespace PS
{
#include "../Shaders/HLSL_DX11_Header/line_ps.h"
}
#endif

#include "../Shaders/GLSL_GL_Header/line_ps.h"
#include "../Shaders/GLSL_GL_Header/line_vs.h"

} // namespace

namespace Effekseer::Tool
{

LineRenderer::LineRenderer(const Backend::GraphicsDeviceRef graphicsDevice)
{
	auto vb = graphicsDevice->CreateVertexBuffer(static_cast<int32_t>(sizeof(Vertex) * 2 * 1024), nullptr, true);

	std::array<int32_t, 2 * 1024> indexes;

	for (int32_t i = 0; i < 1024; i++)
	{
		indexes[0 + i * 2] = 0 + i * 2;
		indexes[1 + i * 2] = 1 + i * 2;
	}

	auto ib = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(2 * 1024), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	// shader
	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mCamera", UniformBufferLayoutElementType::Matrix44, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mProj", UniformBufferLayoutElementType::Matrix44, 1, sizeof(Effekseer::Matrix44)});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElm);

	Effekseer::Backend::ShaderRef shader;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shader = graphicsDevice->CreateShaderFromBinary(VS::g_main, sizeof(VS::g_main), PS::g_main, sizeof(PS::g_main));
#endif
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes({{gl_line_vs}}, {{gl_line_ps}}, uniformLayout);
	}

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "Input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[1].Name = "Input_Color";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "NORMAL";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[2].Name = "Input_UV";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "TEXCOORD";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = true;
	pipParam.IsDepthWriteEnabled = true;
	pipParam.IsBlendEnabled = true;
	pipParam.IsMSAAEnabled = true;
	pipParam.Topology = Effekseer::Backend::TopologyType::Line;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	uniformBufferVS_ = graphicsDevice->CreateUniformBuffer(sizeof(Effekseer::Matrix44) * 2, nullptr);

	vb_ = vb;
	ib_ = ib;
	graphicsDevice_ = graphicsDevice;
	pip_ = pip;
	shader_ = shader;
	vertexLayout_ = vertexLayout;
}

void LineRenderer::DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c)
{
	Vertex v0;
	v0.Pos = p1;
	v0.VColor = c;

	Vertex v1;
	v1.Pos = p2;
	v1.VColor = c;

	vertexies_.push_back(v0);
	vertexies_.push_back(v1);
}

void LineRenderer::Render(const Effekseer::Matrix44& cameraMatrix, const Effekseer::Matrix44& projectionMatrix)
{
	if (vertexies_.empty())
	{
		return;
	}

	vb_->UpdateData(vertexies_.data(), sizeof(Vertex) * vertexies_.size(), 0);

	Effekseer::Matrix44 constantVSBuffer[2];
	constantVSBuffer[0] = cameraMatrix;
	constantVSBuffer[1] = projectionMatrix;

	if (graphicsDevice_->GetDeviceName() == "DirectX11")
	{
	}
	else
	{
		constantVSBuffer[0].Transpose();
		constantVSBuffer[1].Transpose();
	}

	graphicsDevice_->UpdateUniformBuffer(uniformBufferVS_, sizeof(Effekseer::Matrix44) * 2, 0, &constantVSBuffer);

	Effekseer::Backend::DrawParameter drawParam;

	drawParam.TextureCount = 0;
	drawParam.VertexBufferPtr = vb_;
	drawParam.IndexBufferPtr = ib_;
	drawParam.PipelineStatePtr = pip_;
	drawParam.VertexUniformBufferPtr = uniformBufferVS_;
	drawParam.PrimitiveCount = vertexies_.size() / 2;
	drawParam.InstanceCount = 1;
	drawParam.IndexOffset = 0;

	graphicsDevice_->Draw(drawParam);
}

void LineRenderer::ClearCache()
{
	vertexies_.clear();
}

} // namespace Effekseer::Tool