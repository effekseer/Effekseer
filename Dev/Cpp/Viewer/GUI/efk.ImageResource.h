#pragma once

#include <Effekseer.h>
#include <string>

namespace efk
{

class ImageResource
{
private:
	std::shared_ptr<Effekseer::TextureLoader> loader_ = nullptr;
	std::u16string path;
	Effekseer::TextureData* textureData = nullptr;

public:
	//! dummy
	ImageResource() = default;

#if !defined(SWIG)
	ImageResource(std::shared_ptr<Effekseer::TextureLoader> loader);
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