
#pragma once

#include "EffekseerTool/EffekseerTool.Sound.h"
#include "Graphics/efk.Graphics.h"
#include "Graphics/efk.PostEffects.h"
#include "Math/Vector2DI.h"
#include "ViewerEffectBehavior.h"
#include <Effekseer.h>

namespace Effekseer
{
namespace Tool
{

struct RenderedEffectGeneratorConfig
{
	Effekseer::Manager::DrawParameter DrawParameter;
	DistortionType Distortion = DistortionType::Current;
	Effekseer::Color BackgroundColor;
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ProjectionMatrix;

	Effekseer::Vector3D LightDirection;
	Effekseer::Color LightColor;
	Effekseer::Color LightAmbientColor;
	Effekseer::RenderMode RenderMode;
};

class RenderedEffectGenerator
{
	struct HandleHolder
	{
		::Effekseer::Handle Handle = 0;
		int32_t Time = 0;
		bool IsRootStopped = false;

		HandleHolder()
			: Handle(0)
			, Time(0)
		{
		}

		HandleHolder(::Effekseer::Handle handle, int32_t time = 0)
			: Handle(handle)
			, Time(time)
		{
		}
	};

	class DistortingCallback : public EffekseerRenderer::DistortingCallback
	{
	private:
		efk::Graphics* graphics_ = nullptr;
		RenderedEffectGenerator* renderer_ = nullptr;

	public:
		DistortingCallback(efk::Graphics* graphics, RenderedEffectGenerator* renderer);
		virtual ~DistortingCallback();

		bool OnDistorting();

		bool IsEnabled = true;
		bool Blit = true;
	};

protected:
	efk::Graphics* graphics_ = nullptr;
	Effekseer::Manager* manager_ = nullptr;
	EffekseerRenderer::Renderer* renderer_ = nullptr;
	Effekseer::Effect* effect_ = nullptr;

	efk::RenderTexture* renderTexture_ = nullptr;
	efk::DepthTexture* depthTexture_ = nullptr;

	Vector2DI screenSize_;
	ViewerEffectBehavior behavior_;

	std::unique_ptr<efk::BloomEffect> bloomEffect_;
	std::unique_ptr<efk::TonemapEffect> tonemapEffect_;
	std::unique_ptr<efk::LinearToSRGBEffect> linearToSRGBEffect_;

	std::shared_ptr<efk::RenderTexture> hdrRenderTextureMSAA;
	std::shared_ptr<efk::RenderTexture> hdrRenderTexture;
	std::shared_ptr<efk::RenderTexture> linearRenderTexture;
	std::shared_ptr<efk::DepthTexture> depthTexture;
	std::shared_ptr<efk::RenderTexture> backTexture;
	std::shared_ptr<efk::RenderTexture> viewRenderTexture;

	std::unique_ptr<efk::BloomEffect> m_bloomEffect;
	std::unique_ptr<efk::TonemapEffect> m_tonemapEffect;
	std::unique_ptr<efk::LinearToSRGBEffect> m_linearToSRGBEffect;

	bool m_isSRGBMode = false;
	uint32_t msaaSamples = 4;
	efk::TextureFormat textureFormat_ = efk::TextureFormat::RGBA16F;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;
	int32_t m_time = 0;
	int m_step = 1;
	std::vector<HandleHolder> handles_;

	EffekseerTool::Sound* sound_ = nullptr;

	DistortingCallback* m_distortionCallback = nullptr;

	RenderedEffectGeneratorConfig config_;

public:
	void PlayEffect();
	RenderedEffectGenerator();
	virtual ~RenderedEffectGenerator();

	bool Initialize(efk::Graphics* graphics, Effekseer::Setting* setting, int32_t spriteCount, bool isSRGBMode);
	void Resize(const Vector2DI screenSize);
	void Update();
	void Update(int32_t frame);
	void Render();
	void Reset();
	void SetEffect(Effekseer::Effect* effect);
	void SetBehavior(const ViewerEffectBehavior& behavior);

	int32_t GetInstanceCount()
	{
		if (m_time == 0)
			return 0;

		int32_t sum = 0;
		for (int i = 0; i < handles_.size(); i++)
		{
			auto count = manager_->GetInstanceCount(handles_[i].Handle);

			// Root
			if (!handles_[i].IsRootStopped)
				count--;

			if (!manager_->Exists(handles_[i].Handle))
				count = 0;

			sum += count;
		}

		return sum;
	}

	void SetStep(int32_t step)
	{
		m_step = step;
	}

	EffekseerRenderer::Renderer* GetRenderer() const
	{
		return renderer_;
	}

	Effekseer::Manager* GetMamanager() const
	{
		return manager_;
	}

	virtual void OnAfterClear()
	{
	}

	virtual void OnBeforePostprocess()
	{
	}

	void CopyToBack();

	void ResetBack();

	RenderedEffectGeneratorConfig GetConfig() const
	{
		return config_;
	}

	void SetConfig(const RenderedEffectGeneratorConfig& config)
	{
		config_ = config;
	}

	std::shared_ptr<efk::RenderTexture> GetView() const
	{
		return viewRenderTexture;
	}

	efk::BloomEffect* GetBloomEffect() const
	{
		return m_bloomEffect.get();
	}
	efk::TonemapEffect* GetTonemapEffect() const
	{
		return m_tonemapEffect.get();
	}

	void SetSound(EffekseerTool::Sound* sound)
	{
		sound_ = sound;
	}
};

} // namespace Tool
} // namespace Effekseer
