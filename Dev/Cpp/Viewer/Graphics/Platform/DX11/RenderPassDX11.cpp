#include "RenderPassDX11.h"

namespace Effekseer
{
namespace Tool
{

RenderPassDX11::RenderPassDX11(ID3D11Device* device, ID3D11DeviceContext* context)
	: device_(device)
	, context_(context)
{
	ES_SAFE_ADDREF(device_);
	ES_SAFE_ADDREF(context_);
}

RenderPassDX11::~RenderPassDX11()
{
	ES_SAFE_RELEASE(device_);
	ES_SAFE_RELEASE(context_);
}

bool RenderPassDX11::Initialize(std::shared_ptr<efk::RenderTexture> colorTexture, std::shared_ptr<efk::DepthTexture> depthTexture, std::shared_ptr<efk::RenderTexture> resolvedColorTexture)
{
	colorTexture_ = colorTexture;
	depthTexture_ = depthTexture;
	resolvedColorTexture_ = resolvedColorTexture;
	return true;
}

void RenderPassDX11::Resolve()
{
	auto src = static_cast<efk::RenderTextureDX11*>(colorTexture_.get());
	auto dst = static_cast<efk::RenderTextureDX11*>(resolvedColorTexture_.get());

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	
	assert(src->GetDXGIFormat() == dst->GetDXGIFormat());

	context_->ResolveSubresource(dst->GetTexture(), 0, src->GetTexture(), 0, src->GetDXGIFormat());
}

} // namespace Tool
} // namespace Effekseer