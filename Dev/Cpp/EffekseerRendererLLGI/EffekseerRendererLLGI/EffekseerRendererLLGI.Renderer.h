
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_H__

#include "EffekseerRendererLLGI.Base.h"
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

#include "GraphicsDevice.h"
#include <EffekseerRendererCommon/VertexBuffer.h>
#include <LLGI.Buffer.h>
#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>

namespace EffekseerRendererLLGI
{

struct FixedShader
{
	std::vector<LLGI::DataStructure> SpriteUnlit_VS;
	std::vector<LLGI::DataStructure> SpriteLit_VS;
	std::vector<LLGI::DataStructure> SpriteDistortion_VS;
	std::vector<LLGI::DataStructure> ModelUnlit_VS;
	std::vector<LLGI::DataStructure> ModelLit_VS;
	std::vector<LLGI::DataStructure> ModelDistortion_VS;

	std::vector<LLGI::DataStructure> ModelUnlit_PS;
	std::vector<LLGI::DataStructure> ModelLit_PS;
	std::vector<LLGI::DataStructure> ModelDistortion_PS;

	std::vector<LLGI::DataStructure> AdvancedSpriteUnlit_VS;
	std::vector<LLGI::DataStructure> AdvancedSpriteLit_VS;
	std::vector<LLGI::DataStructure> AdvancedSpriteDistortion_VS;
	std::vector<LLGI::DataStructure> AdvancedModelUnlit_VS;
	std::vector<LLGI::DataStructure> AdvancedModelLit_VS;
	std::vector<LLGI::DataStructure> AdvancedModelDistortion_VS;

	std::vector<LLGI::DataStructure> AdvancedModelUnlit_PS;
	std::vector<LLGI::DataStructure> AdvancedModelLit_PS;
	std::vector<LLGI::DataStructure> AdvancedModelDistortion_PS;

	std::vector<LLGI::DataStructure> GpuParticles_Clear_CS;
	std::vector<LLGI::DataStructure> GpuParticles_Spawn_CS;
	std::vector<LLGI::DataStructure> GpuParticles_Update_CS;
	std::vector<LLGI::DataStructure> GpuParticles_Render_VS;
	std::vector<LLGI::DataStructure> GpuParticles_Render_PS;
};

/**
	@brief	Renderer class
*/
class Renderer : public ::EffekseerRenderer::Renderer
{
protected:
	Renderer()
	{
	}
	virtual ~Renderer()
	{
	}

public:
	virtual LLGI::Graphics* GetGraphics() const = 0;
};

class SingleFrameMemoryPool : public ::EffekseerRenderer::SingleFrameMemoryPool, public ::Effekseer::ReferenceObject
{
	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> gpuParticleMemoryPool_;

public:
	SingleFrameMemoryPool(LLGI::SingleFrameMemoryPool* memoryPool,
		std::shared_ptr<LLGI::SingleFrameMemoryPool> gpuParticleMemoryPool = nullptr)
		: gpuParticleMemoryPool_(std::move(gpuParticleMemoryPool))
	{
		memoryPool_ = memoryPool;
		ES_SAFE_ADDREF(memoryPool_);
	}

	virtual ~SingleFrameMemoryPool()
	{
		ES_SAFE_RELEASE(memoryPool_);
	}

	void NewFrame() override
	{
		memoryPool_->NewFrame();
		if (gpuParticleMemoryPool_) gpuParticleMemoryPool_->NewFrame();
	}

	std::shared_ptr<LLGI::SingleFrameMemoryPool> GetGpuParticleMemoryPool() const { return gpuParticleMemoryPool_; }

	LLGI::SingleFrameMemoryPool* GetInternal()
	{
		return memoryPool_;
	}

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

enum class CommandListState
{
	Wait,
	Running,
	RunningWithPlatformCommandList,
};

class CommandList : public ::EffekseerRenderer::CommandList, public ::Effekseer::ReferenceObject
{
private:
	LLGI::Graphics* graphics_ = nullptr;
	struct Frame
	{
		std::shared_ptr<LLGI::CommandList> commandList;
		std::shared_ptr<EffekseerRenderer::VertexBuffer> vertexBuffer;
	};
	std::vector<Frame> frames_;
	size_t frameIndex_ = 0;
	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;
	CommandListState state_ = CommandListState::Wait;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> gpuParticleMemoryPool_;

public:
	CommandList(LLGI::Graphics* graphics, LLGI::CommandList* commandList, LLGI::SingleFrameMemoryPool* memoryPool,
		std::shared_ptr<LLGI::SingleFrameMemoryPool> gpuParticleMemoryPool = nullptr)
		: graphics_(graphics)
		, memoryPool_(memoryPool)
		, gpuParticleMemoryPool_(std::move(gpuParticleMemoryPool))
	{
		ES_SAFE_ADDREF(graphics_);
		ES_SAFE_ADDREF(commandList);
		frames_.push_back({LLGI::CreateSharedPtr(commandList), nullptr});
		ES_SAFE_ADDREF(memoryPool_);
	}

	virtual ~CommandList()
	{
		ES_SAFE_RELEASE(graphics_);
		ES_SAFE_RELEASE(memoryPool_);
	}

	bool Initialize(int32_t frameCount)
	{
		for (int32_t i = 1; i < frameCount; i++)
		{
			auto commandList = LLGI::CreateSharedPtr(graphics_->CreateCommandList(memoryPool_));
			if (!commandList)
			{
				return false;
			}
			frames_.push_back({commandList, nullptr});
		}
		frameIndex_ = frames_.size() - 1;
		return true;
	}

	// As with the memory pool, the caller must finish GPU use before reusing a slot.
	void Begin()
	{
		NextFrame();
		GetInternal()->Begin();
		state_ = CommandListState::Running;
	}

	// External submissions must finish before their frame slot is reused.
	void BeginWithPlatform(void* context)
	{
		NextFrame();
		GetInternal()->BeginWithPlatform(context);
		state_ = CommandListState::RunningWithPlatformCommandList;
	}

private:
	void NextFrame()
	{
		frameIndex_ = (frameIndex_ + 1) % frames_.size();
		auto& buffer = frames_[frameIndex_].vertexBuffer;
		if (buffer)
		{
			buffer->BeginWriteForCommandList();
		}
	}

public:
	std::shared_ptr<EffekseerRenderer::VertexBuffer> GetVertexBuffer(Effekseer::Backend::GraphicsDeviceRef device, int32_t size)
	{
		auto& vertexBuffer = frames_[frameIndex_].vertexBuffer;
		if (!vertexBuffer || vertexBuffer->GetSize() < size)
		{
			auto buffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(
				device, size, 1, EffekseerRenderer::VertexBufferRingMode::ExpandableForCommandList);
			if (!buffer->GetIsValid())
			{
				return nullptr;
			}
			vertexBuffer = buffer;
		}
		return vertexBuffer;
	}

	LLGI::Graphics* GetGraphics()
	{
		return graphics_;
	}

	LLGI::CommandList* GetInternal()
	{
		return frames_[frameIndex_].commandList.get();
	}

	LLGI::SingleFrameMemoryPool* GetMemoryPool()
	{
		return memoryPool_;
	}

	CommandListState GetState() const
	{
		return state_;
	}

	void SetState(CommandListState state)
	{
		state_ = state;
	}

	LLGI::SingleFrameMemoryPool* GetGpuParticleMemoryPool() const { return gpuParticleMemoryPool_.get(); }

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_H__
