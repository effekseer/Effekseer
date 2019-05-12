
#ifndef	__EFFEKSEERRENDERER_LLGI_BASE_H__
#define	__EFFEKSEERRENDERER_LLGI_BASE_H__

#include "EffekseerRendererLLGI.Base.Pre.h"

#include <Effekseer.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include <map>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <queue>

namespace EffekseerRendererLLGI
{

class RendererImplemented;

class RenderStateBase;

class DeviceObject;
class Texture;
class TargetTexture;
class DepthTexture;
class VertexBuffer;
class IndexBuffer;
class Shader;

class SpriteRenderer;
class RibbonRenderer;
class RingRenderer;
class ModelRenderer;
class TrackRenderer;

class TextureLoader;

}

#endif	// __EFFEKSEERRENDERER_LLGI_BASE_H__