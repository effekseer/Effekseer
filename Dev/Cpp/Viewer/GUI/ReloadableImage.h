#pragma once

#include "../Graphics/GraphicsDevice.h"
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

	static std::shared_ptr<Effekseer::Tool::ReloadableImage> Create(std::shared_ptr<GraphicsDevice> graphicsDevice, const char16_t* path);
};

} // namespace Effekseer::Tool