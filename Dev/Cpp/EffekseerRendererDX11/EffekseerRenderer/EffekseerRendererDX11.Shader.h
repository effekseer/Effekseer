
#ifndef	__EFFEKSEERRENDERER_DX11_SHADER_H__
#define	__EFFEKSEERRENDERER_DX11_SHADER_H__

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
class Shader
	: public DeviceObject
{
private:
	
	ID3D11VertexShader*			m_vertexShader;
	ID3D11PixelShader*			m_pixelShader;
	ID3D11InputLayout*			m_vertexDeclaration;
	ID3D11Buffer*			m_constantBufferToVS;
	ID3D11Buffer*			m_constantBufferToPS;

	void*					m_vertexConstantBuffer;
	void*					m_pixelConstantBuffer;

	int32_t					m_vertexRegisterCount;
	int32_t					m_pixelRegisterCount;

	Shader(
		RendererImplemented* renderer,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11InputLayout* vertexDeclaration );
public:
	virtual ~Shader();

	static Shader* Create( 
		RendererImplemented* renderer, 
		const uint8_t vertexShader[], 
		int32_t vertexShaderSize,
		const uint8_t pixelShader[], 
		int32_t pixelShaderSize,
		const char* name, 
		const D3D11_INPUT_ELEMENT_DESC decl[],
		int32_t layoutCount );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	ID3D11VertexShader* GetVertexShader() const { return m_vertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return m_pixelShader; }
	ID3D11InputLayout* GetLayoutInterface() const { return m_vertexDeclaration; }

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	void* GetVertexConstantBuffer() { return m_vertexConstantBuffer; }
	void* GetPixelConstantBuffer() { return m_pixelConstantBuffer; }

	void SetVertexRegisterCount(int32_t count){ m_vertexRegisterCount = count; }
	void SetPixelRegisterCount(int32_t count){ m_pixelRegisterCount = count; }

	void SetConstantBuffer();
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_SHADER_H__