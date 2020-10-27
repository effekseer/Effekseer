
#ifndef __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__
#define __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__

#include <Effekseer.h>
#include <set>

namespace EffekseerRendererGL
{

class DeviceObject;

class DeviceObjectCollection : public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	std::set<DeviceObject*> deviceObjects_;

	/**
		@brief	register an object
	*/
	void Register(DeviceObject* device);

	/**
		@brief	unregister an object
	*/
	void Unregister(DeviceObject* device);

public:
	DeviceObjectCollection() = default;

	~DeviceObjectCollection() = default;

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
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__