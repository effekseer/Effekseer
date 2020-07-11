#pragma once

#include "../../CommandList.h"
#include "efk.GraphicsDX11.h"

namespace Effekseer
{
namespace Tool
{

class CommandListDX11 : public CommandList
{
private:
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;
	std::shared_ptr<RenderPass> renderPass_;

public:
	CommandListDX11(ID3D11Device* device, ID3D11DeviceContext* context);

	virtual ~CommandListDX11();

	void BeginRenderPass(std::shared_ptr<RenderPass> renderPass) override;

	void EndRenderPass() override;
};

} // namespace Tool
} // namespace Effekseer