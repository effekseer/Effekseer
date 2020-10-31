#include "GraphicsDevice.h"

namespace EffekseerRendererDX11
{
namespace Backend
{

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

bool Texture::Init(const Effekseer::Backend::TextureParameter& param)
{
	if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM ||
		param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
	{
		// not supported
		return false;
	}

	auto device = graphicsDevice_->GetDevice();
	assert(device != nullptr);

	auto context = graphicsDevice_->GetContext();
	assert(context != nullptr);

	auto isCompressed = param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB;

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	int32_t sizePerWidth = 0;
	int32_t height = 0;

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t {
		return ((size + alignement - 1) / alignement) * alignement;
	};

	if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
	{
		sizePerWidth = 4 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		sizePerWidth = 4 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		sizePerWidth = 1 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R8_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		sizePerWidth = 8 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R16G16_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		sizePerWidth = 16 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		sizePerWidth = 32 * param.Size[0];
		height = param.Size[1];
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1)
	{
		sizePerWidth = 8 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC1_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC2_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC3_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		sizePerWidth = 8 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC1_UNORM_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC2_UNORM_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = DXGI_FORMAT_BC3_UNORM_SRGB;
	}

	UINT bindFlag = D3D11_BIND_SHADER_RESOURCE;

	if (param.GenerateMipmap)
	{
		bindFlag = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	}

	D3D11_TEXTURE2D_DESC TexDesc{};
	TexDesc.Width = param.Size[0];
	TexDesc.Height = param.Size[1];
	TexDesc.MipLevels = param.GenerateMipmap ? 0 : 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = format;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = bindFlag;
	TexDesc.CPUAccessFlags = 0;

	if (param.GenerateMipmap)
	{
		TexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else
	{
		TexDesc.MiscFlags = 0;
	}

	ID3D11Texture2D* texture = nullptr;
	D3D11_SUBRESOURCE_DATA data = {};

	bool hasInitiData = param.InitialData.size() > 0;

	if (hasInitiData)
	{
		data.pSysMem = param.InitialData.data();
		data.SysMemPitch = sizePerWidth;
		data.SysMemSlicePitch = sizePerWidth * height;
	}

	HRESULT hr = device->CreateTexture2D(&TexDesc, hasInitiData && !param.GenerateMipmap ? &data : nullptr, &texture);

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
	if (param.GenerateMipmap)
	{
		if (hasInitiData)
		{
			context->UpdateSubresource(texture, 0, 0, data.pSysMem, data.SysMemPitch, data.SysMemSlicePitch);
		}
		context->GenerateMips(srv);
	}

	texture_ = Effekseer::CreateUniqueReference(texture);
	srv_ = Effekseer::CreateUniqueReference(srv);

	return true;
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
	return true;
}

GraphicsDevice::GraphicsDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	device_ = Effekseer::CreateUniqueReference(device, true);
	context_ = Effekseer::CreateUniqueReference(context, true);
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

VertexBuffer* GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = new VertexBuffer(this);

	if (!ret->Init(initialData, size, isDynamic))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

IndexBuffer* GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = new IndexBuffer(this);

	if (!ret->Init(initialData, elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

Texture* GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = new Texture(this);

	if (!ret->Init(param))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererDX11