#include "EffectPlatformVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CompilerVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.PlatformVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.TextureVulkan.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"

#include "../../3rdParty/LLGI/src/LLGI.Compiler.h"

#include "../../3rdParty/stb/stb_image_write.h"

static auto code_dx_vs = R"(

#version 420

struct VS_INPUT
{
    vec3 Position;
    vec2 UV;
    vec4 Color;
};

struct VS_OUTPUT
{
    vec4 Position;
    vec2 UV;
    vec4 Color;
};

layout(location = 0) in vec3 input_Position;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Color;

VS_OUTPUT _main(VS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.Position = vec4(_input.Position, 1.0);
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.Position = input_Position;
    _input.UV = input_UV;
    _input.Color = input_Color;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
}


)";

static auto code_dx_ps = R"(

#version 420

struct PS_INPUT
{
    vec4 Position;
    vec2 UV;
    vec4 Color;
};

layout(set = 1, binding = 1) uniform sampler2D _61;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_INPUT _input)
{
    vec4 c = texture(_61, _input.UV);
    return c;
}

void main()
{
    PS_INPUT _input;
    _input.Position = gl_FragCoord;
    _input.UV = input_UV;
    _input.Color = input_Color;
    PS_INPUT param = _input;
    _entryPointOutput = _main(param);
}


)";

class DistortingCallbackVulkan : public EffekseerRenderer::DistortingCallback
{
	EffectPlatformVulkan* platform_ = nullptr;
	Effekseer::Backend::TextureRef texture_ = nullptr;

public:
	DistortingCallbackVulkan(EffectPlatformVulkan* platform)
		: platform_(platform)
	{
	}

	virtual ~DistortingCallbackVulkan()
	{
		texture_.Reset();
	}

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{

		if (texture_ == nullptr)
		{
			auto tex = (LLGI::TextureVulkan*)(platform_->GetCheckedTexture());
			EffekseerRendererVulkan::VulkanImageInfo info;
			info.image = static_cast<VkImage>(tex->GetImage());
			info.format = static_cast<VkFormat>(tex->GetVulkanFormat());
			info.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
			texture_ = EffekseerRendererVulkan::CreateTexture(renderer->GetGraphicsDevice(), info);
		}

		renderer->SetBackground(texture_);

		return true;
	}
};

void EffectPlatformVulkan::CreateShaders()
{
	auto compiler = new LLGI::CompilerVulkan();
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

EffekseerRenderer::RendererRef EffectPlatformVulkan::CreateRenderer()
{
	auto g = static_cast<LLGI::GraphicsVulkan*>(graphics_);
	auto p = static_cast<LLGI::PlatformVulkan*>(platform_);

	::EffekseerRendererVulkan::RenderPassInformation renderPassInfo;
	renderPassInfo.DoesPresentToScreen = true;
	renderPassInfo.RenderTextureCount = 1;
	renderPassInfo.RenderTextureFormats[0] = VK_FORMAT_R8G8B8A8_UNORM;
	renderPassInfo.DepthFormat = VK_FORMAT_D32_SFLOAT;
	auto renderer = ::EffekseerRendererVulkan::Create(static_cast<VkPhysicalDevice>(g->GetPysicalDevice()),
													  static_cast<VkDevice>(g->GetDevice()),
													  static_cast<VkQueue>(g->GetQueue()),
													  static_cast<VkCommandPool>(g->GetCommandPool()),
													  3,
													  renderPassInfo,
													  initParam_.SpriteCount);

	renderer->SetDistortingCallback(new DistortingCallbackVulkan(this));

	sfMemoryPoolEfk_ = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
	commandListEfk_ = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), sfMemoryPoolEfk_);

	CreateResources();

	return renderer;
}

EffectPlatformVulkan::~EffectPlatformVulkan()
{
}

void EffectPlatformVulkan::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	CreateCheckedTexture();
}

void EffectPlatformVulkan::DestroyDevice()
{
	EffectPlatformLLGI::DestroyDevice();
}

void EffectPlatformVulkan::BeginRendering()
{
	EffectPlatformLLGI::BeginRendering();

	auto cl = static_cast<LLGI::CommandListVulkan*>(commandList_.get());
	EffekseerRendererVulkan::BeginCommandList(commandListEfk_, static_cast<VkCommandBuffer>(cl->GetCommandBuffer()));
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformVulkan::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	EffekseerRendererVulkan::EndCommandList(commandListEfk_);

	EffectPlatformLLGI::EndRendering();
}

LLGI::Texture* EffectPlatformVulkan::GetCheckedTexture() const
{
	return checkTexture_;
}