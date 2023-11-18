
#ifndef __EFFEKSEERRENDERER_DX11_SHADER_H__
#define __EFFEKSEERRENDERER_DX11_SHADER_H__

#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h>

namespace EffekseerRendererDX11
{

class Shader : public ::EffekseerRenderer::ShaderBase
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::ShaderRef shaderOverride_;

	Backend::D3D11InputLayoutPtr vertexDeclaration_;
	ID3D11Buffer* m_constantBufferToVS;
	ID3D11Buffer* m_constantBufferToPS;

	void* m_vertexConstantBuffer = nullptr;
	void* m_pixelConstantBuffer = nullptr;
	int32_t vertexConstantBufferSize_ = 0;
	int32_t pixelConstantBufferSize_ = 0;

	Shader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
		   Backend::ShaderRef shader,
		   Backend::D3D11InputLayoutPtr vertexDeclaration);

public:
	virtual ~Shader();

	static Shader* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						  Effekseer::Backend::ShaderRef shader,
						  Effekseer::Backend::VertexLayoutRef layout,
						  const char* name);

public:
	void OverrideShader(::Effekseer::Backend::ShaderRef shader) override
	{
		shaderOverride_ = shader.DownCast<Backend::Shader>();
	}

public:
	ID3D11VertexShader* GetVertexShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			return shaderOverride_->GetVertexShader();
		}

		return shader_->GetVertexShader();
	}
	ID3D11PixelShader* GetPixelShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			return shaderOverride_->GetPixelShader();
		}

		return shader_->GetPixelShader();
	}
	ID3D11InputLayout* GetLayoutInterface() const
	{
		return vertexDeclaration_.get();
	}

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	int32_t GetVertexConstantBufferSize() const
	{
		return vertexConstantBufferSize_;
	}
	int32_t GetPixelConstantBufferSize() const
	{
		return pixelConstantBufferSize_;
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

} // namespace EffekseerRendererDX11

#endif // __EFFEKSEERRENDERER_DX11_SHADER_H__