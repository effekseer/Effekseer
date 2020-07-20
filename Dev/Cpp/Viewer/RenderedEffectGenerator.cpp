#include "RenderedEffectGenerator.h"

#ifdef _WIN32
#include "Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif

#include "Graphics/Platform/GL/efk.GraphicsGL.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Effekseer
{
namespace Tool
{

RenderedEffectGenerator::DistortingCallback::DistortingCallback(efk::Graphics* graphics, RenderedEffectGenerator* renderer)
	: graphics_(graphics)
	, renderer_(renderer)
{
}

RenderedEffectGenerator::DistortingCallback::~DistortingCallback()
{
}

bool RenderedEffectGenerator::DistortingCallback::OnDistorting()
{
	if (Blit)
	{
		renderer_->CopyToBack();
	}

	return IsEnabled;
}

void RenderedEffectGenerator::PlayEffect()
{
	assert(effect_ != NULL);

	for (int32_t z = 0; z < behavior_.CountZ; z++)
	{
		for (int32_t y = 0; y < behavior_.CountY; y++)
		{
			for (int32_t x = 0; x < behavior_.CountX; x++)
			{
				auto lenX = behavior_.Distance * (behavior_.CountX - 1);
				auto lenY = behavior_.Distance * (behavior_.CountY - 1);
				auto lenZ = behavior_.Distance * (behavior_.CountZ - 1);

				auto posX = behavior_.Distance * x - lenX / 2.0f;
				auto posY = behavior_.Distance * y - lenY / 2.0f;
				auto posZ = behavior_.Distance * z - lenZ / 2.0f;

				posX += behavior_.PositionX;
				posY += behavior_.PositionY;
				posZ += behavior_.PositionZ;

				HandleHolder handleHolder(manager_->Play(effect_, posX, posY, posZ));

				Effekseer::Matrix43 mat, matTra, matRot, matScale;
				matTra.Translation(posX, posY, posZ);
				matRot.RotationZXY(m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y);
				matScale.Scaling(m_rootScale.X, m_rootScale.Y, m_rootScale.Z);

				mat.Indentity();
				Effekseer::Matrix43::Multiple(mat, mat, matScale);
				Effekseer::Matrix43::Multiple(mat, mat, matRot);
				Effekseer::Matrix43::Multiple(mat, mat, matTra);

				manager_->SetMatrix(handleHolder.Handle, mat);
				manager_->SetSpeed(handleHolder.Handle, behavior_.PlaybackSpeed);

				handles_.push_back(handleHolder);

				if (behavior_.AllColorR != 255 || behavior_.AllColorG != 255 || behavior_.AllColorB != 255 ||
					behavior_.AllColorA != 255)
				{
					manager_->SetAllColor(handleHolder.Handle,
										  Effekseer::Color(behavior_.AllColorR,
														   behavior_.AllColorG,
														   behavior_.AllColorB,
														   behavior_.AllColorA));
				}
			}
		}
	}

	m_time = 0;
	m_rootLocation.X = behavior_.PositionX;
	m_rootLocation.Y = behavior_.PositionY;
	m_rootLocation.Z = behavior_.PositionZ;
	m_rootRotation.X = behavior_.RotationX;
	m_rootRotation.Y = behavior_.RotationY;
	m_rootRotation.Z = behavior_.RotationZ;
	m_rootScale.X = behavior_.ScaleX;
	m_rootScale.Y = behavior_.ScaleY;
	m_rootScale.Z = behavior_.ScaleZ;
}

RenderedEffectGenerator::RenderedEffectGenerator()
{
}

RenderedEffectGenerator ::~RenderedEffectGenerator()
{
	ES_SAFE_RELEASE(effect_);

	if (renderer_ != nullptr)
	{
		renderer_->Destroy();
		renderer_ = nullptr;
	}

	if (manager_ != nullptr)
	{
		manager_->Destroy();
		manager_ = nullptr;
	}
}

bool RenderedEffectGenerator::Initialize(efk::Graphics* graphics, Effekseer::Setting* setting, int32_t spriteCount, bool isSRGBMode)
{
	graphics_ = graphics;

	manager_ = ::Effekseer::Manager::Create(spriteCount);
	manager_->SetSetting(setting);
	m_isSRGBMode = isSRGBMode;

#ifdef _WIN32
	if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
	{
		auto g = (efk::GraphicsDX11*)graphics;
		renderer_ = ::EffekseerRendererDX11::Renderer::Create(g->GetDevice(), g->GetContext(), spriteCount, D3D11_COMPARISON_LESS_EQUAL, true);
	}
#endif

	if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
	{
		auto g = (efk::GraphicsGL*)graphics;
		renderer_ = ::EffekseerRendererGL::Renderer::Create(spriteCount, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
	}

	m_distortionCallback = new DistortingCallback(graphics, this);
	renderer_->SetDistortingCallback(m_distortionCallback);

	// create postprocessings
	m_bloomEffect.reset(efk::PostEffect::CreateBloom(graphics, renderer_));
	m_tonemapEffect.reset(efk::PostEffect::CreateTonemap(graphics, renderer_));
	m_linearToSRGBEffect.reset(efk::PostEffect::CreateLinearToSRGB(graphics, renderer_));

	if (!(m_bloomEffect != nullptr && m_bloomEffect->GetIsValid() &&
		  m_tonemapEffect != nullptr && m_tonemapEffect->GetIsValid() &&
		  m_linearToSRGBEffect != nullptr && m_linearToSRGBEffect->GetIsValid()))
	{
		spdlog::trace("Failed PostProcessing");
		return false;
	}

	spdlog::trace("OK PostProcessing");

	auto msaaSampleHDR = graphics->GetMultisampleLevel(efk::TextureFormat::RGBA16F);
	auto msaaSample = graphics->GetMultisampleLevel(efk::TextureFormat::RGBA8U);

	auto isHDRSupported = graphics->CheckFormatSupport(efk::TextureFormat::RGBA16F, efk::TextureFeatureType::Texture2D);
	auto isMSAAHDRSupported =
		graphics->CheckFormatSupport(efk::TextureFormat::RGBA16F, efk::TextureFeatureType::Texture2D) &&
		graphics->CheckFormatSupport(efk::TextureFormat::RGBA16F, efk::TextureFeatureType::MultisampledTexture2DResolve) &&
		graphics->CheckFormatSupport(efk::TextureFormat::RGBA16F, efk::TextureFeatureType::MultisampledTexture2DRenderTarget) &&
		msaaSampleHDR > 1;
	auto isMSAASupported = msaaSample > 1;

	if (isHDRSupported || isMSAAHDRSupported)
	{
		textureFormat_ = efk::TextureFormat::RGBA16F;

		if (isMSAAHDRSupported)
		{
			msaaSamples = Effekseer::Min(4, msaaSampleHDR);
		}
		else
		{
			msaaSamples = 1;
		}
	}
	else
	{
		if (isMSAASupported)
		{
			msaaSamples = Effekseer::Min(4, msaaSample);
		}
		else
		{
			msaaSamples = 1;
		}

		textureFormat_ = efk::TextureFormat::RGBA8U;
	}

	spdlog::trace("HDR {} {}", isMSAAHDRSupported, isHDRSupported);

	spdlog::trace("MSAA {} {}", msaaSampleHDR, msaaSample);

	spdlog::trace("OK Check format");

	::Effekseer::SpriteRenderer* sprite_renderer = renderer_->CreateSpriteRenderer();
	::Effekseer::RibbonRenderer* ribbon_renderer = renderer_->CreateRibbonRenderer();
	::Effekseer::RingRenderer* ring_renderer = renderer_->CreateRingRenderer();
	::Effekseer::ModelRenderer* model_renderer = renderer_->CreateModelRenderer();
	::Effekseer::TrackRenderer* track_renderer = renderer_->CreateTrackRenderer();

	if (sprite_renderer == NULL)
	{
		spdlog::trace("FAIL : CreateSpriteRenderer");
		manager_->Destroy();
		manager_ = NULL;
		renderer_->Destroy();
		renderer_ = NULL;
		return false;
	}

	spdlog::trace("OK : CreateRenderers");

	manager_->SetSpriteRenderer(sprite_renderer);
	manager_->SetRibbonRenderer(ribbon_renderer);
	manager_->SetRingRenderer(ring_renderer);
	manager_->SetModelRenderer(model_renderer);
	manager_->SetTrackRenderer(track_renderer);

	return true;
}

void RenderedEffectGenerator::Resize(const Vector2DI screenSize)
{
	if (screenSize_ == screenSize)
	{
		return;
	}

	screenSize_ = screenSize;

	hdrRenderTextureMSAA = nullptr;

	hdrRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
	hdrRenderTexture->Initialize(screenSize, textureFormat_);
	depthTexture = std::shared_ptr<efk::DepthTexture>(efk::DepthTexture::Create(graphics_));
	depthTexture->Initialize(screenSize_.X, screenSize_.Y, msaaSamples);

	if (msaaSamples > 1)
	{
		hdrRenderTextureMSAA = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
		hdrRenderTextureMSAA->Initialize(screenSize, textureFormat_, msaaSamples);
	}

	backTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
	backTexture->Initialize(screenSize, textureFormat_, 1);

	if (m_isSRGBMode)
	{
		linearRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
		linearRenderTexture->Initialize(screenSize, textureFormat_);
	}

	viewRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
	viewRenderTexture->Initialize(screenSize, efk::TextureFormat::RGBA8U);
}

void RenderedEffectGenerator::Update()
{
	if (behavior_.TimeSpan > 0 && m_time > 0 && m_time % behavior_.TimeSpan == 0)
	{
		PlayEffect();
	}

	// dynamic parameter
	for (auto h : handles_)
	{
		manager_->SetDynamicInput(h.Handle, 0, behavior_.DynamicInput1);
		manager_->SetDynamicInput(h.Handle, 1, behavior_.DynamicInput2);
		manager_->SetDynamicInput(h.Handle, 2, behavior_.DynamicInput3);
		manager_->SetDynamicInput(h.Handle, 3, behavior_.DynamicInput4);
	}

	for (auto h : handles_)
	{
		manager_->SetSpeed(h.Handle, behavior_.PlaybackSpeed);
	}

	if (m_time % m_step == 0)
	{
		m_rootLocation.X += behavior_.PositionVelocityX;
		m_rootLocation.Y += behavior_.PositionVelocityY;
		m_rootLocation.Z += behavior_.PositionVelocityZ;

		m_rootRotation.X += behavior_.RotationVelocityX;
		m_rootRotation.Y += behavior_.RotationVelocityY;
		m_rootRotation.Z += behavior_.RotationVelocityZ;

		m_rootScale.X += behavior_.ScaleVelocityX;
		m_rootScale.Y += behavior_.ScaleVelocityY;
		m_rootScale.Z += behavior_.ScaleVelocityZ;

		int32_t index = 0;

		for (int32_t z = 0; z < behavior_.CountZ && index < handles_.size(); z++)
		{
			for (int32_t y = 0; y < behavior_.CountY && index < handles_.size(); y++)
			{
				for (int32_t x = 0; x < behavior_.CountX && index < handles_.size(); x++)
				{
					auto lenX = behavior_.Distance * (behavior_.CountX - 1);
					auto lenY = behavior_.Distance * (behavior_.CountY - 1);
					auto lenZ = behavior_.Distance * (behavior_.CountZ - 1);

					auto posX = behavior_.Distance * x - lenX / 2.0f;
					auto posY = behavior_.Distance * y - lenY / 2.0f;
					auto posZ = behavior_.Distance * z - lenZ / 2.0f;

					posX += m_rootLocation.X;
					posY += m_rootLocation.Y;
					posZ += m_rootLocation.Z;

					Effekseer::Matrix43 mat, matTra, matRot, matScale;
					matTra.Translation(posX, posY, posZ);
					matRot.RotationZXY(m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y);
					matScale.Scaling(m_rootScale.X, m_rootScale.Y, m_rootScale.Z);

					mat.Indentity();
					Effekseer::Matrix43::Multiple(mat, mat, matScale);
					Effekseer::Matrix43::Multiple(mat, mat, matRot);
					Effekseer::Matrix43::Multiple(mat, mat, matTra);

					manager_->SetMatrix(handles_[index].Handle, mat);

					manager_->SetTargetLocation(handles_[index].Handle,
												behavior_.TargetPositionX,
												behavior_.TargetPositionY,
												behavior_.TargetPositionZ);
					index++;
				}
			}
		}

		for (size_t i = 0; i < handles_.size(); i++)
		{
			if (handles_[i].Time >= behavior_.RemovedTime)
			{
				manager_->StopRoot(handles_[i].Handle);
				handles_[i].IsRootStopped = true;
			}
		}

		manager_->Update((float)m_step);
		renderer_->SetTime(m_time / 60.0f);

		for (size_t i = 0; i < handles_.size(); i++)
		{
			handles_[i].Time += m_step;
		}
	}

	m_time += m_step;
}

void RenderedEffectGenerator::Update(int32_t frame)
{
	if (frame <= 0)
	{
	}
	else if (frame == 1)
	{
		Update();
	}
	else
	{
		bool mute = false;

		if (sound_ != nullptr)
		{
			mute = sound_->GetMute();
			sound_->SetMute(true);
		}

		for (size_t i = 0; i < handles_.size(); i++)
		{
			manager_->SetShown(handles_[i].Handle, false);
		}

		for (int i = 0; i < frame - 1; i++)
		{
			Update();
		}

		for (size_t i = 0; i < handles_.size(); i++)
		{
			manager_->SetShown(handles_[i].Handle, true);
		}

		if (sound_ != nullptr)
		{
			sound_->SetMute(mute);
		}

		manager_->Update(static_cast<float>(m_step));
		m_time += m_step;
		renderer_->SetTime(m_time / 60.0f);
	}
}

void RenderedEffectGenerator::Render()
{
	renderer_->SetCameraMatrix(config_.CameraMatrix);
	renderer_->SetProjectionMatrix(config_.ProjectionMatrix);
	renderer_->SetLightDirection(config_.LightDirection);
	renderer_->SetLightColor(config_.LightColor);
	renderer_->SetLightAmbientColor(config_.LightAmbientColor);

	if (msaaSamples > 1)
	{
		graphics_->SetRenderTarget(hdrRenderTextureMSAA.get(), depthTexture.get());
	}
	else
	{
		graphics_->SetRenderTarget(hdrRenderTexture.get(), depthTexture.get());
	}

	// clear
	graphics_->Clear(config_.BackgroundColor);

	OnAfterClear();

	// Distoriton
	if (config_.Distortion == DistortionType::Current)
	{
		CopyToBack();

		m_distortionCallback->Blit = false;
		m_distortionCallback->IsEnabled = true;
	}
	else if (config_.Distortion == DistortionType::Effekseer120)
	{
		m_distortionCallback->Blit = true;
		m_distortionCallback->IsEnabled = true;
	}
	else
	{
		ResetBack();

		m_distortionCallback->Blit = false;
		m_distortionCallback->IsEnabled = false;
	}

	renderer_->BeginRendering();

	if (config_.Distortion == DistortionType::Current)
	{
		manager_->DrawBack(config_.DrawParameter);

		// HACK
		renderer_->EndRendering();

		CopyToBack();

		// HACK
		renderer_->BeginRendering();
		manager_->DrawFront(config_.DrawParameter);
	}
	else
	{
		manager_->Draw(config_.DrawParameter);
	}

	renderer_->EndRendering();

	ResetBack();

	OnBeforePostprocess();

	// all post effects are disabled
	if (m_bloomEffect == nullptr && m_tonemapEffect == nullptr && m_linearToSRGBEffect == nullptr)
	{
		return;
	}

	if (msaaSamples > 1)
	{
		graphics_->ResolveRenderTarget(hdrRenderTextureMSAA.get(), hdrRenderTexture.get());
	}

	// Bloom processing (specifying the same target for src and dest is faster)
	m_bloomEffect->Render(hdrRenderTexture.get(), hdrRenderTexture.get());

	// Tone map processing
	auto tonemapTerget = viewRenderTexture.get();
	if (m_isSRGBMode)
	{
		tonemapTerget = linearRenderTexture.get();
	}

	m_tonemapEffect->Render(hdrRenderTexture.get(), tonemapTerget);

	if (m_isSRGBMode)
	{
		m_linearToSRGBEffect->Render(tonemapTerget, viewRenderTexture.get());
	}

	graphics_->SetRenderTarget(nullptr, nullptr);
}

void RenderedEffectGenerator::Reset()
{
	for (size_t i = 0; i < handles_.size(); i++)
	{
		manager_->StopEffect(handles_[i].Handle);
	}
	handles_.clear();

	manager_->Update();
}

void RenderedEffectGenerator::SetEffect(Effekseer::Effect* effect)
{
	handles_.clear();
	ES_SAFE_ADDREF(effect);
	ES_SAFE_RELEASE(effect_);
	effect_ = effect;
}

void RenderedEffectGenerator::SetBehavior(const ViewerEffectBehavior& behavior)
{
	behavior_ = behavior;
}

void RenderedEffectGenerator::CopyToBack()
{
	if (msaaSamples > 1)
	{
		graphics_->CopyTo(hdrRenderTextureMSAA.get(), backTexture.get());
	}
	else
	{
		graphics_->CopyTo(hdrRenderTexture.get(), backTexture.get());
	}

	if (graphics_->GetDeviceType() == efk::DeviceType::OpenGL)
	{
		::Effekseer::TextureData textureData;
		textureData.HasMipmap = false;
		textureData.Width = 1024;  // dummy
		textureData.Height = 1024; // dummy
		textureData.TextureFormat = Effekseer::TextureFormatType::ABGR8;
		textureData.UserPtr = nullptr;
		textureData.UserID = (GLuint)(size_t)backTexture->GetViewID();
		auto r = (::EffekseerRendererGL::Renderer*)renderer_;
		r->SetBackgroundTexture(&textureData);
	}
#ifdef _WIN32
	else if (graphics_->GetDeviceType() == efk::DeviceType::DirectX11)
	{
		auto r = (::EffekseerRendererDX11::Renderer*)renderer_;
		r->SetBackground((ID3D11ShaderResourceView*)backTexture->GetViewID());
	}
	else
	{
		assert(0);
	}
#endif
}

void RenderedEffectGenerator::ResetBack()
{
	if (graphics_->GetDeviceType() == efk::DeviceType::OpenGL)
	{
		auto r = (::EffekseerRendererGL::Renderer*)renderer_;
		r->SetBackground(0);
	}
#ifdef _WIN32
	else if (graphics_->GetDeviceType() == efk::DeviceType::DirectX11)
	{
		auto r = (::EffekseerRendererDX11::Renderer*)renderer_;
		r->SetBackground(nullptr);
	}
#endif
	else
	{
		assert(0);
	}
}

} // namespace Tool
} // namespace Effekseer