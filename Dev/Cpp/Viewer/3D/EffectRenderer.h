
#pragma once

#include "../Graphics/Color.h"
#include "../Graphics/PostEffects.h"
#include "../Graphics/PostProcess.h"
#include "../Graphics/StaticMeshRenderer.h"
#include "../Graphics/efk.Graphics.h"
#include "../Math/Matrix44F.h"
#include "../Math/Vector2I.h"
#include "../Math/Vector3F.h"
#include <Effekseer.h>

namespace Effekseer
{
namespace Tool
{

struct EffectRendererParameter
{
	DistortionType Distortion = DistortionType::Current;
	Effekseer::Tool::Matrix44F CameraMatrix;
	Effekseer::Tool::Matrix44F ProjectionMatrix;
	Effekseer::Tool::Vector3F CameraPosition;
	Effekseer::Tool::Vector3F CameraFrontDirection;
	Effekseer::Tool::Color BackgroundColor;
	Effekseer::Tool::Vector3F LightDirection;
	Effekseer::Tool::Color LightColor;
	Effekseer::Tool::Color LightAmbientColor;
	RenderingMethodType RenderingMethod;

	bool IsGroundShown = false;
	int32_t GroundExtent = 10;
	float GroundHeight = 0.0f;
};

#if !defined(SWIG)
class RenderImage;

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

#endif

class EffectRenderer
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
		EffectRenderer* generator_ = nullptr;

	public:
		DistortingCallback(efk::Graphics* graphics, EffectRenderer* generator);
		virtual ~DistortingCallback();

		bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;

		bool IsEnabled = true;
		bool Blit = true;
	};

protected:
	std::shared_ptr<efk::Graphics> graphics_ = nullptr;
	Effekseer::ManagerRef manager_;
	EffekseerRenderer::RendererRef renderer_;
	Effekseer::EffectRef effect_;

	std::shared_ptr<Effekseer::Tool::StaticMesh> backgroundMesh_;
	std::shared_ptr<Effekseer::Tool::StaticMeshRenderer> backgroundRenderer_;
	Effekseer::TextureRef backgroundTexture_;
	Effekseer::Color backgroundMeshColor_{};

	Vector2I screenSize_;
	ViewerEffectBehavior behavior_;

	Effekseer::Backend::TextureRef hdrRenderTextureMSAA;
	Effekseer::Backend::TextureRef hdrRenderTexture;
	Effekseer::Backend::TextureRef linearRenderTexture;
	Effekseer::Backend::TextureRef depthTexture;

	//! depth texture to send a renderer for softparticles
	Effekseer::Backend::TextureRef depthRenderTextureMSAA;
	Effekseer::Backend::TextureRef depthRenderTexture;

	Effekseer::Backend::TextureRef backTexture;
	Effekseer::Backend::TextureRef viewRenderTexture;

	std::unique_ptr<PostProcess> overdrawEffect_;
	std::unique_ptr<BloomPostEffect> bloomEffect_;
	std::unique_ptr<LinearToSRGBPostEffect> linearToSRGBEffect_;
	std::unique_ptr<TonemapPostEffect> tonemapEffect_;

	bool m_isSRGBMode = false;
	uint32_t msaaSamples = 4;
	Effekseer::Backend::TextureFormatType textureFormat_ = Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;
	int32_t m_time = 0;
	int m_step = 1;
	std::vector<HandleHolder> handles_;

	DistortingCallback* m_distortionCallback = nullptr;

	Effekseer::Backend::ShaderRef whiteParticleSpriteShader_;
	Effekseer::Backend::ShaderRef whiteParticleModelShader_;

	std::shared_ptr<GroundRenderer> groundRenderer_;

	EffectRendererParameter parameter_;
	Effekseer::Tool::PostEffectParameter postEffectParameter_;

	bool UpdateBackgroundMesh(const Color& backgroundColor);

	virtual bool OnAfterInitialize()
	{
		return true;
	}

	virtual void OnAfterClear()
	{
	}

	virtual void OnBeforePostprocess()
	{
	}

public:
	EffectRenderer();
	virtual ~EffectRenderer();

#if !defined(SWIG)
	void PlayEffect();

	bool Initialize(std::shared_ptr<efk::Graphics> graphics, Effekseer::RefPtr<Effekseer::Setting> setting, int32_t spriteCount, bool isSRGBMode);
	void Resize(const Vector2I screenSize);
	void Update();
	void Update(int32_t frame);
	void Render(std::shared_ptr<RenderImage> renderImage = nullptr);
	void Reset();

	Effekseer::EffectRef GetEffect();
	void SetEffect(Effekseer::EffectRef effect);

	void SetBehavior(const ViewerEffectBehavior& behavior);
	const ViewerEffectBehavior& GetBehavior() const;

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

	void CopyToBack();

	void ResetBack();

	EffectRendererParameter GetConfig() const
	{
		return parameter_;
	}

	void SetConfig(const EffectRendererParameter& config)
	{
		parameter_ = config;

		if (groundRenderer_ != nullptr)
		{
			groundRenderer_->SetExtent(parameter_.GroundExtent);
			groundRenderer_->GroundHeight = parameter_.GroundHeight;
		}
	}

	Effekseer::Backend::TextureRef GetView() const
	{
		return viewRenderTexture;
	}

	BloomPostEffect* GetBloomEffect() const
	{
		return bloomEffect_.get();
	}

	TonemapPostEffect* GetTonemapEffect() const
	{
		return tonemapEffect_.get();
	}

	bool GetIsSRGBMode() const
	{
		return m_isSRGBMode;
	}

	Effekseer::Tool::PostEffectParameter GetPostEffectParameter() const;

	void SetPostEffectParameter(const Effekseer::Tool::PostEffectParameter& param);
#endif
};

} // namespace Tool
} // namespace Effekseer
