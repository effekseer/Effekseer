
#ifndef __EFFEKSEERRENDERER_METAL_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_METAL_RENDERER_IMPLEMENTED_H__

#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#import <MetalKit/MetalKit.h>

namespace EffekseerRendererMetal
{

class RendererImplemented : public ::EffekseerRendererLLGI::RendererImplemented
{
	void GenerateVertexBuffer() override;

	void GenerateIndexBuffer() override;

public:

    RendererImplemented(int32_t squareMaxCount): ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount)
    {}

	~RendererImplemented()
    {}
    
    /*
    void SetExternalCommandBuffer(id<MTLCommandBuffer> extCommandBuffer);
    
    void SetExternalRenderEncoder(id<MTLRenderCommandEncoder> extRenderEncoder);

	bool BeginRendering() override;

	bool EndRendering() override;
    */
    
    //::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;
};

} // namespace EffekseerRendererMetal

#endif // __EFFEKSEERRENDERER_METAL_RENDERER_IMPLEMENTED_H__
