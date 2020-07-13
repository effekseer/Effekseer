#pragma once

#include "efk.Graphics.h"

namespace Effekseer
{
namespace Tool
{

class RenderPass
{
protected:
	std::shared_ptr<efk::RenderTexture> colorTexture_;
	std::shared_ptr<efk::DepthTexture> depthTexture_;
	std::shared_ptr<efk::RenderTexture> resolvedColorTexture_;

public:
	RenderPass() = default;
	virtual ~RenderPass() = default;

	std::shared_ptr<efk::RenderTexture> GetColorTexture() const
	{
		return colorTexture_;
	}

	std::shared_ptr<efk::DepthTexture> GetDepthTexture() const
	{
		return depthTexture_;
	}

	std::shared_ptr<efk::RenderTexture> GetResolvedColorTexture() const
	{
		return resolvedColorTexture_;
	}

	bool HasResolvedTexture() const
	{
		return resolvedColorTexture_ != nullptr;
	}
};

} // namespace Tool
} // namespace Effekseer