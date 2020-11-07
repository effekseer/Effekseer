#include "GraphicsDevice.h"

namespace EffekseerRendererDX9
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

bool VertexBuffer::Allocate(int32_t size, bool isDynamic)
{
	D3DPOOL pool = D3DPOOL_MANAGED;
	int usage = D3DUSAGE_WRITEONLY;

	if (isDynamic)
	{
		pool = D3DPOOL_DEFAULT;
		usage |= D3DUSAGE_DYNAMIC;
	}

	IDirect3DVertexBuffer9* vb = nullptr;
	HRESULT hr = graphicsDevice_->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, nullptr);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		pool = D3DPOOL_DEFAULT;
		hr = graphicsDevice_->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, nullptr);
	}

	buffer_ = Effekseer::CreateUniqueReference(vb);

	return SUCCEEDED(hr);
}

void VertexBuffer::Deallocate()
{
	buffer_.reset();
}

void VertexBuffer::OnLostDevice()
{
	if (isDynamic_)
	{
		Deallocate();
	}
}

void VertexBuffer::OnResetDevice()
{
	if (isDynamic_)
	{
		Allocate(size_, isDynamic_);
	}
}

bool VertexBuffer::Init(int32_t size, bool isDynamic)
{
	size_ = size;
	isDynamic_ = isDynamic;

	return Allocate(size_, isDynamic_);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	auto dirtied = blocks_.Allocate(size, offset);

	assert(buffer_ != nullptr);
	void* dst = nullptr;

	if (isDynamic_)
	{
		buffer_->Lock(offset, size, &dst, dirtied ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE);
	}
	else
	{
		buffer_->Lock(offset, size, &dst, 0);
	}

	memcpy(dst, src, size);
	buffer_->Unlock();
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

bool IndexBuffer::Allocate(int32_t elementCount, int32_t stride)
{

	HRESULT hr;

	IDirect3DIndexBuffer9* ib = nullptr;
	hr = graphicsDevice_->GetDevice()->CreateIndexBuffer(
		elementCount * stride,
		D3DUSAGE_WRITEONLY,
		stride == 4 ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ib,
		nullptr);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		hr = graphicsDevice_->GetDevice()->CreateIndexBuffer(elementCount * stride,
															 D3DUSAGE_WRITEONLY,
															 stride == 4 ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
															 D3DPOOL_DEFAULT,
															 &ib,
															 nullptr);
	}

	buffer_ = Effekseer::CreateUniqueReference(ib);

	elementCount_ = elementCount;
	strideType_ = stride == 4 ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;

	return SUCCEEDED(hr);
}

void IndexBuffer::Deallocate()
{
	buffer_.reset();
}

void IndexBuffer::OnLostDevice()
{
	// Index buffer is not losted
	// Deallocate();
}

void IndexBuffer::OnResetDevice()
{
	// Index buffer is not losted
	// Allocate(elementCount_, stride_);
}

bool IndexBuffer::Init(int32_t elementCount, int32_t stride)
{
	elementCount_ = elementCount;
	stride_ = stride;
	return Allocate(elementCount_, stride_);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	auto dirtied = blocks_.Allocate(size, offset);

	assert(buffer_ != nullptr);
	void* dst = nullptr;

	buffer_->Lock(offset, size, &dst, 0);

	memcpy(dst, src, size);
	buffer_->Unlock();
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

	auto isCompressed = param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB;

	_D3DFORMAT format;
	bool isSwapRequired = false;
	int32_t sizePerWidth = 0;
	int32_t height = 0;

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t {
		return ((size + alignement - 1) / alignement) * alignement;
	};

	if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM || param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		sizePerWidth = 4 * param.Size[0];
		height = param.Size[1];
		format = D3DFMT_A8R8G8B8;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		sizePerWidth = 1 * param.Size[0];
		height = param.Size[1];
		format = D3DFMT_L8;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		sizePerWidth = 8 * param.Size[0];
		height = param.Size[1];
		format = D3DFMT_G16R16F;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		sizePerWidth = 16 * param.Size[0];
		height = param.Size[1];
		format = D3DFMT_A16B16G16R16F;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		sizePerWidth = 32 * param.Size[0];
		height = param.Size[1];
		format = D3DFMT_A32B32G32R32F;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		sizePerWidth = 8 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = D3DFMT_DXT1;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = D3DFMT_DXT3;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		sizePerWidth = 16 * aligned(param.Size[0], blockSize) / blockSize;
		height = aligned(param.Size[1], blockSize) / blockSize;
		format = D3DFMT_DXT5;
	}

	HRESULT hr;
	LPDIRECT3DTEXTURE9 texture = nullptr;
	hr =
		device->CreateTexture(param.Size[0], param.Size[1], 1, param.GenerateMipmap ? D3DUSAGE_AUTOGENMIPMAP : 0, format, D3DPOOL_DEFAULT, &texture, nullptr);

	if (FAILED(hr))
	{
		return false;
	}

	LPDIRECT3DTEXTURE9 tempTexture = nullptr;
	hr = device->CreateTexture(param.Size[0], param.Size[1], 1, 0, format, D3DPOOL_SYSTEMMEM, &tempTexture, nullptr);

	if (FAILED(hr))
	{
		texture->Release();
		return false;
	}

	if (param.InitialData.size() > 0)
	{
		const uint8_t* srcBits = static_cast<const uint8_t*>(param.InitialData.data());
		D3DLOCKED_RECT locked;
		if (SUCCEEDED(tempTexture->LockRect(0, &locked, nullptr, 0)))
		{
			uint8_t* destBits = (uint8_t*)locked.pBits;

			for (int32_t h = 0; h < height; h++)
			{
				memcpy(destBits, srcBits, sizePerWidth);

				// SwapRGB
				if (isSwapRequired)
				{
					int32_t sizePerPixel = sizePerWidth / param.Size[0];
					for (int32_t w = 0; w < param.Size[0]; w++)
					{
						std::swap(destBits[w * sizePerPixel + 0 * sizePerPixel / 4], destBits[w * sizePerPixel + 2 * sizePerPixel / 4]);
					}
				}

				destBits += locked.Pitch;
				srcBits += sizePerWidth;
			}

			tempTexture->UnlockRect(0);
		}
		hr = device->UpdateTexture(tempTexture, texture);
	}

	ES_SAFE_RELEASE(tempTexture);

	texture_ = Effekseer::CreateUniqueReference(texture);
	return true;
}

bool Texture::Init(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto device = graphicsDevice_->GetDevice();
	assert(device != nullptr);

	_D3DFORMAT format;

	if (param.Format == Effekseer::Backend::TextureFormatType::D24S8)
	{
		format = D3DFMT_D24S8;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::D32)
	{
		format = D3DFMT_D32;
	}
	else
	{
		return false;
	}

	HRESULT hr;
	LPDIRECT3DTEXTURE9 texture = nullptr;
	hr =
		device->CreateTexture(param.Size[0], param.Size[1], 0, D3DUSAGE_DEPTHSTENCIL, format, D3DPOOL_DEFAULT, &texture, nullptr);

	if (FAILED(hr))
	{
		return false;
	}

	IDirect3DSurface9* surface = nullptr;
	hr = texture->GetSurfaceLevel(0, &surface);

	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(texture);
		return false;
	}

	texture_ = Effekseer::CreateUniqueReference(texture);
	surface_ = Effekseer::CreateUniqueReference(surface);
	return true;
}

GraphicsDevice::GraphicsDevice(IDirect3DDevice9* device)
{
	device_ = Effekseer::CreateUniqueReference(device, true);
}

GraphicsDevice::~GraphicsDevice()
{
}

IDirect3DDevice9* GraphicsDevice::GetDevice() const
{
	return device_.get();
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

	if (!ret->Init(size, isDynamic))
	{
		return nullptr;
	}

	ret->UpdateData(initialData, size, 0);

	return ret;
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(this);

	if (!ret->Init(elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		return nullptr;
	}

	ret->UpdateData(initialData, elementCount * (stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2), 0);

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

} // namespace Backend
} // namespace EffekseerRendererDX9