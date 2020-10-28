﻿
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererLLGI.Base.h"

#include "GraphicsDevice.h"
#include <LLGI.CommandList.h>
#include <LLGI.Constantbuffer.h>
#include <LLGI.Graphics.h>
#include <LLGI.IndexBuffer.h>
#include <LLGI.VertexBuffer.h>

namespace EffekseerRendererLLGI
{

::Effekseer::TextureLoader* CreateTextureLoader(Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::ModelLoader* CreateModelLoader(Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

struct FixedShader
{
	std::vector<LLGI::DataStructure> SpriteUnlit_VS;
	std::vector<LLGI::DataStructure> SpriteLit_VS;
	std::vector<LLGI::DataStructure> SpriteDistortion_VS;
	std::vector<LLGI::DataStructure> ModelUnlit_VS;
	std::vector<LLGI::DataStructure> ModelLit_VS;
	std::vector<LLGI::DataStructure> ModelDistortion_VS;

	std::vector<LLGI::DataStructure> SpriteUnlit_PS;
	std::vector<LLGI::DataStructure> SpriteLit_PS;
	std::vector<LLGI::DataStructure> SpriteDistortion_PS;
	std::vector<LLGI::DataStructure> ModelUnlit_PS;
	std::vector<LLGI::DataStructure> ModelLit_PS;
	std::vector<LLGI::DataStructure> ModelDistortion_PS;

	std::vector<LLGI::DataStructure> AdvancedSpriteUnlit_VS;
	std::vector<LLGI::DataStructure> AdvancedSpriteLit_VS;
	std::vector<LLGI::DataStructure> AdvancedSpriteDistortion_VS;
	std::vector<LLGI::DataStructure> AdvancedModelUnlit_VS;
	std::vector<LLGI::DataStructure> AdvancedModelLit_VS;
	std::vector<LLGI::DataStructure> AdvancedModelDistortion_VS;

	std::vector<LLGI::DataStructure> AdvancedSpriteUnlit_PS;
	std::vector<LLGI::DataStructure> AdvancedSpriteLit_PS;
	std::vector<LLGI::DataStructure> AdvancedSpriteDistortion_PS;
	std::vector<LLGI::DataStructure> AdvancedModelUnlit_PS;
	std::vector<LLGI::DataStructure> AdvancedModelLit_PS;
	std::vector<LLGI::DataStructure> AdvancedModelDistortion_PS;
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

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_H__
