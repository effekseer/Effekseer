#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

namespace EffekseerRendererDX11
{

Shader::Shader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   ID3D11InputLayout* vertexDeclaration)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
	, m_vertexDeclaration(vertexDeclaration)
	, m_constantBufferToVS(nullptr)
	, m_constantBufferToPS(nullptr)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(m_vertexDeclaration);
	ES_SAFE_RELEASE(m_constantBufferToVS);
	ES_SAFE_RELEASE(m_constantBufferToPS);

	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
					   Effekseer::Backend::ShaderRef shader,
					   const char* name,
					   const D3D11_INPUT_ELEMENT_DESC decl[],
					   int32_t layoutCount)
{
	HRESULT hr;

	ID3D11InputLayout* vertexDeclaration = nullptr;

	auto shaderdx11 = shader.DownCast<Backend::Shader>();

	auto gd = graphicsDevice.DownCast<Backend::GraphicsDevice>();

	hr = gd->GetDevice()->CreateInputLayout(decl, layoutCount, shaderdx11->GetVertexShaderData().data(), shaderdx11->GetVertexShaderData().size(), &vertexDeclaration);

	if (FAILED(hr))
	{
		printf("* %s Layout Error\n", name);
		goto EXIT;
	}

	return new Shader(graphicsDevice, shaderdx11, vertexDeclaration);

EXIT:;
	ES_SAFE_RELEASE(vertexDeclaration);
	return nullptr;
}

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

	auto gd = graphicsDevice_.DownCast<Backend::GraphicsDevice>();
	gd->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &m_constantBufferToVS);

	vertexConstantBufferSize_ = size;
}

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

	auto gd = graphicsDevice_.DownCast<Backend::GraphicsDevice>();
	gd->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &m_constantBufferToPS);

	pixelConstantBufferSize_ = size;
}

void Shader::SetConstantBuffer()
{
	auto gd = graphicsDevice_.DownCast<Backend::GraphicsDevice>();

	if (m_vertexConstantBuffer != nullptr)
	{
		gd->GetContext()->UpdateSubresource(m_constantBufferToVS, 0, nullptr, m_vertexConstantBuffer, 0, 0);
		gd->GetContext()->VSSetConstantBuffers(0, 1, &m_constantBufferToVS);
	}

	if (m_pixelConstantBuffer != nullptr)
	{
		gd->GetContext()->UpdateSubresource(m_constantBufferToPS, 0, nullptr, m_pixelConstantBuffer, 0, 0);
		gd->GetContext()->PSSetConstantBuffers(0, 1, &m_constantBufferToPS);
	}
}

} // namespace EffekseerRendererDX11