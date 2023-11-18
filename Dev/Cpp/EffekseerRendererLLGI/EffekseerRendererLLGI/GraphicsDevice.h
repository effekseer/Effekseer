
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <LLGI.Buffer.h>
#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
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

std::vector<uint8_t> Serialize(const std::vector<LLGI::DataStructure>& data);

std::vector<LLGI::DataStructure> Deserialize(const void* data, int32_t size);

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
	std::shared_ptr<LLGI::Buffer> buffer_;
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

	LLGI::Buffer* GetBuffer()
	{
		return buffer_.get();
	}
};

class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	std::shared_ptr<LLGI::Buffer> buffer_;
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

	LLGI::Buffer* GetBuffer()
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

	bool Init(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData);

	bool Init(uint64_t id, std::function<void()> onDisposed);

	bool Init(LLGI::Texture* texture);

	std::shared_ptr<LLGI::Texture>& GetTexture()
	{
		return texture_;
	}
};

class VertexLayout
	: public DeviceObject,
	  public Effekseer::Backend::VertexLayout
{
public:
	class Element
	{
	public:
		LLGI::VertexLayoutFormat Format;
		std::string Name;
		int32_t Semantic;
	};

private:
	Effekseer::CustomVector<Element> elements_;

public:
	VertexLayout() = default;
	~VertexLayout() = default;

	bool Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount);

	void MakeGenerated();

	const Effekseer::CustomVector<Element>& GetElements() const
	{
		return elements_;
	}
};

class Shader
	: public DeviceObject,
	  public Effekseer::Backend::Shader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::shared_ptr<LLGI::Shader> vertexShader_ = nullptr;
	std::shared_ptr<LLGI::Shader> pixelShader_ = nullptr;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize);

	LLGI::Shader* GetVertexShader() const
	{
		return vertexShader_.get();
	}
	LLGI::Shader* GetPixelShader() const
	{
		return pixelShader_.get();
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

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param, const Effekseer::CustomVector<uint8_t>& initialData) override;

	Effekseer::Backend::TextureRef CreateTexture(uint64_t id, const std::function<void()>& onDisposed);

	Effekseer::Backend::TextureRef CreateTexture(LLGI::Texture* texture);

	Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

	Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsData, int32_t vsDataSize, const void* psData, int32_t psDataSize) override;
};

} // namespace Backend

} // namespace EffekseerRendererLLGI

#endif