
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__

#include "EffekseerRendererLLGI.Base.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>

namespace EffekseerRendererLLGI
{

class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	static const int32_t DepthTestCount = 2;
	static const int32_t DepthWriteCount = 2;
	static const int32_t CulTypeCount = 3;
	static const int32_t AlphaTypeCount = 5;
	static const int32_t TextureFilterCount = 2;
	static const int32_t TextureWrapCount = 2;

	RendererImplemented* renderer_;

public:
	RenderState(RendererImplemented* renderer);
	virtual ~RenderState();

	void Update(bool forced);
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERSTATE_H__
