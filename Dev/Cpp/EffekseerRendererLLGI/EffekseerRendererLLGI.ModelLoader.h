
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__
#define __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

class ModelLoader : public ::Effekseer::ModelLoader
{
private:
	LLGI::Graphics* graphics = nullptr;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(LLGI::Graphics* graphics, ::Effekseer::FileInterface* fileInterface);
	virtual ~ModelLoader();

public:
	void* Load(const EFK_CHAR* path);

	void Unload(void* data);
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_MODELLOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__