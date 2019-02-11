
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.Renderer_Impl.h"

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

}