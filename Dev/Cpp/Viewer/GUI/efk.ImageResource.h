#pragma once

#include <Effekseer.h>
#include <string>

namespace efk
{
	class ImageResource
	{
	private:
		std::u16string	path;
		Effekseer::TextureData* textureData = nullptr;
	public:

		const char16_t* GetPath() const { return path.c_str(); }
#if !SWIG
		void SetPath(const char16_t* path) { this->path = path; }
		Effekseer::TextureData*& GetTextureData() { return textureData; }
#endif
	};
}