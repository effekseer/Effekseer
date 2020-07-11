
#include "EffekseerTool.Renderer.h"
#include "EffekseerTool.Culling.h"
#include "EffekseerTool.Grid.h"
#include "EffekseerTool.Guide.h"
#include "EffekseerTool.Paste.h"

#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../RenderedEffectGenerator.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace EffekseerTool
{

MainScreenRenderedEffectGenerator::~MainScreenRenderedEffectGenerator()
{
	if (backgroundData_ != nullptr)
	{
		textureLoader_->Unload(backgroundData_);
		backgroundData_ = nullptr;
	}

	ES_SAFE_DELETE(textureLoader_);
}

bool MainScreenRenderedEffectGenerator::InitializedPrePost()
{
	grid_ = std::shared_ptr<::EffekseerRenderer::Grid>(::EffekseerRenderer::Grid::Create(graphics_, renderer_));
	if (grid_ == nullptr)
	{
		return false;
	}

	spdlog::trace("OK Grid");

	guide_ = std::shared_ptr<::EffekseerRenderer::Guide>(::EffekseerRenderer::Guide::Create(graphics_, renderer_));
	if (guide_ == nullptr)
	{
		return false;
	}

	spdlog::trace("OK Guide");

	culling_ = std::shared_ptr<::EffekseerRenderer::Culling>(::EffekseerRenderer::Culling::Create(graphics_, renderer_));
	if (culling_ == nullptr)
	{
		return false;
	}

	spdlog::trace("OK Culling");

	background_ = std::shared_ptr<::EffekseerRenderer::Paste>(::EffekseerRenderer::Paste::Create(graphics_, renderer_));
	if (background_ == nullptr)
	{
		return false;
	}

	textureLoader_ = renderer_->CreateTextureLoader();

	spdlog::trace("OK Background");

	return true;
}

void MainScreenRenderedEffectGenerator::OnAfterClear()
{
	// Render background (the size of texture is ignored)
	if (backgroundData_ != nullptr)
	{
		background_->Rendering(backgroundData_, 1024, 1024);
	}

	if (IsGridShown)
	{
		grid_->SetLength(GridLength);
		grid_->IsShownXY = IsGridXYShown;
		grid_->IsShownXZ = IsGridXZShown;
		grid_->IsShownYZ = IsGridYZShown;
		grid_->Rendering(GridColor, IsRightHand);
	}

	{
		culling_->IsShown = IsCullingShown;
		culling_->Radius = CullingRadius;
		culling_->X = CullingPosition.X;
		culling_->Y = CullingPosition.Y;
		culling_->Z = CullingPosition.Z;
		culling_->Rendering(IsRightHand);
	}
}

void MainScreenRenderedEffectGenerator::OnBeforePostprocess()
{
	if (RendersGuide)
	{
		guide_->Rendering(screenSize_.X, screenSize_.Y, GuideWidth, GuideHeight);
	}
}

void MainScreenRenderedEffectGenerator::LoadBackgroundImage(const char16_t* path)
{
	if (backgroundPath == path)
		return;

	backgroundPath = path;

	if (backgroundData_ != nullptr)
	{
		textureLoader_->Unload(backgroundData_);
		backgroundData_ = nullptr;
	}

	backgroundData_ = textureLoader_->Load(path, Effekseer::TextureType::Color);
}

Renderer::Renderer(efk::DeviceType deviceType)
	:  m_projection(PROJECTION_TYPE_PERSPECTIVE)
	, deviceType_(deviceType)

	, RateOfMagnification(1.0f)
	, IsRightHand(true)
	, Distortion(Effekseer::Tool::DistortionType::Current)
	, RenderingMode(Effekseer::RenderMode::Normal)

	, BackgroundColor(0, 0, 0, 255)
	, IsBackgroundTranslucent(false)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	if (m_projection == PROJECTION_TYPE_PERSPECTIVE)
	{
		SetPerspectiveFov(width, height);
	}
	else if (m_projection == PROJECTION_TYPE_ORTHOGRAPHIC)
	{
		SetOrthographic(width, height);
	}

	return true;
}

eProjectionType Renderer::GetProjectionType()
{
	return m_projection;
}

void Renderer::SetProjectionType(eProjectionType type)
{
	m_projection = type;

	if (m_projection == PROJECTION_TYPE_PERSPECTIVE)
	{
		SetPerspectiveFov(screenWidth, screenHeight);
	}
	else if (m_projection == PROJECTION_TYPE_ORTHOGRAPHIC)
	{
		SetOrthographic(screenWidth, screenHeight);
	}
}

void Renderer::SetPerspectiveFov(int width, int height)
{
	::Effekseer::Matrix44 proj;

	if (deviceType_ == efk::DeviceType::OpenGL)
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

	m_projMat = proj;
}

void Renderer::SetOrthographic(int width, int height)
{
	::Effekseer::Matrix44 proj;

	if (IsRightHand)
	{
		// Right hand coordinate
		proj.OrthographicRH((float)width / m_orthoScale / RateOfMagnification,
							(float)height / m_orthoScale / RateOfMagnification,
							ClippingStart,
							ClippingEnd);
	}
	else
	{
		// Left hand coordinate
		proj.OrthographicLH((float)width / m_orthoScale / RateOfMagnification,
							(float)height / m_orthoScale / RateOfMagnification,
							ClippingStart,
							ClippingEnd);
	}

	m_projMat = proj;
}

void Renderer::SetOrthographicScale(float scale)
{
	m_orthoScale = scale;
}

void Renderer::RecalcProjection()
{
	if (m_projection == PROJECTION_TYPE_PERSPECTIVE)
	{
		SetPerspectiveFov(screenWidth, screenHeight);
	}
	else if (m_projection == PROJECTION_TYPE_ORTHOGRAPHIC)
	{
		SetOrthographic(screenWidth, screenHeight);
	}
}

void Renderer::SetScreenSize(int32_t width, int32_t height)
{
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
}


} // namespace EffekseerTool
