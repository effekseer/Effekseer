#include "EffectRenderer.h"
#include "../Sound/SoundDevice.h"
#include "Effect.h"
#include "EffectSetting.h"

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"

namespace WhiteParticle_Model_VS
{
#include <EffekseerRendererDX11/ShaderHeader/model_unlit_vs.h>
}

namespace WhiteParticle_Sprite_VS
{
#include <EffekseerRendererDX11/ShaderHeader/sprite_unlit_vs.h>
}

namespace WhiteParticle_PS
{
#include "../Shaders/HLSL_DX11_Header/white_particle_ps.h"
}

namespace PostEffect_Basic_VS
{
#include "../Shaders/HLSL_DX11_Header/postfx_basic_vs.h"
}

namespace PostEffect_Overdraw_PS
{
#include "../Shaders/HLSL_DX11_Header/postfx_overdraw_ps.h"
}

#endif

#include <EffekseerRendererGL/EffekseerRendererGL.ModelRenderer.h>

#include "../Shaders/GLSL_GL_Header/line_ps.h"
#include "../Shaders/GLSL_GL_Header/line_vs.h"

#include "../Shaders/GLSL_GL_Header/postfx_basic_vs.h"
#include "../Shaders/GLSL_GL_Header/postfx_overdraw_ps.h"
#include "../Shaders/GLSL_GL_Header/white_particle_ps.h"

#include "../Graphics/Platform/GL/efk.GraphicsGL.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <EffekseerRendererGL/ShaderHeader/model_unlit_vs.h>
#include <EffekseerRendererGL/ShaderHeader/sprite_unlit_vs.h>

#include "../GUI/RenderImage.h"

namespace Effekseer
{
namespace Tool
{

bool GroundRenderer::Initialize(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	groudMeshRenderer_ = Effekseer::Tool::StaticMeshRenderer::Create(graphicsDevice);

	if (groudMeshRenderer_ == nullptr)
	{
		return false;
	}

	Effekseer::CustomVector<Effekseer::Tool::StaticMeshVertex> vbData(4);
	Effekseer::CustomVector<int32_t> ibData = {0, 1, 2, 0, 2, 3};

	auto groudMesh = Effekseer::Tool::StaticMesh::Create(graphicsDevice, vbData, ibData, true);

	// Create checker patterns
	Effekseer::Backend::TextureParameter texParams;
	texParams.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	texParams.Size = {128, 128};

	Effekseer::CustomVector<uint8_t> initialData;
	initialData.resize(texParams.Size[0] * texParams.Size[1] * 4);

	for (size_t i = 0; i < initialData.size() / 4; i++)
	{
		const size_t x = i % texParams.Size[0] * 2 / texParams.Size[0];
		const size_t y = i / texParams.Size[1] * 2 / texParams.Size[1];
		if (x ^ y == 0)
		{
			initialData[i * 4 + 0] = 90;
			initialData[i * 4 + 1] = 90;
			initialData[i * 4 + 2] = 90;
			initialData[i * 4 + 3] = 255;
		}
		else
		{
			initialData[i * 4 + 0] = 60;
			initialData[i * 4 + 1] = 60;
			initialData[i * 4 + 2] = 60;
			initialData[i * 4 + 3] = 255;
		}
	}
	groudMesh->Texture = graphicsDevice->CreateTexture(texParams, initialData);

	groudMeshRenderer_->SetStaticMesh(groudMesh);

	UpdateGround();

	return true;
}

void GroundRenderer::SetExtent(int32_t extent)
{
	if (GroundExtent == extent)
	{
		return;
	}

	GroundExtent = extent;
	UpdateGround();
}

void GroundRenderer::UpdateGround()
{
	std::array<Effekseer::Tool::StaticMeshVertex, 4> vbData;

	vbData[0].Pos = {-(float)GroundExtent, 0.0f, -(float)GroundExtent};
	vbData[0].VColor = {255, 255, 255, 255};
	vbData[0].UV = {0.0f, 0.0f};
	vbData[1].Pos = {(float)GroundExtent, 0.0f, -(float)GroundExtent};
	vbData[1].VColor = {255, 255, 255, 255};
	vbData[1].UV = {(float)GroundExtent, 0.0f};
	vbData[2].Pos = {(float)GroundExtent, 0.0f, (float)GroundExtent};
	vbData[2].VColor = {255, 255, 255, 255};
	vbData[2].UV = {(float)GroundExtent, (float)GroundExtent};
	vbData[3].Pos = {-(float)GroundExtent, 0.0f, (float)GroundExtent};
	vbData[3].VColor = {255, 255, 255, 255};
	vbData[3].UV = {0.0f, (float)GroundExtent};

	for (auto& vb : vbData)
	{
		vb.Normal = {0.0f, 1.0f, 0.0f};
	}

	groudMeshRenderer_->GetStaticMesh()->GetVertexBuffer()->UpdateData(
		vbData.data(), static_cast<int32_t>(vbData.size() * sizeof(Effekseer::Tool::StaticMeshVertex)), 0);
}

void GroundRenderer::Render(EffekseerRenderer::RendererRef renderer)
{
	Effekseer::Tool::RendererParameter param{};
	param.CameraMatrix = renderer->GetCameraMatrix();
	param.ProjectionMatrix = renderer->GetProjectionMatrix();
	param.WorldMatrix.Translation(0.0f, GroundHeight, 0.0f);
	param.DirectionalLightDirection = renderer->GetLightDirection().ToFloat4();
	param.DirectionalLightColor = renderer->GetLightColor().ToFloat4();
	param.AmbientLightColor = renderer->GetLightAmbientColor().ToFloat4();
	groudMeshRenderer_->Render(param);
}

std::shared_ptr<GroundRenderer> GroundRenderer::Create(Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice)
{
	auto ret = std::make_shared<GroundRenderer>();
	if (ret->Initialize(graphicsDevice))
	{
		return ret;
	}
	return nullptr;
}

EffectRenderer::DistortingCallback::DistortingCallback(efk::Graphics* graphics, EffectRenderer* generator)
	: graphics_(graphics)
	, generator_(generator)
{
}

EffectRenderer::DistortingCallback::~DistortingCallback()
{
}

bool EffectRenderer::DistortingCallback::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	if (Blit)
	{
		generator_->CopyToBack();
	}

	return IsEnabled;
}

bool EffectRenderer::UpdateBackgroundMesh(const Color& backgroundColor)
{
	if (backgroundMesh_ != nullptr && !(backgroundColor != backgroundMeshColor_))
		return true;

	backgroundMeshColor_ = backgroundColor;

	const float eps = 0.00001f;

	Effekseer::CustomVector<Effekseer::Tool::StaticMeshVertex> vbData;
	vbData.resize(4);
	vbData[0].Pos = {-1.0f, 1.0f, 1.0f - eps};
	vbData[1].Pos = {1.0f, 1.0f, 1.0f - eps};
	vbData[2].Pos = {1.0f, -1.0f, 1.0f - eps};
	vbData[3].Pos = {-1.0f, -1.0f, 1.0f - eps};

	for (auto& vb : vbData)
	{
		vb.UV[0] = (vb.Pos[0] + 1.0f) / 2.0f;
		vb.UV[1] = 1.0f - (vb.Pos[1] + 1.0f) / 2.0f;

		vb.Normal = {0.0f, 1.0f, 0.0f};
		vb.VColor = backgroundMeshColor_;
	}
	Effekseer::CustomVector<int32_t> ibData;
	ibData.resize(6);
	ibData[0] = 0;
	ibData[1] = 1;
	ibData[2] = 2;
	ibData[3] = 0;
	ibData[4] = 2;
	ibData[5] = 3;

	backgroundMesh_ = Effekseer::Tool::StaticMesh::Create(graphics_->GetGraphics()->GetGraphicsDevice(), vbData, ibData);
	if (!backgroundMesh_)
	{
		return false;
	}

	backgroundMesh_->IsLit = false;

	backgroundRenderer_->SetStaticMesh(backgroundMesh_);

	return true;
}

void EffectRenderer::CopyToBack()
{
	if (msaaSamples > 1)
	{
		graphics_->GetGraphics()->CopyTo(hdrRenderTextureMSAA, backTexture);
	}
	else
	{
		graphics_->GetGraphics()->CopyTo(hdrRenderTexture, backTexture);
	}

	renderer_->SetBackground(backTexture);
}

void EffectRenderer::ResetBack()
{
	renderer_->SetBackground(nullptr);
}

EffectRenderer::EffectRenderer()
{
}

EffectRenderer ::~EffectRenderer()
{
}

bool EffectRenderer::Initialize(std::shared_ptr<GraphicsDevice> graphicsDevice,
								std::shared_ptr<SoundDevice> soundDevice,
								std::shared_ptr<EffectSetting> setting,
								int32_t spriteCount,
								bool isSRGBMode)
{
	graphics_ = graphicsDevice;

	manager_ = ::Effekseer::Manager::Create(spriteCount);
	// manager_->LaunchWorkerThreads(4);
	manager_->SetSetting(setting->GetSetting());
	m_isSRGBMode = isSRGBMode;

	auto graphics = graphicsDevice->GetGraphics();

#ifdef _WIN32
	if (graphicsDevice->GetGraphics()->GetDeviceType() == Effekseer::Tool::DeviceType::DirectX11)
	{
		auto g = (efk::GraphicsDX11*)graphics.get();
		renderer_ = ::EffekseerRendererDX11::Renderer::Create(g->GetDevice(), g->GetContext(), spriteCount, D3D11_COMPARISON_LESS_EQUAL, true);
	}
#endif

	if (graphics->GetDeviceType() == Effekseer::Tool::DeviceType::OpenGL)
	{
		auto g = (efk::GraphicsGL*)graphics.get();
		renderer_ = ::EffekseerRendererGL::Renderer::Create(spriteCount, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
	}

	m_distortionCallback = new DistortingCallback(graphics.get(), this);
	renderer_->SetDistortingCallback(m_distortionCallback);

	// create postprocessings
	bloomEffect_ = std::make_unique<BloomPostEffect>(graphics->GetGraphicsDevice());
	linearToSRGBEffect_ = std::make_unique<LinearToSRGBPostEffect>(graphics->GetGraphicsDevice());
	tonemapEffect_ = std::make_unique<TonemapPostEffect>(graphics->GetGraphicsDevice());

	if (!(bloomEffect_ != nullptr && bloomEffect_->GetIsValid() &&
		  tonemapEffect_ != nullptr && tonemapEffect_->GetIsValid() &&
		  linearToSRGBEffect_ != nullptr && linearToSRGBEffect_->GetIsValid()))
	{
		spdlog::trace("Failed PostProcessing");
		return false;
	}

	spdlog::trace("OK PostProcessing");

	auto msaaSampleHDR = graphics->GetMultisampleLevel(Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
	auto msaaSample = graphics->GetMultisampleLevel(Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM);

	auto isHDRSupported = graphics->CheckFormatSupport(Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT, efk::TextureFeatureType::Texture2D);
	auto isMSAAHDRSupported =
		graphics->CheckFormatSupport(Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT, efk::TextureFeatureType::Texture2D) &&
		graphics->CheckFormatSupport(Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT, efk::TextureFeatureType::MultisampledTexture2DResolve) &&
		graphics->CheckFormatSupport(Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT, efk::TextureFeatureType::MultisampledTexture2DRenderTarget) &&
		msaaSampleHDR > 1;
	auto isMSAASupported = msaaSample > 1;

	if (isHDRSupported || isMSAAHDRSupported)
	{
		textureFormat_ = Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT;

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

		textureFormat_ = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	}

	spdlog::trace("HDR {} {}", isMSAAHDRSupported, isHDRSupported);

	spdlog::trace("MSAA {} {}", msaaSampleHDR, msaaSample);

	spdlog::trace("OK Check format");

	::Effekseer::SpriteRendererRef sprite_renderer = renderer_->CreateSpriteRenderer();
	::Effekseer::RibbonRendererRef ribbon_renderer = renderer_->CreateRibbonRenderer();
	::Effekseer::RingRendererRef ring_renderer = renderer_->CreateRingRenderer();
	::Effekseer::ModelRendererRef model_renderer = renderer_->CreateModelRenderer();
	::Effekseer::TrackRendererRef track_renderer = renderer_->CreateTrackRenderer();
	::Effekseer::GpuTimerRef gpu_timer = renderer_->CreateGpuTimer();
	::Effekseer::GpuParticleSystemRef gpu_particles = renderer_->CreateGpuParticleSystem();

	if (sprite_renderer == nullptr)
	{
		spdlog::warn("FAIL : CreateSpriteRenderer");
		return false;
	}

	spdlog::trace("OK : CreateRenderers");

	manager_->SetSpriteRenderer(sprite_renderer);
	manager_->SetRibbonRenderer(ribbon_renderer);
	manager_->SetRingRenderer(ring_renderer);
	manager_->SetModelRenderer(model_renderer);
	manager_->SetTrackRenderer(track_renderer);
	manager_->SetGpuTimer(gpu_timer);
	manager_->SetGpuParticleSystem(gpu_particles);

	if (graphics_->GetGraphics()->GetGraphicsDevice() != nullptr)
	{

		backgroundRenderer_ = Effekseer::Tool::StaticMeshRenderer::Create(graphics_->GetGraphics()->GetGraphicsDevice());

		if (backgroundRenderer_ != nullptr && UpdateBackgroundMesh(parameter_.BackgroundColor))
		{
			spdlog::trace("OK : Background");
		}
		else
		{
			spdlog::warn("FAIL : Background");
			return false;
		}
	}

	groundRenderer_ = GroundRenderer::Create(graphics->GetGraphicsDevice());
	if (groundRenderer_)
	{
		spdlog::trace("OK : GroundRenderer");
	}
	else
	{
		spdlog::warn("FAIL : GroundRenderer");
	}

	if (graphics->GetGraphicsDevice()->GetDeviceName() == "DirectX11")
	{
#ifdef _WIN32
		whiteParticleSpriteShader_ =
			graphics->GetGraphicsDevice()->CreateShaderFromBinary(
				WhiteParticle_Sprite_VS::g_main,
				sizeof(WhiteParticle_Sprite_VS::g_main),
				WhiteParticle_PS::g_main,
				sizeof(WhiteParticle_PS::g_main));

		whiteParticleModelShader_ =
			graphics->GetGraphicsDevice()->CreateShaderFromBinary(
				WhiteParticle_Model_VS::g_main,
				sizeof(WhiteParticle_Model_VS::g_main),
				WhiteParticle_PS::g_main,
				sizeof(WhiteParticle_PS::g_main));

		auto shader = graphics_->GetGraphics()->GetGraphicsDevice()->CreateShaderFromBinary(
			PostEffect_Basic_VS::g_main,
			sizeof(PostEffect_Basic_VS::g_main),
			PostEffect_Overdraw_PS::g_main,
			sizeof(PostEffect_Overdraw_PS::g_main));

		overdrawEffect_ = std::make_unique<PostProcess>(graphics_->GetGraphics()->GetGraphicsDevice(), shader, 0, 0);
#endif
	}
	else if (graphics->GetGraphicsDevice()->GetDeviceName() == "OpenGL")
	{
		const auto texLocUnlit = EffekseerRendererGL::GetTextureLocations(EffekseerRenderer::RendererShaderType::Unlit);

		{
			Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
			EffekseerRendererGL::AddVertexUniformLayout(uniformLayoutElementsLitUnlit);
			EffekseerRendererGL::AddPixelUniformLayout(uniformLayoutElementsLitUnlit);
			auto uniformLayoutUnlitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsLitUnlit);

			whiteParticleSpriteShader_ = graphics->GetGraphicsDevice()->CreateShaderFromCodes(
				{get_sprite_unlit_vs(EffekseerRendererGL::OpenGLDeviceType::OpenGL3)},
				{gl_white_particle_ps},
				uniformLayoutUnlitAd);
		}

		{
			Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElementsLitUnlit;
			EffekseerRendererGL::AddModelVertexUniformLayout(uniformLayoutElementsLitUnlit, false, true, EffekseerRendererGL::OpenGLInstancingCount);
			EffekseerRendererGL::AddPixelUniformLayout(uniformLayoutElementsLitUnlit);
			auto uniformLayoutUnlitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(texLocUnlit, uniformLayoutElementsLitUnlit);

			whiteParticleModelShader_ = graphics->GetGraphicsDevice()->CreateShaderFromCodes(
				{get_model_unlit_vs(EffekseerRendererGL::OpenGLDeviceType::OpenGL3)},
				{gl_white_particle_ps},
				uniformLayoutUnlitAd);
		}

		{
			Effekseer::CustomVector<Effekseer::CustomString<char>> tecLoc{"Sampler_g_sampler"};
			Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> elms;
			auto uniformLayoutUnlitAd = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(tecLoc, elms);

			auto shader = graphics_->GetGraphics()->GetGraphicsDevice()->CreateShaderFromCodes(
				{gl_postfx_basic_vs},
				{gl_postfx_overdraw_ps},
				uniformLayoutUnlitAd);

			overdrawEffect_ = std::make_unique<PostProcess>(graphics_->GetGraphics()->GetGraphicsDevice(), shader, 0, 0);
		}
	}
	else
	{
		spdlog::warn("Overdraw is not suppoted.");
	}

	SetPostEffectParameter(Effekseer::Tool::PostEffectParameter{});

	if (!OnAfterInitialize())
	{
		return false;
	}

	if (soundDevice != nullptr)
	{
		manager_->SetSoundPlayer(soundDevice->GetSound()->CreateSoundPlayer());
	}

	return true;
}

Vector2I EffectRenderer::GetScreenSize() const
{
	return screenSize_;
}

void EffectRenderer::ResizeScreen(const Vector2I& screenSize)
{
	if (screenSize_ == screenSize)
	{
		return;
	}

	screenSize_ = screenSize;

	hdrRenderTextureMSAA = nullptr;

	const auto createRenderTexture = [&](Effekseer::Tool::Vector2I size, Effekseer::Backend::TextureFormatType format, int samples)
	{
		Effekseer::Backend::TextureParameter param;
		param.Format = format;
		param.Size[0] = size.X;
		param.Size[1] = size.Y;
		param.SampleCount = samples;
		param.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;
		return graphics_->GetGraphics()->GetGraphicsDevice()->CreateTexture(param);
	};

	hdrRenderTexture = createRenderTexture(screenSize, textureFormat_, 1);

	Effekseer::Backend::DepthTextureParameter depthTextureParam;
	depthTextureParam.Format = Effekseer::Backend::TextureFormatType::D24S8;
	depthTextureParam.Size[0] = screenSize.X;
	depthTextureParam.Size[1] = screenSize.Y;
	depthTextureParam.SamplingCount = msaaSamples;
	depthTexture = graphics_->GetGraphics()->GetGraphicsDevice()->CreateDepthTexture(depthTextureParam);

	if (msaaSamples > 1)
	{
		depthRenderTextureMSAA = createRenderTexture(screenSize, Effekseer::Backend::TextureFormatType::R32_FLOAT, msaaSamples);
	}

	depthRenderTexture = createRenderTexture(screenSize, Effekseer::Backend::TextureFormatType::R32_FLOAT, 1);

	if (msaaSamples > 1)
	{
		hdrRenderTextureMSAA = createRenderTexture(screenSize, textureFormat_, msaaSamples);
	}

	backTexture = createRenderTexture(screenSize, textureFormat_, 1);

	if (m_isSRGBMode)
	{
		linearRenderTexture = createRenderTexture(screenSize, textureFormat_, 1);
	}
}

void EffectRenderer::PlayEffect()
{
	// TODO : Refactor
	srand(RandomSeed);

	assert(effect_ != nullptr);

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

				HandleHolder handleHolder(manager_->Play(effect_->GetEffect(), posX, posY, posZ));

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

void EffectRenderer::UpdatePaused()
{
	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = renderer_->GetCameraPosition();
	layerParameter.DistanceBias = lodDistanceBias_;
	manager_->SetLayerParameter(0, layerParameter);

	Effekseer::Manager::UpdateParameter updateParameter;
	updateParameter.DeltaFrame = 0.0F;
	updateParameter.UpdateInterval = 0.0f;
	manager_->Update(updateParameter);
}

void EffectRenderer::Update()
{
	/*{
		int32_t gpuTime = manager_->GetGpuTime();
		char log[256];
		snprintf(log, sizeof(log), "GpuTime: %d\n", gpuTime);
		OutputDebugStringA(log);
	}*/

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

	// send triggers
	std::array<bool*, 4> triggers = {
		&behavior_.TriggerInput0,
		&behavior_.TriggerInput1,
		&behavior_.TriggerInput2,
		&behavior_.TriggerInput3};
	for (size_t i = 0; i < triggers.size(); i++)
	{
		bool& trigger = *triggers[i];
		if (trigger)
		{
			for (auto h : handles_)
			{
				if (trigger)
				{
					manager_->SendTrigger(h.Handle, (int32_t)i);
				}
			}
		}
		trigger = false;
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

		Effekseer::Manager::LayerParameter layerParameter;
		layerParameter.ViewerPosition = renderer_->GetCameraPosition();
		layerParameter.DistanceBias = lodDistanceBias_;
		manager_->SetLayerParameter(0, layerParameter);

		Effekseer::Manager::UpdateParameter updateParameter;
		updateParameter.DeltaFrame = (float)m_step;
		updateParameter.UpdateInterval = 0.0;
		manager_->Update(updateParameter);
		manager_->Compute();

		renderer_->SetTime(m_time / 60.0f);

		for (size_t i = 0; i < handles_.size(); i++)
		{
			handles_[i].Time += m_step;
		}
	}

	m_time += 1;
}

void EffectRenderer::Update(int32_t frame)
{
	if (frame <= 0)
	{
		Effekseer::Manager::UpdateParameter updateParameter;
		updateParameter.DeltaFrame = 0.0f;
		updateParameter.UpdateInterval = 0.0;
		manager_->Update(updateParameter);
	}
	else if (frame == 1)
	{
		Update();
	}
	else
	{
		for (size_t i = 0; i < handles_.size(); i++)
		{
			manager_->SetShown(handles_[i].Handle, false);
		}

		const auto updatingTime = Effekseer::Max(0, frame - m_step);

		for (int i = 0; i < updatingTime; i++)
		{
			Update();
		}

		for (size_t i = 0; i < handles_.size(); i++)
		{
			manager_->SetShown(handles_[i].Handle, true);
		}

		for (int i = 0; i < frame - updatingTime; i++)
		{
			Update();
		}
	}
}

void EffectRenderer::SetLODDistanceBias(float distanceBias)
{
	lodDistanceBias_ = distanceBias;
}

void EffectRenderer::Render(std::shared_ptr<RenderImage> renderImage)
{
	// Clear a destination texture
	if (parameter_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		UpdateBackgroundMesh({0, 0, 0, 0});
	}
	else if (backgroundTexture_ != nullptr && backgroundTexture_->GetBackend() != nullptr)
	{
		UpdateBackgroundMesh({255, 255, 255, 255});
	}
	else
	{
		UpdateBackgroundMesh(parameter_.BackgroundColor);
	}

	auto renderTargetImage = renderImage->GetTexture();

	graphics_->GetGraphics()->SetRenderTarget({renderTargetImage}, nullptr);
	graphics_->GetGraphics()->Clear({0, 0, 0, 0});

	// clear
	if (msaaSamples > 1)
	{
		graphics_->GetGraphics()->SetRenderTarget({hdrRenderTextureMSAA, depthRenderTextureMSAA}, depthTexture);
	}
	else
	{
		graphics_->GetGraphics()->SetRenderTarget({hdrRenderTexture, depthRenderTexture}, depthTexture);
	}

	graphics_->GetGraphics()->Clear({0, 0, 0, 0});

	// TODO : refactor
	renderer_->SetCameraMatrix(parameter_.CameraMatrix);
	renderer_->SetProjectionMatrix(parameter_.ProjectionMatrix);

	if (manager_->GetSetting()->GetCoordinateSystem() == Effekseer::CoordinateSystem::RH)
	{
		renderer_->SetLightDirection(parameter_.LightDirection);
	}
	else
	{
		auto temp = parameter_.LightDirection;
		temp.Z = -temp.Z;
		renderer_->SetLightDirection(temp);
	}

	renderer_->SetLightColor(parameter_.LightColor);
	renderer_->SetLightAmbientColor(parameter_.LightAmbientColor);

	if (backgroundRenderer_ != nullptr && backgroundMesh_ != nullptr)
	{
		if (backgroundTexture_ != nullptr)
		{
			backgroundMesh_->Texture = backgroundTexture_->GetBackend();
		}
		else
		{
			backgroundMesh_->Texture = nullptr;
		}

		Effekseer::Tool::RendererParameter param{};
		param.CameraMatrix.Indentity();
		param.ProjectionMatrix.Indentity();
		param.WorldMatrix.Indentity();
		backgroundRenderer_->Render(param);
	}

	if (parameter_.RenderingMethod != RenderingMethodType::Overdraw)
	{
		if (groundRenderer_ != nullptr && parameter_.IsGroundShown)
		{
			groundRenderer_->Render(renderer_);
		}
	}

	OnAfterClear();

	if (msaaSamples > 1)
	{
		graphics_->GetGraphics()->SetRenderTarget({hdrRenderTextureMSAA}, depthTexture);
	}
	else
	{
		graphics_->GetGraphics()->SetRenderTarget({hdrRenderTexture}, depthTexture);
	}

	if (msaaSamples > 1)
	{
		graphics_->GetGraphics()->ResolveRenderTarget(depthRenderTextureMSAA, depthRenderTexture);
	}

	EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
	reconstructionParam.DepthBufferScale = 1.0f;
	reconstructionParam.DepthBufferOffset = 0.0f;
	reconstructionParam.ProjectionMatrix33 = parameter_.ProjectionMatrix.Value.Values[2][2];
	reconstructionParam.ProjectionMatrix43 = parameter_.ProjectionMatrix.Value.Values[2][3];
	reconstructionParam.ProjectionMatrix34 = parameter_.ProjectionMatrix.Value.Values[3][2];
	reconstructionParam.ProjectionMatrix44 = parameter_.ProjectionMatrix.Value.Values[3][3];

	renderer_->SetDepth(depthRenderTexture, reconstructionParam);

	if (parameter_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		auto external = std::make_shared<EffekseerRenderer::ExternalShaderSettings>();
		external->Blend = Effekseer::AlphaBlendType::Add;
		external->StandardShader = whiteParticleSpriteShader_;
		external->ModelShader = whiteParticleModelShader_;
		renderer_->SetExternalShaderSettings(external);
	}

	if (parameter_.RenderingMethod == RenderingMethodType::Normal ||
		parameter_.RenderingMethod == RenderingMethodType::NormalWithWireframe ||
		parameter_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		renderer_->SetRenderMode(Effekseer::RenderMode::Normal);
	}
	else
	{
		renderer_->SetRenderMode(Effekseer::RenderMode::Wireframe);
	}

	// Distoriton
	if (parameter_.Distortion == DistortionType::Current)
	{
		CopyToBack();

		m_distortionCallback->Blit = false;
		m_distortionCallback->IsEnabled = true;
	}
	else if (parameter_.Distortion == DistortionType::Effekseer120)
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

	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZFar = 1.0f;
	drawParameter.ZNear = 0.0f;
	drawParameter.IsSortingEffectsEnabled = true;
	drawParameter.CameraPosition = parameter_.CameraPosition;
	drawParameter.CameraFrontDirection = parameter_.CameraFrontDirection;
	Effekseer::Matrix44 vpm;
	Effekseer::Matrix44::Mul(vpm, parameter_.CameraMatrix, parameter_.ProjectionMatrix);
	drawParameter.ViewProjectionMatrix = vpm;

	if (parameter_.Distortion == DistortionType::Current)
	{
		manager_->DrawBack(drawParameter);

		// HACK
		renderer_->EndRendering();

		CopyToBack();

		// HACK
		renderer_->BeginRendering();
		manager_->DrawFront(drawParameter);
	}
	else
	{
		manager_->Draw(drawParameter);
	}

	renderer_->EndRendering();

	if (parameter_.RenderingMethod == RenderingMethodType::NormalWithWireframe)
	{
		auto external = std::make_shared<EffekseerRenderer::ExternalShaderSettings>();
		external->Blend = Effekseer::AlphaBlendType::Opacity;
		external->StandardShader = whiteParticleSpriteShader_;
		external->ModelShader = whiteParticleModelShader_;
		renderer_->SetExternalShaderSettings(external);

		renderer_->SetRenderMode(Effekseer::RenderMode::Wireframe);

		renderer_->BeginRendering();

		manager_->Draw(drawParameter);

		renderer_->EndRendering();
	}

	if (parameter_.RenderingMethod == RenderingMethodType::NormalWithWireframe ||
		parameter_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		renderer_->SetExternalShaderSettings(nullptr);
	}

	ResetBack();

	OnBeforePostprocess();

	// all post effects are disabled
	if (bloomEffect_ == nullptr && tonemapEffect_ == nullptr && linearToSRGBEffect_ == nullptr)
	{
		return;
	}

	if (msaaSamples > 1)
	{
		graphics_->GetGraphics()->ResolveRenderTarget(hdrRenderTextureMSAA, hdrRenderTexture);
	}

	if (parameter_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		graphics_->GetGraphics()->SetRenderTarget({renderTargetImage}, nullptr);
		overdrawEffect_->GetDrawParameter().SetTexture(0, hdrRenderTexture, Effekseer::Backend::TextureWrapType::Clamp, Effekseer::Backend::TextureSamplingType::Linear);
		overdrawEffect_->Render();
	}
	else
	{
		// Bloom processing (specifying the same target for src and dest is faster)
		bloomEffect_->Render(hdrRenderTexture, hdrRenderTexture);

		// Tone map processing
		auto tonemapTerget = renderTargetImage;
		if (m_isSRGBMode)
		{
			tonemapTerget = linearRenderTexture;
		}

		tonemapEffect_->Render(tonemapTerget, hdrRenderTexture);

		if (m_isSRGBMode)
		{
			linearToSRGBEffect_->Render(renderTargetImage, tonemapTerget);
		}
	}

	graphics_->GetGraphics()->SetRenderTarget({nullptr}, nullptr);
}

std::shared_ptr<Effekseer::Tool::Effect> EffectRenderer::GetEffect() const
{
	return effect_;
}

void EffectRenderer::SetEffect(std::shared_ptr<Effekseer::Tool::Effect> effect)
{
	effect_ = effect;
}

void EffectRenderer::ResetEffect()
{
	for (size_t i = 0; i < handles_.size(); i++)
	{
		manager_->StopEffect(handles_[i].Handle);
	}
	handles_.clear();

	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = renderer_->GetCameraPosition();
	layerParameter.DistanceBias = lodDistanceBias_;
	manager_->SetLayerParameter(0, layerParameter);

	Effekseer::Manager::UpdateParameter updateParameter;
	manager_->Update(updateParameter);
}

const ViewerEffectBehavior& EffectRenderer::GetBehavior() const
{
	return behavior_;
}

void EffectRenderer::SetBehavior(const ViewerEffectBehavior& behavior)
{
	behavior_ = behavior;
}

int EffectRenderer::GetCurrentLOD() const
{
	if (handles_.size() == 0)
	{
		return 0;
	}

	return manager_->GetCurrentLOD(handles_[0].Handle);
}

int32_t EffectRenderer::GetInstanceCount() const
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

void EffectRenderer::SetStep(int32_t step)
{
	m_step = step;
}

Effekseer::Tool::PostEffectParameter EffectRenderer::GetPostEffectParameter() const
{
	return postEffectParameter_;
}

void EffectRenderer::SetPostEffectParameter(const Effekseer::Tool::PostEffectParameter& param)
{
	if (bloomEffect_ != nullptr)
	{
		bloomEffect_->SetEnabled(param.BloomEnabled);
		bloomEffect_->SetParameters(param.BoomIntensity, param.BloomThreshold, param.BloomSoftKnee);
	}

	if (tonemapEffect_ != nullptr)
	{
		tonemapEffect_->SetEnabled(param.ToneMapAlgorithm != 0);
		tonemapEffect_->SetParameters((Effekseer::Tool::TonemapPostEffect::Algorithm)param.ToneMapAlgorithm, param.ToneMapExposure);
	}

	postEffectParameter_ = param;
}

int32_t EffectRenderer::GetCpuTime()
{
	return manager_->GetUpdateTime() + manager_->GetDrawTime();
}

int32_t EffectRenderer::GetGpuTime()
{
	return manager_->GetGpuTime();
}

} // namespace Tool
} // namespace Effekseer