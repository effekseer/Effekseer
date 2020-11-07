
#ifndef __EFFEKSEERRENDERER_GL_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_GL_GRAPHICS_DEVICE_H__

#include "EffekseerRendererGL.GLExtension.h"
#include <Effekseer.h>
#include <assert.h>
#include <functional>
#include <set>

namespace EffekseerRendererGL
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

/**
	@brief	VertexBuffer of OpenGL
	@note
	TODO : Optimize it
*/
class VertexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::VertexBuffer
{
private:
	GLuint buffer_ = 0;
	std::vector<uint8_t> resources_;
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

	GLuint GetBuffer() const
	{
		return buffer_;
	}
};

/**
	@brief	IndexBuffer of OpenGL
	TODO : Optimize it
*/
class IndexBuffer
	: public DeviceObject,
	  public Effekseer::Backend::IndexBuffer
{
private:
	GLuint buffer_ = 0;
	std::vector<uint8_t> resources_;
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

	void UpdateData(const void* src, int32_t size, int32_t offset);

	GLuint GetBuffer() const
	{
		return buffer_;
	}
};

class UniformBuffer
	: public DeviceObject,
	  public Effekseer::Backend::UniformBuffer
{
private:
	Effekseer::CustomVector<uint8_t> buffer_;

public:
	UniformBuffer() = default;
	~UniformBuffer() override = default;

	bool Init(int32_t size, const void* initialData);

	const Effekseer::CustomVector<uint8_t>& GetBuffer() const
	{
		return buffer_;
	}

	void UpdateData(const void* src, int32_t size, int32_t offset);
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend ::Texture
{
private:
	GLuint buffer_ = 0;
	GraphicsDevice* graphicsDevice_ = nullptr;
	std::function<void()> onDisposed_;

	bool InitInternal(const Effekseer::Backend::TextureParameter& param);

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param);

	bool Init(const Effekseer::Backend::RenderTextureParameter& param);

	bool Init(const Effekseer::Backend::DepthTextureParameter& param);

	bool Init(GLuint buffer, const std::function<void()>& onDisposed);

	GLuint GetBuffer() const
	{
		return buffer_;
	}
};

class VertexLayout
	: public DeviceObject,
	  public Effekseer::Backend::VertexLayout
{
private:
	Effekseer::CustomVector<Effekseer::Backend::VertexLayoutElement> elements_;

public:
	VertexLayout() = default;
	~VertexLayout() = default;

	bool Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount);

	const Effekseer::CustomVector<Effekseer::Backend::VertexLayoutElement>& GetElements() const
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
	GLuint program_ = 0;
	GLuint vao_ = 0;

	Effekseer::Backend::UniformLayoutRef layout_ = nullptr;

public:
	Shader(GraphicsDevice* graphicsDevice);
	~Shader() override;
	bool Init(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef& layout);

	GLuint GetProgram() const
	{
		return program_;
	}

	GLuint GetVAO() const
	{
		return vao_;
	}

	const Effekseer::Backend::UniformLayoutRef& GetLayout() const
	{
		return layout_;
	}
};

class PipelineState
	: public DeviceObject,
	  public Effekseer::Backend::PipelineState
{
private:
	Effekseer::Backend::PipelineStateParameter param_;

public:
	PipelineState() = default;
	~PipelineState() = default;

	bool Init(const Effekseer::Backend::PipelineStateParameter& param);

	const Effekseer::Backend::PipelineStateParameter& GetParam() const
	{
		return param_;
	}
};

class RenderPass
	: public DeviceObject,
	  public Effekseer::Backend::RenderPass
{
private:
	GLuint buffer_ = 0;
	GraphicsDevice* graphicsDevice_ = nullptr;
	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> textures_;
	Effekseer::Backend::TextureRef depthTexture_;

public:
	RenderPass(GraphicsDevice* graphicsDevice);
	~RenderPass() override;

	bool Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture);

	GLuint GetBuffer() const
	{
		return buffer_;
	}
};

/**
	@brief	GraphicsDevice of OpenGL
*/
class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
private:
	std::set<DeviceObject*> objects_;
	OpenGLDeviceType deviceType_;
	std::array<GLuint, Effekseer::TextureSlotMax> samplers_;

public:
	GraphicsDevice(OpenGLDeviceType deviceType);

	~GraphicsDevice() override;

	void LostDevice();

	void ResetDevice();

	OpenGLDeviceType GetDeviceType() const;

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param) override;

	Effekseer::Backend::TextureRef CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param) override;

	Effekseer::Backend::TextureRef CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param) override;

	Effekseer::Backend::UniformBufferRef CreateUniformBuffer(int32_t size, const void* initialData) override;

	Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

	Effekseer::Backend::RenderPassRef CreateRenderPass(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef& depthTexture) override;

	Effekseer::Backend::ShaderRef CreateShaderFromKey(const char* key) override;

	Effekseer::Backend::ShaderRef CreateShaderFromCodes(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef layout) override;

	Effekseer::Backend::PipelineStateRef CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param) override;

	void Draw(const Effekseer::Backend::DrawParameter& drawParam) override;

	void BeginRenderPass(Effekseer::Backend::RenderPassRef& renderPass, bool isColorCleared, bool isDepthCleared, Effekseer::Color clearColor) override;

	void EndRenderPass() override;

	bool UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data) override;

	std::string GetDeviceName() const override
	{
		return "OpenGL";
	}

	Texture* CreateTexture(GLuint buffer, const std::function<void()>& onDisposed);
};

} // namespace Backend

} // namespace EffekseerRendererGL

#endif