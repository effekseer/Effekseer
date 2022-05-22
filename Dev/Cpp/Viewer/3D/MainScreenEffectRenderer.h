
#pragma once

#include "../3D/EffectRenderer.h"
#include "../Math/Vector3F.h"
#include "../Graphics/LineRenderer.h"
#include <Effekseer.h>
#include <functional>
#include <string>

namespace EffekseerRenderer
{
class Grid;
class Guide;
class Culling;
class Paste;
} // namespace EffekseerRenderer

namespace Effekseer::Tool
{

class MainScreenEffectRenderer : public Effekseer::Tool::EffectRenderer
{
private:
	std::shared_ptr<::EffekseerRenderer::Grid> grid_;
	std::shared_ptr<::EffekseerRenderer::Guide> guide_;
	std::shared_ptr<::EffekseerRenderer::Culling> culling_;

	std::shared_ptr<Effekseer::Tool::LineRenderer> lineRenderer_;

	Effekseer::TextureLoaderRef textureLoader_;
	std::u16string backgroundPath;

	void OnAfterClear() override;

	void OnBeforePostprocess() override;

	bool OnAfterInitialize() override;



public:
	virtual ~MainScreenEffectRenderer();

	Effekseer::Tool::Color GridColor = Effekseer::Tool::Color(255, 255, 255, 255);

	int32_t GuideWidth = 100;

	int32_t GuideHeight = 100;

	bool RendersGuide = false;

	bool IsGridShown = true;

	bool IsGridXYShown = false;

	bool IsGridXZShown = true;

	bool IsGridYZShown = false;

	float GridLength = 2.0f;

	bool IsCullingShown = false;

	float CullingRadius = 0.0f;

	bool IsRightHand = true;

	Vector3F CullingPosition;

	void LoadBackgroundImage(const char16_t* path);

	void StartRenderingLines();

	void AddLine(float p0x, float p0y, float p0z, float p1x, float p1y, float p1z, Effekseer::Tool::Color color);

	void EndRenderingLines(const Effekseer::Tool::Matrix44F& cameraMatrix, const Effekseer::Tool::Matrix44F& projectionMatrix);
};

} // namespace Effekseer::Tool
