#include "EffekseerRenderer.Renderer_Impl.h"

namespace EffekseerRenderer
{

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

}