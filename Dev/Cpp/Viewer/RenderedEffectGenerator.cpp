#include "RenderedEffectGenerator.h"

#ifdef _WIN32
#include "Graphics/Platform/DX11/efk.GraphicsDX11.h"

namespace WhiteParticle_Model_VS
{
#include <EffekseerRendererDX11/EffekseerRenderer/ShaderHeader/model_unlit_vs.h>
}

namespace WhiteParticle_Sprite_VS
{
#include <EffekseerRendererDX11/EffekseerRenderer/ShaderHeader/sprite_unlit_vs.h>
}

namespace WhiteParticle_PS
{
#include "Shaders/HLSL_DX11_Header/white_particle_ps.h"
}

namespace PostEffect_Basic_VS
{
#include "Shaders/HLSL_DX11_Header/postfx_basic_vs.h"
}

namespace PostEffect_Overdraw_PS
{
#include "Shaders/HLSL_DX11_Header/postfx_overdraw_ps.h"
}

#endif

#include "../EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelRenderer.h"

#include "Shaders/GLSL_GL_Header/line_ps.h"
#include "Shaders/GLSL_GL_Header/line_vs.h"

#include "Shaders/GLSL_GL_Header/postfx_basic_vs.h"
#include "Shaders/GLSL_GL_Header/postfx_overdraw_ps.h"
#include "Shaders/GLSL_GL_Header/white_particle_ps.h"

#include "Graphics/Platform/GL/efk.GraphicsGL.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "../EffekseerRendererGL/EffekseerRenderer/ShaderHeader/model_unlit_vs.h"
#include "../EffekseerRendererGL/EffekseerRenderer/ShaderHeader/sprite_unlit_vs.h"

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
	texParams.InitialData.resize(texParams.Size[0] * texParams.Size[1] * 4);
	for (size_t i = 0; i < texParams.InitialData.size() / 4; i++)
	{
		const size_t x = i % texParams.Size[0] * 2 / texParams.Size[0];
		const size_t y = i / texParams.Size[1] * 2 / texParams.Size[1];
		if (x ^ y == 0)
		{
			texParams.InitialData[i * 4 + 0] = 90;
			texParams.InitialData[i * 4 + 1] = 90;
			texParams.InitialData[i * 4 + 2] = 90;
			texParams.InitialData[i * 4 + 3] = 255;
		}
		else
		{
			texParams.InitialData[i * 4 + 0] = 60;
			texParams.InitialData[i * 4 + 1] = 60;
			texParams.InitialData[i * 4 + 2] = 60;
			texParams.InitialData[i * 4 + 3] = 255;
		}
	}
	groudMesh->Texture = graphicsDevice->CreateTexture(texParams);

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

RenderedEffectGenerator::DistortingCallback::DistortingCallback(efk::Graphics* graphics, RenderedEffectGenerator* generator)
	: graphics_(graphics)
	, generator_(generator)
{
}

RenderedEffectGenerator::DistortingCallback::~DistortingCallback()
{
}

bool RenderedEffectGenerator::DistortingCallback::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	if (Blit)
	{
		generator_->CopyToBack();
	}

	return IsEnabled;
}

bool RenderedEffectGenerator::UpdateBackgroundMesh(const Color& backgroundColor)
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

	backgroundMesh_ = Effekseer::Tool::StaticMesh::Create(graphics_->GetGraphicsDevice(), vbData, ibData);
	if (!backgroundMesh_)
	{
		return false;
	}

	backgroundMesh_->IsLit = false;

	backgroundRenderer_->SetStaticMesh(backgroundMesh_);

	return true;
}

void RenderedEffectGenerator::PlayEffect()
{
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
}

bool RenderedEffectGenerator::Initialize(efk::Graphics* graphics, Effekseer::RefPtr<Effekseer::Setting> setting, int32_t spriteCount, bool isSRGBMode)
{
	graphics_ = graphics;

	manager_ = ::Effekseer::Manager::Create(spriteCount);
	// manager_->LaunchWorkerThreads(4);
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

	if (graphics_->GetGraphicsDevice() != nullptr)
	{

		backgroundRenderer_ = Effekseer::Tool::StaticMeshRenderer::Create(graphics_->GetGraphicsDevice());

		if (backgroundRenderer_ != nullptr && UpdateBackgroundMesh(config_.BackgroundColor))
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

		auto shader = graphics_->GetGraphicsDevice()->CreateShaderFromBinary(
			PostEffect_Basic_VS::g_main,
			sizeof(PostEffect_Basic_VS::g_main),
			PostEffect_Overdraw_PS::g_main,
			sizeof(PostEffect_Overdraw_PS::g_main));

		overdrawEffect_ = std::make_unique<PostProcess>(graphics_->GetGraphicsDevice(), shader, 0, 0);
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

			auto shader = graphics_->GetGraphicsDevice()->CreateShaderFromCodes(
				{gl_postfx_basic_vs},
				{gl_postfx_overdraw_ps},
				uniformLayoutUnlitAd);

			overdrawEffect_ = std::make_unique<PostProcess>(graphics_->GetGraphicsDevice(), shader, 0, 0);
		}
	}
	else
	{
		spdlog::warn("Overdraw is not suppoted.");
	}

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
		depthRenderTextureMSAA = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
		depthRenderTextureMSAA->Initialize(screenSize, Effekseer::Backend::TextureFormatType::R32_FLOAT, msaaSamples);
	}

	depthRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics_));
	depthRenderTexture->Initialize(screenSize, Effekseer::Backend::TextureFormatType::R32_FLOAT, 1);

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
	viewRenderTexture->Initialize(screenSize, Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM);
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

	m_time += 1;
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

		const auto updatingTime = Effekseer::Max(0, frame - m_step);

		for (int i = 0; i < updatingTime; i++)
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

		for (int i = 0; i < frame - updatingTime; i++)
		{
			Update();
		}
	}
}

void RenderedEffectGenerator::Render()
{
	// Clear a destination texture
	if (config_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		UpdateBackgroundMesh({0, 0, 0, 0});
	}
	else if (backgroundTexture_ != nullptr && backgroundTexture_->GetBackend() != nullptr)
	{
		UpdateBackgroundMesh({255, 255, 255, 255});
	}
	else
	{
		UpdateBackgroundMesh(config_.BackgroundColor);
	}

	graphics_->SetRenderTarget({viewRenderTexture->GetAsBackend()}, nullptr);
	graphics_->Clear({0, 0, 0, 0});

	// clear
	if (msaaSamples > 1)
	{
		graphics_->SetRenderTarget({hdrRenderTextureMSAA->GetAsBackend(), depthRenderTextureMSAA->GetAsBackend()}, depthTexture->GetAsBackend());
	}
	else
	{
		graphics_->SetRenderTarget({hdrRenderTexture->GetAsBackend(), depthRenderTexture->GetAsBackend()}, depthTexture->GetAsBackend());
	}

	graphics_->Clear({0, 0, 0, 0});

	// TODO : refactor
	renderer_->SetCameraMatrix(config_.CameraMatrix);
	renderer_->SetProjectionMatrix(config_.ProjectionMatrix);
	renderer_->SetLightDirection(config_.LightDirection);
	renderer_->SetLightColor(config_.LightColor);
	renderer_->SetLightAmbientColor(config_.LightAmbientColor);

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

	if (config_.RenderingMethod != RenderingMethodType::Overdraw)
	{
		if (groundRenderer_ != nullptr && config_.IsGroundShown)
		{
			groundRenderer_->Render(renderer_);
		}
	}

	OnAfterClear();

	if (msaaSamples > 1)
	{
		graphics_->SetRenderTarget({hdrRenderTextureMSAA->GetAsBackend()}, depthTexture->GetAsBackend());
	}
	else
	{
		graphics_->SetRenderTarget({hdrRenderTexture->GetAsBackend()}, depthTexture->GetAsBackend());
	}

	if (msaaSamples > 1)
	{
		graphics_->ResolveRenderTarget(depthRenderTextureMSAA->GetAsBackend(), depthRenderTexture->GetAsBackend());
	}

	EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
	reconstructionParam.DepthBufferScale = 1.0f;
	reconstructionParam.DepthBufferOffset = 0.0f;
	reconstructionParam.ProjectionMatrix33 = config_.ProjectionMatrix.Values[2][2];
	reconstructionParam.ProjectionMatrix43 = config_.ProjectionMatrix.Values[2][3];
	reconstructionParam.ProjectionMatrix34 = config_.ProjectionMatrix.Values[3][2];
	reconstructionParam.ProjectionMatrix44 = config_.ProjectionMatrix.Values[3][3];

	renderer_->SetDepth(depthRenderTexture->GetAsBackend(), reconstructionParam);

	if (config_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		auto external = std::make_shared<EffekseerRenderer::ExternalShaderSettings>();
		external->Blend = Effekseer::AlphaBlendType::Add;
		external->StandardShader = whiteParticleSpriteShader_;
		external->ModelShader = whiteParticleModelShader_;
		renderer_->SetExternalShaderSettings(external);
	}

	if (config_.RenderingMethod == RenderingMethodType::Normal ||
		config_.RenderingMethod == RenderingMethodType::NormalWithWireframe ||
		config_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		renderer_->SetRenderMode(Effekseer::RenderMode::Normal);
	}
	else
	{
		renderer_->SetRenderMode(Effekseer::RenderMode::Wireframe);
	}

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

	if (config_.RenderingMethod == RenderingMethodType::NormalWithWireframe)
	{
		auto external = std::make_shared<EffekseerRenderer::ExternalShaderSettings>();
		external->Blend = Effekseer::AlphaBlendType::Opacity;
		external->StandardShader = whiteParticleSpriteShader_;
		external->ModelShader = whiteParticleModelShader_;
		renderer_->SetExternalShaderSettings(external);

		renderer_->SetRenderMode(Effekseer::RenderMode::Wireframe);

		renderer_->BeginRendering();

		manager_->Draw(config_.DrawParameter);

		renderer_->EndRendering();
	}

	if (config_.RenderingMethod == RenderingMethodType::NormalWithWireframe ||
		config_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		renderer_->SetExternalShaderSettings(nullptr);
	}

	ResetBack();

	OnBeforePostprocess();

	// all post effects are disabled
	if (m_bloomEffect == nullptr && m_tonemapEffect == nullptr && m_linearToSRGBEffect == nullptr)
	{
		return;
	}

	if (msaaSamples > 1)
	{
		graphics_->ResolveRenderTarget(hdrRenderTextureMSAA->GetAsBackend(), hdrRenderTexture->GetAsBackend());
	}

	if (config_.RenderingMethod == RenderingMethodType::Overdraw)
	{
		graphics_->SetRenderTarget({viewRenderTexture->GetAsBackend()}, nullptr);
		overdrawEffect_->GetDrawParameter().TexturePtrs[0] = hdrRenderTexture->GetAsBackend();
		overdrawEffect_->GetDrawParameter().TextureCount = 1;
		overdrawEffect_->Render();
	}
	else
	{
		// Bloom processing (specifying the same target for src and dest is faster)
		m_bloomEffect->Render(hdrRenderTexture->GetAsBackend(), hdrRenderTexture->GetAsBackend());

		// Tone map processing
		auto tonemapTerget = viewRenderTexture->GetAsBackend();
		if (m_isSRGBMode)
		{
			tonemapTerget = linearRenderTexture->GetAsBackend();
		}

		m_tonemapEffect->Render(hdrRenderTexture->GetAsBackend(), tonemapTerget);

		if (m_isSRGBMode)
		{
			m_linearToSRGBEffect->Render(tonemapTerget, viewRenderTexture->GetAsBackend());
		}
	}

	graphics_->SetRenderTarget({nullptr}, nullptr);
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

void RenderedEffectGenerator::SetEffect(Effekseer::EffectRef effect)
{
	handles_.clear();
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
		graphics_->CopyTo(hdrRenderTextureMSAA->GetAsBackend(), backTexture->GetAsBackend());
	}
	else
	{
		graphics_->CopyTo(hdrRenderTexture->GetAsBackend(), backTexture->GetAsBackend());
	}

	renderer_->SetBackground(backTexture->GetAsBackend());
}

void RenderedEffectGenerator::ResetBack()
{
	renderer_->SetBackground(nullptr);
}

} // namespace Tool
} // namespace Effekseer