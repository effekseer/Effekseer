#include "EffectPlatformLLGI.h"
#include "../3rdParty/LLGI/src/LLGI.Buffer.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"
#include "../3rdParty/LLGI/src/LLGI.Compiler.h"
#include "../3rdParty/LLGI/src/LLGI.Graphics.h"
#include "../3rdParty/LLGI/src/LLGI.PipelineState.h"
#include "../3rdParty/LLGI/src/LLGI.Platform.h"
#include "../3rdParty/LLGI/src/LLGI.Shader.h"
#include "../3rdParty/LLGI/src/LLGI.Texture.h"

#ifdef __EFFEKSEER_BUILD_DX12__
#include "../../3rdParty/LLGI/src/DX12/LLGI.CompilerDX12.h"
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CompilerVulkan.h"
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.CompilerWebGPU.h"
#endif
#ifdef __APPLE__
#include "../../3rdParty/LLGI/src/Metal/LLGI.CompilerMetal.h"
#endif

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI/GraphicsDevice.h"
#include "../../3rdParty/stb/stb_image_write.h"

#include <cassert>
#include <cstdio>

namespace
{

const auto ground_vs_dx12 = R"(
struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : UV0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	return output;
}
)";

const auto ground_ps_dx12 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float checker = frac((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = lerp(darkColor, brightColor, step(0.5, checker));
	float distanceFade = saturate(length(input.WorldXZ) * 0.025);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_dx12 = R"(
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 WorldXZ : UV0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.Position.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_vulkan = R"(
#version 420

layout(location = 0) in vec4 input_Position;
layout(location = 1) in vec2 input_WorldXZ;
layout(location = 0) out vec2 output_WorldXZ;

void main()
{
	vec4 position = input_Position;
	position.y = -position.y;
	gl_Position = position;
	output_WorldXZ = input_WorldXZ;
}
)";

const auto ground_ps_vulkan = R"(
#version 420

layout(location = 0) in vec2 input_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	float checker = fract((floor(input_WorldXZ.x) + floor(input_WorldXZ.y)) * 0.5);
	vec3 darkColor = vec3(0.24, 0.32, 0.27);
	vec3 brightColor = vec3(0.39, 0.50, 0.42);
	vec3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(input_WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	output_Color = vec4(color, 1.0);
}
)";

const auto ground_depth_ps_vulkan = R"(
#version 420

layout(location = 0) in vec2 input_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	output_Color = vec4(gl_FragCoord.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float4 Position [[attribute(0)]];
	float2 WorldXZ [[attribute(1)]];
};

struct main0_out
{
	float2 WorldXZ [[user(locn0)]];
	float4 Position [[position]];
};

vertex main0_out main0(main0_in input [[stage_in]])
{
	main0_out output = {};
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	return output;
}
)";

const auto ground_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float2 WorldXZ [[user(locn0)]];
};

fragment float4 main0(main0_in input [[stage_in]])
{
	float checker = fract((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(input.WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_metal = R"(
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_in
{
	float2 WorldXZ [[user(locn0)]];
};

fragment float4 main0(main0_in input [[stage_in]], float4 position [[position]])
{
	return float4(position.z, 1.0, 1.0, 1.0);
}
)";

const auto ground_vs_webgpu = R"(
struct VSOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) worldXZ: vec2<f32>,
};

@vertex
fn main(
	@location(0) position: vec4<f32>,
	@location(1) worldXZ: vec2<f32>
) -> VSOutput {
	var output: VSOutput;
	output.position = position;
	output.worldXZ = worldXZ;
	return output;
}
)";

const auto ground_ps_webgpu = R"(
struct PSInput {
	@location(0) worldXZ: vec2<f32>,
};

@fragment
fn main(input: PSInput) -> @location(0) vec4<f32> {
	let checker = fract((floor(input.worldXZ.x) + floor(input.worldXZ.y)) * 0.5);
	let darkColor = vec3<f32>(0.24, 0.32, 0.27);
	let brightColor = vec3<f32>(0.39, 0.50, 0.42);
	var color = mix(darkColor, brightColor, step(0.5, checker));
	let distanceFade = clamp(length(input.worldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	return vec4<f32>(color, 1.0);
}
)";

const auto ground_depth_ps_webgpu = R"(
struct PSInput {
	@builtin(position) position: vec4<f32>,
	@location(0) worldXZ: vec2<f32>,
};

@fragment
fn main(input: PSInput) -> @location(0) vec4<f32> {
	return vec4<f32>(input.position.z, 1.0, 1.0, 1.0);
}
)";

LLGI::Compiler* CreateGroundCompiler(LLGI::DeviceType deviceType)
{
	switch (deviceType)
	{
	case LLGI::DeviceType::DirectX12:
#ifdef __EFFEKSEER_BUILD_DX12__
		return new LLGI::CompilerDX12();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::Vulkan:
#ifdef __EFFEKSEER_BUILD_VULKAN__
		return new LLGI::CompilerVulkan();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::Metal:
#ifdef __APPLE__
		return new LLGI::CompilerMetal();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::WebGPU:
#ifdef __EFFEKSEER_BUILD_WEBGPU__
		return new LLGI::CompilerWebGPU();
#else
		return nullptr;
#endif
	default:
		return nullptr;
	}
}

} // namespace

void EffectPlatformLLGI::CreateCheckedTexture()
{
	LLGI::TextureInitializationParameter param;
	param.Size.X = initParam_.WindowSize[0];
	param.Size.Y = initParam_.WindowSize[1];
	checkTexture_ = graphics_->CreateTexture(param);

	UpdateBackgroundTexture();
}

void EffectPlatformLLGI::UpdateBackgroundTexture()
{
	if (checkTexture_ == nullptr)
	{
		return;
	}

	auto b = (uint8_t*)checkTexture_->Lock();

	memcpy(b, checkeredPattern_.data(), initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);

	checkTexture_->Unlock();
}

void EffectPlatformLLGI::CreateGroundShaders()
{
	const char* vsCode = nullptr;
	const char* depthVsCode = nullptr;
	const char* psCode = nullptr;
	const char* depthPsCode = nullptr;

	switch (deviceType_)
	{
	case LLGI::DeviceType::DirectX12:
		vsCode = ground_vs_dx12;
		psCode = ground_ps_dx12;
		depthPsCode = ground_depth_ps_dx12;
		break;
	case LLGI::DeviceType::Vulkan:
		vsCode = ground_vs_vulkan;
		psCode = ground_ps_vulkan;
		depthPsCode = ground_depth_ps_vulkan;
		break;
	case LLGI::DeviceType::Metal:
		vsCode = ground_vs_metal;
		psCode = ground_ps_metal;
		depthPsCode = ground_depth_ps_metal;
		break;
	case LLGI::DeviceType::WebGPU:
		vsCode = ground_vs_webgpu;
		psCode = ground_ps_webgpu;
		depthPsCode = ground_depth_ps_webgpu;
		break;
	default:
		assert(false);
		return;
	}

	auto compiler = CreateGroundCompiler(deviceType_);
	if (compiler == nullptr)
	{
		printf("Failed to create an LLGI shader compiler for the ground depth test.\n");
		return;
	}
	if (depthVsCode == nullptr)
	{
		depthVsCode = vsCode;
	}
	compiler->Initialize();

	LLGI::CompilerResult resultVs;
	LLGI::CompilerResult resultDepthVs;
	LLGI::CompilerResult resultPs;
	LLGI::CompilerResult resultDepthPs;
	compiler->Compile(resultVs, vsCode, LLGI::ShaderStageType::Vertex);
	compiler->Compile(resultDepthVs, depthVsCode, LLGI::ShaderStageType::Vertex);
	compiler->Compile(resultPs, psCode, LLGI::ShaderStageType::Pixel);
	compiler->Compile(resultDepthPs, depthPsCode, LLGI::ShaderStageType::Pixel);

	if (!resultVs.Message.empty() || !resultDepthVs.Message.empty() || !resultPs.Message.empty() || !resultDepthPs.Message.empty())
	{
		printf("Failed to compile ground shaders.\nVS: %s\nDepth VS: %s\nPS: %s\nDepth PS: %s\n",
			   resultVs.Message.c_str(),
			   resultDepthVs.Message.c_str(),
			   resultPs.Message.c_str(),
			   resultDepthPs.Message.c_str());
		compiler->Release();
		return;
	}

	if (resultVs.Binary.empty() || resultDepthVs.Binary.empty() || resultPs.Binary.empty() || resultDepthPs.Binary.empty())
	{
		printf("Failed to compile ground shaders: a shader binary is empty.\n");
		compiler->Release();
		return;
	}
	compiler->Release();

	std::vector<LLGI::DataStructure> dataVs;
	std::vector<LLGI::DataStructure> dataDepthVs;
	std::vector<LLGI::DataStructure> dataPs;
	std::vector<LLGI::DataStructure> dataDepthPs;
	for (auto& binary : resultVs.Binary)
	{
		LLGI::DataStructure data;
		data.Data = binary.data();
		data.Size = static_cast<int32_t>(binary.size());
		dataVs.push_back(data);
	}
	for (auto& binary : resultDepthVs.Binary)
	{
		LLGI::DataStructure data;
		data.Data = binary.data();
		data.Size = static_cast<int32_t>(binary.size());
		dataDepthVs.push_back(data);
	}
	for (auto& binary : resultPs.Binary)
	{
		LLGI::DataStructure data;
		data.Data = binary.data();
		data.Size = static_cast<int32_t>(binary.size());
		dataPs.push_back(data);
	}
	for (auto& binary : resultDepthPs.Binary)
	{
		LLGI::DataStructure data;
		data.Data = binary.data();
		data.Size = static_cast<int32_t>(binary.size());
		dataDepthPs.push_back(data);
	}

	groundShader_vs_ = graphics_->CreateShader(dataVs.data(), static_cast<int32_t>(dataVs.size()));
	groundDepthShader_vs_ = graphics_->CreateShader(dataDepthVs.data(), static_cast<int32_t>(dataDepthVs.size()));
	groundShader_ps_ = graphics_->CreateShader(dataPs.data(), static_cast<int32_t>(dataPs.size()));
	groundDepthShader_ps_ = graphics_->CreateShader(dataDepthPs.data(), static_cast<int32_t>(dataDepthPs.size()));
}

void EffectPlatformLLGI::CreateGroundResources()
{
	CreateGroundShaders();
	if (groundShader_vs_ == nullptr || groundShader_ps_ == nullptr || groundDepthShader_vs_ == nullptr || groundDepthShader_ps_ == nullptr)
	{
		return;
	}

	groundVb_ = graphics_->CreateBuffer(LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite, sizeof(GroundPlaneVertex) * 4);
	groundIb_ = graphics_->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, sizeof(uint16_t) * 6);
	if (groundVb_ == nullptr || groundIb_ == nullptr)
	{
		return;
	}

	auto ib = static_cast<uint16_t*>(groundIb_->Lock());
	const auto indices = CreateGroundPlaneIndices();
	for (int32_t i = 0; i < static_cast<int32_t>(indices.size()); i++)
	{
		ib[i] = indices[i];
	}
	groundIb_->Unlock();

	LLGI::RenderTextureInitializationParameter depthColorParam;
	depthColorParam.Size = LLGI::Vec2I(initParam_.WindowSize[0], initParam_.WindowSize[1]);
	depthColorParam.Format = LLGI::TextureFormatType::R16G16B16A16_FLOAT;
	groundDepthColorBuffer_ = graphics_->CreateRenderTexture(depthColorParam);
	if (groundDepthColorBuffer_ == nullptr)
	{
		return;
	}

	LLGI::TextureParameter depthParam;
	depthParam.Size = LLGI::Vec3I(initParam_.WindowSize[0], initParam_.WindowSize[1], 1);
	depthParam.Format = LLGI::TextureFormatType::D32;
	groundDepthBuffer_ = graphics_->CreateTexture(depthParam);
	groundDepthRenderPass_ = graphics_->CreateRenderPass((LLGI::Texture**)&groundDepthColorBuffer_, 1, groundDepthBuffer_);
	if (groundDepthBuffer_ == nullptr || groundDepthRenderPass_ == nullptr)
	{
		return;
	}
	groundDepthRenderPass_->SetIsColorCleared(true);
	groundDepthRenderPass_->SetIsDepthCleared(true);

	groundRppip_ = graphics_->CreateRenderPassPipelineState(renderPass_);
	groundDepthRppip_ = graphics_->CreateRenderPassPipelineState(groundDepthRenderPass_);
	if (groundRppip_ == nullptr || groundDepthRppip_ == nullptr)
	{
		return;
	}

	groundPip_ = graphics_->CreatePiplineState();
	if (groundPip_ == nullptr)
	{
		return;
	}
	groundPip_->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT;
	groundPip_->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
	groundPip_->VertexLayoutNames[0] = "POSITION";
	groundPip_->VertexLayoutNames[1] = "UV";
	groundPip_->VertexLayoutCount = 2;
	groundPip_->IsDepthTestEnabled = true;
	groundPip_->IsDepthWriteEnabled = true;
	groundPip_->DepthFunc = LLGI::DepthFuncType::Less;
	groundPip_->Culling = LLGI::CullingMode::DoubleSide;
	groundPip_->SetShader(LLGI::ShaderStageType::Vertex, groundShader_vs_);
	groundPip_->SetShader(LLGI::ShaderStageType::Pixel, groundShader_ps_);
	groundPip_->SetRenderPassPipelineState(groundRppip_);
	if (!groundPip_->Compile())
	{
		ES_SAFE_RELEASE(groundPip_);
		return;
	}

	groundDepthPip_ = graphics_->CreatePiplineState();
	if (groundDepthPip_ == nullptr)
	{
		return;
	}
	groundDepthPip_->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32A32_FLOAT;
	groundDepthPip_->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
	groundDepthPip_->VertexLayoutNames[0] = "POSITION";
	groundDepthPip_->VertexLayoutNames[1] = "UV";
	groundDepthPip_->VertexLayoutCount = 2;
	groundDepthPip_->IsDepthTestEnabled = true;
	groundDepthPip_->IsDepthWriteEnabled = true;
	groundDepthPip_->DepthFunc = LLGI::DepthFuncType::Less;
	groundDepthPip_->Culling = LLGI::CullingMode::DoubleSide;
	groundDepthPip_->SetShader(LLGI::ShaderStageType::Vertex, groundDepthShader_vs_);
	groundDepthPip_->SetShader(LLGI::ShaderStageType::Pixel, groundDepthShader_ps_);
	groundDepthPip_->SetRenderPassPipelineState(groundDepthRppip_);
	if (!groundDepthPip_->Compile())
	{
		ES_SAFE_RELEASE(groundDepthPip_);
		return;
	}
}

void EffectPlatformLLGI::UpdateGroundVertexBuffer()
{
	auto vb = static_cast<GroundPlaneVertex*>(groundVb_->Lock());
	const auto vertices = CreateGroundPlaneVertices();
	for (int32_t i = 0; i < static_cast<int32_t>(vertices.size()); i++)
	{
		vb[i] = vertices[i];
	}
	groundVb_->Unlock();
}

void EffectPlatformLLGI::DrawGround()
{
	if (groundPip_ == nullptr || groundVb_ == nullptr || groundIb_ == nullptr)
	{
		return;
	}

	UpdateGroundVertexBuffer();
	commandList_->SetVertexBuffer(groundVb_, sizeof(GroundPlaneVertex), 0);
	commandList_->SetIndexBuffer(groundIb_, 2);
	commandList_->SetPipelineState(groundPip_);
	commandList_->Draw(2);
}

void EffectPlatformLLGI::DrawGroundDepthTexture()
{
	if (groundDepthPip_ == nullptr || groundVb_ == nullptr || groundIb_ == nullptr)
	{
		return;
	}

	commandList_->SetVertexBuffer(groundVb_, sizeof(GroundPlaneVertex), 0);
	commandList_->SetIndexBuffer(groundIb_, 2);
	commandList_->SetPipelineState(groundDepthPip_);
	commandList_->Draw(2);
}

Effekseer::Backend::TextureRef EffectPlatformLLGI::CreateEffekseerTexture(LLGI::Texture* texture)
{
	auto graphicsDevice = GetRenderer()->GetGraphicsDevice().DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	if (graphicsDevice == nullptr)
	{
		return nullptr;
	}

	return graphicsDevice->CreateTexture(texture);
}

void EffectPlatformLLGI::CreateResources()
{
	CreateShaders();

	LLGI::RenderTextureInitializationParameter renderParam;
	renderParam.Size = LLGI::Vec2I(initParam_.WindowSize[0], initParam_.WindowSize[1]);
	LLGI::TextureParameter depthParam;
	depthParam.Size = LLGI::Vec3I(initParam_.WindowSize[0], initParam_.WindowSize[1], 1);
	depthParam.Format = LLGI::TextureFormatType::D32;
	colorBuffer_ = graphics_->CreateRenderTexture(renderParam);
	depthBuffer_ = graphics_->CreateTexture(depthParam);
	renderPass_ = graphics_->CreateRenderPass((LLGI::Texture**)&colorBuffer_, 1, depthBuffer_);
	renderPass_->SetIsColorCleared(true);
	renderPass_->SetIsDepthCleared(true);

	vb_ = graphics_->CreateBuffer(LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite, sizeof(SimpleVertex) * 4);
	ib_ = graphics_->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, 2 * 6);
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

	rppip_ = graphics_->CreateRenderPassPipelineState(renderPass_);

	{
		pip_ = graphics_->CreatePiplineState();
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

	CreateGroundResources();
}

EffectPlatformLLGI::EffectPlatformLLGI(LLGI::DeviceType deviceType)
	: deviceType_(deviceType)
{
}

EffectPlatformLLGI ::~EffectPlatformLLGI()
{
}

void EffectPlatformLLGI::InitializeWindow()
{
#if defined(__EMSCRIPTEN__)
	llgiWindow_ = LLGI::CreateWindow("Effekseer WebGPU Browser Test", LLGI::Vec2I(initParam_.WindowSize[0], initParam_.WindowSize[1]));
	if (llgiWindow_ == nullptr)
	{
		throw "Failed to create an window.";
	}
#else
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindow_ = glfwCreateWindow(initParam_.WindowSize[0], initParam_.WindowSize[1], "Example glfw", nullptr, nullptr);

	if (glfwWindow_ == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	llgiWindow_ = new LLGIWindow(glfwWindow_);
#endif

	LLGI::PlatformParameter platformParam;
	platformParam.Device = deviceType_;
	platformParam.WaitVSync = initParam_.VSync;
	platform_ = LLGI::CreatePlatform(platformParam, llgiWindow_);
	graphics_ = platform_->CreateGraphics();
	sfMemoryPool_ = graphics_->CreateSingleFrameMemoryPool(1024 * 1024, 128);
	commandListPool_ = std::make_shared<LLGI::CommandListPool>(graphics_, sfMemoryPool_, 3);
}

void EffectPlatformLLGI::Present()
{
	graphics_->Execute(commandList_.get());
	platform_->Present();
}

bool EffectPlatformLLGI::DoEvent()
{
	if (!platform_->NewFrame())
		return false;

#if !defined(__EMSCRIPTEN__)
	glfwPollEvents();
#endif

	sfMemoryPool_->NewFrame();
	commandList_ = LLGI::CreateSharedPtr(commandListPool_->Get(true));

	return true;
}

void EffectPlatformLLGI::PreDestroyDevice()
{
	graphics_->WaitFinish();

	// Vulkan requires to release before destroy devices
	commandListEfk_.Reset();
	sfMemoryPoolEfk_.Reset();
}

void EffectPlatformLLGI::DestroyDevice()
{
	LLGI::SafeRelease(sfMemoryPool_);
	commandListPool_.reset();
	LLGI::SafeRelease(graphics_);
	LLGI::SafeRelease(platform_);

	ES_SAFE_DELETE(llgiWindow_);

#if !defined(__EMSCRIPTEN__)
	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}
#endif

	ES_SAFE_RELEASE(renderPass_);
	ES_SAFE_RELEASE(colorBuffer_);
	ES_SAFE_RELEASE(depthBuffer_);
	ES_SAFE_RELEASE(groundDepthRenderPass_);
	ES_SAFE_RELEASE(groundDepthColorBuffer_);
	ES_SAFE_RELEASE(groundDepthBuffer_);
	ES_SAFE_RELEASE(shader_vs_);
	ES_SAFE_RELEASE(shader_ps_);
	ES_SAFE_RELEASE(groundShader_vs_);
	ES_SAFE_RELEASE(groundShader_ps_);
	ES_SAFE_RELEASE(groundDepthShader_vs_);
	ES_SAFE_RELEASE(groundDepthShader_ps_);
	ES_SAFE_RELEASE(vb_);
	ES_SAFE_RELEASE(ib_);
	ES_SAFE_RELEASE(rppip_);
	ES_SAFE_RELEASE(pip_);
	ES_SAFE_RELEASE(screenPip_);
	ES_SAFE_RELEASE(groundPip_);
	ES_SAFE_RELEASE(groundDepthPip_);
	ES_SAFE_RELEASE(groundRppip_);
	ES_SAFE_RELEASE(groundDepthRppip_);
	ES_SAFE_RELEASE(groundVb_);
	ES_SAFE_RELEASE(groundIb_);
	ES_SAFE_RELEASE(checkTexture_);

	groundDepthTextureForEffekseer_.Reset();
	usesGpuGroundDepth_ = false;
	commandList_.reset();
}

void EffectPlatformLLGI::BeginCompute()
{
	if (!isCommandListBegun_)
	{
		commandList_->Begin();
		sfMemoryPoolEfk_->NewFrame();
		isCommandListBegun_ = true;
	}
}

void EffectPlatformLLGI::EndCompute()
{
}

void EffectPlatformLLGI::BeginRendering()
{
	LLGI::Color8 color;
	color.R = usesGpuGroundDepth_ ? 22 : 64;
	color.G = usesGpuGroundDepth_ ? 34 : 64;
	color.B = usesGpuGroundDepth_ ? 48 : 64;
	color.A = 255;
	if (!isCommandListBegun_)
	{
		commandList_->Begin();
		sfMemoryPoolEfk_->NewFrame();
		isCommandListBegun_ = true;
	}

	if (usesGpuGroundDepth_)
	{
		UpdateGroundVertexBuffer();

		LLGI::Color8 depthClearColor;
		depthClearColor.R = 255;
		depthClearColor.G = 255;
		depthClearColor.B = 255;
		depthClearColor.A = 255;
		groundDepthRenderPass_->SetClearColor(depthClearColor);
		groundDepthRenderPass_->SetIsColorCleared(true);
		groundDepthRenderPass_->SetIsDepthCleared(true);
		commandList_->BeginRenderPass(groundDepthRenderPass_);
		DrawGroundDepthTexture();
		commandList_->EndRenderPass();
	}

	renderPass_->SetClearColor(color);
	renderPass_->SetIsColorCleared(true);
	renderPass_->SetIsDepthCleared(true);
	commandList_->BeginRenderPass(renderPass_);

	if (usesGpuGroundDepth_)
	{
		DrawGround();
	}
	else
	{
		commandList_->SetVertexBuffer(vb_, sizeof(SimpleVertex), 0);
		commandList_->SetIndexBuffer(ib_, 2);
		commandList_->SetPipelineState(pip_);
		commandList_->SetTexture(checkTexture_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0);
		commandList_->Draw(2);
	}
}

void EffectPlatformLLGI::EndRendering()
{
	commandList_->EndRenderPass();

	auto currentScreen = platform_->GetCurrentScreen(LLGI::Color8(), true);

	if (screenPip_ == nullptr)
	{
		auto rpip = graphics_->CreateRenderPassPipelineState(currentScreen);
		screenFormat_ = rpip->Key.RenderTargetFormats.at(0);

		{
			screenPip_ = graphics_->CreatePiplineState();
			screenPip_->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
			screenPip_->VertexLayouts[1] = LLGI::VertexLayoutFormat::R32G32_FLOAT;
			screenPip_->VertexLayouts[2] = LLGI::VertexLayoutFormat::R8G8B8A8_UNORM;
			screenPip_->VertexLayoutNames[0] = "POSITION";
			screenPip_->VertexLayoutNames[1] = "UV";
			screenPip_->VertexLayoutNames[2] = "COLOR";
			screenPip_->VertexLayoutCount = 3;
			screenPip_->IsDepthTestEnabled = false;
			screenPip_->IsDepthWriteEnabled = false;
			screenPip_->Culling = LLGI::CullingMode::DoubleSide;
			screenPip_->SetShader(LLGI::ShaderStageType::Vertex, shader_vs_);
			screenPip_->SetShader(LLGI::ShaderStageType::Pixel, shader_ps_);
			screenPip_->SetRenderPassPipelineState(rpip);
			screenPip_->Compile();
		}

		ES_SAFE_RELEASE(rpip);
	}

	commandList_->BeginRenderPass(currentScreen);
	commandList_->SetVertexBuffer(vb_, sizeof(SimpleVertex), 0);
	commandList_->SetIndexBuffer(ib_, 2);
	commandList_->SetPipelineState(screenPip_);
	commandList_->SetTexture(colorBuffer_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0);
	commandList_->Draw(2);
	commandList_->EndRenderPass();

	commandList_->End();
	isCommandListBegun_ = false;
}

void EffectPlatformLLGI::ResetBackgroundPattern()
{
	usesGpuGroundDepth_ = false;
	EffectPlatform::ResetBackgroundPattern();
}

void EffectPlatformLLGI::GenerateGroundDepth()
{
	isGroundDepthEnabled_ = true;
	usesGpuGroundDepth_ = false;

	if (groundDepthColorBuffer_ == nullptr || groundDepthRenderPass_ == nullptr || groundPip_ == nullptr || groundDepthPip_ == nullptr)
	{
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		groundDepthTextureForEffekseer_ = CreateEffekseerTexture(groundDepthColorBuffer_);
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		isGroundDepthEnabled_ = false;
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	usesGpuGroundDepth_ = true;

	GetRenderer()->SetDepth(groundDepthTextureForEffekseer_, CreateGroundDepthReconstructionParameter());
}

std::vector<uint8_t> EffectPlatformLLGI::CaptureScreenPixels()
{
	commandList_->WaitUntilCompleted();

	auto texture = colorBuffer_;
	if (texture == nullptr)
	{
		return {};
	}
	auto data = graphics_->CaptureRenderTarget(texture);

	if (texture->GetFormat() == LLGI::TextureFormatType::B8G8R8A8_UNORM)
	{
		for (size_t i = 0; i < data.size(); i += 4)
		{
			std::swap(data[i + 0], data[i + 2]);
		}
	}

	return data;
}

bool EffectPlatformLLGI::TakeScreenshot(const char* path)
{
#if defined(__EMSCRIPTEN__)
	(void)path;
	return false;
#else
	auto data = CaptureScreenPixels();
	if (data.empty())
	{
		return false;
	}
	for (size_t i = 3; i < data.size(); i += 4)
	{
		data[i] = 255;
	}

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
#endif
}
