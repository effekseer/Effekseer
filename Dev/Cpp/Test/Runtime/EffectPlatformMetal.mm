#include "EffectPlatformMetal.h"
#include "../../EffekseerRendererMetal/EffekseerRendererMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.CommandListMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.GraphicsMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.IndexBufferMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.PlatformMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.TextureMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.CompilerMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.Metal_Impl.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"

#include "../../3rdParty/LLGI/src/LLGI.Compiler.h"

#include "../../3rdParty/stb/stb_image_write.h"

static auto code_dx_vs = R"(


#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT
{
    float3 Position;
    float2 UV;
    float4 Color;
};

struct VS_OUTPUT
{
    float4 Position;
    float2 UV;
    float4 Color;
};

struct main0_out
{
    float2 _entryPointOutput_UV [[user(locn0)]];
    float4 _entryPointOutput_Color [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 input_Position [[attribute(0)]];
    float2 input_UV [[attribute(1)]];
    float4 input_Color [[attribute(2)]];
};

static inline __attribute__((always_inline))
VS_OUTPUT _main(thread const VS_INPUT& _input)
{
    VS_OUTPUT _output;
    _output.Position = float4(_input.Position, 1.0);
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    VS_INPUT _input;
    _input.Position = in.input_Position;
    _input.UV = in.input_UV;
    _input.Color = in.input_Color;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    out.gl_Position = flattenTemp.Position;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_Color = flattenTemp.Color;
    return out;
}




)";

static auto code_dx_ps = R"(

#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_INPUT
{
    float4 Position;
    float2 UV;
    float4 Color;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 input_UV [[user(locn0)]];
    float4 input_Color [[user(locn1)]];
};

static inline __attribute__((always_inline))
float4 _main(thread const PS_INPUT& _input, thread texture2d<float> txt, thread sampler smp)
{
    float4 c = txt.sample(smp, _input.UV);
    return c;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> txt [[texture(0)]], sampler smp [[sampler(0)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_INPUT _input;
    _input.Position = gl_FragCoord;
    _input.UV = in.input_UV;
    _input.Color = in.input_Color;
    PS_INPUT param = _input;
    out._entryPointOutput = _main(param, txt, smp);
    return out;
}

)";

class DistortingCallbackMetal : public EffekseerRenderer::DistortingCallback
{
	EffectPlatformMetal* platform_ = nullptr;
    Effekseer::Backend::TextureRef texture_ = nullptr;
    
public:
	DistortingCallbackMetal(EffectPlatformMetal* platform) : platform_(platform)
	{
	}

	virtual ~DistortingCallbackMetal()
	{
        texture_.Reset();
	}

	bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{
        if (texture_ == nullptr)
		{
			auto tex = (LLGI::TextureMetal*)(platform_->GetCheckedTexture());
			texture_ = EffekseerRendererMetal::CreateTexture(renderer->GetGraphicsDevice(), tex->GetTexture());
		}

        renderer->SetBackground(texture_);

		return true;
	}
};

void EffectPlatformMetal::CreateShaders()
{
    auto compiler =  new LLGI::CompilerMetal();
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

	shader_vs_ = graphics_->CreateShader(data_vs.data(), data_vs.size());
	shader_ps_ = graphics_->CreateShader(data_ps.data(), data_ps.size());
}

EffekseerRenderer::RendererRef EffectPlatformMetal::CreateRenderer()
{
	auto renderer = EffekseerRendererMetal::Create(10000, MTLPixelFormatRGBA8Unorm,  MTLPixelFormatDepth32Float, false);

	renderer->SetDistortingCallback(new DistortingCallbackMetal(this));

	sfMemoryPoolEfk_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
	commandListEfk_ = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), sfMemoryPoolEfk_);

	CreateResources();

	return renderer;
}

EffectPlatformMetal::~EffectPlatformMetal() {}

void EffectPlatformMetal::InitializeDevice(const EffectPlatformInitializingParameter& param) { CreateCheckedTexture(); }

void EffectPlatformMetal::DestroyDevice() { EffectPlatformLLGI::DestroyDevice(); }

void EffectPlatformMetal::BeginRendering()
{
	EffectPlatformLLGI::BeginRendering();

	auto cl = static_cast<LLGI::CommandListMetal*>(commandList_.get());
	EffekseerRendererMetal::BeginCommandList(commandListEfk_, cl->GetRenderCommandEncorder());
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformMetal::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	EffekseerRendererMetal::EndCommandList(commandListEfk_);

	EffectPlatformLLGI::EndRendering();
}

LLGI::Texture* EffectPlatformMetal::GetCheckedTexture() const { return checkTexture_; }
