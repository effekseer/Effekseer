#pragma once

#include <Effekseer.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"

static const int WindowWidth = 320;
static const int WindowHeight = 240;

struct EffectPlatformInitializingParameter
{
	bool VSync = true;
	bool IsUpdatedByHandle = false;
	bool IsCullingCreated = true;
	int InstanceCount = 8000;
};

class EffectPlatform
{
private:
	bool isInitialized_ = false;
	bool isTerminated_ = false;
	float time_ = 0;
	EffectPlatformInitializingParameter initParam_;

	Effekseer::Manager* manager_ = nullptr;
	EffekseerRenderer::Renderer* renderer_ = nullptr;
	std::vector<Effekseer::Handle> effectHandles_;

	void CreateCheckeredPattern(int width, int height, uint32_t* pixels);

protected:
	bool isOpenGLMode_ = false;

protected:
	std::vector<Effekseer::Effect*> effects_;
	std::vector<std::vector<uint8_t>> buffers_;
	std::vector<uint32_t> checkeredPattern_;

	EffekseerRenderer::Renderer* GetRenderer() const;
	virtual void* GetNativePtr(int32_t index)
	{
		return nullptr;
	}
	virtual EffekseerRenderer::Renderer* CreateRenderer() = 0;
	virtual void InitializeDevice(const EffectPlatformInitializingParameter& param)
	{
	}
	virtual void PreDestroyDevice()
	{
	}
	virtual void DestroyDevice()
	{
	}
	virtual void BeginRendering()
	{
	}
	virtual void EndRendering()
	{
	}
	virtual void Present()
	{
	}
	virtual bool DoEvent()
	{
		return false;
	}

public:
	EffectPlatform();
	virtual ~EffectPlatform();

	void Initialize(const EffectPlatformInitializingParameter& param);
	void Terminate();

	Effekseer::Handle Play(const char16_t* path, int32_t startFrame = 0);

	bool Update();

	bool Draw();

	void StopAllEffects();

	virtual bool TakeScreenshot(const char* path)
	{
		return false;
	}

	virtual bool SetFullscreen(bool isFullscreen)
	{
		return false;
	}

	Effekseer::Manager* GetManager() const
	{
		return manager_;
	}

	const std::vector<Effekseer::Effect*>& GetEffects() const { return effects_; }
};