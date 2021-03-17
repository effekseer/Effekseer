#pragma once

#include "../../CommandList.h"

namespace Effekseer
{
namespace Tool
{

class CommandListGL : public CommandList
{
private:
	std::shared_ptr<RenderPass> renderPass_;

public:
	CommandListGL();

	virtual ~CommandListGL();

	void BeginRenderPass(std::shared_ptr<RenderPass> renderPass) override;

	void EndRenderPass() override;
};

} // namespace Tool
} // namespace Effekseer