#include "GraphicsDevice.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"

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

void DeviceObject::OnChangeDevice()
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
	auto device = graphicsDevice_->GetDevice();
	if (device == nullptr)
	{
		return false;
	}

	D3DPOOL pool = D3DPOOL_MANAGED;
	int usage = D3DUSAGE_WRITEONLY;

	if (isDynamic)
	{
		pool = D3DPOOL_DEFAULT;
		usage |= D3DUSAGE_DYNAMIC;
	}

	IDirect3DVertexBuffer9* vb = nullptr;
	HRESULT hr = device->CreateVertexBuffer(size, usage, 0, pool, &vb, nullptr);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		pool = D3DPOOL_DEFAULT;
		hr = device->CreateVertexBuffer(size, usage, 0, pool, &vb, nullptr);
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

void VertexBuffer::OnChangeDevice()
{
	Deallocate();
	Allocate(size_, isDynamic_);
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
	auto device = graphicsDevice_->GetDevice();
	if (device == nullptr)
	{
		return false;
	}

	HRESULT hr;

	IDirect3DIndexBuffer9* ib = nullptr;
	hr = device->CreateIndexBuffer(
		elementCount * stride,
		D3DUSAGE_WRITEONLY,
		stride == 4 ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ib,
		nullptr);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		hr = device->CreateIndexBuffer(elementCount * stride,
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

void IndexBuffer::OnChangeDevice()
{
	Deallocate();
	Allocate(elementCount_, stride_);
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

	EffekseerRenderer::CalculateAlignedTextureInformation(param.Format, param.Size, sizePerWidth, height);

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t {
		return ((size + alignement - 1) / alignement) * alignement;
	};

	if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM || param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		format = D3DFMT_A8R8G8B8;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		format = D3DFMT_L8;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		format = D3DFMT_G16R16F;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		format = D3DFMT_A16B16G16R16F;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		format = D3DFMT_A32B32G32R32F;
		isSwapRequired = true;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		format = D3DFMT_DXT1;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		format = D3DFMT_DXT3;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
			 param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		format = D3DFMT_DXT5;
	}
	else
	{
		// not supported
		Effekseer::Log(Effekseer::LogType::Error, "The format is not supported.(" + std::to_string(static_cast<int>(param.Format)) + ")");
		return false;
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

	size_ = param.Size;
	type_ = Effekseer::Backend::TextureType::Color2D;

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

	size_ = param.Size;
	type_ = Effekseer::Backend::TextureType::Depth;

	return true;
}

bool Texture::Init(IDirect3DTexture9* texture, std::function<void(IDirect3DTexture9*&)> onLostDevice, std::function<void(IDirect3DTexture9*&)> onResetDevice)
{
	onLostDevice_ = onLostDevice;
	onResetDevice_ = onResetDevice;

	texture_ = Effekseer::CreateUniqueReference(texture, true);

	IDirect3DSurface9* surface = nullptr;
	auto hr = texture->GetSurfaceLevel(0, &surface);

	if (FAILED(hr))
	{
		return false;
	}

	surface_ = Effekseer::CreateUniqueReference(surface);

	type_ = Effekseer::Backend::TextureType::Color2D;

	D3DSURFACE_DESC desc;
	texture_->GetLevelDesc(0, &desc);
	size_ = {(int32_t)desc.Width, (int32_t)desc.Height};

	return true;
}

void Texture::OnLostDevice()
{
	if (onLostDevice_)
	{
		auto texture = texture_.get();
		onLostDevice_(texture);
		keyTexture_ = texture;
	}

	texture_.reset();
	surface_.reset();
}

void Texture::OnChangeDevice()
{
	texture_.reset();
	surface_.reset();

	// TODO restore
}

void Texture::OnResetDevice()
{
	if (onResetDevice_)
	{
		auto texture = keyTexture_;
		onResetDevice_(texture);

		texture_ = Effekseer::CreateUniqueReference(texture, true);
		keyTexture_ = nullptr;

		if (texture_ != nullptr)
		{
			D3DSURFACE_DESC desc;
			texture_->GetLevelDesc(0, &desc);
			size_ = {(int32_t)desc.Width, (int32_t)desc.Height};

			IDirect3DSurface9* surface = nullptr;
			auto hr = texture->GetSurfaceLevel(0, &surface);

			if (FAILED(hr))
			{
				return;
			}

			surface_ = Effekseer::CreateUniqueReference(surface);
		}
	}
}

bool Shader::GenerateShaders()
{
	auto device = graphicsDevice_->GetDevice();
	assert(device != nullptr);

	HRESULT hr;

	Effekseer::SafeRelease(vertexShader_);
	Effekseer::SafeRelease(pixelShader_);

	hr = device->CreateVertexShader((const DWORD*)vertexShaderData_.data(), &vertexShader_);

	if (FAILED(hr))
	{
		return nullptr;
	}

	hr = device->CreatePixelShader((const DWORD*)pixelShaderData_.data(), &pixelShader_);

	if (FAILED(hr))
	{
		return nullptr;
	}

	return true;
}

void Shader::ResetShaders()
{
	Effekseer::SafeRelease(vertexShader_);
	Effekseer::SafeRelease(pixelShader_);
}

Shader::Shader(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

Shader ::~Shader()
{
	ResetShaders();
	graphicsDevice_->Unregister(this);
	Effekseer::SafeRelease(graphicsDevice_);
}

bool Shader::Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize)
{
	const auto pv = static_cast<const uint8_t*>(vertexShaderData);
	vertexShaderData_.assign(pv, pv + vertexShaderDataSize);

	const auto pp = static_cast<const uint8_t*>(pixelShaderData);
	pixelShaderData_.assign(pp, pp + pixelShaderDataSize);

	return GenerateShaders();
}

void Shader::OnLostDevice()
{
	ResetShaders();
}

void Shader::OnChangeDevice()
{
	ResetShaders();
}

void Shader::OnResetDevice()
{
	GenerateShaders();
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

void GraphicsDevice::ChangeDevice(IDirect3DDevice9* device)
{
	if (device_.get() == device)
	{
		return;
	}

	device_ = Effekseer::CreateUniqueReference(device, true);
	for (auto& o : objects_)
	{
		o->OnChangeDevice();
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

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(IDirect3DTexture9* texture, std::function<void(IDirect3DTexture9*&)> onLostDevice, std::function<void(IDirect3DTexture9*&)> onResetDevice)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(texture, onLostDevice, onResetDevice))
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


} // namespace Backend
} // namespace EffekseerRendererDX9