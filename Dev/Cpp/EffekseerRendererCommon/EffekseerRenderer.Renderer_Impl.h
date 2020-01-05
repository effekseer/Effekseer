#ifndef __EFFEKSEERRENDERER_RENDERER_IMPL_H__
#define __EFFEKSEERRENDERER_RENDERER_IMPL_H__

#include <Effekseer.h>

#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

class Renderer::Impl
{
private:
	::Effekseer::Matrix44 projectionMat_;
	::Effekseer::Matrix44 cameraMat_;
	::Effekseer::Matrix44 cameraProjMat_;

	::Effekseer::Vector3D cameraPosition_;
	::Effekseer::Vector3D cameraFrontDirection_;

	UVStyle textureUVStyle = UVStyle::Normal;
	UVStyle backgroundTextureUVStyle = UVStyle::Normal;
	float time_ = 0.0f;

	Effekseer::RenderMode renderMode_ = Effekseer::RenderMode::Normal;

	::Effekseer::TextureData* whiteProxyTexture_ = nullptr;
	::Effekseer::TextureData* normalProxyTexture_ = nullptr;

public:
	int32_t drawcallCount = 0;
	int32_t drawvertexCount = 0;
	bool isRenderModeValid = true;

	const ::Effekseer::Matrix44& GetProjectionMatrix() const;

	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	const ::Effekseer::Matrix44& GetCameraMatrix() const;

	void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	::Effekseer::Vector3D GetCameraFrontDirection() const;

	::Effekseer::Vector3D GetCameraPosition() const;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	::Effekseer::Matrix44& GetCameraProjectionMatrix();

	void CreateProxyTextures(Renderer* renderer);

	void DeleteProxyTextures(Renderer* renderer);

	::Effekseer::TextureData* GetProxyTexture(EffekseerRenderer::ProxyTextureType type);

	UVStyle GetTextureUVStyle() const;

	void SetTextureUVStyle(UVStyle style);

	UVStyle GetBackgroundTextureUVStyle() const;

	void SetBackgroundTextureUVStyle(UVStyle style);

	int32_t GetDrawCallCount() const;

	int32_t GetDrawVertexCount() const;

	void ResetDrawCallCount();

	void ResetDrawVertexCount();

	float GetTime() const;

	void SetTime(float time);

	Effekseer::RenderMode GetRenderMode() const;

	void SetRenderMode(Effekseer::RenderMode renderMode);
};

} // namespace EffekseerRenderer

#endif