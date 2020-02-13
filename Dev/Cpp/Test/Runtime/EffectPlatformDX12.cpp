#include "EffectPlatformDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.CommandListDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.IndexBufferDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.PlatformDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.VertexBufferDX12.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"

#include "../../3rdParty/LLGI/src/LLGI.Compiler.h"

#include "../../3rdParty/stb/stb_image_write.h"

struct SimpleVertex
{
	LLGI::Vec3F Pos;
	LLGI::Vec2F UV;
	LLGI::Color8 Color;
};

static auto code_dx_vs = R"(
struct VS_INPUT{
    float3 Position : POSITION0;
	float2 UV : UV0;
    float4 Color : COLOR0;
};
struct VS_OUTPUT{
    float4 Position : SV_POSITION;
	float2 UV : UV0;
    float4 Color : COLOR0;
};
    
VS_OUTPUT main(VS_INPUT input){
    VS_OUTPUT output;
        
    output.Position = float4(input.Position, 1.0f);
	output.UV = input.UV;
    output.Color = input.Color;
        
    return output;
}
)";

static auto code_dx_ps = R"(
Texture2D txt : register(t0);
SamplerState smp : register(s0);

struct PS_INPUT
{
    float4  Position : SV_POSITION;
	float2  UV : UV0;
    float4  Color    : COLOR0;
};

float4 main(PS_INPUT input) : SV_TARGET 
{ 
	float4 c;
	c = input.Color * txt.Sample(smp, input.UV);
	return c;
}
)";

class DistortingCallbackDX12 : public EffekseerRenderer::DistortingCallback
{
	EffectPlatformDX12* platform_ = nullptr;
	::EffekseerRenderer::Renderer* renderer_ = nullptr;
	Effekseer::TextureData* textureData_ = nullptr;

public:
	DistortingCallbackDX12(EffectPlatformDX12* platform, ::EffekseerRenderer::Renderer* renderer) : platform_(platform), renderer_(renderer) {}

	virtual ~DistortingCallbackDX12() { 
	
		if (textureData_ != nullptr)
		{
			EffekseerRendererDX12::DeleteTextureData(renderer_, textureData_);
		}
	}

	
	virtual bool OnDistorting() override { 
	
		if (textureData_ == nullptr)
		{
			auto tex = (LLGI::TextureDX12*)(platform_->GetCheckedTexture());
			textureData_ = EffekseerRendererDX12::CreateTextureData(renderer_, tex->Get());
		}

		renderer_->SetBackgroundTexture(textureData_);

		return true;
	}
};

void EffectPlatformDX12::CreateCheckedTexture()
{
	auto g = static_cast<LLGI::GraphicsDX12*>(graphics_);

	LLGI::TextureInitializationParameter param;
	param.Size.X = 1280;
	param.Size.Y = 720;
	checkTexture_ = g->CreateTexture(param);

	auto b = (uint8_t*)checkTexture_->Lock();

	memcpy(b, checkeredPattern_.data(), param.Size.X * param.Size.Y * 4);

	checkTexture_->Unlock();
}

EffekseerRenderer::Renderer* EffectPlatformDX12::CreateRenderer()
{
	auto g = static_cast<LLGI::GraphicsDX12*>(graphics_);
	auto p = static_cast<LLGI::PlatformDX12*>(platform_);

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto renderer =
		EffekseerRendererDX12::Create(g->GetDevice(), g->GetCommandQueue(), g->GetSwapBufferCount(), &format, 1, true, false, 10000);

	renderer->SetDistortingCallback(new DistortingCallbackDX12(this, renderer));

	sfMemoryPoolEfk_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(renderer);
	commandListEfk_ = EffekseerRendererDX12::CreateCommandList(renderer, sfMemoryPoolEfk_);

	LLGI::RenderTextureInitializationParameter renderParam;
	renderParam.Size = LLGI::Vec2I(1280, 720);
	LLGI::DepthTextureInitializationParameter depthParam;
	depthParam.Size = LLGI::Vec2I(1280, 720);
	colorBuffer_ = g->CreateRenderTexture(renderParam);
	depthBuffer_ = g->CreateDepthTexture(depthParam);
	renderPass_ = g->CreateRenderPass((const LLGI::Texture**)&colorBuffer_, 1, depthBuffer_);

	auto compiler = LLGI::CreateCompiler(LLGI::DeviceType::DirectX12);

	LLGI::CompilerResult result_vs;
	LLGI::CompilerResult result_ps;

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	compiler->Compile(result_vs, code_dx_vs, LLGI::ShaderStageType::Vertex);
	assert(result_vs.Message == "");
	compiler->Compile(result_ps, code_dx_ps, LLGI::ShaderStageType::Pixel);
	assert(result_ps.Message == "");

	for (auto& b : result_vs.Binary)
	{
		LLGI::DataStructure d;
		d.Data = b.data();
		d.Size = b.size();
		data_vs.push_back(d);
	}

	for (auto& b : result_ps.Binary)
	{
		LLGI::DataStructure d;
		d.Data = b.data();
		d.Size = b.size();
		data_ps.push_back(d);
	}

	shader_vs_ = g->CreateShader(data_vs.data(), data_vs.size());
	shader_ps_ = g->CreateShader(data_ps.data(), data_ps.size());

	vb_ = g->CreateVertexBuffer(sizeof(SimpleVertex) * 4);
	ib_ = g->CreateIndexBuffer(2, 6);
	auto vb_buf = (SimpleVertex*)vb_->Lock();
	vb_buf[0].Pos = LLGI::Vec3F(-1.0f, 1.0f, 0.5f);
	vb_buf[1].Pos = LLGI::Vec3F(1.0f, 1.0f, 0.5f);
	vb_buf[2].Pos = LLGI::Vec3F(1.0f, -1.0f, 0.5f);
	vb_buf[3].Pos = LLGI::Vec3F(-1.0f, -1.0f, 0.5f);

	vb_buf[0].UV = LLGI::Vec2F(0.0f, 0.0f);
	vb_buf[1].UV = LLGI::Vec2F(1.0f, 0.0f);
	vb_buf[2].UV = LLGI::Vec2F(1.0f, 1.0f);
	vb_buf[3].UV = LLGI::Vec2F(0.0f, 1.0f);

	vb_buf[0].Color = LLGI::Color8();
	vb_buf[1].Color = LLGI::Color8();
	vb_buf[2].Color = LLGI::Color8();
	vb_buf[3].Color = LLGI::Color8();

	vb_->Unlock();

	auto ib_buf = (uint16_t*)ib_->Lock();
	ib_buf[0] = 0;
	ib_buf[1] = 1;
	ib_buf[2] = 2;
	ib_buf[3] = 0;
	ib_buf[4] = 2;
	ib_buf[5] = 3;
	ib_->Unlock();

	compiler->Release();

	rppip_ = g->CreateRenderPassPipelineState(renderPass_);

	{
		pip_ = g->CreatePiplineState();
		pip_->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
		pip_->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
		pip_->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
		pip_->VertexLayoutNames[0] = "POSITION";
		pip_->VertexLayoutNames[1] = "UV";
		pip_->VertexLayoutNames[2] = "COLOR";
		pip_->VertexLayoutCount = 3;
		pip_->IsDepthTestEnabled = false;
		pip_->IsDepthWriteEnabled = false;
		pip_->Culling = LLGI::CullingMode::DoubleSide;
		pip_->SetShader(LLGI::ShaderStageType::Vertex, shader_vs_);
		pip_->SetShader(LLGI::ShaderStageType::Pixel, shader_ps_);
		pip_->SetRenderPassPipelineState(rppip_);
		pip_->Compile();
	}

	return renderer;
}

EffectPlatformDX12::~EffectPlatformDX12() {}

void EffectPlatformDX12::InitializeDevice(const EffectPlatformInitializingParameter& param) { CreateCheckedTexture(); }

void EffectPlatformDX12::DestroyDevice()
{
	EffectPlatformLLGI::DestroyDevice();
	ES_SAFE_RELEASE(commandListEfk_);
	ES_SAFE_RELEASE(sfMemoryPoolEfk_);
	ES_SAFE_RELEASE(renderPass_);
	ES_SAFE_RELEASE(colorBuffer_);
	ES_SAFE_RELEASE(depthBuffer_);
	ES_SAFE_RELEASE(shader_vs_);
	ES_SAFE_RELEASE(shader_ps_);
	ES_SAFE_RELEASE(vb_);
	ES_SAFE_RELEASE(ib_);
	ES_SAFE_RELEASE(rppip_);
	ES_SAFE_RELEASE(pip_);
	ES_SAFE_RELEASE(checkTexture_);
}

void EffectPlatformDX12::BeginRendering()
{
	auto cl = static_cast<LLGI::CommandListDX12*>(commandList_);

	sfMemoryPoolEfk_->NewFrame();

	LLGI::Color8 color;
	color.R = 64;
	color.G = 64;
	color.B = 64;
	color.A = 255;
	renderPass_->SetClearColor(color);
	renderPass_->SetIsColorCleared(true);
	renderPass_->SetIsDepthCleared(true);

	commandList_->Begin();
	commandList_->BeginRenderPass(renderPass_);

	// check
	commandList_->SetVertexBuffer(vb_, sizeof(SimpleVertex), 0);
	commandList_->SetIndexBuffer(ib_);
	commandList_->SetPipelineState(pip_);
	commandList_->SetTexture(
		checkTexture_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
	commandList_->Draw(2);

	EffekseerRendererDX12::BeginCommandList(commandListEfk_, cl->GetCommandList());
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformDX12::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(commandListEfk_);

	commandList_->EndRenderPass();

	auto currentScreen = platform_->GetCurrentScreen(LLGI::Color8(), true);
	commandList_->BeginRenderPass(currentScreen);
	commandList_->SetVertexBuffer(vb_, sizeof(SimpleVertex), 0);
	commandList_->SetIndexBuffer(ib_);
	commandList_->SetPipelineState(pip_);
	commandList_->SetTexture(
		colorBuffer_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
	commandList_->Draw(2);
	commandList_->EndRenderPass();

	commandList_->End();
}

LLGI::Texture* EffectPlatformDX12::GetCheckedTexture() const { return checkTexture_; }