#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

namespace EffekseerRendererDX11
{

Shader::Shader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
			   Backend::ShaderRef shader,
			   Backend::D3D11InputLayoutPtr vertexDeclaration)
	: graphics_device_(graphicsDevice)
	, shader_(shader)
	, vertex_declaration_(std::move(vertexDeclaration))
	, constant_buffer_to_vs_(nullptr)
	, constant_buffer_to_ps_(nullptr)
	, vertex_constant_buffer_(nullptr)
	, pixel_constant_buffer_(nullptr)
{
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(constant_buffer_to_vs_);
	ES_SAFE_RELEASE(constant_buffer_to_ps_);

	ES_SAFE_DELETE_ARRAY(vertex_constant_buffer_);
	ES_SAFE_DELETE_ARRAY(pixel_constant_buffer_);
}

Shader* Shader::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
					   Effekseer::Backend::ShaderRef shader,
					   Effekseer::Backend::VertexLayoutRef layout,
					   const char* name)
{
	auto shaderdx11 = shader.DownCast<Backend::Shader>();
	auto gd = graphicsDevice.DownCast<Backend::GraphicsDevice>();
	auto inputLayout = Backend::CreateInputLayout(*gd.Get(), layout.DownCast<Backend::VertexLayout>(), shaderdx11->GetVertexShaderData().data(), shaderdx11->GetVertexShaderData().size());

	if (inputLayout == nullptr)
	{
		printf("* %s Layout Error\n", name);
		return nullptr;
	}

	return new Shader(graphicsDevice, shaderdx11, std::move(inputLayout));
}

void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(vertex_constant_buffer_);
	vertex_constant_buffer_ = new uint8_t[size];

	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	auto gd = graphics_device_.DownCast<Backend::GraphicsDevice>();
	gd->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &constant_buffer_to_vs_);

	vertex_constant_buffer_size_ = size;
}

void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(pixel_constant_buffer_);
	pixel_constant_buffer_ = new uint8_t[size];

	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	auto gd = graphics_device_.DownCast<Backend::GraphicsDevice>();
	gd->GetDevice()->CreateBuffer(&hBufferDesc, nullptr, &constant_buffer_to_ps_);

	pixel_constant_buffer_size_ = size;
}

void Shader::SetConstantBuffer()
{
	auto gd = graphics_device_.DownCast<Backend::GraphicsDevice>();

	if (vertex_constant_buffer_ != nullptr)
	{
		gd->GetContext()->UpdateSubresource(constant_buffer_to_vs_, 0, nullptr, vertex_constant_buffer_, 0, 0);
		gd->GetContext()->VSSetConstantBuffers(0, 1, &constant_buffer_to_vs_);
	}

	if (pixel_constant_buffer_ != nullptr)
	{
		gd->GetContext()->UpdateSubresource(constant_buffer_to_ps_, 0, nullptr, pixel_constant_buffer_, 0, 0);
		gd->GetContext()->PSSetConstantBuffers(1, 1, &constant_buffer_to_ps_);
	}
}

} // namespace EffekseerRendererDX11