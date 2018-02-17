
#ifndef	__EFFEKSEERRENDERER_PASTE_H__
#define	__EFFEKSEERRENDERER_PASTE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.DeviceObject.h>
#include "../Graphics/Platform/DX9/efk.ImageRendererDX9.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Paste
	: public EffekseerRendererDX9::DeviceObject
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Vector2D	UV;
	};

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	EffekseerRendererDX9::Shader*						m_shader;
	efk::ImageRendererDX9*								imageRenderer = nullptr;

	Paste(EffekseerRendererDX9::RendererImplemented* renderer);
public:

	virtual ~Paste();

	static Paste* Create(EffekseerRendererDX9::RendererImplemented* renderer);

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Rendering(IDirect3DTexture9* texture, int32_t width, int32_t height);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_PASTE_H__