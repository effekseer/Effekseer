#ifndef	__EFFEKSEERRENDERER_RENDERER_IMPL_H__
#define	__EFFEKSEERRENDERER_RENDERER_IMPL_H__

#include <Effekseer.h>

#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

class Renderer::Impl
{
private:
	UVStyle textureUVStyle = UVStyle::Normal;
	UVStyle backgroundTextureUVStyle = UVStyle::Normal;

public:

	UVStyle GetTextureUVStyle() const;

	void SetTextureUVStyle(UVStyle style);

	UVStyle GetBackgroundTextureUVStyle() const;

	void SetBackgroundTextureUVStyle(UVStyle style);

};

}

#endif