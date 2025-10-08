
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
	Effekseer::Backend::GraphicsDeviceRef graphics_device_;
	Backend::ShaderRef shader_;
	Backend::ShaderRef shader_override_;

	Backend::D3D11InputLayoutPtr vertex_declaration_;
	ID3D11Buffer* constant_buffer_to_vs_ = nullptr;
	ID3D11Buffer* constant_buffer_to_ps_ = nullptr;

	void* vertex_constant_buffer_ = nullptr;
	void* pixel_constant_buffer_ = nullptr;
	int32_t vertex_constant_buffer_size_ = 0;
	int32_t pixel_constant_buffer_size_ = 0;

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
		shader_override_ = shader.DownCast<Backend::Shader>();
	}

public:
	ID3D11VertexShader* GetVertexShader() const
	{
		if (shader_override_ != nullptr)
		{
			return shader_override_->GetVertexShader();
		}

		return shader_->GetVertexShader();
	}
	ID3D11PixelShader* GetPixelShader() const
	{
		if (shader_override_ != nullptr)
		{
			return shader_override_->GetPixelShader();
		}

		return shader_->GetPixelShader();
	}
	ID3D11InputLayout* GetLayoutInterface() const
	{
		return vertex_declaration_.get();
	}

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	int32_t GetVertexConstantBufferSize() const
	{
		return vertex_constant_buffer_size_;
	}
	int32_t GetPixelConstantBufferSize() const
	{
		return pixel_constant_buffer_size_;
	}

	void* GetVertexConstantBuffer()
	{
		return vertex_constant_buffer_;
	}
	void* GetPixelConstantBuffer()
	{
		return pixel_constant_buffer_;
	}

	void SetConstantBuffer();
};

} // namespace EffekseerRendererDX11

#endif // __EFFEKSEERRENDERER_DX11_SHADER_H__