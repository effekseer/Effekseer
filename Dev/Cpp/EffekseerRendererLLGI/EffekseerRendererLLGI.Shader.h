
#ifndef __EFFEKSEERRENDERER_LLGI_SHADER_H__
#define __EFFEKSEERRENDERER_LLGI_SHADER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include <LLGI.Graphics.h>
#include <LLGI.Shader.h>

namespace EffekseerRendererLLGI
{

class VertexLayout
{
public:
	LLGI::VertexLayoutFormat Format;
	std::string Name;
	int32_t Semantic;
};

class Shader : public DeviceObject, public ::EffekseerRenderer::ShaderBase
{
private:
	LLGI::Shader* vertexShader_ = nullptr;
	LLGI::Shader* pixelShader_ = nullptr;
	std::vector<VertexLayout> layouts_;

	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;
	int32_t vertexConstantBufferSize = 0;
	int32_t pixelConstantBufferSize = 0;

	Shader(Backend::GraphicsDevice* graphicsDevice,
		   LLGI::Shader* vertexShader,
		   LLGI::Shader* pixelShader,
		   const std::vector<VertexLayout>& layouts,
		   bool hasRefCount);

public:
	virtual ~Shader();

	static Shader* Create(Backend::GraphicsDevice* graphicsDevice,
						  LLGI::DataStructure* vertexData,
						  int32_t vertexDataCount,
						  LLGI::DataStructure* pixelData,
						  int32_t pixelDataCount,
						  const char* name,
						  const std::vector<VertexLayout>& layoutFormats,
						  bool hasRefCount);

public:
	LLGI::Shader* GetVertexShader() const
	{
		return vertexShader_;
	}
	LLGI::Shader* GetPixelShader() const
	{
		return pixelShader_;
	}
	std::vector<VertexLayout>& GetVertexLayouts()
	{
		return layouts_;
	}
	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);
	int32_t GetVertexConstantBufferSize()
	{
		return vertexConstantBufferSize;
	}
	int32_t GetPixelConstantBufferSize()
	{
		return pixelConstantBufferSize;
	}

	void* GetVertexConstantBuffer()
	{
		return m_vertexConstantBuffer;
	}
	void* GetPixelConstantBuffer()
	{
		return m_pixelConstantBuffer;
	}

	void SetConstantBuffer();
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_SHADER_H__
