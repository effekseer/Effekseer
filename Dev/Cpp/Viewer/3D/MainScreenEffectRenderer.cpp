
#include "MainScreenEffectRenderer.h"
#include "../3D/EffectRenderer.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../EffekseerTool/EffekseerTool.Guide.h"
#include "../Graphics/GraphicsDevice.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Effekseer::Tool
{

MainScreenEffectRenderer::~MainScreenEffectRenderer()
{
	if (backgroundTexture_ != nullptr)
	{
		textureLoader_->Unload(backgroundTexture_);
		backgroundTexture_.Reset();
	}
}

void MainScreenEffectRenderer::OnBeforePostprocess()
{
	if (RendersGuide)
	{
		guide_->Rendering(screenSize_.X, screenSize_.Y, GuideWidth, GuideHeight);
	}
}

bool MainScreenEffectRenderer::OnAfterInitialize()
{
	guide_ = std::shared_ptr<::EffekseerRenderer::Guide>(::EffekseerRenderer::Guide::Create(graphics_->GetGraphics()->GetGraphicsDevice()));
	if (guide_ == nullptr)
	{
		return false;
	}

	spdlog::trace("OK Guide");

	lineRenderer_ = std::make_shared<LineRenderer>(graphics_->GetGraphics()->GetGraphicsDevice());
	if (lineRenderer_ == nullptr)
	{
		return false;
	}

	spdlog::trace("OK Generic Line Renderer");

	textureLoader_ = renderer_->CreateTextureLoader();

	return true;
}

void MainScreenEffectRenderer::StartRenderingLines()
{
	lineRenderer_->ClearCache();
}

void MainScreenEffectRenderer::AddLine(float p0x, float p0y, float p0z, float p1x, float p1y, float p1z, Effekseer::Tool::Color color)
{
	lineRenderer_->DrawLine(Vector3D{p0x, p0y, p0z}, Vector3D{p1x, p1y, p1z}, color);
}

void MainScreenEffectRenderer::EndRenderingLines(const Effekseer::Tool::Matrix44F& cameraMatrix, const Effekseer::Tool::Matrix44F& projectionMatrix)
{
	lineRenderer_->Render(cameraMatrix, projectionMatrix);
}

void MainScreenEffectRenderer::LoadBackgroundImage(const char16_t* path)
{
	if (backgroundPath == path)
		return;

	backgroundPath = path;

	if (backgroundTexture_ != nullptr)
	{
		textureLoader_->Unload(backgroundTexture_);
		backgroundTexture_.Reset();
	}

	backgroundTexture_ = textureLoader_->Load(path, Effekseer::TextureType::Color);
}

} // namespace Effekseer::Tool
