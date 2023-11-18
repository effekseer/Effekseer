
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

Shader::Shader(Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   Backend::VertexLayoutRef vertexLayout)
	: graphicsDevice_(graphicsDevice)
	, shader_(shader)
	, vertexLayout_(vertexLayout)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
}

Shader::~Shader()
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
					   Effekseer::Backend::ShaderRef shader,
					   Effekseer::Backend::VertexLayoutRef vertexLayout,
					   const char* name)
{
	assert(graphicsDevice != nullptr);
	assert(shader != nullptr);

	return new Shader(graphicsDevice.DownCast<Backend::GraphicsDevice>(), shader.DownCast<Backend::Shader>(), vertexLayout.DownCast<Backend::VertexLayout>());
}

void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];
	vertexConstantBufferSize = size;
}

void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];
	pixelConstantBufferSize = size;
}

void Shader::SetConstantBuffer()
{
}

} // namespace EffekseerRendererLLGI