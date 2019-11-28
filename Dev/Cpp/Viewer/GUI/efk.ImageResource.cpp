
#include "efk.ImageResource.h"

namespace efk
{

ImageResource::ImageResource(std::shared_ptr<Effekseer::TextureLoader> loader) : loader_(loader) {}

ImageResource::~ImageResource()
{
	if (textureData != nullptr)
	{
		loader_->Unload(textureData);
	}
}

bool ImageResource::Validate()
{

	auto loaded = loader_->Load(path.c_str(), Effekseer::TextureType::Color);

	if (loaded == nullptr)
		return false;

	if (textureData != nullptr)
	{
		loader_->Unload(textureData);
	}

	textureData = loaded;

	return true;
}

void ImageResource::Invalidate()
{

	if (textureData != nullptr)
	{
		loader_->Unload(textureData);
	}

	textureData = nullptr;
}

} // namespace efk