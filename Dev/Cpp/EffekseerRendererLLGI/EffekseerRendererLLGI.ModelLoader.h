
#ifndef __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__
#define __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

class ModelLoader : public ::Effekseer::ModelLoader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface);
	virtual ~ModelLoader();

public:
	Effekseer::Model* Load(const EFK_CHAR* path) override;

	Effekseer::Model* Load(const void* data, int32_t size) override;

	void Unload(Effekseer::Model* data) override;
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__
