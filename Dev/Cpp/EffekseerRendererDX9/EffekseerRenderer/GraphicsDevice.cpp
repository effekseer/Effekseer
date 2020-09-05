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

	IDirect3DVertexBuffer9* vb = NULL;
	HRESULT hr = graphicsDevice_->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, NULL);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		pool = D3DPOOL_DEFAULT;
		hr = graphicsDevice_->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, NULL);
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
	Deallocate();
}

void VertexBuffer::OnResetDevice()
{
	Allocate(size_, isDynamic_);
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
		NULL);

	if (FAILED(hr))
	{
		// DirectX9Ex cannot use D3DPOOL_MANAGED
		hr = graphicsDevice_->GetDevice()->CreateIndexBuffer(elementCount * stride,
															 D3DUSAGE_WRITEONLY,
															 stride == 4 ? D3DFMT_INDEX32 : D3DFMT_INDEX16,
															 D3DPOOL_DEFAULT,
															 &ib,
															 NULL);
	}

	buffer_ = Effekseer::CreateUniqueReference(ib);

	return SUCCEEDED(hr);
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
	Allocate(elementCount_, stride_);
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

VertexBuffer* GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = new VertexBuffer(this);

	if (!ret->Init(size, isDynamic))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	ret->UpdateData(initialData, size, 0);

	return ret;
}

IndexBuffer* GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = new IndexBuffer(this);

	if (!ret->Init(elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	ret->UpdateData(initialData, elementCount * (stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2), 0);

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererDX9