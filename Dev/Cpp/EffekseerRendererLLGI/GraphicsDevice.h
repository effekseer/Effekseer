
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <LLGI.CommandList.h>
#include <LLGI.Constantbuffer.h>
#include <LLGI.Graphics.h>
#include <LLGI.IndexBuffer.h>
#include <LLGI.VertexBuffer.h>
#include <assert.h>
#include <set>

namespace EffekseerRendererLLGI
{
namespace Backend
{

class GraphicsDevice;

class DeviceObject
{
private:
public:
	virtual void OnLostDevice();

	virtual void OnResetDevice();
};

/**
	@brief	VertexBuffer of OpenGL
*/
class VertexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::VertexBuffer
{
private:
	std::shared_ptr<LLGI::VertexBuffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	int32_t size_ = 0;
	bool isDynamic_ = false;

public:
	VertexBuffer(GraphicsDevice* graphicsDevice);

	~VertexBuffer() override;

	bool Allocate(int32_t size, bool isDynamic);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(int32_t size, bool isDynamic);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	LLGI::VertexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

/**
	@brief	IndexBuffer of OpenGL
*/
class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	std::shared_ptr<LLGI::IndexBuffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	int32_t elementCount_ = 0;
	int32_t stride_ = 0;

public:
	IndexBuffer(GraphicsDevice* graphicsDevice);

	~IndexBuffer() override;

	bool Allocate(int32_t elementCount, int32_t stride);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset);

	LLGI::IndexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend::Texture
{
	std::shared_ptr<LLGI::Texture> texture_;
	GraphicsDevice* graphicsDevice_ = nullptr;

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param);
};

/**
	@brief	GraphicsDevice of OpenGL
*/
class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	std::set<DeviceObject*> objects_;
	LLGI::Graphics* graphics_;

public:
	GraphicsDevice(LLGI::Graphics* graphics);

	~GraphicsDevice() override;

	void LostDevice();

	void ResetDevice();

	LLGI::Graphics* GetGraphics();

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	VertexBuffer* CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	IndexBuffer* CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Texture* CreateTexture(const Effekseer::Backend::TextureParameter& param) override;
};

} // namespace Backend

} // namespace EffekseerRendererLLGI

#endif