
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
class Shader
	: public DeviceObject,
	  public ::EffekseerRenderer::ShaderBase
{
private:
	/* 再構成時の元データ保存用 */
	std::vector<uint8_t> m_vertexShaderData;
	std::vector<uint8_t> m_pixelShaderData;
	std::vector<D3DVERTEXELEMENT9> m_elements;

	/* DX9 */
	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DPixelShader9* m_pixelShader;
	IDirect3DVertexDeclaration9* m_vertexDeclaration;

	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;

	int32_t m_vertexRegisterCount;
	int32_t m_pixelRegisterCount;

	Shader(
		RendererImplemented* renderer,
		const uint8_t vertexShader_[],
		int32_t vertexShaderSize,
		const uint8_t pixelShader_[],
		int32_t pixelShaderSize,
		D3DVERTEXELEMENT9 decl[],
		IDirect3DVertexShader9* vertexShader,
		IDirect3DPixelShader9* pixelShader,
		IDirect3DVertexDeclaration9* vertexDeclaration);

public:
	virtual ~Shader();

	static Shader* Create(
		RendererImplemented* renderer,
		const uint8_t vertexShader[],
		int32_t vertexShaderSize,
		const uint8_t pixelShader[],
		int32_t pixelShaderSize,
		const char* name,
		D3DVERTEXELEMENT9 decl[]);

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual void OnChangeDevice();

public:
	IDirect3DVertexShader9* GetVertexShader() const
	{
		return m_vertexShader;
	}
	IDirect3DPixelShader9* GetPixelShader() const
	{
		return m_pixelShader;
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

	void SetVertexRegisterCount(int32_t count)
	{
		assert(count <= 256);
		m_vertexRegisterCount = count;
	}
	void SetPixelRegisterCount(int32_t count)
	{
		assert(count <= 256);
		m_pixelRegisterCount = count;
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