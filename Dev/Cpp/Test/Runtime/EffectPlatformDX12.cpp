#include "EffectPlatformDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.BufferDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.CommandListDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.CompilerDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.PlatformDX12.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"

#include "../../3rdParty/LLGI/src/LLGI.Compiler.h"

#include "../../3rdParty/stb/stb_image_write.h"

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
	Effekseer::Backend::TextureRef texture_ = nullptr;

public:
	DistortingCallbackDX12(EffectPlatformDX12* platform)
		: platform_(platform)
	{
	}

	virtual ~DistortingCallbackDX12()
	{
		texture_.Reset();
	}

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{
		if (texture_ == nullptr)
		{
			auto tex = (LLGI::TextureDX12*)(platform_->GetCheckedTexture());
			texture_ = EffekseerRendererDX12::CreateTexture(renderer->GetGraphicsDevice(), tex->Get());
		}

		renderer->SetBackground(texture_);

		return true;
	}
};

void EffectPlatformDX12::CreateShaders()
{
	auto compiler = new LLGI::CompilerDX12();
	compiler->Initialize();

	LLGI::CompilerResult result_vs;
	LLGI::CompilerResult result_ps;

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	compiler->Compile(result_vs, code_dx_vs, LLGI::ShaderStageType::Vertex);
	assert(result_vs.Message == "");
	compiler->Compile(result_ps, code_dx_ps, LLGI::ShaderStageType::Pixel);
	assert(result_ps.Message == "");
	compiler->Release();

	for (auto& b : result_vs.Binary)
	{
		LLGI::DataStructure d;
		d.Data = b.data();
		d.Size = static_cast<int32_t>(b.size());
		data_vs.push_back(d);
	}

	for (auto& b : result_ps.Binary)
	{
		LLGI::DataStructure d;
		d.Data = b.data();
		d.Size = static_cast<int32_t>(b.size());
		data_ps.push_back(d);
	}

	shader_vs_ = graphics_->CreateShader(data_vs.data(), static_cast<int32_t>(data_vs.size()));
	shader_ps_ = graphics_->CreateShader(data_ps.data(), static_cast<int32_t>(data_ps.size()));
}

EffekseerRenderer::RendererRef EffectPlatformDX12::CreateRenderer()
{
	auto g = static_cast<LLGI::GraphicsDX12*>(graphics_);
	auto p = static_cast<LLGI::PlatformDX12*>(platform_);

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto renderer = EffekseerRendererDX12::Create(
		g->GetDevice(), g->GetCommandQueue(), g->GetSwapBufferCount(), &format, 1, DXGI_FORMAT_D32_FLOAT, false, initParam_.SpriteCount);

	renderer->SetDistortingCallback(new DistortingCallbackDX12(this));

	sfMemoryPoolEfk_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
	commandListEfk_ = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), sfMemoryPoolEfk_);

	CreateResources();

	return renderer;
}

EffectPlatformDX12::~EffectPlatformDX12()
{
}

void EffectPlatformDX12::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	CreateCheckedTexture();
}

void EffectPlatformDX12::DestroyDevice()
{
	EffectPlatformLLGI::DestroyDevice();
}

void EffectPlatformDX12::BeginRendering()
{
	EffectPlatformLLGI::BeginRendering();

	auto cl = static_cast<LLGI::CommandListDX12*>(commandList_.get());
	EffekseerRendererDX12::BeginCommandList(commandListEfk_, cl->GetCommandList());
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformDX12::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(commandListEfk_);

	EffectPlatformLLGI::EndRendering();
}

LLGI::Texture* EffectPlatformDX12::GetCheckedTexture() const
{
	return checkTexture_;
}