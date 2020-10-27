
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_DX9_MODELLOADER_H__
#define __EFFEKSEERRENDERER_DX9_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.DeviceObject.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ModelLoader
	: public ::Effekseer::ModelLoader
{
private:
	RendererImplemented* renderer_ = nullptr;
	LPDIRECT3DDEVICE9 device_ = nullptr;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(RendererImplemented* renderer, ::Effekseer::FileInterface* fileInterface);
	ModelLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface);
	virtual ~ModelLoader();

public:
	void* Load(const EFK_CHAR* path);

	void* Load(const void* data, int32_t size) override;

	void Unload(void* data);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_MODELLOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__