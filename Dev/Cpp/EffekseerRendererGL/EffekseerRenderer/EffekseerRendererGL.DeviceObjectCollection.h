
#ifndef __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__
#define __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererGL.Base.h"
#include <Effekseer.h>
#include <set>

namespace EffekseerRendererGL
{

class DeviceObject;

class GraphicsDevice : public EffekseerRenderer::GraphicsDevice, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	std::set<DeviceObject*> deviceObjects_;
	OpenGLDeviceType deviceType_;

	/**
		@brief	register an object
	*/
	void Register(DeviceObject* device);

	/**
		@brief	unregister an object
	*/
	void Unregister(DeviceObject* device);

public:
	GraphicsDevice(OpenGLDeviceType deviceType)
		: deviceType_(deviceType)
	{
	}

	~GraphicsDevice() = default;

	/**
		@brief
		\~english Call when device lost causes
		\~japanese デバイスロストが発生した時に実行する。
	*/
	void OnLostDevice();

	/**
		@brief
		\~english Call when device reset causes
		\~japanese デバイスがリセットされた時に実行する。
	*/
	void OnResetDevice();

	OpenGLDeviceType GetDeviceType() const
	{
		return deviceType_;
	}

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__