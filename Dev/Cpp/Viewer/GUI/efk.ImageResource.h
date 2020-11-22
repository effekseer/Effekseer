#pragma once

#include "../efk.Base.h"
#include <Effekseer.h>
#include <string>

namespace efk
{

class ImageResource
{
private:
	DeviceType deviceType_;
	Effekseer::TextureLoaderRef loader_;
	std::u16string path;
	Effekseer::TextureData* textureData = nullptr;

public:
	//! dummy
	ImageResource() = default;

#if !defined(SWIG)
	ImageResource(DeviceType deviceType, Effekseer::TextureLoaderRef loader);
#endif
	virtual ~ImageResource();
	bool Validate();
	void Invalidate();
	const char16_t* GetPath() const
	{
		return path.c_str();
	}
	int32_t GetWidth() const
	{
		return textureData->Width;
	}
	int32_t GetHeight() const
	{
		return textureData->Height;
	}

	DeviceType GetDeviceType() const
	{
		return deviceType_;
	}

#if !defined(SWIG)
	void SetPath(const char16_t* path)
	{
		this->path = path;
	}
	Effekseer::TextureData*& GetTextureData()
	{
		return textureData;
	}
#endif
};

} // namespace efk