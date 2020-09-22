
#ifndef __EFFEKSEERRENDERER_DX11_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_DX11_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <assert.h>
#include <d3d11.h>
#include <set>

namespace EffekseerRendererDX11
{
namespace Backend
{

using D3D11BufferPtr = std::unique_ptr<ID3D11Buffer, Effekseer::ReferenceDeleter<ID3D11Buffer>>;
using D3D11DevicePtr = std::unique_ptr<ID3D11Device, Effekseer::ReferenceDeleter<ID3D11Device>>;
using D3D11DeviceContextPtr = std::unique_ptr<ID3D11DeviceContext, Effekseer::ReferenceDeleter<ID3D11DeviceContext>>;
using D3D11ResourcePtr = std::unique_ptr<ID3D11Resource, Effekseer::ReferenceDeleter<ID3D11Resource>>;
using D3D11Texture2DPtr = std::unique_ptr<ID3D11Texture2D, Effekseer::ReferenceDeleter<ID3D11Texture2D>>;
using D3D11ShaderResourceViewPtr = std::unique_ptr<ID3D11ShaderResourceView, Effekseer::ReferenceDeleter<ID3D11ShaderResourceView>>;
using D3D11DepthStencilViewPtr = std::unique_ptr<ID3D11DepthStencilView, Effekseer::ReferenceDeleter<ID3D11DepthStencilView>>;

class GraphicsDevice;

class DirtiedBlock
{
	struct Block
	{
		int32_t offset;
		int32_t size;
	};

	std::vector<Block> blocks_;

public:
	/**
		@brief	Allocate block
		@return	whether is required to discard.
	*/
	bool Allocate(int32_t size, int32_t offset);
};

class DeviceObject
{
private:
public:
	virtual void OnLostDevice();

	virtual void OnResetDevice();
};

/**
	@brief	VertexBuffer of DirectX11
*/
class VertexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::VertexBuffer
{
private:
	DirtiedBlock blocks_;

	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_ = nullptr;
	int32_t size_ = 0;
	bool isDynamic_ = false;

public:
	VertexBuffer(GraphicsDevice* graphicsDevice);

	~VertexBuffer() override;

	bool Allocate(const void* src, int32_t size, bool isDynamic);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(const void* src, int32_t size, bool isDynamic);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}
};

/**
	@brief	IndexBuffer of DirectX11
*/
class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	DirtiedBlock blocks_;

	GraphicsDevice* graphicsDevice_ = nullptr;
	D3D11BufferPtr buffer_ = nullptr;
	int32_t elementCount_ = 0;
	int32_t stride_ = 0;

public:
	IndexBuffer(GraphicsDevice* graphicsDevice);

	~IndexBuffer() override;

	bool Allocate(const void* src, int32_t elementCount, int32_t stride);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(const void* src, int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	ID3D11Buffer* GetBuffer() const
	{
		return buffer_.get();
	}
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend::Texture
{
	D3D11Texture2DPtr texture_;
	D3D11ShaderResourceViewPtr srv_;
	D3D11DepthStencilViewPtr dsv_;

	GraphicsDevice* graphicsDevice_ = nullptr;

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param);

	bool Init(const Effekseer::Backend::DepthTextureParameter& param);
};

/**
	@brief	GraphicsDevice of DirectX11
*/
class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	D3D11DevicePtr device_ = nullptr;
	D3D11DeviceContextPtr context_ = nullptr;

	std::set<DeviceObject*> objects_;

public:
	GraphicsDevice(ID3D11Device* device, ID3D11DeviceContext* context);

	~GraphicsDevice() override;

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetContext() const;

	void LostDevice();

	void ResetDevice();

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	VertexBuffer* CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	IndexBuffer* CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Texture* CreateTexture(const Effekseer::Backend::TextureParameter& param) override;
};

} // namespace Backend

} // namespace EffekseerRendererDX11

#endif