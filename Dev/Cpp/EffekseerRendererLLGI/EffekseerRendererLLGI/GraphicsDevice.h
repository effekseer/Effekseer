
#ifndef __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_LLGI_GRAPHICS_DEVICE_H__

#include <Effekseer.h>
#include <LLGI.Buffer.h>
#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
#include <LLGI.PipelineState.h>
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

class RenderPass
	: public DeviceObject,
	  public Effekseer::Backend::RenderPass
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::shared_ptr<LLGI::RenderPass> renderPass_ = nullptr;
	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures_;
	Effekseer::Backend::TextureRef depthTexture_;

public:
	RenderPass(GraphicsDevice* graphicsDevice);
	~RenderPass() override;

	bool Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture);

	LLGI::RenderPass* GetRenderPass() const
	{
		return renderPass_.get();
	}

	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& GetTextures()
	{
		return textures_;
	}

	Effekseer::Backend::TextureRef& GetDepthTexture()
	{
		return depthTexture_;
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
	std::shared_ptr<LLGI::Shader> computeShader_ = nullptr;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const void* vertexShaderData, int32_t vertexShaderDataSize, const void* pixelShaderData, int32_t pixelShaderDataSize);
	bool InitAsCompute(const void* computeShaderData, int32_t computeShaderDataSize);

	LLGI::Shader* GetVertexShader() const
	{
		return vertexShader_.get();
	}
	LLGI::Shader* GetPixelShader() const
	{
		return pixelShader_.get();
	}
	LLGI::Shader* GetComputeShader() const
	{
		return computeShader_.get();
	}
};

class UniformBuffer
	: public DeviceObject,
	  public Effekseer::Backend::UniformBuffer
{
	std::shared_ptr<LLGI::Buffer> buffer_;
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::function<void()> onDisposed_;

public:
	UniformBuffer(GraphicsDevice* graphicsDevice);
	~UniformBuffer() override;

	bool Init(int32_t size, const void* initialData);

	LLGI::Buffer* GetBuffer() const
	{
		return buffer_.get();
	}

	void UpdateData(const void* src, int32_t size, int32_t offset);
};

class ComputeBuffer
	: public DeviceObject,
	  public Effekseer::Backend::ComputeBuffer
{
	std::shared_ptr<LLGI::Buffer> buffer_;
	int32_t stride_ = 0;
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::function<void()> onDisposed_;

public:
	ComputeBuffer(GraphicsDevice* graphicsDevice);
	~ComputeBuffer() override;

	bool Init(int32_t stride, int32_t size, const void* initialData, bool readOnly);

	LLGI::Buffer* GetBuffer() const
	{
		return buffer_.get();
	}

	int32_t GetStride() const
	{
		return stride_;
	}

	void UpdateData(const void* src, int32_t size, int32_t offset);
};

class PipelineState
	: public DeviceObject,
	  public Effekseer::Backend::PipelineState
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::map<LLGI::RenderPassPipelineState*, std::shared_ptr<LLGI::PipelineState>> pips_;
	Effekseer::Backend::PipelineStateParameter param_;

public:
	PipelineState(GraphicsDevice* graphicsDevice);
	~PipelineState() override;

	bool Init(const Effekseer::Backend::PipelineStateParameter& param);

	LLGI::PipelineState* GetOrCreatePipelineState(LLGI::RenderPassPipelineState* renderPassPipelineState);

	const Effekseer::Backend::PipelineStateParameter& GetParam() const
	{
		return param_;
	}
};

class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	std::set<DeviceObject*> objects_;
	LLGI::Graphics* graphics_ = nullptr;
	LLGI::CommandList* commandList_ = nullptr;
	LLGI::RenderPassPipelineState* renderPassPipelineState_ = nullptr;

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

	Effekseer::Backend::ShaderRef CreateComputeShader(const void* csData, int32_t csDataSize) override;

	Effekseer::Backend::UniformBufferRef CreateUniformBuffer(int32_t size, const void* initialData) override;

	Effekseer::Backend::ComputeBufferRef CreateComputeBuffer(int32_t elementCount, int32_t elementSize, const void* initialData, bool readOnly) override;

	Effekseer::Backend::PipelineStateRef CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param) override;

	Effekseer::Backend::RenderPassRef CreateRenderPass(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef& depthTexture) override;

	void SetCommandList(LLGI::CommandList* commandList);

	void SetRenderPassPipelineState(LLGI::RenderPassPipelineState* renderPassPipelineState);

	void Draw(const Effekseer::Backend::DrawParameter& drawParam) override;

	void BeginRenderPass(Effekseer::Backend::RenderPassRef& renderPass, bool isColorCleared, bool isDepthCleared, Effekseer::Color clearColor) override;

	void EndRenderPass() override;

	void Dispatch(const Effekseer::Backend::DispatchParameter& dispatchParam) override;

	void BeginComputePass() override;

	void EndComputePass() override;

	bool UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data) override;

	bool UpdateComputeBuffer(Effekseer::Backend::ComputeBufferRef& buffer, int32_t size, int32_t offset, const void* data) override;
};

} // namespace Backend

} // namespace EffekseerRendererLLGI

#endif