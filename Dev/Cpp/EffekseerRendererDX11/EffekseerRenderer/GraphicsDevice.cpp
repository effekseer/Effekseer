#include "GraphicsDevice.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"

#include <d3dcompiler.h>

namespace EffekseerRendererDX11
{
namespace Backend
{

static ID3DBlob* CompileVertexShader(const char* vertexShaderText,
									 const char* vertexShaderFileName,
									 const std::vector<D3D_SHADER_MACRO>& macro,
									 std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : NULL,
					NULL,
					"main",
					"vs_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != NULL)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		ES_SAFE_RELEASE(shader);
		return nullptr;
	}

	return shader;
}

static ID3DBlob* CompilePixelShader(const char* vertexShaderText,
									const char* vertexShaderFileName,
									const std::vector<D3D_SHADER_MACRO>& macro,
									std::string& log)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
	flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(vertexShaderText,
					strlen(vertexShaderText),
					vertexShaderFileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : NULL,
					NULL,
					"main",
					"ps_4_0",
					flag,
					0,
					&shader,
					&error);

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			log += "Out of memory\n";
		}
		else
		{
			log += "Unknown error\n";
		}

		if (error != NULL)
		{
			log += (const char*)error->GetBufferPointer();
			error->Release();
		}

		ES_SAFE_RELEASE(shader);
		return nullptr;
	}

	return shader;
}

bool DirtiedBlock::Allocate(int32_t size, int32_t offset)
{
	bool dirtied = false;
	int32_t connected = -1;

	for (size_t i = 0; i < blocks_.size(); i++)
	{
		const auto& block = blocks_[i];
		if ((block.offset <= offset && offset < block.offset + block.size) ||
			(block.offset < offset + size && offset + size <= block.offset + block.size) ||
			(offset <= block.offset && block.offset < offset + size) ||
			(offset < block.offset + block.size && block.offset + block.size <= offset + size))
		{
			dirtied = true;
			break;
		}

		if (offset == block.size + block.offset)
		{
			connected = static_cast<int32_t>(i);
		}
	}

	if (dirtied)
	{
		blocks_.clear();

		Block block;
		block.offset = offset;
		block.size = size;
		blocks_.emplace_back(block);
	}
	else
	{
		if (connected != -1)
		{
			blocks_[connected].size += size;
		}
		else
		{
			Block block;
			block.offset = offset;
			block.size = size;
			blocks_.emplace_back(block);
		}
	}

	return dirtied;
}

void DeviceObject::OnLostDevice()
{
}

void DeviceObject::OnResetDevice()
{
}

VertexBuffer::VertexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

VertexBuffer::~VertexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool VertexBuffer::Allocate(const void* src, int32_t size, bool isDynamic)
{
	D3D11_BUFFER_DESC hBufferDesc;
	ZeroMemory(&hBufferDesc, sizeof(hBufferDesc));

	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hBufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = src;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* vb = nullptr;
	HRESULT hr = graphicsDevice_->GetDevice()->CreateBuffer(&hBufferDesc, src != nullptr ? &hSubResourceData : 0, &vb);
	if (FAILED(hr))
	{
		return false;
	}

	buffer_ = Effekseer::CreateUniqueReference(vb);

	return true;
}

void VertexBuffer::Deallocate()
{
	buffer_.reset();
}

void VertexBuffer::OnLostDevice()
{
	Deallocate();
}

void VertexBuffer::OnResetDevice()
{
	Allocate(nullptr, size_, isDynamic_);
}

bool VertexBuffer::Init(const void* src, int32_t size, bool isDynamic)
{
	size_ = size;
	isDynamic_ = isDynamic;

	return Allocate(src, size_, isDynamic_);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	auto dirtied = blocks_.Allocate(size, offset);

	assert(buffer_ != nullptr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (isDynamic_)
	{
		graphicsDevice_->GetContext()->Map(
			buffer_.get(), 0, dirtied ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	}
	else
	{
		graphicsDevice_->GetContext()->Map(
			buffer_.get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);
	}

	auto dst = static_cast<uint8_t*>(mappedResource.pData);

	dst += offset;
	memcpy(dst, src, size);

	graphicsDevice_->GetContext()->Unmap(buffer_.get(), 0);
}

IndexBuffer::IndexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

IndexBuffer::~IndexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool IndexBuffer::Allocate(const void* src, int32_t elementCount, int32_t stride)
{
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.ByteWidth = elementCount * stride;
	hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = src;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* ib = nullptr;
	if (FAILED(graphicsDevice_->GetDevice()->CreateBuffer(&hBufferDesc, src != nullptr ? &hSubResourceData : nullptr, &ib)))
	{
		return false;
	}

	buffer_ = Effekseer::CreateUniqueReference(ib);

	elementCount_ = elementCount;
	strideType_ = stride == 4 ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;

	return true;
}

void IndexBuffer::Deallocate()
{
	buffer_.reset();
}

void IndexBuffer::OnLostDevice()
{
	Deallocate();
}

void IndexBuffer::OnResetDevice()
{
	Allocate(nullptr, elementCount_, stride_);
}

bool IndexBuffer::Init(const void* src, int32_t elementCount, int32_t stride)
{
	elementCount_ = elementCount;
	stride_ = stride;
	return Allocate(src, elementCount_, stride_);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	auto dirtied = blocks_.Allocate(size, offset);

	assert(buffer_ != nullptr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	graphicsDevice_->GetContext()->Map(
		buffer_.get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);

	auto dst = static_cast<uint8_t*>(mappedResource.pData);

	dst += offset;
	memcpy(dst, src, size);

	graphicsDevice_->GetContext()->Unmap(buffer_.get(), 0);
}

UniformBuffer::UniformBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

UniformBuffer ::~UniformBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool UniformBuffer::Init(int32_t size, const void* initialData)
{
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = initialData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer;
	if (FAILED(graphicsDevice_->GetDevice()->CreateBuffer(&hBufferDesc, initialData != nullptr ? &hSubResourceData : nullptr, &buffer)))
	{
		return false;
	}

	buffer_ = Effekseer::CreateUniqueReference(buffer);

	data_.resize(size);

	if (auto data = reinterpret_cast<const uint8_t*>(initialData))
	{
		data_.assign(data, data + size);
	}
	else
	{
		isDirtied_ = true;
	}

	return true;
}

void UniformBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	assert(data_.size() >= size + offset && offset >= 0);

	if (auto data = static_cast<const uint8_t*>(src))
	{
		memcpy(data_.data() + offset, src, size);
	}

	isDirtied_ = true;
}

void UniformBuffer::UpdateDataActually()
{
	if (!isDirtied_)
		return;
	isDirtied_ = false;

	graphicsDevice_->GetContext()->UpdateSubresource(buffer_.get(), 0, nullptr, data_.data(), 0, 0);
}

bool UniformBuffer::GetIsDirtied() const
{
	return isDirtied_;
}

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

Texture::~Texture()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Texture::Init(
	Effekseer::Backend::TextureFormatType format,
	bool generateMipmap,
	std::array<int32_t, 2> size,
	const Effekseer::CustomVector<uint8_t>& initialData,
	bool isRenderTarget)
{
	assert((!generateMipmap) || (!isRenderTarget && generateMipmap));

	if (format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM ||
		format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
	{
		// not supported
		return false;
	}

	auto device = graphicsDevice_->GetDevice();
	assert(device != nullptr);

	auto context = graphicsDevice_->GetContext();
	assert(context != nullptr);

	auto isCompressed = format == Effekseer::Backend::TextureFormatType::BC1 ||
						format == Effekseer::Backend::TextureFormatType::BC2 ||
						format == Effekseer::Backend::TextureFormatType::BC3 ||
						format == Effekseer::Backend::TextureFormatType::BC1_SRGB ||
						format == Effekseer::Backend::TextureFormatType::BC2_SRGB ||
						format == Effekseer::Backend::TextureFormatType::BC3_SRGB;

	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	int32_t sizePerWidth = 0;
	int32_t height = 0;

	EffekseerRenderer::CalculateAlignedTextureInformation(format, size, sizePerWidth, height);

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t {
		return ((size + alignement - 1) / alignement) * alignement;
	};

	if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
	{
		dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		dxgiFormat = DXGI_FORMAT_R8_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		dxgiFormat = DXGI_FORMAT_R16G16_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		dxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1)
	{
		dxgiFormat = DXGI_FORMAT_BC1_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2)
	{
		dxgiFormat = DXGI_FORMAT_BC2_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3)
	{
		dxgiFormat = DXGI_FORMAT_BC3_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		dxgiFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		dxgiFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		dxgiFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
	}

	UINT bindFlag = D3D11_BIND_SHADER_RESOURCE;

	if (generateMipmap || isRenderTarget)
	{
		bindFlag = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	}

	D3D11_TEXTURE2D_DESC TexDesc{};
	TexDesc.Width = size[0];
	TexDesc.Height = size[1];
	TexDesc.MipLevels = generateMipmap ? 0 : 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = dxgiFormat;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = bindFlag;
	TexDesc.CPUAccessFlags = 0;

	if (generateMipmap)
	{
		TexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else
	{
		TexDesc.MiscFlags = 0;
	}

	ID3D11Texture2D* texture = nullptr;
	D3D11_SUBRESOURCE_DATA data = {};

	bool hasInitiData = initialData.size() > 0;

	if (hasInitiData)
	{
		data.pSysMem = initialData.data();
		data.SysMemPitch = sizePerWidth;
		data.SysMemSlicePitch = sizePerWidth * height;
	}

	HRESULT hr = device->CreateTexture2D(&TexDesc, hasInitiData && !generateMipmap ? &data : nullptr, &texture);

	if (FAILED(hr))
	{
		return false;
	}

	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = TexDesc.Format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = -1;

	hr = device->CreateShaderResourceView(texture, &desc, &srv);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(texture);
		return false;
	}

	// Generate mipmap
	if (generateMipmap)
	{
		if (hasInitiData)
		{
			context->UpdateSubresource(texture, 0, 0, data.pSysMem, data.SysMemPitch, data.SysMemSlicePitch);
		}
		context->GenerateMips(srv);
	}

	texture_ = Effekseer::CreateUniqueReference(texture);
	srv_ = Effekseer::CreateUniqueReference(srv);

	if (isRenderTarget)
	{
		ID3D11RenderTargetView* rtv = nullptr;

		hr = device->CreateRenderTargetView(texture, nullptr, &rtv);
		if (FAILED(hr))
		{
			return false;
		}

		rtv_ = Effekseer::CreateUniqueReference(rtv);
	}

	size_ = size;

	return true;
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = Init(param.Format, param.GenerateMipmap, param.Size, param.InitialData, false);

	type_ = Effekseer::Backend::TextureType::Color2D;

	return ret;
}

bool Texture::Init(const Effekseer::Backend::RenderTextureParameter& param)
{
	auto ret = Init(param.Format, false, param.Size, {}, true);

	type_ = Effekseer::Backend::TextureType::Render;

	return ret;
}

bool Texture::Init(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto device = graphicsDevice_->GetDevice();
	assert(device != nullptr);

	DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS;
	DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT textureFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	if (param.Format == Effekseer::Backend::TextureFormatType::D24S8)
	{
		format = DXGI_FORMAT_R24G8_TYPELESS;
		depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::D32)
	{
		format = DXGI_FORMAT_R32_TYPELESS;
		depthFormat = DXGI_FORMAT_D32_FLOAT;
		textureFormat = DXGI_FORMAT_R32_FLOAT;
	}

	else if (param.Format == Effekseer::Backend::TextureFormatType::D32S8)
	{
		format = DXGI_FORMAT_R32G8X24_TYPELESS;
		depthFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		textureFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	}
	else
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = param.Size[0];
	desc.Height = param.Size[1];
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	if (FAILED(device->CreateTexture2D(&desc, nullptr, &texture)))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	viewDesc.Format = depthFormat;
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	viewDesc.Flags = 0;

	if (FAILED(device->CreateDepthStencilView(texture, &viewDesc, &depthStencilView)))
	{
		ES_SAFE_RELEASE(texture);
		return false;
	}

	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(texture, &srvDesc, &srv)))
	{
		ES_SAFE_RELEASE(texture);
		ES_SAFE_RELEASE(depthStencilView);
		return false;
	}

	texture_ = Effekseer::CreateUniqueReference(texture);
	dsv_ = Effekseer::CreateUniqueReference(depthStencilView);
	srv_ = Effekseer::CreateUniqueReference(srv);

	type_ = Effekseer::Backend::TextureType::Depth;

	return true;
}

bool Texture::Init(ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	srv_ = Effekseer::CreateUniqueReference(srv, true);
	rtv_ = Effekseer::CreateUniqueReference(rtv, true);
	dsv_ = Effekseer::CreateUniqueReference(dsv, true);

	ID3D11Resource* resource = nullptr;

	if (srv_ != nullptr && resource == nullptr)
	{
		srv_->GetResource(&resource);
	}

	if (rtv_ != nullptr && resource == nullptr)
	{
		rtv_->GetResource(&resource);
	}

	if (dsv_ != nullptr && resource == nullptr)
	{
		dsv_->GetResource(&resource);
	}

	if (resource == nullptr)
	{
		return false;
	}

	auto texture = static_cast<ID3D11Texture2D*>(resource);

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	size_[0] = desc.Width;
	size_[1] = desc.Height;

	ES_SAFE_RELEASE(resource);

	type_ = Effekseer::Backend::TextureType::Color2D;

	return true;
}

bool VertexLayout::Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	elements_.resize(elementCount);

	for (int32_t i = 0; i < elementCount; i++)
	{
		elements_[i] = elements[i];
	}

	return true;
}

RenderPass::RenderPass(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

RenderPass::~RenderPass()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool RenderPass::Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture)
{
	textures_ = textures;
	depthTexture_ = depthTexture;

	return true;
}

Shader::Shader(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Shader::Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize)
{
	HRESULT hr;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;

	hr = graphicsDevice_->GetDevice()->CreateVertexShader(vertexShaderData, vertexShaderDataSize, nullptr, &vs);

	if (FAILED(hr))
	{
		return false;
	}

	hr = graphicsDevice_->GetDevice()->CreatePixelShader(pixelShaderData, pixelShaderDataSize, nullptr, &ps);

	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		return false;
	}

	vs_ = Effekseer::CreateUniqueReference(vs);
	ps_ = Effekseer::CreateUniqueReference(ps);

	auto start = reinterpret_cast<const uint8_t*>(vertexShaderData);
	vsData_.assign(start, start + vertexShaderDataSize);
	return true;
}

PipelineState::PipelineState(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

PipelineState ::~PipelineState()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool PipelineState::Init(const Effekseer::Backend::PipelineStateParameter& param)
{
	if (param.ShaderPtr == nullptr)
	{
		return false;
	}

	auto shader = static_cast<Shader*>(param.ShaderPtr.Get());
	auto vertexLayout = static_cast<VertexLayout*>(param.VertexLayoutPtr.Get());

	D3D11_RASTERIZER_DESC rsDesc;

	D3D11_CULL_MODE cullTbl[] = {
		D3D11_CULL_BACK,
		D3D11_CULL_FRONT,
		D3D11_CULL_NONE,
	};

	ID3D11RasterizerState* rasterizerState = nullptr;

	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = cullTbl[static_cast<int32_t>(param.Culling)];
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.FrontCounterClockwise = FALSE;
	graphicsDevice_->GetDevice()->CreateRasterizerState(&rsDesc, &rasterizerState);

	std::array<D3D11_COMPARISON_FUNC, 8> compares;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Never)] = D3D11_COMPARISON_NEVER;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Less)] = D3D11_COMPARISON_LESS;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Equal)] = D3D11_COMPARISON_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::LessEqual)] = D3D11_COMPARISON_LESS_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Greater)] = D3D11_COMPARISON_GREATER;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::NotEqual)] = D3D11_COMPARISON_NOT_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::GreaterEqual)] = D3D11_COMPARISON_GREATER_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Always)] = D3D11_COMPARISON_ALWAYS;

	rasterizerState_ = Effekseer::CreateUniqueReference(rasterizerState);

	ID3D11DepthStencilState* depthStencilState = nullptr;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = param.IsDepthTestEnabled ? TRUE : FALSE;
	dsDesc.DepthWriteMask = param.IsDepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = compares[static_cast<int32_t>(param.DepthFunc)];
	dsDesc.StencilEnable = FALSE;
	graphicsDevice_->GetDevice()->CreateDepthStencilState(&dsDesc, &depthStencilState);

	depthStencilState_ = Effekseer::CreateUniqueReference(depthStencilState);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;

	std::array<D3D11_BLEND_OP, 5> equations;

	equations[static_cast<int32_t>(Effekseer::Backend::BlendEquationType::Add)] = D3D11_BLEND_OP_ADD;
	equations[static_cast<int32_t>(Effekseer::Backend::BlendEquationType::Sub)] = D3D11_BLEND_OP_SUBTRACT;
	equations[static_cast<int32_t>(Effekseer::Backend::BlendEquationType::ReverseSub)] = D3D11_BLEND_OP_REV_SUBTRACT;
	equations[static_cast<int32_t>(Effekseer::Backend::BlendEquationType::Min)] = D3D11_BLEND_OP_MIN;
	equations[static_cast<int32_t>(Effekseer::Backend::BlendEquationType::Max)] = D3D11_BLEND_OP_MAX;

	std::array<D3D11_BLEND, 10> blendFuncs;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::Zero)] = D3D11_BLEND_ZERO;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::One)] = D3D11_BLEND_ONE;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::SrcColor)] = D3D11_BLEND_SRC_COLOR;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::OneMinusSrcColor)] = D3D11_BLEND_INV_SRC_COLOR;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::SrcAlpha)] = D3D11_BLEND_SRC_ALPHA;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::OneMinusSrcAlpha)] = D3D11_BLEND_INV_SRC_ALPHA;

	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::DstColor)] = D3D11_BLEND_DEST_COLOR;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::OneMinusDstColor)] = D3D11_BLEND_INV_DEST_COLOR;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::DstAlpha)] = D3D11_BLEND_DEST_ALPHA;
	blendFuncs[static_cast<int32_t>(Effekseer::Backend::BlendFuncType::OneMinusDstAlpha)] = D3D11_BLEND_INV_DEST_ALPHA;

	for (size_t i = 0; i < 8; i++)
	{
		blendDesc.RenderTarget[i].BlendEnable = param.IsBlendEnabled ? TRUE : FALSE;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[i].DestBlend = blendFuncs[static_cast<int32_t>(param.BlendDstFunc)];
		blendDesc.RenderTarget[i].SrcBlend = blendFuncs[static_cast<int32_t>(param.BlendSrcFunc)];
		blendDesc.RenderTarget[i].BlendOp = equations[static_cast<int32_t>(param.BlendEquationRGB)];
		blendDesc.RenderTarget[i].SrcBlendAlpha = blendFuncs[static_cast<int32_t>(param.BlendDstFuncAlpha)];
		blendDesc.RenderTarget[i].DestBlendAlpha = blendFuncs[static_cast<int32_t>(param.BlendSrcFuncAlpha)];
		blendDesc.RenderTarget[i].BlendOpAlpha = equations[static_cast<int32_t>(param.BlendEquationAlpha)];
	}

	ID3D11BlendState* blendState = nullptr;

	graphicsDevice_->GetDevice()->CreateBlendState(&blendDesc, &blendState);

	blendState_ = Effekseer::CreateUniqueReference(blendState);

	if (vertexLayout->GetElements().size() > LayoutElementMax)
	{
		return false;
	}

	std::array<DXGI_FORMAT, 6> formats;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32_FLOAT)] = DXGI_FORMAT_R32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT)] = DXGI_FORMAT_R32G32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT)] = DXGI_FORMAT_R32G32B32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT)] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM)] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT)] = DXGI_FORMAT_R8G8B8A8_UINT;

	std::array<D3D11_INPUT_ELEMENT_DESC, LayoutElementMax> elements;

	int32_t layoutOffset = 0;
	for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
	{
		elements[i].SemanticName = vertexLayout->GetElements()[i].SemanticName.c_str();
		elements[i].SemanticIndex = vertexLayout->GetElements()[i].SemanticIndex;
		elements[i].Format = formats[static_cast<int32_t>(vertexLayout->GetElements()[i].Format)];
		elements[i].InputSlot = 0;
		elements[i].AlignedByteOffset = layoutOffset;
		elements[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elements[i].InstanceDataStepRate = 0;
		layoutOffset += Effekseer::Backend::GetVertexLayoutFormatSize(vertexLayout->GetElements()[i].Format);
	}

	ID3D11InputLayout* inputLayout = nullptr;

	if (FAILED(graphicsDevice_->GetDevice()->CreateInputLayout(
			elements.data(),
			static_cast<UINT>(vertexLayout->GetElements().size()),
			shader->GetVertexShaderData().data(),
			shader->GetVertexShaderData().size(),
			&inputLayout)))
	{
		return false;
	}

	inputLayout_ = Effekseer::CreateUniqueReference(inputLayout);

	param_ = param;

	return true;
}

GraphicsDevice::GraphicsDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	device_ = Effekseer::CreateUniqueReference(device, true);
	context_ = Effekseer::CreateUniqueReference(context, true);

	const int32_t TextureFilterCount = 2;
	const int32_t TextureWrapCount = 2;

	for (int32_t f = 0; f < TextureFilterCount; f++)
	{
		for (int32_t w = 0; w < TextureWrapCount; w++)
		{
			D3D11_TEXTURE_ADDRESS_MODE Addres[] = {
				D3D11_TEXTURE_ADDRESS_CLAMP,
				D3D11_TEXTURE_ADDRESS_WRAP,
			};

			D3D11_FILTER Filter[] = {
				D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
				D3D11_FILTER_MIN_MAG_MIP_POINT,
			};

			uint32_t Anisotropic[] = {
				0,
				0,
			};

			D3D11_SAMPLER_DESC SamlerDesc = {
				Filter[f],
				Addres[w],
				Addres[w],
				Addres[w],
				0.0f,
				Anisotropic[f],
				D3D11_COMPARISON_ALWAYS,
				{0.0f, 0.0f, 0.0f, 0.0f},
				0.0f,
				D3D11_FLOAT32_MAX,
			};

			ID3D11SamplerState* samplerState = nullptr;

			device->CreateSamplerState(&SamlerDesc, &samplerState);
			samplers_[f][w] = Effekseer::CreateUniqueReference(samplerState);
		}
	}
}

GraphicsDevice::~GraphicsDevice()
{
}

ID3D11Device* GraphicsDevice::GetDevice() const
{
	return device_.get();
}

ID3D11DeviceContext* GraphicsDevice::GetContext() const
{
	return context_.get();
}

void GraphicsDevice::LostDevice()
{
	for (auto& o : objects_)
	{
		o->OnLostDevice();
	}
}

void GraphicsDevice::ResetDevice()
{
	for (auto& o : objects_)
	{
		o->OnResetDevice();
	}
}

void GraphicsDevice::Register(DeviceObject* deviceObject)
{
	objects_.insert(deviceObject);
}

void GraphicsDevice::Unregister(DeviceObject* deviceObject)
{
	objects_.erase(deviceObject);
}

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = Effekseer::MakeRefPtr<VertexBuffer>(this);

	if (!ret->Init(initialData, size, isDynamic))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(this);

	if (!ret->Init(initialData, elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::UniformBufferRef GraphicsDevice::CreateUniformBuffer(int32_t size, const void* initialData)
{
	auto ret = Effekseer::MakeRefPtr<UniformBuffer>(this);

	if (!ret->Init(size, initialData))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::VertexLayoutRef GraphicsDevice::CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	auto ret = Effekseer::MakeRefPtr<VertexLayout>();

	if (!ret->Init(elements, elementCount))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::RenderPassRef GraphicsDevice::CreateRenderPass(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef& depthTexture)
{
	auto ret = Effekseer::MakeRefPtr<RenderPass>(this);

	if (!ret->Init(textures, depthTexture))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize)
{
	auto ret = Effekseer::MakeRefPtr<Shader>(this);

	if (!ret->Init(vsData, vsDataSize, psData, psDataSize))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromCodes(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef layout)
{
	std::string log;
	auto vsb = Effekseer::CreateUniqueReference(CompileVertexShader(vsCode, "", {}, log));
	auto psb = Effekseer::CreateUniqueReference(CompilePixelShader(psCode, "", {}, log));

	if (vsb != nullptr && psb != nullptr)
	{
		return CreateShaderFromBinary(
			vsb->GetBufferPointer(),
			static_cast<int32_t>(vsb->GetBufferSize()),
			psb->GetBufferPointer(),
			static_cast<int32_t>(psb->GetBufferSize()));
	}

	return nullptr;
}

Effekseer::Backend::PipelineStateRef GraphicsDevice::CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<PipelineState>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

void GraphicsDevice::Draw(const Effekseer::Backend::DrawParameter& drawParam)
{
	if (drawParam.VertexBufferPtr == nullptr ||
		drawParam.IndexBufferPtr == nullptr ||
		drawParam.PipelineStatePtr == nullptr)
	{
		return;
	}

	auto vb = static_cast<VertexBuffer*>(drawParam.VertexBufferPtr.Get());
	auto ib = static_cast<IndexBuffer*>(drawParam.IndexBufferPtr.Get());
	auto pip = static_cast<PipelineState*>(drawParam.PipelineStatePtr.Get());
	auto vub = static_cast<UniformBuffer*>(drawParam.VertexUniformBufferPtr.Get());
	auto pub = static_cast<UniformBuffer*>(drawParam.PixelUniformBufferPtr.Get());
	auto shader = static_cast<Shader*>(pip->GetParam().ShaderPtr.Get());
	auto vertexLayout = static_cast<VertexLayout*>(pip->GetParam().VertexLayoutPtr.Get());

	{
		uint32_t vertexSize = 0;
		for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
		{
			const auto& element = vertexLayout->GetElements()[i];
			vertexSize += Effekseer::Backend::GetVertexLayoutFormatSize(element.Format);
		}

		std::array<ID3D11Buffer*, 1> vbs;
		vbs[0] = vb->GetBuffer();
		uint32_t offset = 0;

		context_->IASetVertexBuffers(0, 1, vbs.data(), &vertexSize, &offset);
	}

	{
		std::array<DXGI_FORMAT, 2> formats;
		formats[static_cast<int32_t>(Effekseer::Backend::IndexBufferStrideType::Stride2)] = DXGI_FORMAT_R16_UINT;
		formats[static_cast<int32_t>(Effekseer::Backend::IndexBufferStrideType::Stride4)] = DXGI_FORMAT_R32_UINT;

		context_->IASetIndexBuffer(ib->GetBuffer(), formats[static_cast<int32_t>(ib->GetStrideType())], 0);
	}

	if (vub != nullptr)
	{
		vub->UpdateDataActually();
		auto cb = vub->GetBuffer();
		context_->VSSetConstantBuffers(0, 1, &cb);
	}

	if (pub != nullptr)
	{
		pub->UpdateDataActually();
		auto cb = pub->GetBuffer();
		context_->PSSetConstantBuffers(0, 1, &cb);
	}

	{
		context_->VSSetShader(shader->GetVertexShader(), nullptr, 0);
		context_->PSSetShader(shader->GetPixelShader(), nullptr, 0);
	}

	{
		context_->OMSetDepthStencilState(pip->GetDepthStencilState(), 0);
		context_->RSSetState(pip->GetRasterizerState());
		float blendFactor[] = {0, 0, 0, 0};
		context_->OMSetBlendState(pip->GetBlendState(), blendFactor, 0xFFFFFFFF);
	}

	for (int32_t i = 0; i < drawParam.TextureCount; i++)
	{
		auto texture = static_cast<Texture*>(drawParam.TexturePtrs[i].Get());
		if (texture != nullptr)
		{

			ID3D11SamplerState* samplerTbl[] = {samplers_[static_cast<int>(drawParam.TextureSamplingTypes[i])][static_cast<int>(drawParam.TextureWrapTypes[i])].get()};
			context_->VSSetSamplers(i, 1, samplerTbl);
			context_->PSSetSamplers(i, 1, samplerTbl);

			auto srv = texture->GetSRV();
			GetContext()->VSSetShaderResources(i, 1, &srv);
			GetContext()->PSSetShaderResources(i, 1, &srv);
		}
	}

	// topology

	int32_t indexPerPrimitive{};

	if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Point)
	{
		indexPerPrimitive = 1;
		context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
	else if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Line)
	{
		indexPerPrimitive = 2;
		context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	else if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Triangle)
	{
		indexPerPrimitive = 3;
		context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	context_->IASetInputLayout(pip->GetInputLayout());

	if (drawParam.InstanceCount == 1)
	{
		context_->DrawIndexed(indexPerPrimitive * drawParam.PrimitiveCount, 0, 0);
	}
	else
	{
		context_->DrawIndexedInstanced(indexPerPrimitive * drawParam.PrimitiveCount, drawParam.InstanceCount, 0, 0, 0);
	}
}

void GraphicsDevice::BeginRenderPass(Effekseer::Backend::RenderPassRef& renderPass, bool isColorCleared, bool isDepthCleared, Effekseer::Color clearColor)
{
	auto rp = static_cast<RenderPass*>(renderPass.Get());

	if (rp->GetTextures().size() == 0)
	{
		return;
	}

	std::array<ID3D11RenderTargetView*, 8> rtvs;
	ID3D11DepthStencilView* dsv = nullptr;
	rtvs.fill(nullptr);

	for (size_t i = 0; i < rp->GetTextures().size(); i++)
	{
		rtvs[i] = static_cast<Texture*>(rp->GetTextures().at(i).Get())->GetRTV();
	}

	if (rp->GetDepthTexture() != nullptr)
	{
		dsv = static_cast<Texture*>(rp->GetDepthTexture().Get())->GetDSV();
	}

	context_->OMSetRenderTargets(static_cast<UINT>(rp->GetTextures().size()), rtvs.data(), dsv);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(rp->GetTextures().at(0)->GetSize()[0]);
	vp.Height = static_cast<float>(rp->GetTextures().at(0)->GetSize()[1]);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context_->RSSetViewports(1, &vp);

	if (isColorCleared)
	{
		float color[] = {clearColor.R / 255.0f, clearColor.G / 255.0f, clearColor.B / 255.0f, clearColor.A / 255.0f};

		for (size_t i = 0; i < rp->GetTextures().size(); i++)
		{
			context_->ClearRenderTargetView(rtvs[i], color);
		}
	}

	if (isDepthCleared && dsv)
	{
		context_->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void GraphicsDevice::EndRenderPass()
{
}

bool GraphicsDevice::UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data)
{
	if (buffer == nullptr)
	{
		return false;
	}

	auto b = static_cast<UniformBuffer*>(buffer.Get());

	b->UpdateData(data, size, offset);

	return true;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(srv, rtv, dsv))
	{
		return nullptr;
	}

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererDX11