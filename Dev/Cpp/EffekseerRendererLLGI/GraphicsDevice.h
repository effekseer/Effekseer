
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <LLGI.CommandList.h>
#include <LLGI.ConstantBuffer.h>
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
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class Shader;
class VertexLayout;
class FrameBuffer;
class Texture;
class RenderPass;
class PipelineState;
class UniformLayout;

using GraphicsDeviceRef = Effekseer::RefPtr<GraphicsDevice>;
using VertexBufferRef = Effekseer::RefPtr<VertexBuffer>;
using IndexBufferRef = Effekseer::RefPtr<IndexBuffer>;
using UniformBufferRef = Effekseer::RefPtr<UniformBuffer>;
using ShaderRef = Effekseer::RefPtr<Shader>;
using VertexLayoutRef = Effekseer::RefPtr<VertexLayout>;
using FrameBufferRef = Effekseer::RefPtr<FrameBuffer>;
using TextureRef = Effekseer::RefPtr<Texture>;
using RenderPassRef = Effekseer::RefPtr<RenderPass>;
using PipelineStateRef = Effekseer::RefPtr<PipelineState>;
using UniformLayoutRef = Effekseer::RefPtr<UniformLayout>;

class DeviceObject
{
private:
public:
	virtual void OnLostDevice();

	virtual void OnResetDevice();
};

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

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

	LLGI::VertexBuffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	std::shared_ptr<LLGI::IndexBuffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	int32_t stride_ = 0;

public:
	IndexBuffer(GraphicsDevice* graphicsDevice);

	~IndexBuffer() override;

	bool Allocate(int32_t elementCount, int32_t stride);

	void Deallocate();

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Init(int32_t elementCount, int32_t stride);

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

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
	std::function<void()> onDisposed_;

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param);

	bool Init(uint64_t id, std::function<void()> onDisposed);

	bool Init(LLGI::Texture* texture);

	std::shared_ptr<LLGI::Texture>& GetTexture()
	{
		return texture_;
	}
};

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

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param) override;

	Effekseer::Backend::TextureRef CreateTexture(uint64_t id, const std::function<void()>& onDisposed);

	Effekseer::Backend::TextureRef CreateTexture(LLGI::Texture* texture);
};

} // namespace Backend

} // namespace EffekseerRendererLLGI

#endif