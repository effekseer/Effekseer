#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef	__EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__
#define	__EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:
	ID3D11Device*						device;
	::Effekseer::FileInterface*			m_fileInterface;
	::Effekseer::DefaultFileInterface	m_defaultFileInterface;

public:
	TextureLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface = NULL );
	virtual ~TextureLoader();

public:
	Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;

	void Unload(Effekseer::TextureData* data ) override;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__