#pragma once

#include "../../RenderPass.h"
#include "efk.GraphicsDX11.h"

namespace Effekseer
{
namespace Tool
{

class RenderPassDX11 : public RenderPass
{
protected:
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;

public:
	RenderPassDX11(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual ~RenderPassDX11();

	bool Initialize(std::shared_ptr<efk::RenderTexture> colorTexture, std::shared_ptr<efk::DepthTexture> depthTexture, std::shared_ptr<efk::RenderTexture> resolvedColorTexture);

	void Resolve();
};

} // namespace Tool
} // namespace Effekseer