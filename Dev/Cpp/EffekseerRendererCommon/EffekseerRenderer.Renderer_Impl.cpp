#include "EffekseerRenderer.Renderer_Impl.h"
#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

Renderer::Impl::~Impl()
{
	ES_SAFE_DELETE(depthTexture_);
}

::Effekseer::Vector3D Renderer::Impl::GetLightDirection() const
{
	return ToStruct(lightDirection_);
}

void Renderer::Impl::SetLightDirection(const ::Effekseer::Vector3D& direction)
{
	lightDirection_ = direction;
}

const ::Effekseer::Color& Renderer::Impl::GetLightColor() const
{
	return lightColor_;
}

void Renderer::Impl::SetLightColor(const ::Effekseer::Color& color)
{
	lightColor_ = color;
}

const ::Effekseer::Color& Renderer::Impl::GetLightAmbientColor() const
{
	return lightAmbient_;
}

void Renderer::Impl::SetLightAmbientColor(const ::Effekseer::Color& color)
{
	lightAmbient_ = color;
}

void Renderer::Impl::CalculateCameraProjectionMatrix()
{
	cameraProjMat_ = cameraMat_ * projectionMat_;
}

::Effekseer::Matrix44 Renderer::Impl::GetProjectionMatrix() const
{
	return ToStruct(projectionMat_);
}

void Renderer::Impl::SetProjectionMatrix(const ::Effekseer::Matrix44& mat)
{
	projectionMat_ = mat;
}

::Effekseer::Matrix44 Renderer::Impl::GetCameraMatrix() const
{
	return ToStruct(cameraMat_);
}

void Renderer::Impl::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	cameraFrontDirection_ = ::Effekseer::Vec3f(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);

	auto localPos = ::Effekseer::Vec3f(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
	auto f = cameraFrontDirection_;
	auto r = ::Effekseer::Vec3f(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
	auto u = ::Effekseer::Vec3f(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

	cameraPosition_ = r * localPos.GetX() + u * localPos.GetY() + f * localPos.GetZ();

	// To optimize particle, cameraFontDirection_ is normalized
	cameraFrontDirection_ = cameraFrontDirection_.NormalizePrecisely();
	cameraMat_ = mat;
}

::Effekseer::Matrix44 Renderer::Impl::GetCameraProjectionMatrix() const
{
	return ToStruct(cameraProjMat_);
}

::Effekseer::Vector3D Renderer::Impl::GetCameraFrontDirection() const
{
	return ToStruct(cameraFrontDirection_);
}

::Effekseer::Vector3D Renderer::Impl::GetCameraPosition() const
{
	return ToStruct(cameraPosition_);
}

void Renderer::Impl::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	cameraFrontDirection_ = front;
	cameraPosition_ = position;

	// To optimize particle, cameraFontDirection_ is normalized
	cameraFrontDirection_ = cameraFrontDirection_.NormalizePrecisely();
}

void Renderer::Impl::CreateProxyTextures(Renderer* renderer)
{
	whiteProxyTexture_ = renderer->CreateProxyTexture(::EffekseerRenderer::ProxyTextureType::White);
	normalProxyTexture_ = renderer->CreateProxyTexture(::EffekseerRenderer::ProxyTextureType::Normal);
}

void Renderer::Impl::DeleteProxyTextures(Renderer* renderer)
{
	renderer->DeleteProxyTexture(whiteProxyTexture_);
	renderer->DeleteProxyTexture(normalProxyTexture_);
	whiteProxyTexture_ = nullptr;
	normalProxyTexture_ = nullptr;
}

::Effekseer::TextureData* Renderer::Impl::GetProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	if (type == EffekseerRenderer::ProxyTextureType::White)
		return whiteProxyTexture_;
	if (type == EffekseerRenderer::ProxyTextureType::Normal)
		return normalProxyTexture_;
	return nullptr;
}

UVStyle Renderer::Impl::GetTextureUVStyle() const
{
	return textureUVStyle;
}

void Renderer::Impl::SetTextureUVStyle(UVStyle style)
{
	textureUVStyle = style;
}

UVStyle Renderer::Impl::GetBackgroundTextureUVStyle() const
{
	return backgroundTextureUVStyle;
}

void Renderer::Impl::SetBackgroundTextureUVStyle(UVStyle style)
{
	backgroundTextureUVStyle = style;
}

int32_t Renderer::Impl::GetDrawCallCount() const
{
	return drawcallCount;
}

int32_t Renderer::Impl::GetDrawVertexCount() const
{
	return drawvertexCount;
}

void Renderer::Impl::ResetDrawCallCount()
{
	drawcallCount = 0;
}

void Renderer::Impl::ResetDrawVertexCount()
{
	drawvertexCount = 0;
}

float Renderer::Impl::GetTime() const
{
	return time_;
}

void Renderer::Impl::SetTime(float time)
{
	time_ = time;
}

Effekseer::RenderMode Renderer::Impl::GetRenderMode() const
{
	if (!isRenderModeValid)
	{
		printf("RenderMode is not implemented.\n");
		return Effekseer::RenderMode::Normal;
	}

	return renderMode_;
}

void Renderer::Impl::SetRenderMode(Effekseer::RenderMode renderMode)
{
	renderMode_ = renderMode;
}

void Renderer::Impl::GetDepth(::Effekseer::TextureData*& texture, DepthReconstructionParameter& reconstructionParam)
{
	texture = depthTexture_;
	reconstructionParam = reconstructionParam_;
}

void Renderer::Impl::GetDepth(::Effekseer::Backend::TextureRef& texture, DepthReconstructionParameter& reconstructionParam)
{
	texture = depthBackendTexture_;
	reconstructionParam = reconstructionParam_;
}

void Renderer::Impl::SetDepth(::Effekseer::Backend::TextureRef texture, const DepthReconstructionParameter& reconstructionParam)
{
	depthBackendTexture_ = texture;
	reconstructionParam_ = reconstructionParam;

	if (texture != nullptr)
	{
		if (depthTexture_ == nullptr)
		{
			depthTexture_ = new Effekseer::TextureData();		
		}
		depthTexture_->HasMipmap = depthBackendTexture_->GetHasMipmap();
		depthTexture_->TexturePtr = depthBackendTexture_;
	}
	else if (texture == nullptr && depthTexture_ != nullptr)
	{
		ES_SAFE_DELETE(depthTexture_);
	}
}

} // namespace EffekseerRenderer