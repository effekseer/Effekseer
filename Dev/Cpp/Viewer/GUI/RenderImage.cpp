#include "RenderImage.h"

namespace Effekseer::Tool
{

RenderImage::RenderImage(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	: Effekseer::ToolRuntime::RenderImage(graphicsDevice)
{
}

std::shared_ptr<Effekseer::Tool::RenderImage> RenderImage::Create(std::shared_ptr<GraphicsDevice> graphicsDevice)
{
	return std::make_shared<Effekseer::Tool::RenderImage>(graphicsDevice->GetGraphics()->GetGraphicsDevice());
}

} // namespace Effekseer::Tool
