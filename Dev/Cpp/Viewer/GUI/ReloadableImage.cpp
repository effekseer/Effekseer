#include "ReloadableImage.h"
#include <IO/IO.h>

namespace Effekseer::Tool
{

ReloadableImage::ReloadableImage(const std::u16string& path, Effekseer::TextureLoaderRef textureLoader)
	: path_(path)
	, loader_(textureLoader)
{
}

ReloadableImage::~ReloadableImage()
{
	Invalidate();
}

bool ReloadableImage::Validate()
{
	Invalidate();

	auto staticFile = Effekseer::IO::GetInstance()->LoadFile(path_.c_str());
	if (staticFile == nullptr)
		return false;

	textureOriginal_ = loader_->Load(staticFile->GetData(), staticFile->GetSize(), Effekseer::TextureType::Color, true);

	if (textureOriginal_ == nullptr)
		return false;

	texture_ = textureOriginal_->GetBackend();

	return true;
}

void ReloadableImage::Invalidate()
{
	if (textureOriginal_ != nullptr)
	{
		loader_->Unload(textureOriginal_);
	}

	textureOriginal_ = nullptr;
}

} // namespace Effekseer::Tool