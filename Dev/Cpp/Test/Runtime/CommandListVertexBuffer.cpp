#include "../TestHelper.h"
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h>

#include <cstring>

#ifdef __EFFEKSEER_BUILD_DX12__
#include <DX12/LLGI.CommandListDX12.h>
#include <EffekseerRendererDX12.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h>
#include <dxgi1_4.h>
#include <d3d12sdklayers.h>
#include <LLGI.Platform.h>
#include <DX12/LLGI.GraphicsDX12.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <wrl/client.h>
#pragma comment(lib, "dxgi.lib")
#endif

namespace
{
class MemoryVertexBuffer : public Effekseer::Backend::VertexBuffer
{
public:
	std::vector<uint8_t> data;
	explicit MemoryVertexBuffer(int32_t size) : data(size) {}

	void UpdateData(const void* src, int32_t size, int32_t offset) override
	{
		EXPECT_TRUE(offset >= 0 && size >= 0 && offset + size <= static_cast<int32_t>(data.size()));
		memcpy(data.data() + offset, src, size);
	}
};

class MemoryGraphicsDevice : public Effekseer::Backend::GraphicsDevice
{
public:
	int allocationCount = 0;
	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void*, bool) override
	{
		allocationCount++;
		return Effekseer::MakeRefPtr<MemoryVertexBuffer>(size);
	}
};

Effekseer::RefPtr<MemoryVertexBuffer> Write(
	const std::shared_ptr<EffekseerRenderer::VertexBufferRing>& ring, int32_t size, uint8_t value)
{
	std::tuple<void*, int32_t> allocation;
	EXPECT_TRUE(ring->Allocate(size, 1, allocation));
	memset(std::get<0>(allocation), value, size);
	return ring->Upload().DownCast<MemoryVertexBuffer>();
}

void CommandListVertexBufferLifetime()
{
	auto device = Effekseer::MakeRefPtr<MemoryGraphicsDevice>();
	// Native objects are unnecessary: this test inspects bytes before GPU submission.
	auto first = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(nullptr, nullptr, nullptr);
	auto second = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(nullptr, nullptr, nullptr);
	first->ResetVertexBuffers();
	auto firstRing = first->GetVertexBuffer(device, 16);
	auto recordedFirst = Write(firstRing, 8, 0x11);

	// Rebinding within the same recording must append, including across renderers/views.
	auto rebound = first->GetVertexBuffer(device, 16);
	EXPECT_TRUE(rebound == firstRing);
	Write(rebound, 8, 0x22);
	EXPECT_TRUE(recordedFirst->data == std::vector<uint8_t>({
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
		0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}));

	// Recording the next frame before submitting the first must preserve its bytes.
	second->ResetVertexBuffers();
	auto secondRing = second->GetVertexBuffer(device, 16);
	auto recordedSecond = Write(secondRing, 16, 0x33);
	EXPECT_TRUE(recordedFirst.Get() != recordedSecond.Get());
	EXPECT_TRUE(recordedFirst->data[0] == 0x11 && recordedFirst->data[8] == 0x22);
	EXPECT_TRUE(device->allocationCount == 2);

	// Overflow within a recording must grow rather than overwrite earlier draws.
	EXPECT_TRUE(!firstRing->CanAllocate(1, 1));
	firstRing->RenewBuffer();
	auto overflow = Write(firstRing, 16, 0x44);
	EXPECT_TRUE(overflow.Get() != recordedFirst.Get());
	EXPECT_TRUE(recordedFirst->data[0] == 0x11);
	EXPECT_TRUE(device->allocationCount == 3);

	// Simulate completion of the first recording. Its pages can now be reused,
	// while the second recording must remain intact. Allocation must stabilize.
	for (int i = 0; i < 10; i++)
	{
		first->ResetVertexBuffers();
		auto reused = Write(first->GetVertexBuffer(device, 16), 16, 0x55);
		EXPECT_TRUE(reused.Get() == recordedFirst.Get());
		firstRing->RenewBuffer();
		EXPECT_TRUE(Write(firstRing, 16, 0x66).Get() == overflow.Get());
		EXPECT_TRUE(recordedSecond->data == std::vector<uint8_t>(16, 0x33));
		EXPECT_TRUE(device->allocationCount == 3);
	}

	// Different renderer capacities in one recording keep separate pages.
	auto smaller = first->GetVertexBuffer(device, 8);
	Write(smaller, 8, 0x77);
	EXPECT_TRUE(recordedFirst->data == std::vector<uint8_t>(16, 0x55));
}

TestRegister registerCommandListVertexBufferLifetime(
	"Runtime.CommandListVertexBufferLifetime", CommandListVertexBufferLifetime);

#ifdef __EFFEKSEER_BUILD_DX12__
void CommandListVertexBufferDX12(bool useExternalCommandList)
{
	using Microsoft::WRL::ComPtr;
	ComPtr<IDXGIFactory4> factory;
	EXPECT_TRUE(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))));
	ComPtr<IDXGIAdapter> adapter;
	EXPECT_TRUE(SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))));
	ComPtr<ID3D12Device> device;
	EXPECT_TRUE(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))));
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	ComPtr<ID3D12CommandQueue> queue;
	EXPECT_TRUE(SUCCEEDED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue))));
	auto graphicsDevice = EffekseerRendererDX12::CreateGraphicsDevice(device.Get(), queue.Get(), 3);
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto renderer = EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 16);
	EXPECT_TRUE(renderer != nullptr);
	auto pool = EffekseerRenderer::CreateSingleFrameMemoryPool(graphicsDevice);
	auto graphics = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>()->GetGraphics();
	std::array<Effekseer::RefPtr<EffekseerRenderer::CommandList>, 2> lists;
	std::array<std::shared_ptr<LLGI::CommandList>, 2> nativeLists;
	std::array<std::shared_ptr<LLGI::Buffer>, 2> readbacks;
	for (size_t i = 0; i < lists.size(); i++)
	{
		lists[i] = EffekseerRenderer::CreateCommandList(graphicsDevice, pool);
		if (useExternalCommandList)
		{
			nativeLists[i] = LLGI::CreateSharedPtr(graphics->CreateCommandList(
				pool.DownCast<EffekseerRendererLLGI::SingleFrameMemoryPool>()->GetInternal()));
		}
		readbacks[i] = LLGI::CreateSharedPtr(graphics->CreateBuffer(
			LLGI::BufferUsageType::MapRead | LLGI::BufferUsageType::CopyDst, 16));
		EXPECT_TRUE(readbacks[i] != nullptr);
	}

	for (int round = 0; round < 3; round++)
	{
		// Deliberately record BOTH frames before executing either, as in the report.
		for (size_t i = 0; i < lists.size(); i++)
		{
			pool->NewFrame();
			ID3D12GraphicsCommandList* nativeList = nullptr;
			if (useExternalCommandList)
			{
				nativeLists[i]->Begin();
				nativeList = static_cast<LLGI::CommandListDX12*>(nativeLists[i].get())->GetCommandList();
			}
			EffekseerRendererDX12::BeginCommandList(lists[i], nativeList);
			renderer->SetCommandList(lists[i]);
			auto ring = renderer->GetImpl()->InternalVertexBuffer;
			std::tuple<void*, int32_t> allocation;
			EXPECT_TRUE(ring->Allocate(16, 1, allocation));
			EXPECT_TRUE(std::get<1>(allocation) == 0);
			memset(std::get<0>(allocation), static_cast<int>(i) + 1 + round * 2, 16);
			auto vertexBuffer = ring->Upload().DownCast<EffekseerRendererLLGI::Backend::VertexBuffer>();
			lists[i].DownCast<EffekseerRendererLLGI::CommandList>()->GetInternal()->CopyBuffer(
				vertexBuffer->GetBuffer(), readbacks[i].get());
			renderer->SetCommandList(nullptr);
			EffekseerRendererDX12::EndCommandList(lists[i]);
			if (useExternalCommandList)
			{
				nativeLists[i]->End();
			}
		}

		for (size_t i = 0; i < lists.size(); i++)
		{
			if (useExternalCommandList)
			{
				graphics->Execute(nativeLists[i].get());
			}
			else
			{
				EffekseerRendererDX12::ExecuteCommandList(lists[i]);
			}
		}
		for (size_t i = 0; i < lists.size(); i++)
		{
			auto submitted = useExternalCommandList ? nativeLists[i].get()
				: lists[i].DownCast<EffekseerRendererLLGI::CommandList>()->GetInternal();
			submitted->WaitUntilCompleted();
			auto data = static_cast<const uint8_t*>(readbacks[i]->Lock());
			EXPECT_TRUE(data != nullptr);
			for (int j = 0; j < 16; j++)
			{
				EXPECT_TRUE(data[j] == static_cast<uint8_t>(i + 1 + round * 2));
			}
			readbacks[i]->Unlock();
		}
	}
}

TestRegister registerCommandListVertexBufferDX12(
	"Runtime.CommandListVertexBufferDX12", [] { CommandListVertexBufferDX12(false); });
TestRegister registerCommandListVertexBufferDX12External(
	"Runtime.CommandListVertexBufferDX12.External", [] { CommandListVertexBufferDX12(true); });

void PlatformFramesDX12()
{
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D12Debug> debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
	{
		debug->EnableDebugLayer();
	}
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("DX12 frame reuse test", {64, 64}));
	EXPECT_TRUE(window != nullptr);
	ShowWindow(static_cast<HWND>(window->GetNativePtr(0)), SW_HIDE);
	LLGI::PlatformParameter parameters;
	parameters.Device = LLGI::DeviceType::DirectX12;
	parameters.WaitVSync = false;
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(parameters, window.get()));
	EXPECT_TRUE(platform != nullptr);
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());
	auto pool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(4096, 16));
	std::vector<std::shared_ptr<LLGI::CommandList>> lists(platform->GetMaxFrameCount());
	for (auto& list : lists)
	{
		list = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool.get()));
	}
	ComPtr<ID3D12InfoQueue> infoQueue;
	static_cast<LLGI::GraphicsDX12*>(graphics.get())->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue));
	if (infoQueue)
	{
		infoQueue->ClearStoredMessages();
	}
	for (size_t frame = 0; frame < 24; frame++)
	{
		EXPECT_TRUE(platform->NewFrame());
		auto& list = lists[frame % lists.size()];
		if (frame >= lists.size())
		{
			list->WaitUntilCompleted();
		}
		pool->NewFrame();
		list->Begin();
		list->BeginRenderPass(platform->GetCurrentScreen({0, 32, 64, 255}, true, false));
		list->EndRenderPass();
		list->End();
		graphics->Execute(list.get());
		if (frame == 0)
		{
			// Hold the presentation commands on the GPU. Async Present must return
			// before we release them, rather than waiting for the whole queue.
			auto dx12 = static_cast<LLGI::GraphicsDX12*>(graphics.get());
			ComPtr<ID3D12Fence> gate;
			EXPECT_TRUE(SUCCEEDED(dx12->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&gate))));
			EXPECT_TRUE(SUCCEEDED(dx12->GetCommandQueue()->Wait(gate.Get(), 1)));
			std::mutex mutex;
			std::condition_variable condition;
			bool returned = false;
			bool timedOut = false;
			std::thread release([&]
			{
				std::unique_lock<std::mutex> lock(mutex);
				timedOut = !condition.wait_for(lock, std::chrono::seconds(5), [&] { return returned; });
				gate->Signal(1);
			});
			platform->Present();
			{
				std::lock_guard<std::mutex> lock(mutex);
				returned = true;
			}
			condition.notify_one();
			release.join();
			EXPECT_TRUE(!timedOut);
			// Keep the gate alive until the queue has passed its Wait operation.
			graphics->WaitFinish();
		}
		else
		{
			platform->Present();
		}
	}
	graphics->WaitFinish();
	if (infoQueue)
	{
		for (UINT64 i = 0; i < infoQueue->GetNumStoredMessages(); i++)
		{
			SIZE_T size = 0;
			infoQueue->GetMessage(i, nullptr, &size);
			std::vector<uint8_t> storage(size);
			auto message = reinterpret_cast<D3D12_MESSAGE*>(storage.data());
			infoQueue->GetMessage(i, message, &size);
			if (message->Severity <= D3D12_MESSAGE_SEVERITY_ERROR)
			{
				fprintf(stderr, "%s\n", message->pDescription);
				EXPECT_TRUE(false);
			}
		}
	}
}

TestRegister registerPlatformFramesDX12(
	"Runtime.PlatformFramesDX12", PlatformFramesDX12, TestExecutionMode::FilterOnly);
#endif
} // namespace
