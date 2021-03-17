
#ifndef __EFFEKSEERRENDERER_LLGI_BASE_H__
#define __EFFEKSEERRENDERER_LLGI_BASE_H__

#include "EffekseerRendererLLGI.Base.Pre.h"

#include <Effekseer.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

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

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_BASE_H__