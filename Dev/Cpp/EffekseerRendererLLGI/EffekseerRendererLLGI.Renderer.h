
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererLLGI.Base.h"

#include <LLGI.CommandList.h>
#include <LLGI.Constantbuffer.h>
#include <LLGI.Graphics.h>
#include <LLGI.IndexBuffer.h>
#include <LLGI.VertexBuffer.h>

namespace EffekseerRendererLLGI
{

class GraphicsDevice;

::Effekseer::TextureLoader* CreateTextureLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::ModelLoader* CreateModelLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

struct FixedShader
{
	std::vector<LLGI::DataStructure> StandardTexture_VS;
	std::vector<LLGI::DataStructure> StandardLightingTexture_VS;
	std::vector<LLGI::DataStructure> StandardDistortedTexture_VS;

	std::vector<LLGI::DataStructure> ModelShaderLightingTextureNormal_VS;
	std::vector<LLGI::DataStructure> ModelShaderTexture_VS;
	std::vector<LLGI::DataStructure> ModelShaderDistortionTexture_VS;

	std::vector<LLGI::DataStructure> StandardTexture_PS;
	std::vector<LLGI::DataStructure> StandardLightingTexture_PS;
	std::vector<LLGI::DataStructure> StandardDistortedTexture_PS;

	std::vector<LLGI::DataStructure> ModelShaderLightingTextureNormal_PS;
	std::vector<LLGI::DataStructure> ModelShaderTexture_PS;
	std::vector<LLGI::DataStructure> ModelShaderDistortionTexture_PS;
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

	/**
		@brief	\~English	Get background
				\~Japanese	背景を取得する
	*/
	virtual Effekseer::TextureData* GetBackground() = 0;

	/**
		@brief	\~English	Set background
				\~Japanese	背景を設定する
	*/
	virtual void SetBackground(LLGI::Texture* background) = 0;
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

class CommandList : public ::EffekseerRenderer::CommandList, public ::Effekseer::ReferenceObject
{
private:
	LLGI::Graphics* graphics_ = nullptr;
	LLGI::CommandList* commandList_ = nullptr;
	LLGI::SingleFrameMemoryPool* memoryPool_ = nullptr;

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

	LLGI::SingleFrameMemoryPool* GetMemoryPooll()
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

class DeviceObject;

class GraphicsDevice : public ::EffekseerRenderer::GraphicsDevice, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	std::set<DeviceObject*> deviceObjects_;

	LLGI::Graphics* graphics_ = nullptr;

	/**
		@brief	register an object
	*/
	void Register(DeviceObject* device);

	/**
		@brief	unregister an object
	*/
	void Unregister(DeviceObject* device);

public:
	GraphicsDevice(LLGI::Graphics* graphics)
		: graphics_(graphics)
	{
		ES_SAFE_ADDREF(graphics_);
	}

	virtual ~GraphicsDevice()
	{
		ES_SAFE_RELEASE(graphics_);
	}

	/**
		@brief
		\~english Call when device lost causes
		\~japanese デバイスロストが発生した時に実行する。
	*/
	void OnLostDevice();

	/**
		@brief
		\~english Call when device reset causes
		\~japanese デバイスがリセットされた時に実行する。
	*/
	void OnResetDevice();

	LLGI::Graphics* GetGraphics() const
	{
		return graphics_;
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

/**
@brief	\~English	Model
		\~Japanese	モデル
*/
class Model : public Effekseer::Model
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;

public:
	struct InternalModel
	{
		LLGI::VertexBuffer* VertexBuffer;
		LLGI::IndexBuffer* IndexBuffer;
		int32_t VertexCount;
		int32_t IndexCount;
		int32_t FaceCount;

		InternalModel()
		{
			VertexBuffer = nullptr;
			IndexBuffer = nullptr;
			VertexCount = 0;
			IndexCount = 0;
			FaceCount = 0;
		}

		virtual ~InternalModel()
		{
			ES_SAFE_RELEASE(VertexBuffer);
			ES_SAFE_RELEASE(IndexBuffer);
		}
	};

	InternalModel* InternalModels = nullptr;
	int32_t ModelCount;
	bool IsLoadedOnGPU = false;

	Model(uint8_t* data, int32_t size, GraphicsDevice* graphicsDevice)
		: Effekseer::Model(data, size)
		, InternalModels(nullptr)
		, graphicsDevice_(graphicsDevice)
		, ModelCount(0)
	{
		this->m_vertexSize = sizeof(VertexWithIndex);
		ES_SAFE_ADDREF(graphicsDevice_);
	}

	virtual ~Model()
	{
		ES_SAFE_DELETE_ARRAY(InternalModels);
		ES_SAFE_RELEASE(graphicsDevice_);
	}

	bool LoadToGPU();
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_H__
