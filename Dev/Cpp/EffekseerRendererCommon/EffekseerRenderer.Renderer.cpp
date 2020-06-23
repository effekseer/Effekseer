
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.Renderer_Impl.h"
#include <assert.h>

namespace EffekseerRenderer
{

Renderer::Renderer()
{
	impl = new Impl();
}

Renderer::~Renderer()
{
	ES_SAFE_DELETE(impl);
}

Renderer::Impl* Renderer::GetImpl()
{
	return impl;
}

::Effekseer::Vector3D Renderer::GetLightDirection() const
{
	return impl->GetLightDirection();
}

void Renderer::SetLightDirection(const ::Effekseer::Vector3D& direction)
{
	impl->SetLightDirection(direction);
}

const ::Effekseer::Color& Renderer::GetLightColor() const
{
	return impl->GetLightColor();
}

void Renderer::SetLightColor(const ::Effekseer::Color& color)
{
	impl->SetLightColor(color);
}

const ::Effekseer::Color& Renderer::GetLightAmbientColor() const
{
	return impl->GetLightAmbientColor();
}

void Renderer::SetLightAmbientColor(const ::Effekseer::Color& color)
{
	impl->SetLightAmbientColor(color);
}

::Effekseer::Matrix44 Renderer::GetProjectionMatrix() const
{
	return impl->GetProjectionMatrix();
}

void Renderer::SetProjectionMatrix(const ::Effekseer::Matrix44& mat)
{
	impl->SetProjectionMatrix(mat);
}

::Effekseer::Matrix44 Renderer::GetCameraMatrix() const
{
	return impl->GetCameraMatrix();
}

void Renderer::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	impl->SetCameraMatrix(mat);
}

::Effekseer::Matrix44 Renderer::GetCameraProjectionMatrix() const
{
	return impl->GetCameraProjectionMatrix();
}

::Effekseer::Vector3D Renderer::GetCameraFrontDirection() const
{
	return impl->GetCameraFrontDirection();
}

::Effekseer::Vector3D Renderer::GetCameraPosition() const
{
	return impl->GetCameraPosition();
}

void Renderer::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	impl->SetCameraParameter(front, position);
}

int32_t Renderer::GetDrawCallCount() const
{
	return impl->GetDrawCallCount();
}

int32_t Renderer::GetDrawVertexCount() const
{
	return impl->GetDrawVertexCount();
}

void Renderer::ResetDrawCallCount()
{
	impl->ResetDrawCallCount();
}

void Renderer::ResetDrawVertexCount()
{
	impl->ResetDrawVertexCount();
}

Effekseer::RenderMode Renderer::GetRenderMode() const
{
	return impl->GetRenderMode();
}

void Renderer::SetRenderMode(Effekseer::RenderMode renderMode)
{
	impl->SetRenderMode(renderMode);
}

UVStyle Renderer::GetTextureUVStyle() const
{
	return impl->GetTextureUVStyle();
}

void Renderer::SetTextureUVStyle(UVStyle style)
{
	impl->SetTextureUVStyle(style);
}

UVStyle Renderer::GetBackgroundTextureUVStyle() const
{
	return impl->GetBackgroundTextureUVStyle();
}

void Renderer::SetBackgroundTextureUVStyle(UVStyle style)
{
	impl->SetBackgroundTextureUVStyle(style);
}

float Renderer::GetTime() const
{
	return impl->GetTime();
}

void Renderer::SetTime(float time)
{
	impl->SetTime(time);
}

void Renderer::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	// not implemented
	assert(0);
}

} // namespace EffekseerRenderer