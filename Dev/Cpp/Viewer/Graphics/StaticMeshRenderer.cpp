#include "StaticMeshRenderer.h"

namespace
{
#ifdef _WIN32

using BYTE = uint8_t;

namespace VS
{
#include "../Shaders/HLSL_DX11_Header/static_mesh_vs.h"
}

namespace PS
{
#include "../Shaders/HLSL_DX11_Header/static_mesh_ps.h"
}
#endif

#include "../Shaders/GLSL_GL_Header/static_mesh_ps.h"
#include "../Shaders/GLSL_GL_Header/static_mesh_vs.h"

} // namespace

namespace Effekseer
{
namespace Tool
{

std::shared_ptr<StaticMesh> StaticMesh::Create(Effekseer::RefPtr<Backend::GraphicsDevice> graphicsDevice, Effekseer::CustomVector<StaticMeshVertex> vertexes, Effekseer::CustomVector<int32_t> indexes, bool isDyanmic)
{
	auto vb = graphicsDevice->CreateVertexBuffer(static_cast<int32_t>(sizeof(StaticMeshVertex) * vertexes.size()), vertexes.data(), isDyanmic);
	auto ib = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(indexes.size()), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	if (vb == nullptr || ib == nullptr)
	{
		return nullptr;
	}

	auto ret = std::make_shared<StaticMesh>();
	ret->vb_ = vb;
	ret->ib_ = ib;
	ret->indexCount_ = ib->GetElementCount();
	return ret;
}

StaticMeshRenderer::StaticMeshRenderer()
{
}

std::shared_ptr<StaticMeshRenderer> StaticMeshRenderer::Create(RefPtr<Backend::GraphicsDevice> graphicsDevice)
{
	// shader
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElements;
	uniformLayoutElements.resize(7);
	uniformLayoutElements[0].Name = "CBVS0.projectionMatrix";
	uniformLayoutElements[0].Offset = 0;
	uniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
	uniformLayoutElements[1].Name = "CBVS0.cameraMatrix";
	uniformLayoutElements[1].Offset = sizeof(float) * 16;
	uniformLayoutElements[1].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[1].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
	uniformLayoutElements[2].Name = "CBVS0.worldMatrix";
	uniformLayoutElements[2].Offset = sizeof(float) * 32;
	uniformLayoutElements[2].Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElements[2].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;

	uniformLayoutElements[3].Name = "CBPS0.isLit";
	uniformLayoutElements[3].Offset = 0;
	uniformLayoutElements[3].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[3].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[4].Name = "CBPS0.directionalLightDirection";
	uniformLayoutElements[4].Offset = sizeof(float) * 4;
	uniformLayoutElements[4].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[4].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[5].Name = "CBPS0.directionalLightColor";
	uniformLayoutElements[5].Offset = sizeof(float) * 8;
	uniformLayoutElements[5].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[5].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
	uniformLayoutElements[6].Name = "CBPS0.ambientLightColor";
	uniformLayoutElements[6].Offset = sizeof(float) * 12;
	uniformLayoutElements[6].Stage = Effekseer::Backend::ShaderStageType::Pixel;
	uniformLayoutElements[6].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;

	// constant buffer
	auto vcb = graphicsDevice->CreateUniformBuffer(sizeof(UniformBufferVS), nullptr);
	auto pcb = graphicsDevice->CreateUniformBuffer(sizeof(UniformBufferPS), nullptr);
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler"}, std::move(uniformLayoutElements));

	Effekseer::Backend::ShaderRef shader;

	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		shader = graphicsDevice->CreateShaderFromBinary(VS::g_main, sizeof(VS::g_main), PS::g_main, sizeof(PS::g_main));
#endif
	}
	else
	{
		shader = graphicsDevice->CreateShaderFromCodes({{gl_static_mesh_vs}}, {{gl_static_mesh_ps}}, uniformLayout);
	}

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(4);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "input_Pos";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "input_UV";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "TEXCOORD";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[2].Name = "input_Color";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "NORMAL";
	vertexLayoutElements[3].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[3].Name = "input_Normal";
	vertexLayoutElements[3].SemanticIndex = 1;
	vertexLayoutElements[3].SemanticName = "NORMAL";

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = true;
	pipParam.IsDepthWriteEnabled = true;
	pipParam.IsBlendEnabled = false;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	auto ret = std::make_shared<StaticMeshRenderer>();

	ret->graphicsDevice_ = graphicsDevice;
	ret->pip_ = pip;
	ret->shader_ = shader;
	ret->uniformBufferVS_ = vcb;
	ret->uniformBufferPS_ = pcb;
	ret->vertexLayout_ = vertexLayout;

	Backend::TextureParameter texParam;
	const int32_t textureSize = 16;
	texParam.Format = Backend::TextureFormatType::R8G8B8A8_UNORM;
	texParam.Size = {textureSize, textureSize};

	CustomVector<uint8_t> initialTexData;
	initialTexData.resize(textureSize * textureSize * 4);
	for (auto& v : initialTexData)
	{
		v = 255;
	}

	texParam.InitialData = std::move(initialTexData);

	ret->dummyTexture_ = graphicsDevice->CreateTexture(texParam);

	return ret;
}

void StaticMeshRenderer::Render(const RendererParameter& rendererParameter)
{
	if (staticMesh_ == nullptr)
	{
		return;
	}

	UniformBufferVS uvs{};
	UniformBufferPS ups{};

	uvs.projectionMatrix = rendererParameter.ProjectionMatrix;
	uvs.cameraMatrix = rendererParameter.CameraMatrix;
	uvs.worldMatrix = rendererParameter.WorldMatrix;

	if (graphicsDevice_->GetDeviceName() == "DirectX11")
	{
	}
	else
	{
		uvs.cameraMatrix.Transpose();
		uvs.projectionMatrix.Transpose();
		uvs.worldMatrix.Transpose();
	}

	ups.isLit[0] = staticMesh_->IsLit ? 1.0f : 0.0f;
	ups.directionalLightDirection = rendererParameter.DirectionalLightDirection;
	ups.directionalLightColor = rendererParameter.DirectionalLightColor;
	ups.ambientLightColor = rendererParameter.AmbientLightColor;

	graphicsDevice_->UpdateUniformBuffer(uniformBufferVS_, sizeof(UniformBufferVS), 0, &uvs);
	graphicsDevice_->UpdateUniformBuffer(uniformBufferPS_, sizeof(UniformBufferPS), 0, &ups);

	Effekseer::Backend::DrawParameter drawParam;

	drawParam.TextureCount = 1;
	drawParam.TexturePtrs[0] = staticMesh_->Texture != nullptr ? staticMesh_->Texture : dummyTexture_;
	drawParam.TextureSamplingTypes[0] = Backend::TextureSamplingType::Linear;
	drawParam.TextureWrapTypes[0] = Backend::TextureWrapType::Repeat;

	drawParam.VertexBufferPtr = staticMesh_->GetVertexBuffer();
	drawParam.IndexBufferPtr = staticMesh_->GetIndexBuffer();
	drawParam.PipelineStatePtr = pip_;
	drawParam.VertexUniformBufferPtr = uniformBufferVS_;
	drawParam.PixelUniformBufferPtr = uniformBufferPS_;
	drawParam.PrimitiveCount = staticMesh_->GetIndexCount() / 3;
	drawParam.InstanceCount = 1;
	graphicsDevice_->Draw(drawParam);
}

void StaticMeshRenderer::SetStaticMesh(const std::shared_ptr<StaticMesh>& mesh)
{
	staticMesh_ = mesh;
}

std::shared_ptr<StaticMesh>& StaticMeshRenderer::GetStaticMesh()
{
	return staticMesh_;
}

} // namespace Tool
} // namespace Effekseer
