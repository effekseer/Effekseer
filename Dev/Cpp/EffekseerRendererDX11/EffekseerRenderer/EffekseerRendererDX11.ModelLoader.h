﻿
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_DX11_MODELLOADER_H__
#define __EFFEKSEERRENDERER_DX11_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "GraphicsDevice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ModelLoader : public ::Effekseer::ModelLoader
{
private:
	ID3D11Device* device;
	Backend::GraphicsDevice* graphicsDevice_ = nullptr;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface);
	virtual ~ModelLoader();

public:
	Effekseer::Model* Load(const char16_t* path) override;

	Effekseer::Model* Load(const void* data, int32_t size) override;

	void Unload(Effekseer::Model* data) override;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_MODELLOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__