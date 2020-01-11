
#define NOMINMAX
#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

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

#include "app.h"

VulkanApp app;

struct RenderPassCacheItem
{
    std::shared_ptr<LLGI::RenderPass> llgiRenderPass = nullptr;
    VkImage renderTargetImage = VK_NULL_HANDLE;
    VkImage depthBufferImage = VK_NULL_HANDLE;
};

::EffekseerRendererLLGI::RendererImplemented* g_renderer;
LLGI::GraphicsVulkan* m_llgiGraphics;
LLGI::SingleFrameMemoryPool* m_singleFrameMemoryPool;
LLGI::CommandListVulkan* m_llgiCommandList;
EffekseerRendererLLGI::CommandList* m_efkCommandList;
std::vector<RenderPassCacheItem> m_llgiRenderPassCache;

static ::Effekseer::Manager*				g_manager = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;

void MainLoop()
{
    m_llgiRenderPassCache.resize(app.getSwapBufferCount());

    for (;;)
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return;
            }
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {

                    // エフェクトの移動処理を行う
                    g_manager->AddLocation(g_handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

                    // エフェクトの更新処理を行う
                    g_manager->Update();

            VulkanApp::CurrentFrameInfo info;
            if (app.beginFrame(&info))
            {

                std::shared_ptr<LLGI::RenderPass> renderPass = nullptr;
                if (m_llgiRenderPassCache[info.currentFrameIndex].renderTargetImage == info.renderTargetImage &&
                    m_llgiRenderPassCache[info.currentFrameIndex].depthBufferImage == info.depthBufferImage) {
                    renderPass = m_llgiRenderPassCache[info.currentFrameIndex].llgiRenderPass;
                }
                else {
                    // このサンプルではバックバッファへのみの描画となるため Screen とする。レンダーターゲットへ描画したい場合は Render を指定する。
                    auto textureType = LLGI::TextureType::Screen;

                    auto llgiRenderTarget = new LLGI::TextureVulkan();
                    llgiRenderTarget->InitializeFromExternal(textureType, info.renderTargetImage, info.renderTargetImageView, info.renderTargetFormat, LLGI::Vec2I(info.frameWidth, info.frameHeight));
                    auto llgiDepthBuffer = new LLGI::TextureVulkan();
                    llgiDepthBuffer->InitializeFromExternal(LLGI::TextureType::Depth, info.depthBufferImage, info.depthBufferImageView, info.depthBufferFormat, LLGI::Vec2I(info.frameWidth, info.frameHeight));

                    LLGI::Texture* llgiRenderTargets[] = { llgiRenderTarget };
                    LLGI::RenderPass* llgiRenderPass = m_llgiGraphics->CreateRenderPass((const LLGI::Texture**)llgiRenderTargets, 1, llgiDepthBuffer);
                    llgiRenderPass->SetIsColorCleared(false);
                    llgiRenderPass->SetIsDepthCleared(false);

                    renderPass = std::shared_ptr<LLGI::RenderPass>(llgiRenderPass, [](LLGI::RenderPass* ptr) { ptr->Release(); });

                    llgiRenderTarget->Release();
                    llgiDepthBuffer->Release();

                    m_llgiRenderPassCache[info.currentFrameIndex].llgiRenderPass = renderPass;
                    m_llgiRenderPassCache[info.currentFrameIndex].renderTargetImage = info.renderTargetImage;
                    m_llgiRenderPassCache[info.currentFrameIndex].depthBufferImage = info.depthBufferImage;
                }

                m_singleFrameMemoryPool->NewFrame();

                LLGI::RenderPassPipelineState* llgtRenderPassPipelineState = m_llgiGraphics->CreateRenderPassPipelineState(renderPass.get());
                g_renderer->SetRenderPassPipelineState(llgtRenderPassPipelineState);

                // エフェクトの描画開始処理を行う。
                m_llgiCommandList->BeginExternal(info.recordingCommandBuffer);
                g_renderer->SetCommandList(m_efkCommandList);
                g_renderer->BeginRendering();

                m_llgiCommandList->BeginRenderPass(renderPass.get());

                // エフェクトの描画を行う。
                g_manager->Draw();

                // エフェクトの描画終了処理を行う。
                g_renderer->EndRendering();
                m_llgiCommandList->EndRenderPass();
                m_llgiCommandList->EndExternal();

                llgtRenderPassPipelineState->Release();



                app.endFrame();
            }



            ::Sleep(16);

        }
    }
}


int main(int argc, char **argv)
{
    app.init();

    int maxDrawcall = 128;
    auto renderer = ::EffekseerRendererVulkan::Create(
        app.getPhysicalDevice(),
        app.getDevice(),
        app.getGraphicsQueue(),
        app.getCommandPool(),
        app.getSwapBufferCount(),
        1000);
    g_renderer = dynamic_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
    m_llgiGraphics = dynamic_cast<LLGI::GraphicsVulkan*>(g_renderer->GetGraphics());

    m_singleFrameMemoryPool = m_llgiGraphics->CreateSingleFrameMemoryPool(1024 * 1024, maxDrawcall);

    m_llgiCommandList = new LLGI::CommandListVulkan();
    if (!m_llgiCommandList->Initialize(m_llgiGraphics, maxDrawcall, LLGI::CommandListPreCondition::Standalone)) {
        return 1;
    }

    m_efkCommandList = new EffekseerRendererLLGI::CommandList(m_llgiGraphics, m_llgiCommandList, m_singleFrameMemoryPool);




    //// 描画用インスタンスの生成
    //g_renderer = ::EffekseerRendererDX9::Renderer::Create(g_d3d_device, 2000);

    // エフェクト管理用インスタンスの生成
    g_manager = ::Effekseer::Manager::Create(2000);

    // 描画用インスタンスから描画機能を設定
    g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
    g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
    g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
    g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
    g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

    // 描画用インスタンスからテクスチャの読込機能を設定
    // 独自拡張可能、現在はファイルから読み込んでいる。
    g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
    g_manager->SetModelLoader(g_renderer->CreateModelLoader());

    //// 音再生用インスタンスの生成
    //g_sound = ::EffekseerSound::Sound::Create(g_xa2, 16, 16);

    //// 音再生用インスタンスから再生機能を指定
    //g_manager->SetSoundPlayer(g_sound->CreateSoundPlayer());

    //// 音再生用インスタンスからサウンドデータの読込機能を設定
    //// 独自拡張可能、現在はファイルから読み込んでいる。
    //g_manager->SetSoundLoader(g_sound->CreateSoundLoader());

    // 視点位置を確定
    g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

    // 投影行列を設定
    g_renderer->SetProjectionMatrix(
        ::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)app.getBackbufferWidth() / (float)app.getBackbufferHeight(), 1.0f, 50.0f));

    // カメラ行列を設定
    g_renderer->SetCameraMatrix(
        ::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

    // エフェクトの読込
    g_effect = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_VULKAN_ASSETS_DIR_U16 "Laser01.efk");

    // エフェクトの再生
    g_handle = g_manager->Play(g_effect, 0, 0, 0);

    MainLoop();

    // エフェクトの停止
    g_manager->StopEffect(g_handle);

    // エフェクトの破棄
    ES_SAFE_RELEASE(g_effect);

    // 先にエフェクト管理用インスタンスを破棄
    g_manager->Destroy();

    //// 次に音再生用インスタンスを破棄
    //g_sound->Destroy();

    // 次に描画用インスタンスを破棄
    //g_renderer->Destroy();

    //// XAudio2の解放
    //if (g_xa2_master != NULL)
    //{
    //    g_xa2_master->DestroyVoice();
    //    g_xa2_master = NULL;
    //}
    //ES_SAFE_RELEASE(g_xa2);

    //// DirectXの解放
    //ES_SAFE_RELEASE(g_d3d_device);
    //ES_SAFE_RELEASE(g_d3d);

    //// COMの終了処理
    //CoUninitialize();

    {
        m_llgiRenderPassCache.clear();

        if (m_efkCommandList) {
            m_efkCommandList->Release();
            m_efkCommandList = nullptr;
        }

        if (m_llgiCommandList) {
            m_llgiCommandList->Release();
            m_llgiCommandList = nullptr;
        }

        if (g_renderer) {
            g_renderer->Destroy();
            g_renderer = nullptr;
        }

        if (m_singleFrameMemoryPool) {
            m_singleFrameMemoryPool->Release();
            m_singleFrameMemoryPool = nullptr;
        }

        //if (m_commandPool) {
        //    vkDestroyCommandPool(m_nativeInterface->getDevice(), m_commandPool, nullptr);
        //    m_commandPool = VK_NULL_HANDLE;
        //}
    }

    app.cleanup();

    return 0;
}

