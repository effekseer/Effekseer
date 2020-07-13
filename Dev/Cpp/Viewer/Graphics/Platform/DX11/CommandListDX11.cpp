#include "CommandListDX11.h"
#include "RenderPassDX11.h"

namespace Effekseer
{
namespace Tool
{

CommandListDX11::CommandListDX11(ID3D11Device* device, ID3D11DeviceContext* context)
	: device_(device)
	, context_(context)
{
	ES_SAFE_ADDREF(device_);
	ES_SAFE_ADDREF(context_);
}

CommandListDX11::~CommandListDX11()
{
	ES_SAFE_RELEASE(device_);
	ES_SAFE_RELEASE(context_);
}

void CommandListDX11::BeginRenderPass(std::shared_ptr<RenderPass> renderPass)
{
	renderPass_ = renderPass;
}

void CommandListDX11::EndRenderPass()
{
	auto rp = static_cast<RenderPassDX11*>(renderPass_.get());

	if (rp->HasResolvedTexture())
	{
		rp->Resolve();
	}

	renderPass_ = nullptr;
}

} // namespace Tool
} // namespace Effekseer