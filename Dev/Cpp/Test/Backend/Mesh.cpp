#include "../TestHelper.h"
#include "../Window/RenderingWindowGL.h"

#ifdef _WIN32
#include "../../EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h"
#include "../Window/RenderingWindowDX11.h"
#include <EffekseerRendererDX11.h>

namespace DX11VS
{
#include "../Shaders/HLSL_DX11_Header/mesh_vs.h"
}

namespace DX11PS
{
#include "../Shaders/HLSL_DX11_Header/mesh_ps.h"
}

#endif

#undef None
#include "../../EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h"
#include <EffekseerRendererGL.h>
#include <memory>

#include "../Shaders/GLSL_GL_Header/mesh_ps.h"
#include "../Shaders/GLSL_GL_Header/mesh_vs.h"

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindowGL* window)
{
	return nullptr;
}

#ifdef _WIN32
Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindowDX11* window)
{
	auto gd = static_cast<EffekseerRendererDX11::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto rt = gd->CreateTexture(nullptr, window->GetRenderTargetView(), nullptr);
	auto dt = gd->CreateTexture(nullptr, nullptr, window->GetDepthStencilView());

	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> rts;
	rts.resize(1);
	rts.at(0) = rt;

	auto rp = gd->CreateRenderPass(rts, dt);
	return rp;
}
#endif

Effekseer::Backend::ShaderRef GenerateShader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, Effekseer::Backend::UniformLayoutRef layout, RenderingWindowGL*)
{
	return graphicsDevice->CreateShaderFromCodes({{gl_mesh_vs}}, {{gl_mesh_ps}}, layout);
}

#ifdef _WIN32
Effekseer::Backend::ShaderRef GenerateShader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, Effekseer::Backend::UniformLayoutRef layout, RenderingWindowDX11*)
{
	return graphicsDevice->CreateShaderFromBinary(DX11VS::g_main, sizeof(DX11VS::g_main), DX11PS::g_main, sizeof(DX11PS::g_main));
}
#endif

Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowGL* window)
{
	return EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
}

#ifdef _WIN32
Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowDX11* window)
{
	return EffekseerRendererDX11::CreateGraphicsDevice(window->GetDevice(), window->GetContext());
}
#endif

template <typename WINDOW>
void Backend_Mesh()
{
	auto window = std::make_shared<WINDOW>(std::array<int, 2>({1280, 720}), "Backend.Mesh");

	auto graphicsDevice = GenerateGraphicsDevice(window.get());

	std::array<SimpleVertex, 4> vbData;
	vbData[0].Position = {-0.5f, 0.5f, 0.5f};
	vbData[0].Color = {0, 0, 0, 255};
	vbData[1].Position = {0.5f, 0.5f, 0.5f};
	vbData[1].Color = {255, 0, 0, 255};
	vbData[2].Position = {0.5f, -0.5f, 0.5f};
	vbData[2].Color = {255, 255, 0, 255};
	vbData[3].Position = {-0.5f, -0.5f, 0.5f};
	vbData[3].Color = {0, 255, 0, 255};

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
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{uniformLayoutElement});

	auto shader = GenerateShader(graphicsDevice, uniformLayout, window.get());

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

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout;
	pipParam.ShaderPtr = shader;
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsBlendEnabled = false;

	auto pip = graphicsDevice->CreatePipelineState(pipParam);

	auto renderPass = GenerateRenderPass(graphicsDevice, window.get());
	int count = 0;
	while (count < 60 && window->DoEvent())
	{
		graphicsDevice->BeginRenderPass(renderPass, true, true, Effekseer::Color(80, 80, 80, 255));

		shiftVertex[0] += 0.01f;
		shiftVertex[0] = fmodf(shiftVertex[0], 0.5f);
		graphicsDevice->UpdateUniformBuffer(cb, sizeof(float) * 4, 0, shiftVertex.data());

		Effekseer::Backend::DrawParameter drawParam;
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
TestRegister Test_Backend_Mesh_GL("Backend.Mesh_GL", []() -> void { Backend_Mesh<RenderingWindowGL>(); });

#ifdef _WIN32
TestRegister Test_Backend_Mesh_DX11("Backend.Mesh_DX11", []() -> void { Backend_Mesh<RenderingWindowDX11>(); });
#endif

#endif