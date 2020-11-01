#include "../TestHelper.h"
#include "../Window/RenderingWindowGL.h"

#ifdef _WIN32
#include "../../EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h"
#include "../Window/RenderingWindowDX11.h"
#include <EffekseerRendererDX11.h>
#endif

#undef None
#include "../../EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h"
#include <EffekseerRendererGL.h>
#include <memory>

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

static auto vs_shader_gl = R"(

#version 140
in vec3 in_position;
in vec2 in_uv;
in vec4 in_color;

uniform vec4 shift_vertex;

out vec4 vsps_color;

void main(void)
{
    gl_Position = vec4(in_position, 1.0) + shift_vertex;
    vsps_color = in_color;	
}

)";

static auto ps_shader_gl = R"(

#version 140
in vec4 vsps_color;

out vec4 fragColor;

void main(void)
{
    fragColor = vsps_color;
}

)";

static auto vs_shader_dx11 = R"(

struct VS_Input
{
	float3 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : NORMAL0;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

float4 shift_vertex : register(c0);

VS_Output main(const VS_Input input)
{
	VS_Output output = (VS_Output)0;
	output.Pos =float4(input.Pos.x, input.Pos.y, input.Pos.z, 1.0) + shift_vertex;
	output.UV = input.UV;
	output.Color = input.Color;
	return output;
}

)";

static auto ps_shader_dx11 = R"(

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

float4 main(PS_Input input): SV_Target
{
	return input.Color;
}

)";

Effekseer::Backend::RenderPass* GenerateRenderPass(Effekseer::Backend::GraphicsDevice* graphicsDevice, RenderingWindowGL* window)
{
	return nullptr;
}

Effekseer::Backend::RenderPass* GenerateRenderPass(Effekseer::Backend::GraphicsDevice* graphicsDevice, RenderingWindowDX11* window)
{
	auto gd = static_cast<EffekseerRendererDX11::Backend::GraphicsDevice*>(graphicsDevice);
	auto rt = gd->CreateTexture(nullptr, window->GetRenderTargetView(), nullptr);
	auto dt = gd->CreateTexture(nullptr, nullptr, window->GetDepthStencilView());
	auto rp = gd->CreateRenderPass(reinterpret_cast<Effekseer::Backend::Texture**>(&rt), 1, dt);
	ES_SAFE_RELEASE(rt);
	ES_SAFE_RELEASE(dt);
	return rp;
}

Effekseer::Backend::Shader* GenerateShader(Effekseer::Backend::GraphicsDevice* graphicsDevice, Effekseer::Backend::UniformLayout* layout, RenderingWindowGL*)
{
	return graphicsDevice->CreateShaderFromCodes(vs_shader_gl, ps_shader_gl, layout);
}

Effekseer::Backend::Shader* GenerateShader(Effekseer::Backend::GraphicsDevice* graphicsDevice, Effekseer::Backend::UniformLayout* layout, RenderingWindowDX11*)
{
	return graphicsDevice->CreateShaderFromCodes(vs_shader_dx11, ps_shader_dx11, layout);
}

Effekseer::Backend::GraphicsDevice* GenerateGraphicsDevice(RenderingWindowGL* window)
{
	return EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
}

Effekseer::Backend::GraphicsDevice* GenerateGraphicsDevice(RenderingWindowDX11* window)
{
	return EffekseerRendererDX11::CreateGraphicsDevice(window->GetDevice(), window->GetContext());
}

template <typename WINDOW>
void Backend_Mesh()
{
	auto window = std::make_shared<WINDOW>(std::array<int, 2>({1280, 720}), "Backend.Mesh");

	std::shared_ptr<Effekseer::Backend::GraphicsDevice> graphicsDevice = Effekseer::CreateReference(GenerateGraphicsDevice(window.get()));

	std::array<SimpleVertex, 4> vbData;
	vbData[0].Position = {-0.5f, 0.5f, 0.5f};
	vbData[0].Color = {0, 0, 0, 255};
	vbData[1].Position = {0.5f, 0.5f, 0.5f};
	vbData[1].Color = {255, 0, 0, 255};
	vbData[2].Position = {0.5f, -0.5f, 0.5f};
	vbData[2].Color = {255, 255, 0, 255};
	vbData[3].Position = {-0.5f, -0.5f, 0.5f};
	vbData[3].Color = {0, 255, 0, 255};

	auto vb = Effekseer::CreateReference(graphicsDevice->CreateVertexBuffer(sizeof(SimpleVertex) * 4, vbData.data(), false));

	std::array<int32_t, 6> ibData;
	ibData[0] = 0;
	ibData[1] = 1;
	ibData[2] = 2;
	ibData[3] = 0;
	ibData[4] = 2;
	ibData[5] = 3;

	auto ib = Effekseer::CreateReference(graphicsDevice->CreateIndexBuffer(6, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4));

	// shader
	Effekseer::Backend::UniformLayoutElement uniformLayoutElement;
	uniformLayoutElement.Name = "shift_vertex";
	uniformLayoutElement.Offset = 0;
	uniformLayoutElement.Stage = Effekseer::Backend::ShaderStageType::Vertex;
	uniformLayoutElement.Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;

	// constant buffer
	std::array<float, 4> shiftVertex;
	shiftVertex.fill(0);
	auto cb = Effekseer::CreateReference(graphicsDevice->CreateUniformBuffer(sizeof(float) * 4, shiftVertex.data()));
	auto uniformLayout = Effekseer::CreateReference(new Effekseer::Backend::UniformLayout({}, {uniformLayoutElement}));

	auto shader = Effekseer::CreateReference(GenerateShader(graphicsDevice.get(), uniformLayout.get(), window.get()));

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "in_position";
	vertexLayoutElements[0].SemanticIndex = 0;
	vertexLayoutElements[0].SemanticName = "POSITION";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "in_uv";
	vertexLayoutElements[1].SemanticIndex = 0;
	vertexLayoutElements[1].SemanticName = "TEXCOORD";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[2].Name = "in_color";
	vertexLayoutElements[2].SemanticIndex = 0;
	vertexLayoutElements[2].SemanticName = "NORMAL";

	auto vertexLayout = Effekseer::CreateReference(graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size())));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout.get();
	pipParam.ShaderPtr = shader.get();
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsBlendEnabled = false;

	auto pip = Effekseer::CreateReference(graphicsDevice->CreatePipelineState(pipParam));

	auto renderPass = Effekseer::CreateReference(GenerateRenderPass(graphicsDevice.get(), window.get()));
	int count = 0;
	while (count < 60 && window->DoEvent())
	{
		graphicsDevice->BeginRenderPass(renderPass.get(), true, true, Effekseer::Color(80, 80, 80, 255));

		shiftVertex[0] += 0.01f;
		shiftVertex[0] = fmodf(shiftVertex[0], 0.5f);
		graphicsDevice->UpdateUniformBuffer(cb.get(), sizeof(float) * 4, 0, shiftVertex.data());

		Effekseer::Backend::DrawParameter drawParam;
		drawParam.VertexBufferPtr = vb.get();
		drawParam.IndexBufferPtr = ib.get();
		drawParam.PipelineStatePtr = pip.get();
		drawParam.VertexUniformBufferPtr = cb.get();
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
TestRegister Test_Backend_Mesh_DX11("Backend.Mesh_DX11", []() -> void { Backend_Mesh<RenderingWindowDX11>(); });
#endif