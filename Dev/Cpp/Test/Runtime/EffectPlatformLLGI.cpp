#include "EffectPlatformLLGI.h"
#include "../3rdParty/LLGI/src/LLGI.Buffer.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"
#include "../3rdParty/LLGI/src/LLGI.Graphics.h"
#include "../3rdParty/LLGI/src/LLGI.PipelineState.h"
#include "../3rdParty/LLGI/src/LLGI.Platform.h"
#include "../3rdParty/LLGI/src/LLGI.Shader.h"
#include "../3rdParty/LLGI/src/LLGI.Texture.h"

#include "../../3rdParty/stb/stb_image_write.h"

void EffectPlatformLLGI::CreateCheckedTexture()
{
	LLGI::TextureInitializationParameter param;
	param.Size.X = initParam_.WindowSize[0];
	param.Size.Y = initParam_.WindowSize[1];
	checkTexture_ = graphics_->CreateTexture(param);

	auto b = (uint8_t*)checkTexture_->Lock();

	memcpy(b, checkeredPattern_.data(), param.Size.X * param.Size.Y * 4);

	checkTexture_->Unlock();
}

void EffectPlatformLLGI::CreateResources()
{
	CreateShaders();

	LLGI::RenderTextureInitializationParameter renderParam;
	renderParam.Size = LLGI::Vec2I(initParam_.WindowSize[0], initParam_.WindowSize[1]);
	LLGI::DepthTextureInitializationParameter depthParam;
	depthParam.Size = LLGI::Vec2I(initParam_.WindowSize[0], initParam_.WindowSize[1]);
	colorBuffer_ = graphics_->CreateRenderTexture(renderParam);
	depthBuffer_ = graphics_->CreateDepthTexture(depthParam);
	renderPass_ = graphics_->CreateRenderPass((LLGI::Texture**)&colorBuffer_, 1, depthBuffer_);

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

	LLGI::PlatformParameter platformParam;
	platformParam.Device = deviceType_;
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

	glfwPollEvents();

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

	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}

	ES_SAFE_RELEASE(renderPass_);
	ES_SAFE_RELEASE(colorBuffer_);
	ES_SAFE_RELEASE(depthBuffer_);
	ES_SAFE_RELEASE(shader_vs_);
	ES_SAFE_RELEASE(shader_ps_);
	ES_SAFE_RELEASE(vb_);
	ES_SAFE_RELEASE(ib_);
	ES_SAFE_RELEASE(rppip_);
	ES_SAFE_RELEASE(pip_);
	ES_SAFE_RELEASE(screenPip_);
	ES_SAFE_RELEASE(checkTexture_);

	commandList_.reset();
}

void EffectPlatformLLGI::BeginRendering()
{
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
	commandList_->SetIndexBuffer(ib_, 2);
	commandList_->SetPipelineState(pip_);
	commandList_->SetTexture(
		checkTexture_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
	commandList_->Draw(2);

	sfMemoryPoolEfk_->NewFrame();
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
	commandList_->SetTexture(
		colorBuffer_, LLGI::TextureWrapMode::Repeat, LLGI::TextureMinMagFilter::Nearest, 0, LLGI::ShaderStageType::Pixel);
	commandList_->Draw(2);
	commandList_->EndRenderPass();

	commandList_->End();
}

bool EffectPlatformLLGI::TakeScreenshot(const char* path)
{
	commandList_->WaitUntilCompleted();

	LLGI::Color8 color;
	color.R = 255;
	color.G = 255;
	color.B = 255;
	color.A = 255;

	auto texture = platform_->GetCurrentScreen(color, true)->GetRenderTexture(0);
	auto data = graphics_->CaptureRenderTarget(texture);

	if (screenFormat_ == LLGI::TextureFormatType::B8G8R8A8_UNORM)
	{
		for (size_t i = 0; i < data.size(); i += 4)
		{
			std::swap(data[i + 0], data[i + 2]);
		}
	}

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
}
