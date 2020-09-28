
#ifndef __EFFEKSEERRENDERER_GL_GRAPHICS_DEVICE_H__
#define __EFFEKSEERRENDERER_GL_GRAPHICS_DEVICE_H__

#include "EffekseerRendererGL.GLExtension.h"
#include <Effekseer.h>
#include <assert.h>
#include <set>

namespace EffekseerRendererGL
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
};

class Texture
	: public DeviceObject,
	  public Effekseer::Backend ::Texture
{
private:
	GLuint buffer_ = 0;
	GraphicsDevice* graphicsDevice_ = nullptr;

	bool InitInternal(const Effekseer::Backend::TextureParameter& param);

public:
	Texture(GraphicsDevice* graphicsDevice);
	~Texture() override;

	bool Init(const Effekseer::Backend::TextureParameter& param);

	bool Init(const Effekseer::Backend::RenderTextureParameter& param);

	bool Init(const Effekseer::Backend::DepthTextureParameter& param);
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
};

class Shader
	: public DeviceObject,
	  public Effekseer::Backend::Shader
{
private:
public:
	Shader() = default;
	~Shader() = default;
};

class PipelineState
	: public DeviceObject,
	  public Effekseer::Backend::PipelineState
{
private:
public:
	PipelineState() = default;
	~PipelineState() = default;
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

public:
	GraphicsDevice(OpenGLDeviceType deviceType);

	~GraphicsDevice() override;

	void LostDevice();

	void ResetDevice();

	OpenGLDeviceType GetDeviceType() const;

	void Register(DeviceObject* deviceObject);

	void Unregister(DeviceObject* deviceObject);

	VertexBuffer* CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	IndexBuffer* CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;

	Texture* CreateTexture(const Effekseer::Backend::TextureParameter& param) override;

	Texture* CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param) override;

	Texture* CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param) override;

	UniformBuffer* CreateUniformBuffer(int32_t size, const void* initialData) override;

	VertexLayout* CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount) override;

	PipelineState* CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param)
	{
		// not implemented
		assert(0);
		return nullptr;
	}

	Shader* CreateShaderFromKey(const char* key)
	{
		// not implemented
		assert(0);
		return nullptr;
	}

	virtual void Draw(const Effekseer::Backend::DrawParameter& drawParam)
	{
		// not implemented
		assert(0);
	}
};

} // namespace Backend

} // namespace EffekseerRendererGL

#endif