
#include "efk.ImageResource.h"
#include <IO/IO.h>

namespace efk
{

ImageResource::ImageResource(DeviceType deviceType, Effekseer::TextureLoaderRef loader)
	: deviceType_(deviceType)
	, loader_(loader)
{
}

ImageResource::~ImageResource()
{
	Invalidate();
}

bool ImageResource::Validate()
{
	Invalidate();

	auto staticFile = Effekseer::IO::GetInstance()->LoadFile(path.c_str());
	if (staticFile == nullptr)
		return false;

	texture = loader_->Load(staticFile->GetData(), staticFile->GetSize(), Effekseer::TextureType::Color, true);

	if (texture == nullptr)
		return false;

	return true;
}

void ImageResource::Invalidate()
{
	if (texture != nullptr)
	{
		loader_->Unload(texture);
	}

	texture = nullptr;
}

} // namespace efk