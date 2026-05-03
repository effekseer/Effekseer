#include "EffectPlatformWebGPU.h"
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.CommandListWebGPU.h"
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.CompilerWebGPU.h"
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.GraphicsWebGPU.h"
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.TextureWebGPU.h"
#include "../../3rdParty/LLGI/src/LLGI.CommandList.h"
#include <EffekseerRendererLLGI/GraphicsDevice.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h>

static auto code_wgsl_vs = R"(
struct VSInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
    @location(2) color: vec4<f32>,
};

struct VSOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) color: vec4<f32>,
};

@vertex
fn main(input: VSInput) -> VSOutput {
    var output: VSOutput;
    output.position = vec4<f32>(input.position, 1.0);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}
)";

static auto code_wgsl_ps = R"(
@group(1) @binding(0) var colorTexture: texture_2d<f32>;
@group(2) @binding(0) var colorSampler: sampler;

struct PSInput {
    @location(0) uv: vec2<f32>,
    @location(1) color: vec4<f32>,
};

@fragment
fn main(input: PSInput) -> @location(0) vec4<f32> {
    return textureSample(colorTexture, colorSampler, input.uv);
}
)";

class DistortingCallbackWebGPU : public EffekseerRenderer::DistortingCallback
{
	EffectPlatformWebGPU* platform_ = nullptr;
	Effekseer::Backend::TextureRef texture_ = nullptr;

public:
	DistortingCallbackWebGPU(EffectPlatformWebGPU* platform)
		: platform_(platform)
	{
	}

	virtual ~DistortingCallbackWebGPU() { texture_.Reset(); }

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{
		platform_->UpdateBackgroundTextureForDistortion();

		if (texture_ == nullptr)
		{
			auto graphicsDevice = renderer->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
			texture_ = graphicsDevice->CreateTexture(platform_->GetBackgroundTexture());
		}

		renderer->SetBackground(texture_);
		return true;
	}
};

void EffectPlatformWebGPU::CreateShaders()
{
	auto compiler = new LLGI::CompilerWebGPU();
	compiler->Initialize();

	LLGI::CompilerResult result_vs;
	LLGI::CompilerResult result_ps;

	std::vector<LLGI::DataStructure> data_vs;
	std::vector<LLGI::DataStructure> data_ps;

	compiler->Compile(result_vs, code_wgsl_vs, LLGI::ShaderStageType::Vertex);
	assert(result_vs.Message == "");
	compiler->Compile(result_ps, code_wgsl_ps, LLGI::ShaderStageType::Pixel);
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

EffekseerRenderer::RendererRef EffectPlatformWebGPU::CreateRenderer()
{
	auto g = static_cast<LLGI::GraphicsWebGPU*>(graphics_);

	::EffekseerRendererWebGPU::RenderPassInformation renderPassInfo;
	renderPassInfo.DoesPresentToScreen = false;
	renderPassInfo.RenderTextureCount = 1;
	renderPassInfo.RenderTextureFormats[0] = wgpu::TextureFormat::RGBA8Unorm;
	renderPassInfo.DepthFormat = wgpu::TextureFormat::Depth32Float;

	auto graphicsDevice = ::EffekseerRendererWebGPU::CreateGraphicsDevice(g->GetDevice());
	auto renderer = ::EffekseerRendererWebGPU::Create(graphicsDevice, renderPassInfo, initParam_.SpriteCount);

	renderer->SetDistortingCallback(new DistortingCallbackWebGPU(this));

	sfMemoryPoolEfk_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());

	CreateResources();

	return renderer;
}

EffectPlatformWebGPU::~EffectPlatformWebGPU() {}

void EffectPlatformWebGPU::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	CreateCheckedTexture();
}

void EffectPlatformWebGPU::DestroyDevice()
{
	ES_SAFE_RELEASE(backgroundTexture_);

	EffectPlatformLLGI::DestroyDevice();
}

void EffectPlatformWebGPU::BeginRendering()
{
	EffectPlatformLLGI::BeginRendering();

	auto memoryPool = static_cast<EffekseerRendererLLGI::SingleFrameMemoryPool*>(sfMemoryPoolEfk_.Get());
	commandListEfk_ = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(graphics_, commandList_.get(), memoryPool->GetInternal());
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformWebGPU::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	commandListEfk_.Reset();

	EffectPlatformLLGI::EndRendering();
}

LLGI::Texture* EffectPlatformWebGPU::GetBackgroundTexture()
{
	if (backgroundTexture_ == nullptr)
	{
		LLGI::TextureParameter param;
		param.Size = LLGI::Vec3I(initParam_.WindowSize[0], initParam_.WindowSize[1], 1);
		backgroundTexture_ = graphics_->CreateTexture(param);
	}

	return backgroundTexture_;
}

void EffectPlatformWebGPU::UpdateBackgroundTextureForDistortion()
{
	auto background = GetBackgroundTexture();

	commandList_->EndRenderPass();
	commandList_->CopyTexture(colorBuffer_, background);

	renderPass_->SetIsColorCleared(false);
	renderPass_->SetIsDepthCleared(false);
	commandList_->BeginRenderPass(renderPass_);
}
