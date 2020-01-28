/*
#include "EffekseerRendererMetal.MaterialLoader.h"

#include "../../3rdParty/LLGI/src/LLGI.Compiler.h"

namespace EffekseerRendererMetal
{

void MaterialLoader::Deserialize(uint8_t* data, uint32_t datasize, LLGI::CompilerResult& result)
{
    result.Binary.resize(1);
    result.Binary[0].resize(datasize);
    memcpy(result.Binary[0].data(), data, datasize);
}

MaterialLoader::MaterialLoader(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice,
                               ::Effekseer::FileInterface* fileInterface,
                               ::Effekseer::CompiledMaterialPlatformType platformType,
                               ::Effekseer::MaterialCompiler* materialCompiler)
    : ::EffekseerRendererLLGI::MaterialLoader(graphicsDevice, fileInterface, platformType, materialCompiler)
{
}

}
*/