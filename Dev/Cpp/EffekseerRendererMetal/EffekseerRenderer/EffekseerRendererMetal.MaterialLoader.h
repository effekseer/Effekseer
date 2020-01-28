/*
#ifndef __EFFEKSEERRENDERER_METAL_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_METAL_MATERIALLOADER_H__

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.MaterialLoader.h"

namespace EffekseerRendererMetal
{

class MaterialLoader : public ::EffekseerRendererLLGI::MaterialLoader
{
protected:
    void Deserialize(uint8_t* data, uint32_t datasize, LLGI::CompilerResult& result) override;
    
public:
    MaterialLoader(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice,
                   ::Effekseer::FileInterface* fileInterface,
                   ::Effekseer::CompiledMaterialPlatformType platformType,
                   ::Effekseer::MaterialCompiler* materialCompiler);
    
};

}
#endif // __EFFEKSEERRENDERER_METAL_MATERIALLOADER_H__
*/