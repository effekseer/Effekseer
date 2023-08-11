
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
Shader::Shader(Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   Backend::VertexLayoutRef vertexLayout)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
	, vertexLayout_(vertexLayout)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
	, m_vertexRegisterCount(0)
	, m_pixelRegisterCount(0)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader* Shader::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
					   Effekseer::Backend::ShaderRef shader,
					   Effekseer::Backend::VertexLayoutRef vertexLayout)
{
	return new Shader(graphicsDevice.DownCast<Backend::GraphicsDevice>(),
					  shader.DownCast<Backend::Shader>(),
					  vertexLayout.DownCast<Backend::VertexLayout>());
}

void Shader::SetVertexConstantBufferSize(int32_t size)
{
	// TOTO replace align method
	size = ((size + 15) / 16) * 16;

	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];
	m_vertexRegisterCount = size / (sizeof(float) * 4);

	assert(m_vertexRegisterCount <= 256);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	size = ((size + 15) / 16) * 16;

	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];
	m_pixelRegisterCount = size / (sizeof(float) * 4);

	assert(m_pixelRegisterCount <= 256);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	// DirectX9 ignores a slot
	if (m_vertexRegisterCount > 0)
	{
		graphicsDevice_->GetDevice()->SetVertexShaderConstantF(0, (float*)m_vertexConstantBuffer, m_vertexRegisterCount);
	}

	if (m_pixelRegisterCount > 0)
	{
		graphicsDevice_->GetDevice()->SetPixelShaderConstantF(0, (float*)m_pixelConstantBuffer, m_pixelRegisterCount);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9