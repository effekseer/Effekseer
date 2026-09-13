
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_H__

#include "EffekseerRendererLLGI.Base.h"
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>
#include <EffekseerRendererCommon/VertexBuffer.h>
#include <stdexcept>

#include "GraphicsDevice.h"
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

public:
	SingleFrameMemoryPool(LLGI::SingleFrameMemoryPool* memoryPool)
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
	}

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
	LLGI::CommandList* commandList_ = nullptr;
	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;
	CommandListState state_ = CommandListState::Wait;
	std::vector<std::shared_ptr<EffekseerRenderer::VertexBufferRing>> vertexBuffers_;

public:
	CommandList(LLGI::Graphics* graphics, LLGI::CommandList* commandList, LLGI::SingleFrameMemoryPool* memoryPool)
		: graphics_(graphics)
		, commandList_(commandList)
		, memoryPool_(memoryPool)
	{
		ES_SAFE_ADDREF(graphics_);
		ES_SAFE_ADDREF(commandList_);
		ES_SAFE_ADDREF(memoryPool_);
	}

	virtual ~CommandList()
	{
		ES_SAFE_RELEASE(graphics_);
		ES_SAFE_RELEASE(commandList_);
		ES_SAFE_RELEASE(memoryPool_);
	}

	LLGI::Graphics* GetGraphics()
	{
		return graphics_;
	}

	LLGI::CommandList* GetInternal()
	{
		return commandList_;
	}

	LLGI::SingleFrameMemoryPool* GetMemoryPool()
	{
		return memoryPool_;
	}

	CommandListState GetState() const
	{
		return state_;
	}

	// Call only when starting a new recording, after the previous GPU use has completed.
	void ResetVertexBuffers()
	{
		for (auto& buffer : vertexBuffers_)
		{
			buffer->BeginWriteForCommandList();
		}
	}

	std::shared_ptr<EffekseerRenderer::VertexBufferRing> GetVertexBuffer(
		Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size)
	{
		for (auto& buffer : vertexBuffers_)
		{
			if (buffer->GetSize() == size)
			{
				return buffer;
			}
		}

		// Each recording owns its pages, including commands not yet submitted to the GPU.
		auto buffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(
			graphicsDevice, size, 1, EffekseerRenderer::VertexBufferRingMode::ExpandableForCommandList);
		if (!buffer->GetIsValid())
		{
			throw std::runtime_error("Failed to allocate a command list vertex buffer.");
		}
		vertexBuffers_.emplace_back(buffer);
		return buffer;
	}

	void SetState(CommandListState state)
	{
		state_ = state;
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

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_H__
