#include "../TestHelper.h"
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h>
#include <array>
#include <cstring>

#ifdef __EFFEKSEER_BUILD_DX12__
#include <DX12/LLGI.CommandListDX12.h>
#include <DX12/LLGI.BufferDX12.h>
#include <EffekseerRendererDX12.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.GpuParticles.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h>
#include <dxgi1_4.h>
#include <d3d12sdklayers.h>
#include <d3dcompiler.h>
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
#ifdef __EFFEKSEER_BUILD_DX12__
void CommandListVertexBufferDX12(bool useExternalCommandList, bool sameFrame = false, int frameCount = 3, bool sharedWrapper = false)
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
	auto graphicsDevice = EffekseerRendererDX12::CreateGraphicsDevice(device.Get(), queue.Get(), frameCount);
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto renderer = EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 16);
	EXPECT_TRUE(renderer != nullptr);
	auto pool = EffekseerRenderer::CreateSingleFrameMemoryPool(graphicsDevice);
	auto graphics = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>()->GetGraphics();
	std::array<Effekseer::RefPtr<EffekseerRenderer::CommandList>, 2> lists;
	std::array<std::shared_ptr<LLGI::CommandList>, 2> nativeLists;
	std::array<LLGI::CommandList*, 2> recordedLists{};
	std::array<std::shared_ptr<LLGI::Buffer>, 2> readbacks;
	std::array<std::shared_ptr<LLGI::Buffer>, 2> overflowReadbacks;
	for (size_t i = 0; i < lists.size(); i++)
	{
		lists[i] = sharedWrapper && i > 0 ? lists[0] : EffekseerRenderer::CreateCommandList(graphicsDevice, pool);
		if (useExternalCommandList)
		{
			nativeLists[i] = LLGI::CreateSharedPtr(graphics->CreateCommandList(
				pool.DownCast<EffekseerRendererLLGI::SingleFrameMemoryPool>()->GetInternal()));
		}
		readbacks[i] = LLGI::CreateSharedPtr(graphics->CreateBuffer(
			LLGI::BufferUsageType::MapRead | LLGI::BufferUsageType::CopyDst, 16));
		EXPECT_TRUE(readbacks[i] != nullptr);
		overflowReadbacks[i] = LLGI::CreateSharedPtr(graphics->CreateBuffer(
			LLGI::BufferUsageType::MapRead | LLGI::BufferUsageType::CopyDst, 16));
		EXPECT_TRUE(overflowReadbacks[i] != nullptr);
	}

	for (int round = 0; round < 9; round++)
	{
		// Deliberately record BOTH frames before executing either, as in the report.
		for (size_t i = 0; i < lists.size(); i++)
		{
			if (!sameFrame || i == 0) pool->NewFrame();
			ID3D12GraphicsCommandList* nativeList = nullptr;
			if (useExternalCommandList)
			{
				nativeLists[i]->Begin();
				nativeList = static_cast<LLGI::CommandListDX12*>(nativeLists[i].get())->GetCommandList();
			}
			EffekseerRendererDX12::BeginCommandList(lists[i], nativeList);
			recordedLists[i] = lists[i].DownCast<EffekseerRendererLLGI::CommandList>()->GetInternal();
			renderer->SetCommandList(lists[i]);
			EXPECT_TRUE(renderer->BeginRendering());
			auto ring = renderer->GetImpl()->InternalVertexBuffer;
			std::tuple<void*, int32_t> allocation;
			EXPECT_TRUE(ring->Allocate(ring->GetSize(), 1, allocation));
			EXPECT_TRUE(std::get<1>(allocation) == 0);
			memset(std::get<0>(allocation), static_cast<int>(i) + 1 + round * 2, 16);
			auto vertexBuffer = ring->Upload().DownCast<EffekseerRendererLLGI::Backend::VertexBuffer>();
			lists[i].DownCast<EffekseerRendererLLGI::CommandList>()->GetInternal()->CopyBuffer(
				vertexBuffer->GetBuffer(), readbacks[i].get());
			// Rebinding and another render pass must retain the offset in this frame.
			EXPECT_TRUE(renderer->EndRendering());
			renderer->SetCommandList(lists[i]);
			EXPECT_TRUE(renderer->BeginRendering());
			EXPECT_TRUE(renderer->GetImpl()->InternalVertexBuffer == ring);
			EXPECT_TRUE(!ring->CanAllocate(1, 1));
			ring->RenewBuffer();
			EXPECT_TRUE(ring->Allocate(16, 1, allocation));
			memset(std::get<0>(allocation), static_cast<int>(i) + 101 + round * 2, 16);
			auto overflow = ring->Upload().DownCast<EffekseerRendererLLGI::Backend::VertexBuffer>();
			lists[i].DownCast<EffekseerRendererLLGI::CommandList>()->GetInternal()->CopyBuffer(
				overflow->GetBuffer(), overflowReadbacks[i].get());
			EXPECT_TRUE(renderer->EndRendering());
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
				graphics->Execute(recordedLists[i]);
			}
		}
		for (size_t i = 0; i < lists.size(); i++)
		{
			auto submitted = useExternalCommandList ? nativeLists[i].get()
				: recordedLists[i];
			submitted->WaitUntilCompleted();
			auto data = static_cast<const uint8_t*>(readbacks[i]->Lock());
			EXPECT_TRUE(data != nullptr);
			for (int j = 0; j < 16; j++)
			{
				EXPECT_TRUE(data[j] == static_cast<uint8_t>(i + 1 + round * 2));
			}
			readbacks[i]->Unlock();
			data = static_cast<const uint8_t*>(overflowReadbacks[i]->Lock());
			EXPECT_TRUE(data != nullptr);
			for (int j = 0; j < 16; j++)
			{
				EXPECT_TRUE(data[j] == static_cast<uint8_t>(i + 101 + round * 2));
			}
			overflowReadbacks[i]->Unlock();
		}
	}
}

TestRegister registerCommandListVertexBufferDX12(
	"Runtime.CommandListVertexBufferDX12", [] { CommandListVertexBufferDX12(false); });
TestRegister registerCommandListVertexBufferDX12External(
	"Runtime.CommandListVertexBufferDX12.External", [] { CommandListVertexBufferDX12(true); });

TestRegister registerCommandListVertexBufferDX12SameFrame(
	"Runtime.CommandListVertexBufferDX12.SameFrame", [] { CommandListVertexBufferDX12(true, true); });

TestRegister registerCommandListVertexBufferDX12Shared(
	"Runtime.CommandListVertexBufferDX12.Shared", [] { CommandListVertexBufferDX12(true, false, 3, true); });
TestRegister registerCommandListVertexBufferDX12TwoFrames(
	"Runtime.CommandListVertexBufferDX12.TwoFrames", [] { CommandListVertexBufferDX12(true, false, 2, true); });
TestRegister registerCommandListVertexBufferDX12FourFrames(
	"Runtime.CommandListVertexBufferDX12.FourFrames", [] { CommandListVertexBufferDX12(true, false, 4, true); });

// Each recording uses a different constant-buffer descriptor. Record all of
// them before submission to verify that one Effekseer wrapper retains all frame slots.
void CommandListDescriptorsDX12(int frameCount)
{
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D12Debug> debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) debug->EnableDebugLayer();
	ComPtr<IDXGIFactory4> factory;
	EXPECT_TRUE(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))));
	ComPtr<IDXGIAdapter> adapter;
	EXPECT_TRUE(SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))));
	ComPtr<ID3D12Device> device;
	EXPECT_TRUE(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))));
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	ComPtr<ID3D12CommandQueue> queue;
	EXPECT_TRUE(SUCCEEDED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue))));
	auto wait = [&] {
		ComPtr<ID3D12Fence> fence;
		EXPECT_TRUE(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));
		auto event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		EXPECT_TRUE(event != nullptr);
		EXPECT_TRUE(SUCCEEDED(queue->Signal(fence.Get(), 1)));
		EXPECT_TRUE(SUCCEEDED(fence->SetEventOnCompletion(1, event)));
		EXPECT_TRUE(WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0);
		CloseHandle(event);
	};
	auto graphics = LLGI::CreateSharedPtr(new LLGI::GraphicsDX12(device.Get(),
		[] { return std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, LLGI::Texture*>(); }, wait, queue.Get(), frameCount));
	auto pool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(4096, 16));
	auto first = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool.get()));
	auto wrapper = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(graphics.get(), first.get(), pool.get());
	EXPECT_TRUE(wrapper->Initialize(frameCount));
	auto compile = [&](const char* source, const char* profile) {
		ComPtr<ID3DBlob> binary, errors;
		EXPECT_TRUE(SUCCEEDED(D3DCompile(source, strlen(source), nullptr, nullptr, nullptr,
			"main", profile, 0, 0, &binary, &errors)));
		LLGI::DataStructure data;
		data.Data = binary->GetBufferPointer();
		data.Size = static_cast<int32_t>(binary->GetBufferSize());
		return LLGI::CreateSharedPtr(graphics->CreateShader(&data, 1));
	};
	auto vs = compile("float4 main(float3 p : POSITION) : SV_POSITION { return float4(p,1); }", "vs_5_0");
	auto ps = compile("cbuffer C : register(b1) { float4 color; }; float4 main() : SV_TARGET { return color; }", "ps_5_0");
	const float vertices[] = {-1, -1, 0, -1, 3, 0, 3, -1, 0};
	const uint16_t indices[] = {0, 1, 2};
	auto vb = LLGI::CreateSharedPtr(graphics->CreateBuffer(LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite, sizeof(vertices)));
	auto ib = LLGI::CreateSharedPtr(graphics->CreateBuffer(LLGI::BufferUsageType::Index | LLGI::BufferUsageType::MapWrite, sizeof(indices)));
	memcpy(vb->Lock(), vertices, sizeof(vertices));
	vb->Unlock();
	memcpy(ib->Lock(), indices, sizeof(indices));
	ib->Unlock();
	std::vector<std::shared_ptr<LLGI::CommandList>> native(frameCount);
	std::vector<std::shared_ptr<LLGI::Texture>> targets(frameCount);
	std::vector<std::shared_ptr<LLGI::RenderPass>> passes(frameCount);
	for (int i = 0; i < frameCount; i++)
	{
		native[i] = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool.get()));
		LLGI::RenderTextureInitializationParameter parameter;
		parameter.Size = {16, 16};
		targets[i] = LLGI::CreateSharedPtr(graphics->CreateRenderTexture(parameter));
		auto texture = targets[i].get();
		passes[i] = LLGI::CreateSharedPtr(graphics->CreateRenderPass(&texture, 1, nullptr));
		passes[i]->SetIsColorCleared(true);
		passes[i]->SetClearColor({0, 0, 0, 255});
	}
	auto renderState = LLGI::CreateSharedPtr(graphics->CreateRenderPassPipelineState(passes[0].get()));
	auto pipeline = LLGI::CreateSharedPtr(graphics->CreatePiplineState());
	pipeline->VertexLayoutCount = 1;
	pipeline->VertexLayouts[0] = LLGI::VertexLayoutFormat::R32G32B32_FLOAT;
	pipeline->VertexLayoutNames[0] = "POSITION";
	pipeline->Culling = LLGI::CullingMode::DoubleSide;
	pipeline->SetShader(LLGI::ShaderStageType::Vertex, vs.get());
	pipeline->SetShader(LLGI::ShaderStageType::Pixel, ps.get());
	pipeline->SetRenderPassPipelineState(renderState.get());
	EXPECT_TRUE(pipeline->Compile());
	for (int round = 0; round < 4; round++)
	{
		for (int i = 0; i < frameCount; i++)
		{
			pool->NewFrame();
			native[i]->Begin();
			LLGI::PlatformContextDX12 context;
			context.commandList = static_cast<LLGI::CommandListDX12*>(native[i].get())->GetCommandList();
			wrapper->BeginWithPlatform(&context);
			auto recorded = wrapper->GetInternal();
			auto constant = LLGI::CreateSharedPtr(pool->CreateConstantBuffer(16));
			const float color[] = {(i + 1) * 40.0f / 255.0f, round * 40.0f / 255.0f, 0, 1};
			memcpy(constant->Lock(), color, sizeof(color));
			constant->Unlock();
			native[i]->BeginRenderPass(passes[i].get());
			recorded->SetPipelineState(pipeline.get());
			recorded->SetVertexBuffer(vb.get(), 12, 0);
			recorded->SetIndexBuffer(ib.get(), 2);
			recorded->SetConstantBuffer(constant.get(), 1);
			recorded->Draw(1);
			native[i]->EndRenderPass();
			recorded->EndWithPlatform();
			native[i]->End();
		}
		for (auto& list : native) graphics->Execute(list.get());
		graphics->WaitFinish();
		ComPtr<ID3D12InfoQueue> info;
		device->QueryInterface(IID_PPV_ARGS(&info));
		if (info)
		{
			for (UINT64 j = 0; j < info->GetNumStoredMessages(); j++)
			{
				SIZE_T size = 0;
				info->GetMessage(j, nullptr, &size);
				std::vector<uint8_t> bytes(size);
				auto message = reinterpret_cast<D3D12_MESSAGE*>(bytes.data());
				info->GetMessage(j, message, &size);
				if (message->Severity <= D3D12_MESSAGE_SEVERITY_ERROR)
				{
					fprintf(stderr, "%s\n", message->pDescription);
					EXPECT_TRUE(false);
				}
			}
		}

		for (int i = 0; i < frameCount; i++)
		{
			const auto pixels = graphics->CaptureRenderTarget(targets[i].get());
			EXPECT_TRUE(pixels.size() == 16 * 16 * 4);
			const int center = (8 * 16 + 8) * 4;
			EXPECT_TRUE(abs(static_cast<int>(pixels[center]) - (i + 1) * 40) <= 1);
			EXPECT_TRUE(abs(static_cast<int>(pixels[center + 1]) - round * 40) <= 1);
		}
	}
}

TestRegister registerCommandListDescriptorsDX12TwoFrames(
	"Runtime.CommandListDescriptorsDX12.TwoFrames", [] { CommandListDescriptorsDX12(2); });
TestRegister registerCommandListDescriptorsDX12ThreeFrames(
	"Runtime.CommandListDescriptorsDX12.ThreeFrames", [] { CommandListDescriptorsDX12(3); });
TestRegister registerCommandListDescriptorsDX12FourFrames(
	"Runtime.CommandListDescriptorsDX12.FourFrames", [] { CommandListDescriptorsDX12(4); });

class TestGpuParticleSystem : public EffekseerRendererLLGI::GpuParticleSystem
{
public:
	using GpuParticleSystem::GpuParticleSystem;
	using GpuParticleSystem::UploadConstants;
};

// Record distinct constants across three unsubmitted frames, fill the pool,
// then reuse its regions only after GPU completion.
void GpuParticleConstantsDX12()
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
	auto backend = EffekseerRendererDX12::CreateGraphicsDevice(device.Get(), queue.Get(), 3);
	auto graphicsDevice = backend.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	auto graphics = graphicsDevice->GetGraphics();
	constexpr int Steps = 12;
	auto rendererPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(4096, 32));
	auto constantPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(Steps * 256, 128));
	auto pool = Effekseer::MakeRefPtr<EffekseerRendererLLGI::SingleFrameMemoryPool>(rendererPool.get(), constantPool);
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto renderer = EffekseerRendererDX12::Create(backend, &format, 1, DXGI_FORMAT_UNKNOWN, false, 16);
	auto particles = Effekseer::MakeRefPtr<TestGpuParticleSystem>(renderer.DownCast<EffekseerRendererLLGI::RendererImplemented>().Get());
	const char* source =
		"cbuffer C : register(b0) { uint index; uint value; uint2 padding; };"
		"RWStructuredBuffer<uint> output : register(u0);"
		"[numthreads(1,1,1)] void main() { output[index] = value; }";
	ComPtr<ID3DBlob> binary, errors;
	EXPECT_TRUE(SUCCEEDED(D3DCompile(source, strlen(source), nullptr, nullptr, nullptr,
		"main", "cs_5_0", 0, 0, &binary, &errors)));
	LLGI::DataStructure data;
	data.Data = binary->GetBufferPointer();
	data.Size = static_cast<int32_t>(binary->GetBufferSize());
	auto serialized = EffekseerRendererLLGI::Backend::Serialize({data});
	auto shader = graphicsDevice->CreateComputeShader(serialized.data(), static_cast<int32_t>(serialized.size()));
	Effekseer::Backend::PipelineStateParameter pipelineParameter;
	pipelineParameter.ShaderPtr = shader;
	auto pipeline = graphicsDevice->CreatePipelineState(pipelineParameter);
	EXPECT_TRUE(pipeline != nullptr);
	std::array<uint32_t, 4> values = {0, 0, 123, 456};
	auto uniform = graphicsDevice->CreateUniformBuffer(sizeof(values), values.data());
	std::array<std::shared_ptr<LLGI::CommandList>, 3> lists;
	std::array<Effekseer::Backend::StorageBufferRef, 3> outputs;
	std::array<ComPtr<ID3D12Resource>, 3> readbacks;
	for (int frame = 0; frame < 3; frame++)
	{
		lists[frame] = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool->GetInternal()));
		outputs[frame] = graphicsDevice->CreateStorageBuffer(Steps, sizeof(uint32_t), nullptr,
			Effekseer::Backend::StorageBufferUsage::ReadWrite);
		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_READBACK;
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = Steps * sizeof(uint32_t);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		EXPECT_TRUE(SUCCEEDED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readbacks[frame]))));
	}
	for (uint32_t round = 0; round < 3; round++)
	{
		for (uint32_t frame = 0; frame < 3; frame++)
		{
			pool->NewFrame();
			auto list = lists[frame].get();
			list->Begin();
			auto wrapper = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(graphics, list, pool->GetInternal(), pool->GetGpuParticleMemoryPool());
			renderer->SetCommandList(wrapper);
			Effekseer::Backend::DispatchParameter dispatch;
			dispatch.PipelineStatePtr = pipeline;
			dispatch.UniformBufferPtrs[0] = uniform;
			dispatch.SetStorageBuffer(0, outputs[frame], Effekseer::Backend::StorageBufferAccess::ReadWrite);
			dispatch.GroupCount = {1, 1, 1};
			dispatch.ThreadCount = {1, 1, 1};
			for (uint32_t step = 0; step < Steps; step++)
			{
				values[0] = step;
				values[1] = 1000 * round + 100 * frame + step + 1;
				EXPECT_TRUE(particles->UploadConstants(uniform, sizeof(values), values.data()));
				graphicsDevice->Dispatch(dispatch);
			}
			// Exhaustion must fail without overwriting earlier dispatches' constants.
			values.fill(0);
			EXPECT_TRUE(!particles->UploadConstants(uniform, sizeof(values), values.data()));
			auto native = static_cast<LLGI::CommandListDX12*>(list)->GetCommandList();
			auto output = static_cast<LLGI::BufferDX12*>(outputs[frame].DownCast<EffekseerRendererLLGI::Backend::StorageBuffer>()->GetBuffer());
			output->ResourceBarrier(native, D3D12_RESOURCE_STATE_COPY_SOURCE);
			native->CopyBufferRegion(readbacks[frame].Get(), 0, output->Get(), 0, Steps * sizeof(uint32_t));
			list->End();
		}
		values.fill(0);
		// No GPU work can observe any constants until every update is finished.
		for (auto& list : lists) graphics->Execute(list.get());
		for (auto& list : lists) list->WaitUntilCompleted();
		for (uint32_t frame = 0; frame < 3; frame++)
		{
			uint32_t* actual = nullptr;
			EXPECT_TRUE(SUCCEEDED(readbacks[frame]->Map(0, nullptr, reinterpret_cast<void**>(&actual))));
			for (uint32_t step = 0; step < Steps; step++)
			{
				if (actual[step] != 1000 * round + 100 * frame + step + 1)
				{
					fprintf(stderr, "round=%u frame=%u step=%u actual=%u\n", round, frame, step, actual[step]);
				}
				EXPECT_TRUE(actual[step] == 1000 * round + 100 * frame + step + 1);
			}
			readbacks[frame]->Unmap(0, nullptr);
		}
	}
	renderer->SetCommandList(nullptr);
}

TestRegister registerGpuParticleConstantsDX12("Runtime.GpuParticleConstantsDX12", GpuParticleConstantsDX12);

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
