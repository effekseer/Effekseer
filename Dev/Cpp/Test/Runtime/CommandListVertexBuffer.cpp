#include "../TestHelper.h"
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h>

#include <LLGI.Platform.h>
#include <array>
#include <cstring>

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

// The same GPU readback scenario runs on every LLGI backend. Native API
// integration checks live separately in CommandListDX12.cpp.
class ReadbackVertexBuffer : public Effekseer::Backend::VertexBuffer
{
public:
	std::shared_ptr<LLGI::Buffer> Buffer;

	ReadbackVertexBuffer(LLGI::Graphics* graphics, int32_t size)
	{
		// Production vertex buffers need not support copies on every API.
		// Enable CopySrc only for these test buffers to inspect GPU-visible bytes.
		Buffer = LLGI::CreateSharedPtr(graphics->CreateBuffer(
			LLGI::BufferUsageType::Vertex | LLGI::BufferUsageType::MapWrite | LLGI::BufferUsageType::CopySrc, size));
		EXPECT_TRUE(Buffer != nullptr);
	}

	void UpdateData(const void* src, int32_t size, int32_t offset) override
	{
		auto dst = Buffer->Lock(offset, size);
		EXPECT_TRUE(dst != nullptr);
		memcpy(dst, src, size);
		Buffer->Unlock();
	}
};

class ReadbackGraphicsDevice : public Effekseer::Backend::GraphicsDevice
{
	std::shared_ptr<LLGI::Graphics> graphics_;

public:
	explicit ReadbackGraphicsDevice(std::shared_ptr<LLGI::Graphics> graphics) : graphics_(graphics) {}

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void*, bool) override
	{
		return Effekseer::MakeRefPtr<ReadbackVertexBuffer>(graphics_.get(), size);
	}
};

void CommandListVertexBufferGPU(LLGI::DeviceType deviceType)
{
	auto window = std::unique_ptr<LLGI::Window>(LLGI::CreateWindow("Command list vertex buffers", {64, 64}));
	EXPECT_TRUE(window != nullptr);
	LLGI::PlatformParameter parameters;
	parameters.Device = deviceType;
	parameters.WaitVSync = false;
	auto platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(parameters, window.get()));
	EXPECT_TRUE(platform != nullptr);
	auto graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());
	EXPECT_TRUE(graphics != nullptr);
	auto device = Effekseer::MakeRefPtr<ReadbackGraphicsDevice>(graphics);
	auto pool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(4096, 16));
	EXPECT_TRUE(pool != nullptr);
	struct Frame
	{
		std::shared_ptr<LLGI::CommandList> Native;
		Effekseer::RefPtr<EffekseerRendererLLGI::CommandList> CommandList;
		std::array<std::shared_ptr<LLGI::Buffer>, 2> Readbacks;
		std::array<Effekseer::Backend::VertexBufferRef, 2> Pages;
	};
	std::array<Frame, 3> frames;
	for (auto& frame : frames)
	{
		frame.Native = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool.get()));
		EXPECT_TRUE(frame.Native != nullptr);
		frame.CommandList = Effekseer::MakeRefPtr<EffekseerRendererLLGI::CommandList>(graphics.get(), frame.Native.get(), pool.get());
		for (auto& readback : frame.Readbacks)
		{
			readback = LLGI::CreateSharedPtr(graphics->CreateBuffer(LLGI::BufferUsageType::MapRead | LLGI::BufferUsageType::CopyDst, 16));
			EXPECT_TRUE(readback != nullptr);
		}
	}
	for (int round = 0; round < 3; round++)
	{
		// Record all frames before submitting any, including overflow pages and
		// a second binding within each recording. Each half has distinct bytes.
		for (size_t i = 0; i < frames.size(); i++)
		{
			auto& frame = frames[i];
			pool->NewFrame();
			frame.Native->Begin();
			frame.CommandList->ResetVertexBuffers();
			auto ring = frame.CommandList->GetVertexBuffer(device, 16);
			for (size_t page = 0; page < frame.Readbacks.size(); page++)
			{
				if (page > 0)
				{
					EXPECT_TRUE(!ring->CanAllocate(1, 1));
					ring->RenewBuffer();
				}
				Effekseer::Backend::VertexBufferRef uploaded;
				for (int half = 0; half < 2; half++)
				{
					EXPECT_TRUE(frame.CommandList->GetVertexBuffer(device, 16) == ring);
					std::tuple<void*, int32_t> allocation;
					EXPECT_TRUE(ring->Allocate(8, 1, allocation));
					EXPECT_TRUE(std::get<1>(allocation) == half * 8);
					memset(std::get<0>(allocation), 1 + round * 12 + i * 4 + page * 2 + half, 8);
					uploaded = ring->Upload();
				}
				if (round == 0)
				{
					frame.Pages[page] = uploaded;
				}
				else
				{
					EXPECT_TRUE(frame.Pages[page] == uploaded);
				}
				auto buffer = uploaded.DownCast<ReadbackVertexBuffer>();
				frame.Native->CopyBuffer(buffer->Buffer.get(), frame.Readbacks[page].get());
			}
			frame.Native->End();
		}
		for (auto& frame : frames)
		{
			graphics->Execute(frame.Native.get());
		}
		// Complete all submitted recordings and readbacks before reusing pages.
		for (size_t i = 0; i < frames.size(); i++)
		{
			auto& frame = frames[i];
			frame.Native->WaitUntilCompleted();
			for (size_t page = 0; page < frame.Readbacks.size(); page++)
			{
				const auto data = static_cast<const uint8_t*>(frame.Readbacks[page]->Lock());
				EXPECT_TRUE(data != nullptr);
				for (int byte = 0; byte < 16; byte++)
				{
					EXPECT_TRUE(data[byte] == static_cast<uint8_t>(1 + round * 12 + i * 4 + page * 2 + byte / 8));
				}
				frame.Readbacks[page]->Unlock();
			}
		}
	}
	graphics->WaitFinish();
}

#ifdef __EFFEKSEER_BUILD_DX12__
TestRegister registerCommandListVertexBufferGPUDX12("Runtime.CommandListVertexBufferGPU.DX12",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::DirectX12); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
TestRegister registerCommandListVertexBufferGPUVulkan("Runtime.CommandListVertexBufferGPU.Vulkan",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Vulkan); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
TestRegister registerCommandListVertexBufferGPUMetal("Runtime.CommandListVertexBufferGPU.Metal",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Metal); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
TestRegister registerCommandListVertexBufferGPUWebGPU("Runtime.CommandListVertexBufferGPU.WebGPU",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::WebGPU); }, TestExecutionMode::FilterOnly);
#endif
} // namespace
