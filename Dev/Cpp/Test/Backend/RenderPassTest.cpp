#include "../TestHelper.h"
#include "../Window/RenderingWindowGL.h"
#include "Effekseer.h"
#include <EffekseerRendererGL.h>

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

static auto vs_render_shader_gl = R"(

#version 330
in vec3 in_position;
in vec2 in_uv;
in vec4 in_color;

uniform sampler2D i_shift;
uniform sampler2D i_color_shift;

out vec4 vsps_color;

void main(void)
{
	vec4 shift = texelFetch(i_shift, ivec2(0,0), 0);
	vec4 color_shift = texelFetch(i_color_shift, ivec2(0,0), 0);
    gl_Position = vec4(in_position, 1.0) + vec4(shift.x, 0, 0, 0);
    vsps_color = in_color + color_shift;
}

)";

static auto ps_render_shader_gl = R"(

#version 330
in vec4 vsps_color;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(mod(vsps_color.x,1),mod(vsps_color.y,1),mod(vsps_color.z,1),0);
}

)";

static auto vs_update_shader_gl = R"(

#version 330
in vec3 in_position;
in vec2 in_uv;
in vec4 in_color;

void main(void)
{
	gl_Position = vec4(in_position, 1.0);
}

)";

static auto ps_update_shader_gl = R"(

#version 330
uniform sampler2D i_shift;
uniform sampler2D i_color_shift;
layout(location = 0) out vec4 o_shift;
layout(location = 1) out vec4 o_color_shift;

void main(void)
{
	vec4 shift = texelFetch(i_shift, ivec2(0,0), 0);
	o_shift = vec4(mod(shift.x + 0.002, 1),0,0,0);
	vec4 color_shift = texelFetch(i_color_shift, ivec2(0,0), 0);
	o_color_shift = vec4(mod(color_shift.x + 0.008, 1),
						mod(color_shift.y + 0.008, 1),
						mod(color_shift.z + 0.008, 1),0);
}

)";

class ShiftBuffer
{
public:
	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, 4> textures;
	Effekseer::Backend::RenderPassRef renderPass;
	ShiftBuffer(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	{
		Effekseer::Backend::RenderTextureParameter texParam;
		int texWidth = 2;
		int texHeight = 2;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {texWidth, texHeight};

		auto positionTexture = graphicsDevice->CreateRenderTexture(texParam);
		auto colorTexture = graphicsDevice->CreateRenderTexture(texParam);
		textures.resize(2);
		textures.at(0) = positionTexture;
		textures.at(1) = colorTexture;

		Effekseer::Backend::DepthTextureParameter depthTexParam;
		depthTexParam.Size = texParam.Size;

		auto depthTex = graphicsDevice->CreateDepthTexture(depthTexParam);
		renderPass = graphicsDevice->CreateRenderPass(textures, depthTex);
	}
};

template <typename WINDOW>
class RenderPassTestContext
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
	std::vector<ShiftBuffer> buffers;
	int pingpong = 0;
	Effekseer::Backend::ShaderRef updateShader;
	Effekseer::Backend::ShaderRef renderShader;
	Effekseer::Backend::RenderPassRef windowRenderPass;
	Effekseer::Backend::VertexBufferRef updateVB;
	Effekseer::Backend::VertexBufferRef renderVB;
	Effekseer::Backend::IndexBufferRef updateIB;
	Effekseer::Backend::IndexBufferRef renderIB;
	Effekseer::Backend::VertexLayoutRef updateVL;
	Effekseer::Backend::VertexLayoutRef renderVL;

	void InitUpdateVertex()
	{
		std::array<SimpleVertex, 4> vbData;
		vbData[0].Position = {-1.0f, 1.0f, 0.5f};
		vbData[0].Color = {0, 0, 0, 255};
		vbData[1].Position = {1.0f, -1.0f, 0.5f};
		vbData[1].Color = {0, 0, 0, 255};
		vbData[2].Position = {-1.0f, -1.0f, 0.5f};
		vbData[2].Color = {0, 0, 0, 255};

		updateVB = graphicsDevice->CreateVertexBuffer(sizeof(SimpleVertex) * 3, vbData.data(), false);

		std::array<int32_t, 3> ibData;
		ibData[0] = 0;
		ibData[1] = 1;
		ibData[2] = 2;
		updateIB = graphicsDevice->CreateIndexBuffer(3, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

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

		updateVL = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	}

	void InitRenderVertex()
	{
		std::array<SimpleVertex, 4> vbData;
		vbData[0].Position = {-0.5f, 0.5f, 0.5f};
		vbData[0].Color = {0, 0, 0, 255};
		vbData[1].Position = {0.5f, 0.5f, 0.5f};
		vbData[1].Color = {255, 0, 0, 255};
		vbData[2].Position = {0.5f, -0.5f, 0.5f};
		vbData[2].Color = {255, 255, 0, 255};
		vbData[3].Position = {-0.5f, -0.5f, 0.5f};
		vbData[3].Color = {0, 255, 0, 255};

		renderVB = graphicsDevice->CreateVertexBuffer(sizeof(SimpleVertex) * 4, vbData.data(), false);

		std::array<int32_t, 6> ibData;
		ibData[0] = 0;
		ibData[1] = 1;
		ibData[2] = 2;
		ibData[3] = 0;
		ibData[4] = 2;
		ibData[5] = 3;

		renderIB = graphicsDevice->CreateIndexBuffer(6, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

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

		renderVL = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	}

public:
	std::shared_ptr<WINDOW> window;

	RenderPassTestContext()
	{
		window = std::make_shared<WINDOW>(std::array<int, 2>({1280, 720}), "Backend.RenderPassTesT");
		graphicsDevice = EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
		buffers.push_back(ShiftBuffer(graphicsDevice));
		buffers.push_back(ShiftBuffer(graphicsDevice));
		auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"i_shift", "i_color_shift"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});
		updateShader = graphicsDevice->CreateShaderFromCodes({{vs_update_shader_gl}}, {{ps_update_shader_gl}}, uniformLayout);
		renderShader = graphicsDevice->CreateShaderFromCodes({{vs_render_shader_gl}}, {{ps_render_shader_gl}}, uniformLayout);
		windowRenderPass = nullptr;

		InitUpdateVertex();
		InitRenderVertex();
	}

	void Update()
	{
		const int sourceIndex = pingpong;
		const int targetIndex = (pingpong + 1) % 2;

		Effekseer::Backend::PipelineStateParameter pipParam;

		// OpenGL doesn't require it
		pipParam.FrameBufferPtr = nullptr;
		pipParam.VertexLayoutPtr = updateVL;
		pipParam.ShaderPtr = updateShader;
		pipParam.IsDepthTestEnabled = false;
		pipParam.IsBlendEnabled = false;

		auto pip = graphicsDevice->CreatePipelineState(pipParam);

		graphicsDevice->BeginRenderPass(buffers[targetIndex].renderPass, true, true, Effekseer::Color(80, 80, 80, 255));
		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = buffers[sourceIndex].textures.size();

		for (int i = 0; i < drawParam.TextureCount; i++)
		{
			drawParam.TexturePtrs[i] = buffers[sourceIndex].textures.at(i);
			drawParam.TextureSamplingTypes[i] = Effekseer::Backend::TextureSamplingType::Nearest;
			drawParam.TextureWrapTypes[i] = Effekseer::Backend::TextureWrapType::Clamp;
		}
		drawParam.PipelineStatePtr = pip;

		drawParam.VertexBufferPtr = updateVB;
		drawParam.IndexBufferPtr = updateIB;
		drawParam.PipelineStatePtr = pip;
		drawParam.PrimitiveCount = 1;
		drawParam.InstanceCount = 1;

		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();

		pingpong = (pingpong + 1) % 2;
	}

	void Render()
	{

		Effekseer::Backend::PipelineStateParameter pipParam;

		// OpenGL doesn't require it
		pipParam.FrameBufferPtr = nullptr;
		pipParam.VertexLayoutPtr = renderVL;
		pipParam.ShaderPtr = renderShader;
		pipParam.IsDepthTestEnabled = false;
		pipParam.IsBlendEnabled = false;

		auto pip = graphicsDevice->CreatePipelineState(pipParam);

		graphicsDevice->BeginRenderPass(windowRenderPass, true, true, Effekseer::Color(80, 80, 80, 255));
		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = buffers[pingpong].textures.size();
		for (int i = 0; i < drawParam.TextureCount; i++)
		{
			drawParam.TexturePtrs[i] = buffers[pingpong].textures.at(i);
			drawParam.TextureSamplingTypes[i] = Effekseer::Backend::TextureSamplingType::Nearest;
			drawParam.TextureWrapTypes[i] = Effekseer::Backend::TextureWrapType::Clamp;
		}
		drawParam.PipelineStatePtr = pip;

		drawParam.VertexBufferPtr = renderVB;
		drawParam.IndexBufferPtr = renderIB;
		drawParam.PipelineStatePtr = pip;
		drawParam.PrimitiveCount = 2;
		drawParam.InstanceCount = 1;

		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();
	}
};

template <typename WINDOW>
void RenderPassTest()
{
	RenderPassTestContext<WINDOW> renderPassTestContext;
	int count = 0;
	while (count < 60 && renderPassTestContext.window->DoEvent())
	{
		renderPassTestContext.Update();
		renderPassTestContext.Render();
		renderPassTestContext.window->Present();

		count++;
	}
}

#if !defined(__FROM_CI__)
TestRegister Test_Backend_RenderPass("Backend.RenderPass", []() -> void { RenderPassTest<RenderingWindowGL>(); });
#endif
