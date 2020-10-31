
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

Shader::Shader(Backend::GraphicsDevice* graphicsDevice,
			   LLGI::Shader* vertexShader,
			   LLGI::Shader* pixelShader,
			   const std::vector<VertexLayout>& layouts,
			   bool hasRefCount)
	: DeviceObject(graphicsDevice, hasRefCount)
	, vertexShader_(vertexShader)
	, pixelShader_(pixelShader)
	, layouts_(layouts)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(vertexShader_);
	ES_SAFE_RELEASE(pixelShader_);
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(Backend::GraphicsDevice* graphicsDevice,
					   LLGI::DataStructure* vertexData,
					   int32_t vertexDataCount,
					   LLGI::DataStructure* pixelData,
					   int32_t pixelDataCount,
					   const char* name,
					   const std::vector<VertexLayout>& layouts,
					   bool hasRefCount)
{
	assert(graphicsDevice != nullptr);
	assert(graphicsDevice->GetGraphics() != nullptr);

	auto vertexShader = graphicsDevice->GetGraphics()->CreateShader(vertexData, vertexDataCount);
	auto pixelShader = graphicsDevice->GetGraphics()->CreateShader(pixelData, pixelDataCount);

	return new Shader(graphicsDevice, vertexShader, pixelShader, layouts, hasRefCount);
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