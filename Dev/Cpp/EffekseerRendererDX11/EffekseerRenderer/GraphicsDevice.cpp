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

	ID3D11Buffer* vb = NULL;
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

	ID3D11Buffer* ib = NULL;
	if (FAILED(graphicsDevice_->GetDevice()->CreateBuffer(&hBufferDesc, src != nullptr ? &hSubResourceData : nullptr, &ib)))
	{
		return false;
	}

	buffer_ = Effekseer::CreateUniqueReference(ib);

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

} // namespace Backend
} // namespace EffekseerRendererDX11