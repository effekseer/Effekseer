
#pragma once

#include "EffekseerTool.Base.h"

#include "../Graphics/StaticMeshRenderer.h"
#include "../RenderedEffectGenerator.h"
#include "../efk.Base.h"
#include <functional>
#include <string>

namespace EffekseerTool
{

class MainScreenRenderedEffectGenerator : public Effekseer::Tool::RenderedEffectGenerator
{
private:
	std::shared_ptr<::EffekseerRenderer::Grid> grid_;
	std::shared_ptr<::EffekseerRenderer::Guide> guide_;
	std::shared_ptr<::EffekseerRenderer::Culling> culling_;

	Effekseer::TextureLoaderRef textureLoader_;
	std::u16string backgroundPath;

public:
	virtual ~MainScreenRenderedEffectGenerator();

	bool InitializedPrePost();

	void OnAfterClear() override;

	void OnBeforePostprocess() override;

	Effekseer::Color GridColor = Effekseer::Color(255, 255, 255, 255);

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

	Effekseer::Vector3D CullingPosition;

	void LoadBackgroundImage(const char16_t* path);
};

} // namespace EffekseerTool
