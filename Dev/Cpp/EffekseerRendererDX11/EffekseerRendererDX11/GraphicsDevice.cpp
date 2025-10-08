#define NOMINMAX
#include "GraphicsDevice.h"
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>

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

DXGI_FORMAT GetTextureFormatType(Effekseer::Backend::TextureFormatType format)
{
	if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		return DXGI_FORMAT_R8_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16_FLOAT)
	{
		return DXGI_FORMAT_R16_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R32_FLOAT)
	{
		return DXGI_FORMAT_R32_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		return DXGI_FORMAT_R16G16_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1)
	{
		return DXGI_FORMAT_BC1_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2)
	{
		return DXGI_FORMAT_BC2_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3)
	{
		return DXGI_FORMAT_BC3_UNORM;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	}

	return DXGI_FORMAT_UNKNOWN;
}

D3D11InputLayoutPtr CreateInputLayout(GraphicsDevice& graphicsDevice, VertexLayoutRef vertexLayout, const void* vertexBufferData, int32_t vertexBufferSize)
{
	Effekseer::CustomAlignedVector<D3D11_INPUT_ELEMENT_DESC> elements;

	std::array<DXGI_FORMAT, 6> formats;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32_FLOAT)] = DXGI_FORMAT_R32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT)] = DXGI_FORMAT_R32G32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT)] = DXGI_FORMAT_R32G32B32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT)] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM)] = DXGI_FORMAT_R8G8B8A8_UNORM;
	formats[static_cast<int32_t>(Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT)] = DXGI_FORMAT_R8G8B8A8_UINT;

	int32_t layoutOffset = 0;
	for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
	{
		D3D11_INPUT_ELEMENT_DESC elm = {};
		elm.SemanticName = vertexLayout->GetElements()[i].SemanticName.c_str();
		elm.SemanticIndex = vertexLayout->GetElements()[i].SemanticIndex;
		elm.Format = formats[static_cast<int32_t>(vertexLayout->GetElements()[i].Format)];
		elm.InputSlot = 0;
		elm.AlignedByteOffset = layoutOffset;
		elm.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elm.InstanceDataStepRate = 0;
		layoutOffset += Effekseer::Backend::GetVertexLayoutFormatSize(vertexLayout->GetElements()[i].Format);

		elements.emplace_back(elm);
	}

	ID3D11InputLayout* inputLayout = nullptr;

	if (FAILED(graphicsDevice.GetDevice()->CreateInputLayout(
			elements.data(),
			static_cast<UINT>(elements.size()),
			vertexBufferData,
			vertexBufferSize,
			&inputLayout)))
	{
		return nullptr;
	}

	return Effekseer::CreateUniqueReference(inputLayout);
}

void DeviceObject::OnLostDevice()
{
}

void DeviceObject::OnResetDevice()
{
}

VertexBuffer::VertexBuffer(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
	graphics_device_->Register(this);
}

VertexBuffer::~VertexBuffer()
{
	graphics_device_->Unregister(this);
	ES_SAFE_RELEASE(graphics_device_);
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
	HRESULT hr = graphics_device_->GetDevice()->CreateBuffer(&hBufferDesc, src != nullptr ? &hSubResourceData : 0, &vb);
	if (FAILED(hr))
	{
		return false;
	}

	buffer_ = Effekseer::CreateUniqueReference(vb);

	if (is_dynamic_)
	{
		blocks_.Allocate(size, 0);
	}

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
	Allocate(nullptr, size_, is_dynamic_);
}

bool VertexBuffer::Init(const void* src, int32_t size, bool isDynamic)
{
	size_ = size;
	is_dynamic_ = isDynamic;

	return Allocate(src, size_, is_dynamic_);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	auto dirtied = blocks_.Allocate(size, offset);

	assert(buffer_ != nullptr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (is_dynamic_)
	{
		graphics_device_->GetContext()->Map(
			buffer_.get(), 0, dirtied ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	}
	else
	{
		graphics_device_->GetContext()->Map(
			buffer_.get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);
	}

	auto dst = static_cast<uint8_t*>(mappedResource.pData);

	dst += offset;
	memcpy(dst, src, size);

	graphics_device_->GetContext()->Unmap(buffer_.get(), 0);
}

void VertexBuffer::MakeAllDirtied()
{
	blocks_.Allocate(size_, 0);
}

IndexBuffer::IndexBuffer(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
	graphics_device_->Register(this);
}

IndexBuffer::~IndexBuffer()
{
	graphics_device_->Unregister(this);
	ES_SAFE_RELEASE(graphics_device_);
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
	if (FAILED(graphics_device_->GetDevice()->CreateBuffer(&hBufferDesc, src != nullptr ? &hSubResourceData : nullptr, &ib)))
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

	graphics_device_->GetContext()->Map(
		buffer_.get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);

	auto dst = static_cast<uint8_t*>(mappedResource.pData);

	dst += offset;
	memcpy(dst, src, size);

	graphics_device_->GetContext()->Unmap(buffer_.get(), 0);
}

UniformBuffer::UniformBuffer(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
	graphics_device_->Register(this);
}

UniformBuffer ::~UniformBuffer()
{
	graphics_device_->Unregister(this);
	ES_SAFE_RELEASE(graphics_device_);
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
	if (FAILED(graphics_device_->GetDevice()->CreateBuffer(&hBufferDesc, initialData != nullptr ? &hSubResourceData : nullptr, &buffer)))
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
		is_dirtied_ = true;
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

	is_dirtied_ = true;
}

void UniformBuffer::UpdateDataActually()
{
	if (!is_dirtied_)
		return;
	is_dirtied_ = false;

	graphics_device_->GetContext()->UpdateSubresource(buffer_.get(), 0, nullptr, data_.data(), 0, 0);
}

bool UniformBuffer::GetIsDirtied() const
{
	return is_dirtied_;
}

ComputeBuffer::ComputeBuffer(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
	graphics_device_->Register(this);
}

ComputeBuffer ::~ComputeBuffer()
{
	graphics_device_->Unregister(this);
	ES_SAFE_RELEASE(graphics_device_);
}

bool ComputeBuffer::Init(int32_t elementCount, int32_t elementSize, const void* initialData)
{
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = static_cast<UINT>(elementCount * elementSize);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hBufferDesc.StructureByteStride = static_cast<UINT>(elementSize);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = initialData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer;
	if (FAILED(graphics_device_->GetDevice()->CreateBuffer(&hBufferDesc, initialData != nullptr ? &hSubResourceData : nullptr, &buffer)))
	{
		return false;
	}
	buffer_ = Effekseer::CreateUniqueReference(buffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(elementCount);

	ID3D11ShaderResourceView* srv = nullptr;
	if (FAILED(graphics_device_->GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv)))
	{
		return false;
	}
	srv_ = Effekseer::CreateUniqueReference(srv);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(elementCount);
	uavDesc.Buffer.Flags = 0;

	ID3D11UnorderedAccessView* uav = nullptr;
	if (FAILED(graphics_device_->GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav)))
	{
		return false;
	}
	uav_ = Effekseer::CreateUniqueReference(uav);

	return true;
}

void ComputeBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	D3D11_BOX dstBox{};
	dstBox.left = static_cast<UINT>(offset);
	dstBox.right = static_cast<UINT>(offset + size);
	dstBox.bottom = 1;
	dstBox.back = 1;
	graphics_device_->GetContext()->UpdateSubresource(buffer_.get(), 0, &dstBox, src, 0, 0);
}

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
	graphics_device_->Register(this);
}

Texture::~Texture()
{
	graphics_device_->Unregister(this);
	ES_SAFE_RELEASE(graphics_device_);
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData)
{
	if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM ||
		param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
	{
		// not supported
		return false;
	}

	auto device = graphics_device_->GetDevice();
	assert(device != nullptr);

	auto context = graphics_device_->GetContext();
	assert(context != nullptr);

	auto isCompressed = param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB;

	int32_t sizePerWidth = 0;
	int32_t height = 0;

	EffekseerRenderer::CalculateAlignedTextureInformation(param.Format, {param.Size[0], param.Size[1]}, sizePerWidth, height);

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t
	{
		return ((size + alignement - 1) / alignement) * alignement;
	};

	const DXGI_FORMAT dxgiFormat = GetTextureFormatType(param.Format);

	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		// not supported
		Effekseer::Log(Effekseer::LogType::Error, "The format is not supported.(" + std::to_string(static_cast<int>(param.Format)) + ")");
		return false;
	}

	UINT bindFlag = D3D11_BIND_SHADER_RESOURCE;

	if (param.MipLevelCount < 1 || (param.Usage & Effekseer::Backend::TextureUsageType::RenderTarget) != Effekseer::Backend::TextureUsageType::None)
	{
		bindFlag = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	}

	uint32_t quality = 0;
	device->CheckMultisampleQualityLevels(dxgiFormat, param.SampleCount, &quality);

	if (param.Dimension == 2)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = param.Size[0];
		texDesc.Height = param.Size[1];
		texDesc.MipLevels = param.MipLevelCount;
		texDesc.ArraySize = Effekseer::Max(1, param.Size[2]);
		texDesc.Format = dxgiFormat;
		texDesc.SampleDesc.Count = param.SampleCount;
		texDesc.SampleDesc.Quality = quality - 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = bindFlag;
		texDesc.CPUAccessFlags = 0;

		if (param.MipLevelCount != 1)
		{
			texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		else
		{
			texDesc.MiscFlags = 0;
		}

		ID3D11Texture2D* texture = nullptr;
		std::vector<D3D11_SUBRESOURCE_DATA> data;

		bool hasInitData = initialData.size() > 0;

		if (hasInitData)
		{
			data.resize(texDesc.ArraySize);
			for (int i = 0; i < texDesc.ArraySize; i++)
			{
				data[i].pSysMem = initialData.data() + (sizePerWidth * height * i);
				data[i].SysMemPitch = sizePerWidth;
				data[i].SysMemSlicePitch = sizePerWidth * height;
			}
		}

		HRESULT hr = device->CreateTexture2D(&texDesc, hasInitData && param.MipLevelCount == 1 ? data.data() : nullptr, &texture);

		if (FAILED(hr))
		{
			return false;
		}

		ID3D11ShaderResourceView* srv = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = texDesc.Format;

		if ((param.Usage & Effekseer::Backend::TextureUsageType::Array) != Effekseer::Backend::TextureUsageType::None)
		{
			desc.ViewDimension = (param.SampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = Effekseer::Max(1, param.Size[2]);
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.MostDetailedMip = 0;
			desc.Texture2DArray.MipLevels = -1;
		}
		else
		{
			desc.ViewDimension = (param.SampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = -1;
		}

		hr = device->CreateShaderResourceView(texture, &desc, &srv);
		if (FAILED(hr))
		{
			ES_SAFE_RELEASE(texture);
			return false;
		}

		// Generate mipmap
		if (param.MipLevelCount != 1)
		{
			if (hasInitData)
			{
				for (int i = 0; i < texDesc.ArraySize; i++)
				{
					D3D11_BOX box;
					box.left = 0;
					box.top = 0;
					box.right = param.Size[0];
					box.bottom = param.Size[1];
					box.front = 0;
					box.back = 1;
					context->UpdateSubresource(texture, i, &box, data[i].pSysMem, data[i].SysMemPitch, data[i].SysMemSlicePitch);
				}
			}
			context->GenerateMips(srv);
		}

		texture2d_ = Effekseer::CreateUniqueReference(texture);
		srv_ = Effekseer::CreateUniqueReference(srv);
	}
	else
	{
		D3D11_TEXTURE3D_DESC texDesc{};
		texDesc.Width = param.Size[0];
		texDesc.Height = param.Size[1];
		texDesc.Depth = param.Size[2];
		texDesc.MipLevels = param.MipLevelCount;
		texDesc.Format = dxgiFormat;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = bindFlag;
		texDesc.CPUAccessFlags = 0;

		if (param.MipLevelCount != 1)
		{
			texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		else
		{
			texDesc.MiscFlags = 0;
		}

		ID3D11Texture3D* texture = nullptr;
		D3D11_SUBRESOURCE_DATA data = {};

		bool hasInitiData = initialData.size() > 0;

		if (hasInitiData)
		{
			data.pSysMem = initialData.data();
			data.SysMemPitch = sizePerWidth;
			data.SysMemSlicePitch = sizePerWidth * height;
		}

		HRESULT hr = device->CreateTexture3D(&texDesc, hasInitiData && param.MipLevelCount == 1 ? &data : nullptr, &texture);

		if (FAILED(hr))
		{
			return false;
		}

		ID3D11ShaderResourceView* srv = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = texDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MostDetailedMip = 0;
		desc.Texture3D.MipLevels = -1;

		hr = device->CreateShaderResourceView(texture, &desc, &srv);
		if (FAILED(hr))
		{
			ES_SAFE_RELEASE(texture);
			return false;
		}

		// Generate mipmap
		if (param.MipLevelCount != 1)
		{
			if (hasInitiData)
			{
				context->UpdateSubresource(texture, 0, 0, data.pSysMem, data.SysMemPitch, data.SysMemSlicePitch);
			}
			context->GenerateMips(srv);
		}

		texture3d_ = Effekseer::CreateUniqueReference(texture);
		srv_ = Effekseer::CreateUniqueReference(srv);
	}

	if ((param.Usage & Effekseer::Backend::TextureUsageType::RenderTarget) != Effekseer::Backend::TextureUsageType::None)
	{
		ID3D11RenderTargetView* rtv = nullptr;

		auto hr = device->CreateRenderTargetView(GetResource(), nullptr, &rtv);
		if (FAILED(hr))
		{
			return false;
		}

		rtv_ = Effekseer::CreateUniqueReference(rtv);
	}

	param_ = param;
	return true;
}

bool Texture::Init(const Effekseer::Backend::RenderTextureParameter& param)
{
	Effekseer::Backend::TextureParameter paramInternal;
	paramInternal.Size = {param.Size[0], param.Size[1], 1};
	paramInternal.Format = param.Format;
	paramInternal.MipLevelCount = 1;
	paramInternal.SampleCount = param.SamplingCount;
	paramInternal.Dimension = 2;
	paramInternal.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;
	return Init(paramInternal, {});
}

bool Texture::Init(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto device = graphics_device_->GetDevice();
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

	const int samplingCount = param.SamplingCount;
	uint32_t quality = 0;
	device->CheckMultisampleQualityLevels(format, samplingCount, &quality);

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = param.Size[0];
	desc.Height = param.Size[1];
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = samplingCount;
	desc.SampleDesc.Quality = quality - 1;
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
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; // is it correct?
	viewDesc.Flags = 0;

	if (FAILED(device->CreateDepthStencilView(texture, &viewDesc, &depthStencilView)))
	{
		ES_SAFE_RELEASE(texture);
		return false;
	}

	ID3D11ShaderResourceView* srv = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureFormat;
	srvDesc.ViewDimension = (samplingCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(texture, &srvDesc, &srv)))
	{
		ES_SAFE_RELEASE(texture);
		ES_SAFE_RELEASE(depthStencilView);
		return false;
	}

	texture2d_ = Effekseer::CreateUniqueReference(texture);
	dsv_ = Effekseer::CreateUniqueReference(depthStencilView);
	srv_ = Effekseer::CreateUniqueReference(srv);

	param_.Format = param.Format;
	param_.Dimension = 2;
	param_.Size = {param.Size[0], param.Size[1], 1};
	param_.MipLevelCount = 1;
	param_.SampleCount = param.SamplingCount;
	param_.Usage = Effekseer::Backend::TextureUsageType::None;

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

	ES_SAFE_RELEASE(resource);

	// TODO : make correct
	param_.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	param_.Dimension = 2;
	param_.Size = {static_cast<int32_t>(desc.Width), static_cast<int32_t>(desc.Height), 1};
	param_.MipLevelCount = desc.MipLevels;
	param_.SampleCount = desc.SampleDesc.Count;
	param_.Usage = Effekseer::Backend::TextureUsageType::External;
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
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
}

RenderPass::~RenderPass()
{
	ES_SAFE_RELEASE(graphics_device_);
}

bool RenderPass::Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture)
{
	textures_ = textures;
	depthTexture_ = depthTexture;

	return true;
}

Shader::Shader(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
}

Shader::~Shader()
{
	ES_SAFE_RELEASE(graphics_device_);
}

bool Shader::Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize)
{
	HRESULT hr;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;

	hr = graphics_device_->GetDevice()->CreateVertexShader(vertexShaderData, vertexShaderDataSize, nullptr, &vs);

	if (FAILED(hr))
	{
		return false;
	}

	hr = graphics_device_->GetDevice()->CreatePixelShader(pixelShaderData, pixelShaderDataSize, nullptr, &ps);

	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		return false;
	}

	vs_ = Effekseer::CreateUniqueReference(vs);
	ps_ = Effekseer::CreateUniqueReference(ps);

	auto start = reinterpret_cast<const uint8_t*>(vertexShaderData);
	vs_data_.assign(start, start + vertexShaderDataSize);
	return true;
}

bool Shader::InitAsCompute(const void* computeShaderData, int32_t computeShaderDataSize)
{
	ID3D11ComputeShader* cs = nullptr;
	if (FAILED(graphics_device_->GetDevice()->CreateComputeShader(computeShaderData, computeShaderDataSize, nullptr, &cs)))
	{
		return false;
	}
	cs_ = Effekseer::CreateUniqueReference(cs);
	return true;
}

PipelineState::PipelineState(GraphicsDevice* graphicsDevice)
	: graphics_device_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphics_device_);
}

PipelineState ::~PipelineState()
{
	ES_SAFE_RELEASE(graphics_device_);
}

bool PipelineState::Init(const Effekseer::Backend::PipelineStateParameter& param)
{
	if (param.ShaderPtr == nullptr)
	{
		return false;
	}

	auto shader = static_cast<Shader*>(param.ShaderPtr.Get());
	if (shader->GetComputeShader() != nullptr)
	{
		param_ = param;
		return true;
	}

	auto vertexLayout = param.VertexLayoutPtr.DownCast<VertexLayout>();

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
	rsDesc.MultisampleEnable = param.IsMSAAEnabled ? TRUE : FALSE;
	rsDesc.FrontCounterClockwise = FALSE;
	graphics_device_->GetDevice()->CreateRasterizerState(&rsDesc, &rasterizerState);

	std::array<D3D11_COMPARISON_FUNC, 8> compares;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Never)] = D3D11_COMPARISON_NEVER;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Less)] = D3D11_COMPARISON_LESS;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Equal)] = D3D11_COMPARISON_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::LessEqual)] = D3D11_COMPARISON_LESS_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Greater)] = D3D11_COMPARISON_GREATER;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::NotEqual)] = D3D11_COMPARISON_NOT_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::GreaterEqual)] = D3D11_COMPARISON_GREATER_EQUAL;
	compares[static_cast<int32_t>(Effekseer::Backend::CompareFuncType::Always)] = D3D11_COMPARISON_ALWAYS;

	rasterizer_state_ = Effekseer::CreateUniqueReference(rasterizerState);

	ID3D11DepthStencilState* depthStencilState = nullptr;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = param.IsDepthTestEnabled ? TRUE : FALSE;
	dsDesc.DepthWriteMask = param.IsDepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = compares[static_cast<int32_t>(param.DepthFunc)];
	dsDesc.StencilEnable = FALSE;
	graphics_device_->GetDevice()->CreateDepthStencilState(&dsDesc, &depthStencilState);

	depth_stencil_state_ = Effekseer::CreateUniqueReference(depthStencilState);

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
		blendDesc.RenderTarget[i].DestBlendAlpha = blendFuncs[static_cast<int32_t>(param.BlendDstFuncAlpha)];
		blendDesc.RenderTarget[i].SrcBlendAlpha = blendFuncs[static_cast<int32_t>(param.BlendSrcFuncAlpha)];
		blendDesc.RenderTarget[i].BlendOpAlpha = equations[static_cast<int32_t>(param.BlendEquationAlpha)];
	}

	ID3D11BlendState* blendState = nullptr;

	graphics_device_->GetDevice()->CreateBlendState(&blendDesc, &blendState);

	blend_state_ = Effekseer::CreateUniqueReference(blendState);

	if (vertexLayout->GetElements().size() >= LayoutElementMax)
	{
		return false;
	}

	input_layout_ = CreateInputLayout(*graphics_device_, vertexLayout, shader->GetVertexShaderData().data(), shader->GetVertexShaderData().size());
	if (input_layout_ == nullptr)
	{
		return false;
	}

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

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param, initialData))
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

bool GraphicsDevice::CopyTexture(Effekseer::Backend::TextureRef& dst, Effekseer::Backend::TextureRef& src, const std::array<int, 3>& dstPos, const std::array<int, 3>& srcPos, const std::array<int, 3>& size, int32_t dstLayer, int32_t srcLayer)
{
	// validate
	if ((src->GetParameter().Usage & Effekseer::Backend::TextureUsageType::Array) == Effekseer::Backend::TextureUsageType::None && srcLayer != 0)
	{
		Effekseer::Log(Effekseer::LogType::Error, "srcLayer must be 0 when an usage contains Array");
		return false;
	}

	if ((dst->GetParameter().Usage & Effekseer::Backend::TextureUsageType::Array) == Effekseer::Backend::TextureUsageType::None && dstLayer != 0)
	{
		Effekseer::Log(Effekseer::LogType::Error, "dstLayer must be 0 when an usage contains Array");
		return false;
	}

	if (src->GetParameter().Dimension != 3 && srcPos[2] != 0)
	{
		Effekseer::Log(Effekseer::LogType::Error, "srcPos[2] must be 0 when a dimention is 2");
		return false;
	}

	if (dst->GetParameter().Dimension != 3 && dstPos[2] != 0)
	{
		Effekseer::Log(Effekseer::LogType::Error, "dstPos[2] must be 0 when a dimention is 2");
		return false;
	}

	auto dstdx11 = dst.DownCast<Texture>()->GetResource();
	auto srcdx11 = src.DownCast<Texture>()->GetResource();

	D3D11_BOX box;
	box.left = srcPos[0];
	box.top = srcPos[1];
	box.front = srcPos[2];

	box.right = srcPos[0] + size[0];
	box.bottom = srcPos[1] + size[1];
	box.back = srcPos[2] + size[2];

	context_->CopySubresourceRegion(dstdx11, dstLayer, dstPos[0], dstPos[1], dstPos[2], srcdx11, srcLayer, &box);

	return true;
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

Effekseer::Backend::ComputeBufferRef GraphicsDevice::CreateComputeBuffer(int32_t elementCount, int32_t elementSize, const void* initialData, bool readOnly)
{
	auto ret = Effekseer::MakeRefPtr<ComputeBuffer>(this);

	if (!ret->Init(elementCount, elementSize, initialData))
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

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromCodes(const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout)
{
	if (vsCodes.size() != 1 || psCodes.size() != 1)
	{
		return nullptr;
	}

	std::string log;
	auto vsb = Effekseer::CreateUniqueReference(CompileVertexShader(vsCodes[0].data(), "", {}, log));
	auto psb = Effekseer::CreateUniqueReference(CompilePixelShader(psCodes[0].data(), "", {}, log));

	if (vsb != nullptr && psb != nullptr)
	{
		if (log.size() > 0)
		{
			Effekseer::Log(Effekseer::LogType::Warning, log);
		}

		return CreateShaderFromBinary(
			vsb->GetBufferPointer(),
			static_cast<int32_t>(vsb->GetBufferSize()),
			psb->GetBufferPointer(),
			static_cast<int32_t>(psb->GetBufferSize()));
	}
	else
	{
		Effekseer::Log(Effekseer::LogType::Error, log);
	}

	return nullptr;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateComputeShader(const void* csData, int32_t csDataSize)
{
	auto ret = Effekseer::MakeRefPtr<Shader>(this);

	if (!ret->InitAsCompute(csData, csDataSize))
	{
		return nullptr;
	}

	return ret;
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

void GraphicsDevice::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
{
	// D3D11_VIEWPORT vp;
	// vp.TopLeftX = x;
	// vp.TopLeftY = y;
	// vp.Width = (float)width;
	// vp.Height = (float)height;
	// vp.MinDepth = 0.0f;
	// vp.MaxDepth = 1.0f;
	// context->RSSetViewports(1, &vp);
}

void GraphicsDevice::Draw(const Effekseer::Backend::DrawParameter& drawParam)
{
	constexpr int32_t BufferSlotCount = Effekseer::Backend::DrawParameter::BufferSlotCount;
	constexpr int32_t ResourceSlotCount = Effekseer::Backend::DrawParameter::ResourceSlotCount;

	if (drawParam.VertexBufferPtr == nullptr ||
		drawParam.IndexBufferPtr == nullptr ||
		drawParam.PipelineStatePtr == nullptr)
	{
		return;
	}

	auto vb = static_cast<VertexBuffer*>(drawParam.VertexBufferPtr.Get());
	auto ib = static_cast<IndexBuffer*>(drawParam.IndexBufferPtr.Get());
	auto pip = static_cast<PipelineState*>(drawParam.PipelineStatePtr.Get());
	auto shader = static_cast<Shader*>(pip->GetParam().ShaderPtr.Get());
	auto vertexLayout = static_cast<VertexLayout*>(pip->GetParam().VertexLayoutPtr.Get());

	{
		uint32_t vertexSize = 0;
		for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
		{
			const auto& element = vertexLayout->GetElements()[i];
			vertexSize += Effekseer::Backend::GetVertexLayoutFormatSize(element.Format);
		}

		std::array<ID3D11Buffer*, 1> vbs{};
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

	// Constant buffers (VS)
	std::array<ID3D11Buffer*, BufferSlotCount> vubs{};
	for (int32_t i = 0; i < BufferSlotCount; i++)
	{
		if (auto vub = static_cast<UniformBuffer*>(drawParam.VertexUniformBufferPtrs[i].Get()))
		{
			vub->UpdateDataActually();
			vubs[i] = vub->GetBuffer();
		}
	}
	context_->VSSetConstantBuffers(0, BufferSlotCount, vubs.data());

	// Constant buffers (PS)
	std::array<ID3D11Buffer*, BufferSlotCount> pubs{};
	for (int32_t i = 0; i < BufferSlotCount; i++)
	{
		if (auto pub = static_cast<UniformBuffer*>(drawParam.PixelUniformBufferPtrs[i].Get()))
		{
			pub->UpdateDataActually();
			pubs[i] = pub->GetBuffer();
		}
	}
	context_->PSSetConstantBuffers(0, BufferSlotCount, pubs.data());

	// Vertex/Pixel shader
	{
		context_->VSSetShader(shader->GetVertexShader(), nullptr, 0);
		context_->PSSetShader(shader->GetPixelShader(), nullptr, 0);
	}

	// Render State
	{
		context_->OMSetDepthStencilState(pip->GetDepthStencilState(), 0);
		context_->RSSetState(pip->GetRasterizerState());
		float blendFactor[] = {0, 0, 0, 0};
		context_->OMSetBlendState(pip->GetBlendState(), blendFactor, 0xFFFFFFFF);
	}

	// Shader Resources
	std::array<ID3D11SamplerState*, ResourceSlotCount> samplers{};
	std::array<ID3D11ShaderResourceView*, ResourceSlotCount> srvs{};
	for (int32_t slot = 0; slot < (int32_t)ResourceSlotCount; slot++)
	{
		auto& binder = drawParam.ResourceBinders[slot];
		if (auto textureBinder = std::get_if<Effekseer::Backend::TextureBinder>(&binder))
		{
			auto texture = textureBinder->Texture.DownCast<Backend::Texture>();
			srvs[slot] = texture->GetSRV();
			samplers[slot] = samplers_[static_cast<int>(textureBinder->SamplingType)][static_cast<int>(textureBinder->WrapType)].get();
		}
		else if (auto computeBufferBinder = std::get_if<Effekseer::Backend::ComputeBufferBinder>(&binder))
		{
			auto computeBuffer = computeBufferBinder->ComputeBuffer.DownCast<Backend::ComputeBuffer>();
			if (computeBufferBinder->ReadOnly)
			{
				srvs[slot] = computeBuffer->GetSRV();
			}
		}
	}

	GetContext()->VSSetShaderResources(0, ResourceSlotCount, srvs.data());
	GetContext()->PSSetShaderResources(0, ResourceSlotCount, srvs.data());
	GetContext()->VSSetSamplers(0, ResourceSlotCount, samplers.data());
	GetContext()->PSSetSamplers(0, ResourceSlotCount, samplers.data());

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
		context_->DrawIndexed(indexPerPrimitive * drawParam.PrimitiveCount, drawParam.IndexOffset, 0);
	}
	else
	{
		context_->DrawIndexedInstanced(indexPerPrimitive * drawParam.PrimitiveCount, drawParam.InstanceCount, drawParam.IndexOffset, 0, 0);
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

	const auto renderTexture = rp->GetTextures().at(0);
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(renderTexture->GetParameter().Size[0]);
	vp.Height = static_cast<float>(renderTexture->GetParameter().Size[1]);
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

void GraphicsDevice::Dispatch(const Effekseer::Backend::DispatchParameter& dispatchParam)
{
	constexpr int32_t BufferSlotCount = Effekseer::Backend::DispatchParameter::BufferSlotCount;
	constexpr int32_t ResourceSlotCount = Effekseer::Backend::DispatchParameter::ResourceSlotCount;
	constexpr int32_t UAVSlotCount = 8;

	auto pipline = dispatchParam.PipelineStatePtr.DownCast<PipelineState>();
	auto shader = pipline->GetParam().ShaderPtr.DownCast<Shader>();

	std::array<ID3D11Buffer*, BufferSlotCount> csCBufs = {};
	std::array<ID3D11UnorderedAccessView*, UAVSlotCount> csUAVs = {};
	std::array<ID3D11ShaderResourceView*, ResourceSlotCount> csSRVs = {};
	std::array<ID3D11SamplerState*, ResourceSlotCount> csSamplers = {};

	for (int32_t i = 0; i < BufferSlotCount; i++)
	{
		if (auto uniformBuffer = dispatchParam.UniformBufferPtrs[i].DownCast<UniformBuffer>())
		{
			uniformBuffer->UpdateDataActually();
			csCBufs[i] = uniformBuffer->GetBuffer();
		}
	}

	for (int32_t slot = 0; slot < (int32_t)ResourceSlotCount; slot++)
	{
		auto& binder = dispatchParam.ResourceBinders[slot];
		if (auto textureBinder = std::get_if<Effekseer::Backend::TextureBinder>(&binder))
		{
			auto texture = textureBinder->Texture.DownCast<Backend::Texture>();
			csSRVs[slot] = texture->GetSRV();
			csSamplers[slot] = samplers_[static_cast<int>(textureBinder->SamplingType)][static_cast<int>(textureBinder->WrapType)].get();
		}
		else if (auto computeBufferBinder = std::get_if<Effekseer::Backend::ComputeBufferBinder>(&binder))
		{
			auto computeBuffer = computeBufferBinder->ComputeBuffer.DownCast<Backend::ComputeBuffer>();
			if (computeBufferBinder->ReadOnly)
			{
				csSRVs[slot] = computeBuffer->GetSRV();
			}
			else if (slot < (int32_t)csUAVs.size())
			{
				csUAVs[slot] = computeBuffer->GetUAV();
			}
		}
	}

	context_->CSSetShader(shader->GetComputeShader(), nullptr, 0);
	context_->CSSetConstantBuffers(0, (UINT)csCBufs.size(), csCBufs.data());
	context_->CSSetShaderResources(0, (UINT)csSRVs.size(), csSRVs.data());
	context_->CSSetSamplers(0, (UINT)csSamplers.size(), csSamplers.data());
	context_->CSSetUnorderedAccessViews(0, (UINT)csUAVs.size(), csUAVs.data(), nullptr);
	
	auto gc = dispatchParam.GroupCount;
	context_->Dispatch(std::min(gc[0], 65536), std::min(gc[1], 65536), std::min(gc[2], 65536));

	std::array<ID3D11UnorderedAccessView*, UAVSlotCount> csClearUAVs = {};
	context_->CSSetUnorderedAccessViews(0, (UINT)csClearUAVs.size(), csClearUAVs.data(), nullptr);
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

bool GraphicsDevice::UpdateComputeBuffer(Effekseer::Backend::ComputeBufferRef& buffer, int32_t size, int32_t offset, const void* data)
{
	if (buffer == nullptr)
	{
		return false;
	}

	auto b = static_cast<ComputeBuffer*>(buffer.Get());

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