
#pragma once

#include "EffekseerTool/EffekseerTool.Sound.h"
#include "Graphics/PostProcess.h"
#include "Graphics/StaticMeshRenderer.h"
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
	RenderingMethodType RenderingMethod;

	bool IsGroundShown = false;
	int32_t GroundExtent = 10;
	float GroundHeight = 0.0f;
};

class GroundRenderer
{
private:
	std::shared_ptr<Effekseer::Tool::StaticMeshRenderer> groudMeshRenderer_;
	bool Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);
	int32_t GroundExtent = 10;

public:
	float GroundHeight = 0.0f;

	void SetExtent(int32_t extent);

	static std::shared_ptr<GroundRenderer> Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice);

	void UpdateGround();

	void Render(EffekseerRenderer::RendererRef renderer);
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
		RenderedEffectGenerator* generator_ = nullptr;

	public:
		DistortingCallback(efk::Graphics* graphics, RenderedEffectGenerator* generator);
		virtual ~DistortingCallback();

		bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;

		bool IsEnabled = true;
		bool Blit = true;
	};

protected:
	efk::Graphics* graphics_ = nullptr;
	Effekseer::ManagerRef manager_;
	EffekseerRenderer::RendererRef renderer_;
	Effekseer::EffectRef effect_;

	std::shared_ptr<Effekseer::Tool::StaticMesh> backgroundMesh_;
	std::shared_ptr<Effekseer::Tool::StaticMeshRenderer> backgroundRenderer_;
	Effekseer::TextureRef backgroundTexture_;
	Effekseer::Color backgroundMeshColor_{};

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

	//! depth texture to send a renderer for softparticles
	std::shared_ptr<efk::RenderTexture> depthRenderTextureMSAA;
	std::shared_ptr<efk::RenderTexture> depthRenderTexture;

	std::shared_ptr<efk::RenderTexture> backTexture;
	std::shared_ptr<efk::RenderTexture> viewRenderTexture;

	std::unique_ptr<efk::BloomEffect> m_bloomEffect;
	std::unique_ptr<efk::TonemapEffect> m_tonemapEffect;
	std::unique_ptr<efk::LinearToSRGBEffect> m_linearToSRGBEffect;

	std::unique_ptr<PostProcess> overdrawEffect_;

	bool m_isSRGBMode = false;
	uint32_t msaaSamples = 4;
	Effekseer::Backend::TextureFormatType textureFormat_ = Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;
	int32_t m_time = 0;
	int m_step = 1;
	std::vector<HandleHolder> handles_;

	EffekseerTool::Sound* sound_ = nullptr;

	DistortingCallback* m_distortionCallback = nullptr;

	Effekseer::Backend::ShaderRef whiteParticleSpriteShader_;
	Effekseer::Backend::ShaderRef whiteParticleModelShader_;

	RenderedEffectGeneratorConfig config_;

	std::shared_ptr<GroundRenderer> groundRenderer_;

	bool UpdateBackgroundMesh(const Color& backgroundColor);

public:
	void PlayEffect();
	RenderedEffectGenerator();
	virtual ~RenderedEffectGenerator();

	bool Initialize(efk::Graphics* graphics, Effekseer::RefPtr<Effekseer::Setting> setting, int32_t spriteCount, bool isSRGBMode);
	void Resize(const Vector2DI screenSize);
	void Update();
	void Update(int32_t frame);
	void Render();
	void Reset();
	void SetEffect(Effekseer::EffectRef effect);
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

	const EffekseerRenderer::RendererRef& GetRenderer() const
	{
		return renderer_;
	}

	Effekseer::ManagerRef GetMamanager() const
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

		if (groundRenderer_ != nullptr)
		{
			groundRenderer_->SetExtent(config_.GroundExtent);
			groundRenderer_->GroundHeight = config_.GroundHeight;
		}
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

	bool GetIsSRGBMode() const
	{
		return m_isSRGBMode;
	}
};

} // namespace Tool
} // namespace Effekseer
