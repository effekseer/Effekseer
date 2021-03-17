#include "CommandListGL.h"
#include "RenderPassGL.h"

namespace Effekseer
{
namespace Tool
{

CommandListGL::CommandListGL()
{
}

CommandListGL::~CommandListGL()
{
}

void CommandListGL::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
{
	renderPass_ = renderPass;
}

void CommandListGL::EndRenderPass()
{
	auto rp = static_cast<RenderPassGL*>(renderPass_.get());

	if (rp->HasResolvedTexture())
	{
		rp->Resolve();
	}

	renderPass_ = nullptr;
}

} // namespace Tool
} // namespace Effekseer