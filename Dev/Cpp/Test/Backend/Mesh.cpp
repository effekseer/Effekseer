#include "../TestHelper.h"
#include "../Window/Window.h"

#undef None
#include <EffekseerRendererGL.h>

#include <memory>

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

static auto vs_shader = R"(

#version 140
in vec3 in_position;
in vec2 in_uv;
in vec4 in_color;

out vec4 vsps_color;

void main(void)
{
    gl_Position = vec4(in_position, 1.0);
    vsps_color = in_color;	
}

)";

static auto ps_shader = R"(

#version 140
in vec4 vsps_color;

out vec4 fragColor;

void main(void)
{
    fragColor = vsps_color;
}

)";

void Backend_Mesh()
{
	auto window = std::make_shared<RenderingWindow>(true, std::array<int, 2>({1280, 720}), "Backend.Mesh");

	auto graphicsDevice = Effekseer::CreateReference(EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3));

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
	auto uniformLayout = Effekseer::CreateReference(new Effekseer::Backend::UniformLayout({}, {}));

	auto shader = Effekseer::CreateReference(graphicsDevice->CreateShaderFromCodes(vs_shader, ps_shader, uniformLayout.get()));

	std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
	vertexLayoutElements.resize(3);
	vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
	vertexLayoutElements[0].Name = "in_position";
	vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
	vertexLayoutElements[1].Name = "in_uv";
	vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM;
	vertexLayoutElements[2].Name = "in_color";
	auto vertexLayout = Effekseer::CreateReference(graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size())));

	Effekseer::Backend::PipelineStateParameter pipParam;

	// OpenGL doesn't require it
	pipParam.FrameBufferPtr = nullptr;
	pipParam.VertexLayoutPtr = vertexLayout.get();
	pipParam.ShaderPtr = shader.get();
	pipParam.IsDepthTestEnabled = false;
	pipParam.IsBlendEnabled = false;

	auto pip = Effekseer::CreateReference(graphicsDevice->CreatePipelineState(pipParam));

	int count = 0;
	while (count < 60 && window->DoEvent())
	{
		graphicsDevice->BeginRenderPass(nullptr, true, true, Effekseer::Color(80, 80, 80, 255));

		Effekseer::Backend::DrawParameter drawParam;
		drawParam.VertexBufferPtr = vb.get();
		drawParam.IndexBufferPtr = ib.get();
		drawParam.PipelineStatePtr = pip.get();
		drawParam.PrimitiveCount = 2;
		drawParam.InstanceCount = 1;
		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();

		window->Present();

		count++;
	}
}

#if !defined(__FROM_CI__)
TestRegister Test_Backend_Mesh("Backend.Mesh", []() -> void { Backend_Mesh(); });
#endif