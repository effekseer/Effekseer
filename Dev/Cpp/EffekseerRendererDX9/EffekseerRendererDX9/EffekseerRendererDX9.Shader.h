
#ifndef __EFFEKSEERRENDERER_DX9_SHADER_H__
#define __EFFEKSEERRENDERER_DX9_SHADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader : public ::EffekseerRenderer::ShaderBase
{
private:
	void* vertexConstantBuffer_;
	void* pixelConstantBuffer_;

	int32_t vertexRegisterCount_;
	int32_t pixelRegisterCount_;

	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::ShaderRef shaderOverride_;

	Backend::VertexLayoutRef vertexLayout_;

	Shader(Backend::GraphicsDeviceRef graphicsDevice,
		   Backend::ShaderRef shader,
		   Backend::VertexLayoutRef vertexLayout);

public:
	~Shader() override;

	static Shader* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						  Effekseer::Backend::ShaderRef shader,
						  Effekseer::Backend::VertexLayoutRef vertexLayout);

public:
	void OverrideShader(::Effekseer::Backend::ShaderRef shader) override
	{
		shaderOverride_ = shader.DownCast<Backend::Shader>();
	}

public:
	IDirect3DVertexShader9* GetVertexShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetVertexShader();
		}
		return shader_->GetVertexShader();
	}
	IDirect3DPixelShader9* GetPixelShader() const
	{
		if (shaderOverride_ != nullptr)
		{
			shaderOverride_->GetPixelShader();
		}
		return shader_->GetPixelShader();
	}
	IDirect3DVertexDeclaration9* GetLayoutInterface() const
	{
		return vertexLayout_->GetVertexDeclaration();
	}

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	void* GetVertexConstantBuffer()
	{
		return vertexConstantBuffer_;
	}
	void* GetPixelConstantBuffer()
	{
		return pixelConstantBuffer_;
	}

	void SetConstantBuffer();
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_SHADER_H__