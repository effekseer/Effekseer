#include "EffekseerRenderer.Renderer_Impl.h"
#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

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

UVStyle Renderer::Impl::GetTextureUVStyle() const { return textureUVStyle; }

void Renderer::Impl::SetTextureUVStyle(UVStyle style) { textureUVStyle = style; }

UVStyle Renderer::Impl::GetBackgroundTextureUVStyle() const { return backgroundTextureUVStyle; }

void Renderer::Impl::SetBackgroundTextureUVStyle(UVStyle style) { backgroundTextureUVStyle = style; }

int32_t Renderer::Impl::GetDrawCallCount() const { return drawcallCount; }

int32_t Renderer::Impl::GetDrawVertexCount() const { return drawvertexCount; }

void Renderer::Impl::ResetDrawCallCount() { drawcallCount = 0; }

void Renderer::Impl::ResetDrawVertexCount() { drawvertexCount = 0; }

float Renderer::Impl::GetTime() const { return time_; }

void Renderer::Impl::SetTime(float time) { time_ = time; }

Effekseer::RenderMode Renderer::Impl::GetRenderMode() const
{
	if (!isRenderModeValid)
	{
		printf("RenderMode is not implemented.\n");
		return Effekseer::RenderMode::Normal;
	}

	return renderMode_;
}

void Renderer::Impl::SetRenderMode(Effekseer::RenderMode renderMode) { renderMode_ = renderMode; }

} // namespace EffekseerRenderer