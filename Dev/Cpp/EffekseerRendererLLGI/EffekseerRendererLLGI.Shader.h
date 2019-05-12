
#ifndef __EFFEKSEERRENDERER_LLGI_SHADER_H__
#define __EFFEKSEERRENDERER_LLGI_SHADER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include <LLGI.Graphics.h>
#include <LLGI.Shader.h>

namespace EffekseerRendererLLGI
{

class Shader : public DeviceObject, public ::EffekseerRenderer::ShaderBase
{
private:
	LLGI::Shader* vertexShader_ = nullptr;
	LLGI::Shader* pixelShader_ = nullptr;
	std::vector<LLGI::VertexLayoutFormat> layoutFormats;

	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;
	int32_t vertexConstantBufferSize = 0;
	int32_t pixelConstantBufferSize = 0;

	int32_t m_vertexRegisterCount;
	int32_t m_pixelRegisterCount;

	Shader(RendererImplemented* renderer,
		   LLGI::Shader* vertexShader,
		   LLGI::Shader* pixelShader,
		   const std::vector<LLGI::VertexLayoutFormat>& layoutFormats);

public:
	virtual ~Shader();

	static Shader* Create(RendererImplemented* renderer,
						  LLGI::DataStructure* vertexData,
						  int32_t vertexDataCount,
						  LLGI::DataStructure* pixelData,
						  int32_t pixelDataCount,
						  const char* name,
						  const std::vector<LLGI::VertexLayoutFormat>& layoutFormats);

public:
	LLGI::Shader* GetVertexShader() const { return vertexShader_; }
	LLGI::Shader* GetPixelShader() const { return pixelShader_; }
	std::vector<LLGI::VertexLayoutFormat>& GetVertexLayoutFormat() { return layoutFormats; }
	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);
	int32_t GetVertexConstantBufferSize() { return vertexConstantBufferSize; }
	int32_t GetPixelConstantBufferSize() { return pixelConstantBufferSize; }

	void* GetVertexConstantBuffer() { return m_vertexConstantBuffer; }
	void* GetPixelConstantBuffer() { return m_pixelConstantBuffer; }

	void SetVertexRegisterCount(int32_t count) { m_vertexRegisterCount = count; }
	void SetPixelRegisterCount(int32_t count) { m_pixelRegisterCount = count; }

	void SetConstantBuffer();
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_SHADER_H__
