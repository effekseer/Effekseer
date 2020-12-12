#ifndef __EFFEKSEERRENDERER_RENDERER_IMPL_H__
#define __EFFEKSEERRENDERER_RENDERER_IMPL_H__

#include <Effekseer.Internal.h>
#include <Effekseer.h>

#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

class Renderer::Impl : public ::Effekseer::AlignedAllocationPolicy<16>
{
private:
	::Effekseer::Mat44f projectionMat_;
	::Effekseer::Mat44f cameraMat_;
	::Effekseer::Mat44f cameraProjMat_;

	::Effekseer::Vec3f cameraPosition_{0.0f, 0.0f, 0.0f};
	::Effekseer::Vec3f cameraFrontDirection_{0.0f, 0.0f, 1.0f};

	::Effekseer::Vec3f lightDirection_ = ::Effekseer::Vec3f(1.0f, 1.0f, 1.0f);
	::Effekseer::Color lightColor_ = ::Effekseer::Color(255, 255, 255, 255);
	::Effekseer::Color lightAmbient_ = ::Effekseer::Color(40, 40, 40, 255);

	UVStyle textureUVStyle = UVStyle::Normal;
	UVStyle backgroundTextureUVStyle = UVStyle::Normal;
	float time_ = 0.0f;

	Effekseer::RenderMode renderMode_ = Effekseer::RenderMode::Normal;

	::Effekseer::TextureData* whiteProxyTexture_ = nullptr;
	::Effekseer::TextureData* normalProxyTexture_ = nullptr;

	void SetCameraParameterInternal(const ::Effekseer::Vec3f& front, const ::Effekseer::Vec3f& position);

public:
	int32_t drawcallCount = 0;
	int32_t drawvertexCount = 0;
	bool isRenderModeValid = true;

	::Effekseer::Vector3D GetLightDirection() const;

	void SetLightDirection(const ::Effekseer::Vector3D& direction);

	const ::Effekseer::Color& GetLightColor() const;

	void SetLightColor(const ::Effekseer::Color& color);

	const ::Effekseer::Color& GetLightAmbientColor() const;

	void SetLightAmbientColor(const ::Effekseer::Color& color);

	void CalculateCameraProjectionMatrix();

	::Effekseer::Matrix44 GetProjectionMatrix() const;

	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	::Effekseer::Matrix44 GetCameraMatrix() const;

	void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	::Effekseer::Vector3D GetCameraFrontDirection() const;

	::Effekseer::Vector3D GetCameraPosition() const;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	::Effekseer::Matrix44 GetCameraProjectionMatrix() const;

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