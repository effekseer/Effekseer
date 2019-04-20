
#include "EffekseerTool.Renderer.h"
#include "EffekseerTool.Grid.h"
#include "EffekseerTool.Guide.h"
#include "EffekseerTool.Culling.h"
#include "EffekseerTool.Paste.h"

#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"



namespace EffekseerTool
{
	Renderer::DistortingCallback::DistortingCallback(efk::Graphics* renderer)
		: renderer(renderer)
	{
		IsEnabled = true;
		Blit = true;
	}

	Renderer::DistortingCallback::~DistortingCallback()
	{
	
	}

	bool Renderer::DistortingCallback::OnDistorting()
	{
		if (Blit)
		{
			renderer->CopyToBackground();

			if (renderer->GetDeviceType() == efk::DeviceType::OpenGL)
			{
				auto r = (::EffekseerRendererGL::Renderer*)renderer->GetRenderer();
				r->SetBackground((GLuint)(size_t)renderer->GetBack());
			}
#ifdef _WIN32
			else if (renderer->GetDeviceType() == efk::DeviceType::DirectX11)
			{
				auto r = (::EffekseerRendererDX11::Renderer*)renderer->GetRenderer();
				r->SetBackground((ID3D11ShaderResourceView*)renderer->GetBack());
			}
			else
			{
				auto r = (::EffekseerRendererDX9::Renderer*)renderer->GetRenderer();
				r->SetBackground((IDirect3DTexture9*)renderer->GetBack());
			}
#endif
		}

		return IsEnabled;
	}

	Renderer::Renderer(int32_t squareMaxCount, bool isSRGBMode, efk::DeviceType deviceType)
		: m_squareMaxCount(squareMaxCount)
		, m_projection(PROJECTION_TYPE_PERSPECTIVE)
		, m_renderer(NULL)

		, RateOfMagnification(1.0f)

		, m_grid(NULL)
		, m_guide(NULL)
		, m_culling(NULL)
		, m_background(NULL)

		, GuideWidth(100)
		, GuideHeight(100)
		, RendersGuide(false)

		, IsGridShown(true)

		, IsGridXYShown(false)
		, IsGridXZShown(true)
		, IsGridYZShown(false)

		, IsRightHand(true)
		, GridLength(2.0f)

		, IsCullingShown(false)
		, CullingRadius(0.0f)
		, CullingPosition()

		, Distortion(eDistortionType::DistortionType_Current)
		, RenderingMode(Effekseer::RenderMode::Normal)

		, m_isSRGBMode(isSRGBMode)

		, BackgroundColor(0, 0, 0, 255)
		, GridColor(255, 255, 255, 255)
		, IsBackgroundTranslucent(false)
	{
		if (deviceType == efk::DeviceType::OpenGL)
		{
			graphics = new efk::GraphicsGL();
		}
#ifdef _WIN32
		else if (deviceType == efk::DeviceType::DirectX11)
		{
			graphics = new efk::GraphicsDX11();
		}
		else
		{
			graphics = new efk::GraphicsDX9();
		}
#endif
		graphics->LostedDevice = [this]() -> void
		{
			if (backgroundData != nullptr)
			{
				textureLoader->Unload(backgroundData);
				backgroundData = nullptr;
			}

			viewRenderTexture.reset();
			
			if (LostedDevice != nullptr)
			{
				LostedDevice();
			}
		};

		graphics->ResettedDevice = [this]() -> void
		{
			if (ResettedDevice != nullptr)
			{
				ResettedDevice();
			}

			backgroundData = textureLoader->Load(backgroundPath.c_str(), Effekseer::TextureType::Color);
		};
	}

	Renderer::~Renderer()
	{
		assert(!m_recording);

		if (backgroundData != nullptr)
		{
			textureLoader->Unload(backgroundData);
			backgroundData = nullptr;
		}

		ES_SAFE_DELETE(textureLoader);

		ES_SAFE_DELETE(m_guide);
		ES_SAFE_DELETE(m_grid);
		ES_SAFE_DELETE(m_culling);

		ES_SAFE_DELETE(m_background);

		ES_SAFE_DELETE(graphics);
	}

bool Renderer::Initialize( void* handle, int width, int height )
{
	if (!graphics->Initialize(handle, width, height, m_isSRGBMode, m_squareMaxCount))
	{
		return false;
	}

	m_windowWidth = width;
	m_windowHeight = height;

	currentWidth = m_windowWidth;
	currentHeight = m_windowHeight;

	m_distortionCallback = new DistortingCallback(graphics);
	m_renderer = graphics->GetRenderer();
	m_renderer->SetDistortingCallback(m_distortionCallback);

	// グリッド生成
	m_grid = ::EffekseerRenderer::Grid::Create(graphics);

	// ガイド作成
	m_guide = ::EffekseerRenderer::Guide::Create(graphics);

	m_culling = ::EffekseerRenderer::Culling::Create(graphics);

	// 背景作成
	m_background = ::EffekseerRenderer::Paste::Create(graphics);

	// create postprocessings
	m_bloomEffect.reset(efk::PostEffect::CreateBloom(graphics));
	m_tonemapEffect.reset(efk::PostEffect::CreateTonemap(graphics));

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	textureLoader = graphics->GetRenderer()->CreateTextureLoader();

	return true;
}

bool Renderer::Present()
{
	return graphics->Present();
}

void Renderer::ResetDevice()
{
	graphics->ResetDevice();
}

eProjectionType Renderer::GetProjectionType()
{
	return m_projection;
}

void Renderer::SetProjectionType( eProjectionType type )
{
	m_projection = type;

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( currentWidth, currentHeight );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( currentWidth, currentHeight );
	}
}

void Renderer::SetPerspectiveFov( int width, int height )
{
	::Effekseer::Matrix44 proj;

	if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
	{
		if (IsRightHand)
		{
			// Right hand coordinate
			proj.PerspectiveFovRH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
			// Left hand coordinate
			proj.PerspectiveFovLH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
	}
	else
	{
		if (IsRightHand)
		{
			// Right hand coordinate
			proj.PerspectiveFovRH(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
			// Left hand coordinate
			proj.PerspectiveFovLH(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
	}
	

	proj.Values[0][0] *= RateOfMagnification;
	proj.Values[1][1] *= RateOfMagnification;

	m_renderer->SetProjectionMatrix( proj );
}

void Renderer::SetOrthographic( int width, int height )
{
	::Effekseer::Matrix44 proj;

	if( IsRightHand )
	{
		// Right hand coordinate
		proj.OrthographicRH(
			(float)width / 16.0f / RateOfMagnification, (float)height / 16.0f / RateOfMagnification, ClippingStart, ClippingEnd);
	}
	else
	{
		// Left hand coordinate
		proj.OrthographicLH(
			(float)width / 16.0f / RateOfMagnification, (float)height / 16.0f / RateOfMagnification, ClippingStart, ClippingEnd);
	}

	m_renderer->SetProjectionMatrix( proj );
}

bool Renderer::Resize( int width, int height )
{
	m_windowWidth = width;
	m_windowHeight = height;

	if (!isScreenMode)
	{
		currentWidth = width;
		currentHeight = height;
	}
	
	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	graphics->Resize(width, height);

	return true;
}

void Renderer::RecalcProjection()
{
	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( currentWidth, currentHeight );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( currentWidth, currentHeight );
	}
}

bool Renderer::BeginRendering()
{
	if (m_bloomEffect == nullptr &&  m_tonemapEffect == nullptr)
	{
		targetRenderTexture = graphics->GetRenderTexture();
		targetDepthTexture = graphics->GetDepthTexture();
		graphics->SetRenderTarget(targetRenderTexture, targetDepthTexture);
	}
	else
	{
	
		if (hdrRenderTexture == nullptr || hdrRenderTexture->GetWidth() != screenWidth || hdrRenderTexture->GetHeight() != screenHeight)
		{
			hdrRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics));
			hdrRenderTexture->Initialize(screenWidth, screenHeight, efk::TextureFormat::RGBA16F, msaaSamples);
			depthTexture = std::shared_ptr<efk::DepthTexture>(efk::DepthTexture::Create(graphics));
			depthTexture->Initialize(screenWidth, screenHeight, msaaSamples);
	
			if (msaaSamples > 1)
			{
				postfxRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics));
				postfxRenderTexture->Initialize(screenWidth, screenHeight, efk::TextureFormat::RGBA16F);
			}
			else
			{
				postfxRenderTexture = hdrRenderTexture;
			}
		}
		targetRenderTexture = graphics->GetRenderTexture();
		targetDepthTexture = graphics->GetDepthTexture();
		graphics->SetRenderTarget(hdrRenderTexture.get(), depthTexture.get());	
	}
	
	graphics->BeginScene();

	if (!m_recording)
	{
		graphics->Clear(Effekseer::Color(0, 0, 0, 0));
	}

	if( m_recording && IsBackgroundTranslucent )
	{
		graphics->Clear(Effekseer::Color(0, 0, 0, 0));
	}
	else
	{
		graphics->Clear(Effekseer::Color(BackgroundColor.R, BackgroundColor.G, BackgroundColor.B, 255));
	}

	// Render background (the size of texture is ignored)
	if( !m_recording && backgroundData != nullptr)
	{
		if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
		{
			m_background->Rendering((void*)backgroundData->UserID, 1024, 1024);
		}
#ifdef _WIN32
		else if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
		{
			m_background->Rendering((ID3D11ShaderResourceView*)backgroundData->UserPtr, 1024, 1024);
		}
		else
		{
			m_background->Rendering((IDirect3DTexture9*)backgroundData->UserPtr, 1024, 1024);
		}
#endif
	}
	else if(!m_recording)
	{
		m_background->Rendering(nullptr, 1024, 1024);
	}

	if( !m_recording && IsGridShown )
	{
		m_grid->SetLength( GridLength );
		m_grid->IsShownXY = IsGridXYShown;
		m_grid->IsShownXZ = IsGridXZShown;
		m_grid->IsShownYZ = IsGridYZShown;
		m_grid->Rendering(GridColor, IsRightHand);
	}

	if( !m_recording )
	{
		m_culling->IsShown = IsCullingShown;
		m_culling->Radius = CullingRadius;
		m_culling->X = CullingPosition.X;
		m_culling->Y = CullingPosition.Y;
		m_culling->Z = CullingPosition.Z;
		m_culling->Rendering( IsRightHand );
	}

	// ガイド部分が描画されるように拡大
	if (m_recording)
	{
		m_cameraMatTemp = m_renderer->GetCameraMatrix();
		m_projMatTemp = m_renderer->GetProjectionMatrix();
		auto proj = m_projMatTemp;

		::Effekseer::Matrix44 mat;
		mat.Values[0][0] = (float) currentWidth / (float) GuideWidth;
		mat.Values[1][1] = (float) currentHeight / (float) GuideHeight;
		::Effekseer::Matrix44::Mul(proj, proj, mat);

		m_renderer->SetProjectionMatrix(proj);
	}
	
	/*// Distoriton
	if (Distortion == eDistortionType::DistortionType_Current)
	{
		CopyToBackground();
		
		if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
		{
			auto r = (::EffekseerRendererGL::Renderer*)graphics->GetRenderer();
			r->SetBackground((GLuint)(size_t)graphics->GetBack());
		}
#ifdef _WIN32
		else if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::RendererImplemented*)m_renderer;
			r->SetBackground((ID3D11ShaderResourceView*)graphics->GetBack());
		}
		else
		{
			auto r = (EffekseerRendererDX9::RendererImplemented*)m_renderer;
			r->SetBackground((IDirect3DTexture9*)graphics->GetBack());
		}
#endif

		m_distortionCallback->Blit = false;
		m_distortionCallback->IsEnabled = true;
	}
	else if (Distortion == eDistortionType::DistortionType_Effekseer120)
	{
		m_distortionCallback->Blit = true;
		m_distortionCallback->IsEnabled = true;
	}
	else
	{
		if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
		{
			auto r = (::EffekseerRendererGL::Renderer*)graphics->GetRenderer();
			r->SetBackground(0);
		}
#ifdef _WIN32
		else if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::RendererImplemented*)m_renderer;
			r->SetBackground(nullptr);
		}
		else
		{
			auto r = (EffekseerRendererDX9::RendererImplemented*)m_renderer;
			r->SetBackground(nullptr);
		}
#endif

		m_distortionCallback->Blit = false;
		m_distortionCallback->IsEnabled = false;
	}*/

	m_renderer->SetRenderMode(RenderingMode);

	m_renderer->BeginRendering();
	
	return true;
}

bool Renderer::EndRendering()
{
	m_renderer->EndRendering();

	if( RendersGuide && !m_recording )
	{
		m_guide->Rendering( currentWidth, currentHeight, GuideWidth, GuideHeight );
	}

	if (!m_recording)
	{
		if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
		{
			auto r = (::EffekseerRendererGL::Renderer*)graphics->GetRenderer();
			r->SetBackground(0);
		}
#ifdef _WIN32
		else if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::RendererImplemented*)m_renderer;
			r->SetBackground(nullptr);
		}
		else
		{
			auto r = (EffekseerRendererDX9::RendererImplemented*)m_renderer;
			r->SetBackground(nullptr);
		}
#endif
	}

	if (m_recording)
	{
		m_renderer->SetCameraMatrix(m_cameraMatTemp);
		m_renderer->SetProjectionMatrix(m_projMatTemp);
	}
	
	graphics->EndScene();

	return true;
}

bool Renderer::BeginRenderToView(int32_t width, int32_t height)
{
	if (viewRenderTexture == nullptr || viewRenderTexture->GetWidth() != width || viewRenderTexture->GetHeight() != height)
	{
		viewRenderTexture = std::shared_ptr<efk::RenderTexture>(efk::RenderTexture::Create(graphics));
		viewRenderTexture->Initialize(width, height, efk::TextureFormat::RGBA8U);

		viewDepthTexture = std::shared_ptr<efk::DepthTexture>(efk::DepthTexture::Create(graphics));
		viewDepthTexture->Initialize(width, height);
	}

	graphics->SetRenderTarget(viewRenderTexture.get(), viewDepthTexture.get());

	m_cameraMatTemp = m_renderer->GetCameraMatrix();
	m_projMatTemp = m_renderer->GetProjectionMatrix();

	if (m_projection == PROJECTION_TYPE_PERSPECTIVE)
	{
		SetPerspectiveFov(width, height);
	}
	else if (m_projection == PROJECTION_TYPE_ORTHOGRAPHIC)
	{
		SetOrthographic(width, height);
	}

	screenWidth = width;
	screenHeight = height;

	currentWidth = screenWidth;
	currentHeight = screenHeight;

	isScreenMode = true;

	return true;
}

bool Renderer::EndRenderToView()
{
	isScreenMode = false;

	m_renderer->SetCameraMatrix(m_cameraMatTemp);
	m_renderer->SetProjectionMatrix(m_projMatTemp);

	graphics->SetRenderTarget(nullptr, nullptr);

	currentWidth = m_windowWidth;
	currentHeight = m_windowHeight;

	return true;
}

void Renderer::RenderPostEffect()
{
	auto src = hdrRenderTexture.get();
	auto dest = postfxRenderTexture.get();
	
	// all post effects are disabled
	if (m_bloomEffect == nullptr && m_tonemapEffect == nullptr)
	{
		return;
	}

	if (src != dest)
	{
		graphics->ResolveRenderTarget(src, dest);
		src = dest;
	}

	if (m_bloomEffect) {
		// Bloom processing (specifying the same target for src and dest is faster)
		m_bloomEffect->Render(src, src);
	}
	if (m_tonemapEffect) {
		// Tone map processing(final target is specified as dest)
		dest = targetRenderTexture;
		m_tonemapEffect->Render(src, dest);
	}
}

bool Renderer::BeginRecord( int32_t width, int32_t height )
{
	assert( !m_recording );
	
	m_recordingWidth = width;
	m_recordingHeight = height;

	m_recording = true;

	graphics->BeginRecord(m_recordingWidth, m_recordingHeight);

	return true;
}

void Renderer::EndRecord(std::vector<Effekseer::Color>& pixels, bool generateAlpha, bool removeAlpha)
{
	assert(m_recording);

	graphics->EndRecord(pixels);

	auto f2b = [](float v) -> uint8_t
	{
		auto v_ = v * 255;
		if (v_ > 255) v_ = 255;
		if (v_ < 0) v_ = 0;
		return v_;
	};

	auto b2f = [](uint8_t v) -> float
	{
		auto v_ = (float)v / 255.0f;
		return v_;
	};

	// 強制透明化
	for (int32_t i = 0; i < m_recordingWidth * m_recordingHeight; i++)
	{
		if (generateAlpha)
		{
			auto rf = b2f(pixels[i].R);
			auto gf = b2f(pixels[i].G);
			auto bf = b2f(pixels[i].B);
			auto oaf = b2f(pixels[i].A);

			rf = rf * oaf;
			gf = gf * oaf;
			bf = bf * oaf;

			auto af = rf;
			af = Effekseer::Max(af, gf);
			af = Effekseer::Max(af, bf);

			if (af > 0.0f)
			{
				pixels[i].R = f2b(rf / af);
				pixels[i].G = f2b(gf / af);
				pixels[i].B = f2b(bf / af);
			}

			pixels[i].A = f2b(af);
		}

		if (removeAlpha)
		{
			pixels[i].A = 255;
		}
	}

	m_recording = false;
}

void Renderer::LoadBackgroundImage(const char16_t* path)
{
	if (backgroundPath == path) return;

	backgroundPath = path;
	
	if (backgroundData != nullptr)
	{
		textureLoader->Unload(backgroundData);
		backgroundData = nullptr;
	}

	backgroundData = textureLoader->Load(path, Effekseer::TextureType::Color);
}

void Renderer::CopyToBackground()
{
	graphics->CopyToBackground();

	if (graphics->GetDeviceType() == efk::DeviceType::OpenGL)
	{
		auto r = (::EffekseerRendererGL::Renderer*)graphics->GetRenderer();
		r->SetBackground((GLuint)(size_t)graphics->GetBack());
	}
#ifdef _WIN32
	else if (graphics->GetDeviceType() == efk::DeviceType::DirectX11)
	{
		auto r = (::EffekseerRendererDX11::Renderer*)graphics->GetRenderer();
		r->SetBackground((ID3D11ShaderResourceView*)graphics->GetBack());
	}
	else
	{
		auto r = (::EffekseerRendererDX9::Renderer*)graphics->GetRenderer();
		r->SetBackground((IDirect3DTexture9*)graphics->GetBack());
	}
#endif
}

uint64_t Renderer::GetViewID()
{
	return viewRenderTexture->GetViewID();
}

}
