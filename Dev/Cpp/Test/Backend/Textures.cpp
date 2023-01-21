#include "../TestHelper.h"
#include "Helper.h"

#include <RenderingEnvironment/RenderingEnvironmentGL.h>

#ifdef _WIN32
#include <Windows.h>
namespace DX11VS
{
#include "../Shaders/HLSL_DX11_Header/mesh_vs.h"
}

namespace DX11PS
{
#include "../Shaders/HLSL_DX11_Header/textures_ps.h"
}

#include <RenderingEnvironment/RenderingEnvironmentDX11.h>

#endif

#undef None
#include <memory>

#include "../Shaders/GLSL_GL_Header/mesh_vs.h"
#include "../Shaders/GLSL_GL_Header/textures_ps.h"

template <typename WINDOW>
void Backend_Textures()
{
	std::shared_ptr<RenderingEnvironment> window = std::make_shared<WINDOW>(std::array<int, 2>({1280, 720}), "Backend.Textures");

	Effekseer::Backend::GraphicsDeviceRef graphicsDevice = window->GetGraphicsDevice();

	std::array<SimpleVertex, 4> vbData;
	vbData[0].Position = {-0.5f, 0.5f, 0.5f};
	vbData[0].Color = {0, 0, 0, 255};
	vbData[0].UV = {0.0f, 0.0f};
	vbData[1].Position = {0.5f, 0.5f, 0.5f};
	vbData[1].Color = {255, 0, 0, 255};
	vbData[1].UV = {1.0f, 0.0f};
	vbData[2].Position = {0.5f, -0.5f, 0.5f};
	vbData[2].Color = {255, 255, 0, 255};
	vbData[2].UV = {1.0f, 1.0f};
	vbData[3].Position = {-0.5f, -0.5f, 0.5f};
	vbData[3].Color = {0, 255, 0, 255};
	vbData[3].UV = {0.0f, 1.0f};

	auto vb = graphicsDevice->CreateVertexBuffer(sizeof(SimpleVertex) * 4, vbData.data(), false);

	std::array<int32_t, 6> ibData;
	ibData[0] = 0;
	ibData[1] = 1;
	ibData[2] = 2;
	ibData[3] = 0;
	ibData[4] = 2;
	ibData[5] = 3;

	auto ib = graphicsDevice->CreateIndexBuffer(6, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

	// shader
	Effekseer::Backend::UniformLayoutElement uniformLayoutElement;
	uniformLayoutElement.Name = "CBVS0.shift_vertex";
	uniformLayoutElement.Offset = 0;
	uniformLayoutElement.Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElement.Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;

	// constant buffer
	std::array<float, 4> shiftVertex;
	shiftVertex.fill(0);
	auto cb = graphicsDevice->CreateUniformBuffer(sizeof(float) * 4, shiftVertex.data());

	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
		Effekseer::CustomVector<Effekseer::CustomString<char>>{"Sampler_g_sampler1", "Sampler_g_sampler2", "Sampler_g_sampler3"},
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{uniformLayoutElement});

	std::map<std::string, ShaderContainer> shaders;
#ifdef _WIN32
	shaders["DirectX11"].InitAsBinary(DX11VS::g_main, sizeof(DX11VS::g_main), DX11PS::g_main, sizeof(DX11PS::g_main));
#endif
	shaders["OpenGL"].VertexCodes = {{gl_mesh_vs}};
	shaders["OpenGL"].PixelCodes = {{gl_textures_ps}};
	auto shader = window->CreateShader(shaders, uniformLayout);

	assert(shader != nullptr);

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
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

	auto vertexLayout = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));

	// Create textures
	Effekseer::Backend::TextureParameter texParamSrc1;
	texParamSrc1.MipLevelCount = 1;
	texParamSrc1.Size = {1, 1, 1};
	texParamSrc1.Dimension = 2;
	auto texSrc1 = graphicsDevice->CreateTexture(texParamSrc1, {255, 0, 0, 255});

	Effekseer::Backend::TextureParameter texParamSrc2;
	texParamSrc2.MipLevelCount = 1;
	texParamSrc2.Size = {1, 1, 1};
	texParamSrc2.Dimension = 2;
	auto texSrc2 = graphicsDevice->CreateTexture(texParamSrc2, {0, 255, 0, 255});

	Effekseer::Backend::TextureParameter texParamSrc3;
	texParamSrc3.MipLevelCount = 1;
	texParamSrc3.Size = {1, 1, 1};
	texParamSrc3.Dimension = 2;
	auto texSrc3 = graphicsDevice->CreateTexture(texParamSrc3, {0, 0, 255, 255});

	Effekseer::Backend::TextureParameter texParamDst1;
	texParamDst1.MipLevelCount = 1;
	texParamDst1.Size = {1, 1, 1};
	texParamDst1.Dimension = 2;
	auto texDst1 = graphicsDevice->CreateTexture(texParamDst1, Effekseer::CustomVector<uint8_t>(4));

	Effekseer::Backend::TextureParameter texParamDst2;
	texParamDst2.MipLevelCount = 1;
	texParamDst2.Size = {1, 1, 3};
	texParamDst2.Dimension = 2;
	texParamDst2.Usage = Effekseer::Backend::TextureUsageType::Array;
	auto texDst2 = graphicsDevice->CreateTexture(texParamDst2, Effekseer::CustomVector<uint8_t>(12));

	Effekseer::Backend::TextureParameter texParamDst3;
	texParamDst3.MipLevelCount = 1;
	texParamDst3.Size = {1, 1, 3};
	texParamDst3.Dimension = 3;
	auto texDst3 = graphicsDevice->CreateTexture(texParamDst3, Effekseer::CustomVector<uint8_t>(12));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsBlendEnabled = false;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	auto renderPass = window->GetScreenRenderPass();
	int count = 0;
	while (count < 60 && window->DoEvent())
	{
		graphicsDevice->CopyTexture(texDst1, texSrc1, {0, 0, 0}, {0, 0, 0}, {1, 1, 1}, 0, 0);
		graphicsDevice->CopyTexture(texDst2, texSrc2, {0, 0, 0}, {0, 0, 0}, {1, 1, 1}, 1, 0);
		graphicsDevice->CopyTexture(texDst3, texSrc3, {0, 0, 1}, {0, 0, 0}, {1, 1, 1}, 0, 0);

		graphicsDevice->BeginRenderPass(renderPass, true, true, Effekseer::Color(80, 80, 80, 255));

		graphicsDevice->UpdateUniformBuffer(cb, sizeof(float) * 4, 0, shiftVertex.data());

		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = 3;
		drawParam.TexturePtrs[0] = texDst1;
		drawParam.TextureSamplingTypes[0] = Effekseer::Backend::TextureSamplingType::Nearest;
		drawParam.TextureWrapTypes[0] = Effekseer::Backend::TextureWrapType::Clamp;

		drawParam.TexturePtrs[1] = texDst2;
		drawParam.TextureSamplingTypes[1] = Effekseer::Backend::TextureSamplingType::Nearest;
		drawParam.TextureWrapTypes[1] = Effekseer::Backend::TextureWrapType::Clamp;

		drawParam.TexturePtrs[2] = texDst3;
		drawParam.TextureSamplingTypes[2] = Effekseer::Backend::TextureSamplingType::Nearest;
		drawParam.TextureWrapTypes[2] = Effekseer::Backend::TextureWrapType::Clamp;

		drawParam.VertexBufferPtr = vb;
		drawParam.IndexBufferPtr = ib;
		drawParam.PipelineStatePtr = pip;
		drawParam.VertexUniformBufferPtr = cb;
		drawParam.PrimitiveCount = 2;
		drawParam.InstanceCount = 1;
		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();

		window->Present();

		count++;
	}
}

#if !defined(__FROM_CI__)
TestRegister Test_Backend_Textures_GL("Backend.Textures_GL", []() -> void { Backend_Textures<RenderingEnvironmentGL>(); });

#ifdef _WIN32
TestRegister Test_Backend_Textures_DX11("Backend.Textures_DX11", []() -> void { Backend_Textures<RenderingEnvironmentDX11>(); });
#endif

#endif