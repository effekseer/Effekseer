#include "../TestHelper.h"
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h>
#include <EffekseerRendererCommon/VertexBuffer.h>

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

void RendererVertexBufferLifetime()
{
	for (int count : {2, 3, 4})
	{
		auto device = Effekseer::MakeRefPtr<MemoryGraphicsDevice>();
		auto ring = std::make_shared<EffekseerRenderer::VertexBufferRing>(device, 16, 3,
			EffekseerRenderer::VertexBufferRingMode::ExpandableForCommandList);
		std::vector<Effekseer::RefPtr<MemoryVertexBuffer>> pages;
		for (int frame = 1; frame <= count; frame++)
		{
			int allocations = device->allocationCount;
			ring->BeginWriteForFrame(frame, count);
			EXPECT_TRUE(device->allocationCount == allocations);
			auto first = Write(ring, 8, frame);
			ring->BeginWriteForFrame(frame, count);
			EXPECT_TRUE(Write(ring, 8, frame) == first);
			pages.push_back(first);
			ring->RenewBuffer();
			pages.push_back(Write(ring, 16, frame + 10));
		}
		for (int frame = 0; frame < count; frame++)
		{
			EXPECT_TRUE(pages[frame * 2]->data == std::vector<uint8_t>(16, frame + 1));
			EXPECT_TRUE(pages[frame * 2 + 1]->data == std::vector<uint8_t>(16, frame + 11));
		}
		int allocations = device->allocationCount;
		for (int frame = 1; frame <= count; frame++)
		{
			ring->BeginWriteForFrame(frame + count, count);
			EXPECT_TRUE(Write(ring, 16, 0x55) == pages[(frame - 1) * 2]);
			ring->RenewBuffer();
			EXPECT_TRUE(Write(ring, 16, 0x66) == pages[(frame - 1) * 2 + 1]);
		}
		EXPECT_TRUE(device->allocationCount == allocations);
	}
}

TestRegister registerRendererVertexBufferLifetime(
	"Runtime.RendererVertexBufferLifetime", RendererVertexBufferLifetime);

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

void CommandListVertexBufferGPU(LLGI::DeviceType deviceType, bool sameFrame = false)
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
		std::array<std::shared_ptr<LLGI::Buffer>, 2> Readbacks;
	};
	auto framePool = Effekseer::MakeRefPtr<EffekseerRendererLLGI::SingleFrameMemoryPool>(pool.get());
	auto ring = std::make_shared<EffekseerRenderer::VertexBufferRing>(device, 16, 3,
		EffekseerRenderer::VertexBufferRingMode::ExpandableForCommandList);
	std::vector<Frame> frames(sameFrame ? 3 : pool->GetSwapBufferCount());
	for (auto& frame : frames)
	{
		frame.Native = LLGI::CreateSharedPtr(graphics->CreateCommandList(pool.get()));
		EXPECT_TRUE(frame.Native != nullptr);
		for (auto& readback : frame.Readbacks)
		{
			readback = LLGI::CreateSharedPtr(graphics->CreateBuffer(LLGI::BufferUsageType::MapRead | LLGI::BufferUsageType::CopyDst, 16));
			EXPECT_TRUE(readback != nullptr);
		}
	}
	for (int round = 0; round < 6; round++)
	{
		// Record all frames before submitting any, including overflow pages and
		// a second binding within each recording. Each half has distinct bytes.
		for (size_t i = 0; i < frames.size(); i++)
		{
			auto& frame = frames[i];
			if (!sameFrame || i == 0) framePool->NewFrame();
			frame.Native->Begin();
			ring->BeginWriteForFrame(framePool->GetFrameNumber(), pool->GetSwapBufferCount());
			for (size_t page = 0; page < frame.Readbacks.size(); page++)
			{
				if (!ring->CanAllocate(16, 1))
				{
					EXPECT_TRUE(!ring->CanAllocate(1, 1));
					ring->RenewBuffer();
				}
				Effekseer::Backend::VertexBufferRef uploaded;
				for (int half = 0; half < 2; half++)
				{
					ring->BeginWriteForFrame(framePool->GetFrameNumber(), pool->GetSwapBufferCount());
					std::tuple<void*, int32_t> allocation;
					EXPECT_TRUE(ring->Allocate(8, 1, allocation));
					EXPECT_TRUE(std::get<1>(allocation) == half * 8);
					memset(std::get<0>(allocation), 1 + round * 12 + i * 4 + page * 2 + half, 8);
					uploaded = ring->Upload();
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
TestRegister registerSharedFrameVertexBufferDX12("Runtime.SharedFrameVertexBuffer.DX12",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::DirectX12); }, TestExecutionMode::FilterOnly);
TestRegister registerSharedFrameVertexBufferSameFrameDX12("Runtime.SharedFrameVertexBuffer.SameFrame.DX12",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::DirectX12, true); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
TestRegister registerSharedFrameVertexBufferVulkan("Runtime.SharedFrameVertexBuffer.Vulkan",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Vulkan); }, TestExecutionMode::FilterOnly);
TestRegister registerSharedFrameVertexBufferSameFrameVulkan("Runtime.SharedFrameVertexBuffer.SameFrame.Vulkan",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Vulkan, true); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
TestRegister registerSharedFrameVertexBufferMetal("Runtime.SharedFrameVertexBuffer.Metal",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Metal); }, TestExecutionMode::FilterOnly);
TestRegister registerSharedFrameVertexBufferSameFrameMetal("Runtime.SharedFrameVertexBuffer.SameFrame.Metal",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::Metal, true); }, TestExecutionMode::FilterOnly);
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
TestRegister registerSharedFrameVertexBufferWebGPU("Runtime.SharedFrameVertexBuffer.WebGPU",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::WebGPU); }, TestExecutionMode::FilterOnly);
TestRegister registerSharedFrameVertexBufferSameFrameWebGPU("Runtime.SharedFrameVertexBuffer.SameFrame.WebGPU",
	[] { CommandListVertexBufferGPU(LLGI::DeviceType::WebGPU, true); }, TestExecutionMode::FilterOnly);
#endif
} // namespace
