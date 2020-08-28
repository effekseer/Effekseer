
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
};

} // namespace Backend

} // namespace EffekseerRendererGL

#endif