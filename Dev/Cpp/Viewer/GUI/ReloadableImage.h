#pragma once

#include "Image.h"

namespace Effekseer::Tool
{

class ReloadableImage : public Image
{
private:
	Effekseer::TextureLoaderRef loader_;
	Effekseer::TextureRef textureOriginal_;
	std::u16string path_;

public:
	//! dummy
	ReloadableImage() = default;

#if !defined(SWIG)
	ReloadableImage(const std::u16string& path, Effekseer::TextureLoaderRef textureLoader);
#endif
	~ReloadableImage();

	bool Validate();
	void Invalidate();
};

} // namespace Effekseer::Tool