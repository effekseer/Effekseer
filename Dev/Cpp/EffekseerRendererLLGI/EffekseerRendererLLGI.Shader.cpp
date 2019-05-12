
#include "EffekseerRendererLLGI.Shader.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

Shader::Shader(RendererImplemented* renderer,
			   LLGI::Shader* vertexShader,
			   LLGI::Shader* pixelShader,
			   const std::vector<LLGI::VertexLayoutFormat>& layoutFormats)
	: DeviceObject(renderer)
	, vertexShader_(vertexShader)
	, pixelShader_(pixelShader)
	, layoutFormats(layoutFormats)
	, m_vertexConstantBuffer(NULL)
	, m_pixelConstantBuffer(NULL)
	, m_vertexRegisterCount(0)
	, m_pixelRegisterCount(0)
{
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(vertexShader_);
	ES_SAFE_RELEASE(pixelShader_);
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(RendererImplemented* renderer,
					   LLGI::DataStructure* vertexData,
					   int32_t vertexDataCount,
					   LLGI::DataStructure* pixelData,
					   int32_t pixelDataCount,
					   const char* name,
					   const std::vector<LLGI::VertexLayoutFormat>& layoutFormats)
{
	assert(renderer != NULL);
	assert(renderer->GetGraphics() != NULL);

	auto vertexShader = renderer->GetGraphics()->CreateShader(vertexData, vertexDataCount);
	auto pixelShader = renderer->GetGraphics()->CreateShader(pixelData, pixelDataCount);

	return new Shader(renderer, vertexShader, pixelShader, layoutFormats);
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
	if (m_vertexRegisterCount > 0)
	{
	}

	if (m_pixelRegisterCount > 0)
	{
	}
}

} // namespace EffekseerRendererLLGI