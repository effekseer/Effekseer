
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.Shader.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::Shader(
	RendererImplemented* renderer,
	const uint8_t vertexShader_[],
	int32_t vertexShaderSize,
	const uint8_t pixelShader_[],
	int32_t pixelShaderSize,
	D3DVERTEXELEMENT9 decl[],
	IDirect3DVertexShader9* vertexShader,
	IDirect3DPixelShader9* pixelShader,
	IDirect3DVertexDeclaration9* vertexDeclaration)
	: DeviceObject(renderer)
	, m_vertexShader(vertexShader)
	, m_pixelShader(pixelShader)
	, m_vertexDeclaration(vertexDeclaration)
	, m_vertexConstantBuffer(NULL)
	, m_pixelConstantBuffer(NULL)
	, m_vertexRegisterCount(0)
	, m_pixelRegisterCount(0)
{
	m_vertexShaderData.resize(vertexShaderSize);
	memcpy(&m_vertexShaderData[0], vertexShader_, vertexShaderSize);

	m_pixelShaderData.resize(pixelShaderSize);
	memcpy(&m_pixelShaderData[0], pixelShader_, pixelShaderSize);

	int32_t index = 0;
	D3DVERTEXELEMENT9 end = D3DDECL_END();
	while (decl[index].Stream != 0xFF)
	{
		m_elements.push_back(decl[index]);
		index++;
	}
	m_elements.push_back(end);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	ES_SAFE_RELEASE(m_vertexShader);
	ES_SAFE_RELEASE(m_pixelShader);
	ES_SAFE_RELEASE(m_vertexDeclaration);
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader* Shader::Create(
	RendererImplemented* renderer,
	const uint8_t vertexShader[],
	int32_t vertexShaderSize,
	const uint8_t pixelShader[],
	int32_t pixelShaderSize,
	const char* name,
	D3DVERTEXELEMENT9 decl[])
{
	assert(renderer != NULL);
	assert(renderer->GetDevice() != NULL);

	HRESULT hr;

	IDirect3DVertexShader9* vs = NULL;
	IDirect3DPixelShader9* ps = NULL;

	hr = renderer->GetDevice()->CreateVertexShader(
		(const DWORD*)vertexShader,
		&vs);

	if (FAILED(hr))
	{
		printf("* %s Error\n", name);
		printf("Unknown Error\n");

		return NULL;
	}

	hr = renderer->GetDevice()->CreatePixelShader(
		(const DWORD*)pixelShader,
		&ps);

	if (FAILED(hr))
	{
		printf("* %s Error\n", name);
		printf("Unknown Error\n");

		return NULL;
	}

	IDirect3DVertexDeclaration9* vertexDeclaration = NULL;
	renderer->GetDevice()->CreateVertexDeclaration(decl, &vertexDeclaration);

	return new Shader(
		renderer,

		vertexShader,
		vertexShaderSize,
		pixelShader,
		pixelShaderSize,
		decl,

		vs,
		ps,
		vertexDeclaration);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnLostDevice()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnResetDevice()
{
	if (m_vertexShader == NULL)
	{

		GetRenderer()->GetDevice()->CreateVertexShader(
			(const DWORD*)&m_vertexShaderData[0],
			&m_vertexShader);

		GetRenderer()->GetDevice()->CreatePixelShader(
			(const DWORD*)&m_pixelShaderData[0],
			&m_pixelShader);

		GetRenderer()->GetDevice()->CreateVertexDeclaration(&m_elements[0], &m_vertexDeclaration);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnChangeDevice()
{
	ES_SAFE_RELEASE(m_vertexShader);
	ES_SAFE_RELEASE(m_pixelShader);
	ES_SAFE_RELEASE(m_vertexDeclaration);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	if (m_vertexRegisterCount > 0)
	{
		GetRenderer()->GetDevice()->SetVertexShaderConstantF(
			0,
			(float*)m_vertexConstantBuffer,
			m_vertexRegisterCount);
	}

	if (m_pixelRegisterCount > 0)
	{
		GetRenderer()->GetDevice()->SetPixelShaderConstantF(
			0,
			(float*)m_pixelConstantBuffer,
			m_pixelRegisterCount);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9