
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
	, vertexConstantBuffer_(nullptr)
	, pixelConstantBuffer_(nullptr)
	, vertexRegisterCount_(0)
	, pixelRegisterCount_(0)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	ES_SAFE_DELETE_ARRAY(vertexConstantBuffer_);
	ES_SAFE_DELETE_ARRAY(pixelConstantBuffer_);
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

	ES_SAFE_DELETE_ARRAY(vertexConstantBuffer_);
	vertexConstantBuffer_ = new uint8_t[size];
	vertexRegisterCount_ = size / (sizeof(float) * 4);

	assert(vertexRegisterCount_ <= 256);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	size = ((size + 15) / 16) * 16;

	ES_SAFE_DELETE_ARRAY(pixelConstantBuffer_);
	pixelConstantBuffer_ = new uint8_t[size];
	pixelRegisterCount_ = size / (sizeof(float) * 4);

	assert(pixelRegisterCount_ <= 256);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	// DirectX9 ignores a slot
	if (vertexRegisterCount_ > 0)
	{
		graphicsDevice_->GetDevice()->SetVertexShaderConstantF(0, (float*)vertexConstantBuffer_, vertexRegisterCount_);
	}

	if (pixelRegisterCount_ > 0)
	{
		graphicsDevice_->GetDevice()->SetPixelShaderConstantF(0, (float*)pixelConstantBuffer_, pixelRegisterCount_);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9