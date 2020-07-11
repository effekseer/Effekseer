
#pragma once

#include "efk.Graphics.h"

namespace Effekseer
{
namespace Tool
{

class CommandList
{
protected:
public:
	CommandList() = default;
	virtual ~CommandList() = default;

	virtual void BeginRenderPass(std::shared_ptr<RenderPass> renderPass) = 0;

	virtual void EndRenderPass() = 0;
};

} // namespace Tool
} // namespace Effekseer