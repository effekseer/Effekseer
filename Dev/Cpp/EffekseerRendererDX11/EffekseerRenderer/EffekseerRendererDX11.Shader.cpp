
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::Shader(RendererImplemented* renderer,
			   Backend::ShaderRef shader,
			   ID3D11InputLayout* vertexDeclaration,
			   bool hasRefCount)
	: DeviceObject(renderer, hasRefCount)
	, shader_(shader)
	, m_vertexDeclaration(vertexDeclaration)
	, m_constantBufferToVS(nullptr)
	, m_constantBufferToPS(nullptr)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	ES_SAFE_RELEASE(m_vertexDeclaration);
	ES_SAFE_RELEASE(m_constantBufferToVS);
	ES_SAFE_RELEASE(m_constantBufferToPS);

	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader* Shader::Create(RendererImplemented* renderer,
					   Effekseer::Backend::ShaderRef shader,
					   const char* name,
					   const D3D11_INPUT_ELEMENT_DESC decl[],
					   int32_t layoutCount,
					   bool hasRefCount)
{
	assert(renderer != nullptr);
	assert(renderer->GetDevice() != nullptr);

	HRESULT hr;

	ID3D11InputLayout* vertexDeclaration = nullptr;

	auto shaderdx11 = shader.DownCast<Backend::Shader>();

	hr = renderer->GetDevice()->CreateInputLayout(decl, layoutCount, shaderdx11->GetVertexShaderData().data(), shaderdx11->GetVertexShaderData().size(), &vertexDeclaration);

	if (FAILED(hr))
	{
		printf("* %s Layout Error\n", name);
		goto EXIT;
	}

	return new Shader(renderer, shaderdx11, vertexDeclaration, hasRefCount);

EXIT:;
	ES_SAFE_RELEASE(vertexDeclaration);
	return nullptr;
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
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];

	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	GetRenderer()->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &m_constantBufferToVS);

	vertexConstantBufferSize_ = size;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];

	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	GetRenderer()->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &m_constantBufferToPS);

	pixelConstantBufferSize_ = size;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	if (m_vertexConstantBuffer != nullptr)
	{
		GetRenderer()->GetContext()->UpdateSubresource(m_constantBufferToVS, 0, nullptr, m_vertexConstantBuffer, 0, 0);
		GetRenderer()->GetContext()->VSSetConstantBuffers(0, 1, &m_constantBufferToVS);
	}

	if (m_pixelConstantBuffer != nullptr)
	{
		GetRenderer()->GetContext()->UpdateSubresource(m_constantBufferToPS, 0, nullptr, m_pixelConstantBuffer, 0, 0);
		GetRenderer()->GetContext()->PSSetConstantBuffers(0, 1, &m_constantBufferToPS);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11