#include "RenderImage.h"

namespace Effekseer::Tool
{

RenderImage::RenderImage(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
}

void RenderImage::Resize(int32_t width, int32_t height)
{
	if (texture_ != nullptr)
	{
		if (texture_->GetParameter().Size[0] == width && texture_->GetParameter().Size[1] == height)
		{
			return;
		}
	}

	Effekseer::Backend::TextureParameter param;
	param.Size[0] = width;
	param.Size[1] = height;
	param.Usage = Effekseer::Backend::TextureUsageType::RenderTarget;

	texture_ = graphicsDevice_->CreateTexture(param);
}

std::shared_ptr<Effekseer::Tool::RenderImage> RenderImage::Create(std::shared_ptr<GraphicsDevice> graphicsDevice)
{
	return std::make_shared<Effekseer::Tool::RenderImage>(graphicsDevice->GetGraphics()->GetGraphicsDevice());
}

} // namespace Effekseer::Tool