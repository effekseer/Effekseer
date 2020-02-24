
#define NOMINMAX
#include "../../Dev/Cpp/Effekseer/Effekseer.h"

#include "../../Dev/Cpp/EffekseerRendererVulkan/EffekseerRenderer/EffekseerRendererVulkan.Renderer.h"

#include "../../Dev/Cpp/EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h"
#include "../../Dev/Cpp/EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

#include "../../Dev/Cpp/3rdParty/LLGI/src/Utils/LLGI.CommandListPool.h"
#include "../../Dev/Cpp/3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../../Dev/Cpp/3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../Dev/Cpp/3rdParty/LLGI/src/Vulkan/LLGI.TextureVulkan.h"
#include "../../Dev/Cpp/3rdParty/LLGI/src/LLGI.Platform.h"

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Dev/Cpp/3rdParty/stb/stb_image.h"
#include "../../Dev/Cpp/3rdParty/stb/stb_image_write.h"

#include "VulkanApp.h"

// Information for associating Vulkan framebuffer with LLGI::RenerPass.
// Used to efficiently reuse resources.
struct RenderPassCacheItem
{
    std::shared_ptr<::LLGI::RenderPass> llgiRenderPass = nullptr;
    VkImage renderTargetImage = VK_NULL_HANDLE;
    VkImage depthBufferImage = VK_NULL_HANDLE;
};

// Rendering interfaces. (for Vulkan objects wrapping)
::LLGI::GraphicsVulkan* g_llgiGraphics = nullptr;
::LLGI::SingleFrameMemoryPool* g_singleFrameMemoryPool = nullptr;
::LLGI::CommandListVulkan* g_llgiCommandList = nullptr;
std::vector<RenderPassCacheItem> g_llgiRenderPassCache;

// Effekseer interfaces.
::EffekseerRendererLLGI::RendererImplemented* g_renderer = nullptr;
::EffekseerRendererLLGI::CommandList* g_efkCommandList = nullptr;
::Effekseer::Manager* g_manager = nullptr;
::Effekseer::Effect* g_effect = nullptr;
::Effekseer::Handle g_handle = -1;
::Effekseer::Vector3D g_position;

VulkanApp g_app;

void InitRenderer()
{
	int maxDrawcall = 128;
	auto renderer = ::EffekseerRendererVulkan::Create(
		g_app.getPhysicalDevice(),
		g_app.getDevice(),
		g_app.getGraphicsQueue(),
		g_app.getCommandPool(),
		g_app.getSwapBufferCount(),
		1000);
	g_renderer = dynamic_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	g_llgiGraphics = dynamic_cast<LLGI::GraphicsVulkan*>(g_renderer->GetGraphics());

	g_singleFrameMemoryPool = g_llgiGraphics->CreateSingleFrameMemoryPool(1024 * 1024, maxDrawcall);

	g_llgiCommandList = new LLGI::CommandListVulkan();
	if (!g_llgiCommandList->Initialize(g_llgiGraphics, maxDrawcall, LLGI::CommandListPreCondition::Standalone)) {
		throw "Failed LLGI::CommandListVulkan::Initialize";
	}

	g_efkCommandList = new EffekseerRendererLLGI::CommandList(g_llgiGraphics, g_llgiCommandList, g_singleFrameMemoryPool);
}

void CleanupRenderer()
{
	g_llgiRenderPassCache.clear();

	if (g_efkCommandList) {
		g_efkCommandList->Release();
		g_efkCommandList = nullptr;
	}

	if (g_llgiCommandList) {
		g_llgiCommandList->Release();
		g_llgiCommandList = nullptr;
	}

	if (g_renderer) {
		g_renderer->Destroy();
		g_renderer = nullptr;
	}

	if (g_singleFrameMemoryPool) {
		g_singleFrameMemoryPool->Release();
		g_singleFrameMemoryPool = nullptr;
	}
}

void MainLoop()
{
    g_llgiRenderPassCache.resize(g_app.getSwapBufferCount());

    while (g_app.pollEvents())
	{
		// Move effect.
		g_manager->AddLocation(g_handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

		// Update effect.
		g_manager->Update();

		// Rendering process.
        CurrentFrameInfo info;
        if (g_app.beginFrame(&info))
        {
			// Prepare the frame.
			// Create LLGI::RenderPass to wrap the back buffer.
			// Reuse from cache as needed.
            std::shared_ptr<LLGI::RenderPass> renderPass = nullptr;
            if (g_llgiRenderPassCache[info.currentFrameIndex].renderTargetImage == info.renderTargetImage &&
                g_llgiRenderPassCache[info.currentFrameIndex].depthBufferImage == info.depthBufferImage) {
                renderPass = g_llgiRenderPassCache[info.currentFrameIndex].llgiRenderPass;
            }
            else {
                // In this sample, only the back buffer is drawn, so it is called "TextureType::Screen".
				// Specify "TextureType::Render" if you want to draw to a render target.
                auto textureType = LLGI::TextureType::Screen;

                auto llgiRenderTarget = new LLGI::TextureVulkan();
                llgiRenderTarget->InitializeFromExternal(textureType, info.renderTargetImage, info.renderTargetImageView, info.renderTargetFormat, LLGI::Vec2I(info.frameWidth, info.frameHeight));
                auto llgiDepthBuffer = new LLGI::TextureVulkan();
                llgiDepthBuffer->InitializeFromExternal(LLGI::TextureType::Depth, info.depthBufferImage, info.depthBufferImageView, info.depthBufferFormat, LLGI::Vec2I(info.frameWidth, info.frameHeight));

                LLGI::Texture* llgiRenderTargets[] = { llgiRenderTarget };
                LLGI::RenderPass* llgiRenderPass = g_llgiGraphics->CreateRenderPass((const LLGI::Texture**)llgiRenderTargets, 1, llgiDepthBuffer);
                llgiRenderPass->SetIsColorCleared(false);
                llgiRenderPass->SetIsDepthCleared(false);

                renderPass = std::shared_ptr<LLGI::RenderPass>(llgiRenderPass, [](LLGI::RenderPass* ptr) { ptr->Release(); });

                llgiRenderTarget->Release();
                llgiDepthBuffer->Release();

                g_llgiRenderPassCache[info.currentFrameIndex].llgiRenderPass = renderPass;
                g_llgiRenderPassCache[info.currentFrameIndex].renderTargetImage = info.renderTargetImage;
                g_llgiRenderPassCache[info.currentFrameIndex].depthBufferImage = info.depthBufferImage;
            }

            g_singleFrameMemoryPool->NewFrame();

            LLGI::RenderPassPipelineState* llgtRenderPassPipelineState = g_llgiGraphics->CreateRenderPassPipelineState(renderPass.get());
            g_renderer->SetRenderPassPipelineState(llgtRenderPassPipelineState);

            // Begin rendering.
            g_llgiCommandList->BeginExternal(info.recordingCommandBuffer);
            g_renderer->SetCommandList(g_efkCommandList);
            g_renderer->BeginRendering();
            g_llgiCommandList->BeginRenderPass(renderPass.get());

            // Draw effects.
            g_manager->Draw();

			// End rendering.
			g_renderer->EndRendering();
            g_llgiCommandList->EndRenderPass();
            g_llgiCommandList->EndExternal();

			// End frame.
            llgtRenderPassPipelineState->Release();
            g_app.endFrame();
        }

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main(int argc, char **argv)
{
    g_app.init();

	// Creating an rendering instance.
	InitRenderer();

    // Creating an effect management instance.
    g_manager = ::Effekseer::Manager::Create(2000);

    // Set the rendering function from the drawing instance.
    g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
    g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
    g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
    g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
    g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

    // Set the resource reading function from the rendering instance.
    g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
    g_manager->SetModelLoader(g_renderer->CreateModelLoader());
    g_manager->SetMaterialLoader(g_renderer->CreateMaterialLoader());

    // View position.
    g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

    // Set Projection matrix.
    g_renderer->SetProjectionMatrix(
        ::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)g_app.getBackbufferWidth() / (float)g_app.getBackbufferHeight(), 1.0f, 50.0f));

    // Set Camera matrix.
    g_renderer->SetCameraMatrix(
        ::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

    // Load effect.
    g_effect = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_VULKAN_ASSETS_DIR_U16 "Laser01.efk");

    // Play effect.
    g_handle = g_manager->Play(g_effect, 0, 0, 0);

    MainLoop();

    // Stop effect.
    g_manager->StopEffect(g_handle);

    // Destory effect.
    ES_SAFE_RELEASE(g_effect);

    // Destory effect management instance first.
    g_manager->Destroy();

	// Destroy an rendering instance.
	CleanupRenderer();

    g_app.cleanup();

    return 0;
}

