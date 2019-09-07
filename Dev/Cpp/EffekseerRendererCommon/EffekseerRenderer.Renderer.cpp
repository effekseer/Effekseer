
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.Renderer_Impl.h"
#include <assert.h>

namespace EffekseerRenderer
{

Renderer::Renderer() { impl = new Impl(); }

Renderer::~Renderer() { ES_SAFE_DELETE(impl); }

int32_t Renderer::GetDrawCallCount() const { return impl->GetDrawCallCount(); }

int32_t Renderer::GetDrawVertexCount() const { return impl->GetDrawVertexCount(); }

void Renderer::ResetDrawCallCount() { impl->ResetDrawCallCount(); }

void Renderer::ResetDrawVertexCount() { impl->ResetDrawVertexCount(); }

UVStyle Renderer::GetTextureUVStyle() const { return impl->GetTextureUVStyle(); }

void Renderer::SetTextureUVStyle(UVStyle style) { impl->SetTextureUVStyle(style); }

UVStyle Renderer::GetBackgroundTextureUVStyle() const { return impl->GetBackgroundTextureUVStyle(); }

void Renderer::SetBackgroundTextureUVStyle(UVStyle style) { impl->SetBackgroundTextureUVStyle(style); }

float Renderer::GetTime() const { return impl->GetTime(); }

void Renderer::SetTime(float time) { impl->SetTime(time); }

void Renderer::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	// not implemented
	assert(0);
}

} // namespace EffekseerRenderer