﻿
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

	textureLoader_ = renderer_->CreateTextureLoader();

	if (graphics_->GetGraphicsDevice() != nullptr)
	{

		backgroundRenderer_ = Effekseer::Tool::StaticMeshRenderer::Create(graphics_->GetGraphicsDevice());

		if (backgroundRenderer_ == nullptr)
		{
			return false;
		}

		const float eps = 0.0001f;

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
			vb.VColor = {255, 255, 255, 255};
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
		backgroundMesh_->IsLit = false;

		backgroundRenderer_->SetStaticMesh(backgroundMesh_);
	}

	spdlog::trace("OK Background");

	if (graphics_->GetGraphicsDevice() != nullptr)
	{
		staticMeshRenderer_ = Effekseer::Tool::StaticMeshRenderer::Create(graphics_->GetGraphicsDevice());

		if (staticMeshRenderer_ == nullptr)
		{
			return false;
		}

		Effekseer::CustomVector<Effekseer::Tool::StaticMeshVertex> vbData;
		vbData.resize(4);
		vbData[0].Pos = {-10.0f, 0.0f, -10.0f};
		vbData[0].VColor = {90, 90, 90, 255};
		vbData[1].Pos = {10.0f, 0.0f, -10.0f};
		vbData[1].VColor = {90, 90, 90, 255};
		vbData[2].Pos = {10.0f, 0.0f, 10.0f};
		vbData[2].VColor = {90, 90, 90, 255};
		vbData[3].Pos = {-10.0f, 0.0f, 10.0f};
		vbData[3].VColor = {90, 90, 90, 255};

		for (auto& vb : vbData)
		{
			vb.Normal = {0.0f, 1.0f, 0.0f};
		}
		Effekseer::CustomVector<int32_t> ibData;
		ibData.resize(6);
		ibData[0] = 0;
		ibData[1] = 1;
		ibData[2] = 2;
		ibData[3] = 0;
		ibData[4] = 2;
		ibData[5] = 3;

		auto staticMesh = Effekseer::Tool::StaticMesh::Create(graphics_->GetGraphicsDevice(), vbData, ibData);

		staticMeshRenderer_->SetStaticMesh(staticMesh);
	}

	return true;
}

void MainScreenRenderedEffectGenerator::OnAfterClear()
{
	if (backgroundRenderer_ != nullptr && backgroundMesh_ != nullptr && backgroundData_ != nullptr)
	{
		backgroundMesh_->Texture = backgroundData_->TexturePtr;

		Effekseer::Tool::RendererParameter param{};
		param.CameraMatrix.Indentity();
		param.ProjectionMatrix.Indentity();
		backgroundRenderer_->Render(param);
	}

	if (staticMeshRenderer_ != nullptr && IsGroundShown)
	{
		Effekseer::Tool::RendererParameter param{};
		param.CameraMatrix = renderer_->GetCameraMatrix();
		param.ProjectionMatrix = renderer_->GetProjectionMatrix();
		param.DirectionalLightDirection = renderer_->GetLightDirection().ToFloat4();
		param.DirectionalLightColor = renderer_->GetLightColor().ToFloat4();
		param.AmbientLightColor = renderer_->GetLightAmbientColor().ToFloat4();

		staticMeshRenderer_->Render(param);
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

ViewPointController::ViewPointController()
	: m_projection(PROJECTION_TYPE_PERSPECTIVE)
	, RateOfMagnification(1.0f)
	, IsRightHand(true)
	, RenderingMode(Effekseer::RenderMode::Normal)
{
}

ViewPointController::~ViewPointController()
{
}

void ViewPointController::Initialize(efk::DeviceType deviceType, int width, int height)
{
	deviceType_ = deviceType;
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
}

eProjectionType ViewPointController::GetProjectionType()
{
	return m_projection;
}

void ViewPointController::SetProjectionType(eProjectionType type)
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

void ViewPointController::SetPerspectiveFov(int width, int height)
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

void ViewPointController::SetOrthographic(int width, int height)
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

void ViewPointController::SetOrthographicScale(float scale)
{
	m_orthoScale = scale;
}

void ViewPointController::RecalcProjection()
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

void ViewPointController::SetScreenSize(int32_t width, int32_t height)
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
