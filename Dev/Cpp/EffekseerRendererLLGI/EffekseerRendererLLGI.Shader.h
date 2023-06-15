
#ifndef __EFFEKSEERRENDERER_LLGI_SHADER_H__
#define __EFFEKSEERRENDERER_LLGI_SHADER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"
#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{

class Shader : public ::EffekseerRenderer::ShaderBase
{
private:
	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::VertexLayoutRef vertexLayout_;

	void* m_vertexConstantBuffer;
	void* m_pixelConstantBuffer;
	int32_t vertexConstantBufferSize = 0;
	int32_t pixelConstantBufferSize = 0;

	Shader(Backend::GraphicsDeviceRef graphicsDevice,
		   Backend::ShaderRef shader,
		   Backend::VertexLayoutRef vertexLayout);

public:
	~Shader() override;

	static Shader* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						  Effekseer::Backend::ShaderRef shader,
						  Effekseer::Backend::VertexLayoutRef vertexLayout,
						  const char* name);

public:
	LLGI::Shader* GetVertexShader() const
	{
		return shader_->GetVertexShader();
	}
	LLGI::Shader* GetPixelShader() const
	{
		return shader_->GetPixelShader();
	}

	const Backend::VertexLayoutRef& GetVertexLayouts()
	{
		return vertexLayout_;
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
