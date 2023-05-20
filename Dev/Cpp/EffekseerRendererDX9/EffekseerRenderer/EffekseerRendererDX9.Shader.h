
#ifndef __EFFEKSEERRENDERER_DX9_SHADER_H__
#define __EFFEKSEERRENDERER_DX9_SHADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
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
class Shader : public DeviceObject, public ::EffekseerRenderer::ShaderBase
{
private:
	/* 再構成時の元データ保存用 */
	std::vector<D3DVERTEXELEMENT9> m_elements;

	/* DX9 */
	IDirect3DVertexDeclaration9* m_vertexDeclaration;

	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;

	int32_t m_vertexRegisterCount;
	int32_t m_pixelRegisterCount;

	Backend::ShaderRef shader_;

	Shader(RendererImplemented* renderer,
		   Backend::ShaderRef shader,
		   D3DVERTEXELEMENT9 decl[],
		   IDirect3DVertexDeclaration9* vertexDeclaration,
		   bool hasRefCount);

public:
	virtual ~Shader();

	static Shader* Create(RendererImplemented* renderer,
						  Effekseer::Backend::ShaderRef shader,
						  const char* name,
						  D3DVERTEXELEMENT9 decl[],
						  bool hasRefCount);

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual void OnChangeDevice();

public:
	IDirect3DVertexShader9* GetVertexShader() const
	{
		return shader_->GetVertexShader();
	}
	IDirect3DPixelShader9* GetPixelShader() const
	{
		return shader_->GetPixelShader();
	}
	IDirect3DVertexDeclaration9* GetLayoutInterface() const
	{
		return m_vertexDeclaration;
	}

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	void* GetVertexConstantBuffer()
	{
		return m_vertexConstantBuffer;
	}
	void* GetPixelConstantBuffer()
	{
		return m_pixelConstantBuffer;
	}

	void SetConstantBuffer();
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_SHADER_H__