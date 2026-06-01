#include "EffectPlatformLLGI.h"
#include "../3rdParty/LLGI/src/LLGI.Buffer.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"
#include "../3rdParty/LLGI/src/LLGI.Graphics.h"
#include "../3rdParty/LLGI/src/LLGI.PipelineState.h"
#include "../3rdParty/LLGI/src/LLGI.Platform.h"
#include "../3rdParty/LLGI/src/LLGI.Shader.h"
#include "../3rdParty/LLGI/src/LLGI.Texture.h"

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI/GraphicsDevice.h"
#include "../../3rdParty/stb/stb_image_write.h"
#include <EffekseerToolRuntime/GroundRendering.h>
#include <EffekseerToolRuntime/LLGIShaderCompiler.h>

#include <cassert>
#include <cstdio>

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
	Effekseer::ToolRuntime::GroundShaderBackend shaderBackend = Effekseer::ToolRuntime::GroundShaderBackend::DirectX12;

	switch (deviceType_)
	{
	case LLGI::DeviceType::DirectX12:
		shaderBackend = Effekseer::ToolRuntime::GroundShaderBackend::DirectX12;
		break;
	case LLGI::DeviceType::Vulkan:
		shaderBackend = Effekseer::ToolRuntime::GroundShaderBackend::Vulkan;
		break;
	case LLGI::DeviceType::Metal:
		shaderBackend = Effekseer::ToolRuntime::GroundShaderBackend::Metal;
		break;
	case LLGI::DeviceType::WebGPU:
		shaderBackend = Effekseer::ToolRuntime::GroundShaderBackend::WebGPU;
		break;
	default:
		assert(false);
		return;
	}
	const auto& shaderCode = Effekseer::ToolRuntime::GetGroundShaderCode(shaderBackend);
	const auto vsCode = shaderCode.Vertex;
	const auto depthVsCode = shaderCode.DepthVertex != nullptr ? shaderCode.DepthVertex : shaderCode.Vertex;
	const auto psCode = shaderCode.Pixel;
	const auto depthPsCode = shaderCode.DepthPixel;

	Effekseer::ToolRuntime::LLGIShaderCompileInput inputs[4];
	inputs[0].Name = "VS";
	inputs[0].Code = vsCode;
	inputs[0].Stage = LLGI::ShaderStageType::Vertex;
	inputs[1].Name = "Depth VS";
	inputs[1].Code = depthVsCode;
	inputs[1].Stage = LLGI::ShaderStageType::Vertex;
	inputs[2].Name = "PS";
	inputs[2].Code = psCode;
	inputs[2].Stage = LLGI::ShaderStageType::Pixel;
	inputs[3].Name = "Depth PS";
	inputs[3].Code = depthPsCode;
	inputs[3].Stage = LLGI::ShaderStageType::Pixel;

	Effekseer::ToolRuntime::LLGICompiledShader compiledShaders[4];
	std::string message;
	if (!Effekseer::ToolRuntime::CompileLLGIShaders(deviceType_, inputs, 4, compiledShaders, message))
	{
		printf("Failed to compile ground shaders.\n%s\n", message.c_str());
		return;
	}

	auto dataVs = compiledShaders[0].GetDataStructures();
	auto dataDepthVs = compiledShaders[1].GetDataStructures();
	auto dataPs = compiledShaders[2].GetDataStructures();
	auto dataDepthPs = compiledShaders[3].GetDataStructures();

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

void EffectPlatformLLGI::DrawGround(Effekseer::ToolRuntime::GroundRenderPass pass)
{
	auto pipeline = pass == Effekseer::ToolRuntime::GroundRenderPass::Depth ? groundDepthPip_ : groundPip_;
	if (pipeline == nullptr || groundVb_ == nullptr || groundIb_ == nullptr)
	{
		return;
	}

	if (pass == Effekseer::ToolRuntime::GroundRenderPass::Color)
	{
		UpdateGroundVertexBuffer();
	}

	commandList_->SetVertexBuffer(groundVb_, sizeof(GroundPlaneVertex), 0);
	commandList_->SetIndexBuffer(groundIb_, 2);
	commandList_->SetPipelineState(pipeline);
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
		DrawGround(Effekseer::ToolRuntime::GroundRenderPass::Depth);
		commandList_->EndRenderPass();
	}

	renderPass_->SetClearColor(color);
	renderPass_->SetIsColorCleared(true);
	renderPass_->SetIsDepthCleared(true);
	commandList_->BeginRenderPass(renderPass_);

	if (usesGpuGroundDepth_)
	{
		DrawGround(Effekseer::ToolRuntime::GroundRenderPass::Color);
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
