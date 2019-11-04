#include "EffectPlatformDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.CommandListDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../../3rdParty/LLGI/src/DX12/LLGI.PlatformDX12.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"

#include "../../3rdParty/stb/stb_image_write.h"

void EffectPlatformDX12::CreateCheckedTexture() {}

EffekseerRenderer::Renderer* EffectPlatformDX12::CreateRenderer()
{
	auto g = static_cast<LLGI::GraphicsDX12*>(graphics_);
	auto p = static_cast<LLGI::PlatformDX12*>(platform_);

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto renderer =
		EffekseerRendererDX12::Create(g->GetDevice(), g->GetCommandQueue(), g->GetSwapBufferCount(), &format, 1, false, false, 10000);

	sfMemoryPoolEfk_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(renderer);
	commandListEfk_ = EffekseerRendererDX12::CreateCommandList(renderer, sfMemoryPoolEfk_);

	return renderer;
}

EffectPlatformDX12::~EffectPlatformDX12() {}

void EffectPlatformDX12::InitializeDevice(const EffectPlatformInitializingParameter& param) { CreateCheckedTexture(); }

void EffectPlatformDX12::DestroyDevice()
{
	EffectPlatformLLGI::DestroyDevice();
	ES_SAFE_RELEASE(commandListEfk_);
	ES_SAFE_RELEASE(sfMemoryPoolEfk_);
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

	commandList_->Begin();
	commandList_->BeginRenderPass(platform_->GetCurrentScreen(color, true));

	EffekseerRendererDX12::BeginCommandList(commandListEfk_, cl->GetCommandList());
	GetRenderer()->SetCommandList(commandListEfk_);
}

void EffectPlatformDX12::EndRendering()
{
	GetRenderer()->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(commandListEfk_);

	commandList_->EndRenderPass();
	commandList_->End();
}
