#include "EffectPlatformDX11.h"

#include "../../3rdParty/stb/stb_image_write.h"
#include <EffekseerToolRuntime/GroundRendering.h>
#include <d3dcompiler.h>
#include <cstring>

#pragma comment(lib, "d3dcompiler.lib")

namespace
{

bool CompileGroundShader(const char* code, const char* target, ID3DBlob** shader)
{
	ID3DBlob* error = nullptr;
	auto hr = D3DCompile(code, strlen(code), nullptr, nullptr, nullptr, "main", target, 0, 0, shader, &error);
	if (FAILED(hr))
	{
		if (error != nullptr)
		{
			OutputDebugStringA(static_cast<const char*>(error->GetBufferPointer()));
			error->Release();
		}
		return false;
	}

	ES_SAFE_RELEASE(error);
	return true;
}

} // namespace

class DistortingCallbackDX11 : public EffekseerRenderer::DistortingCallback
{
	ID3D11Device* g_D3d11Device = NULL;
	ID3D11DeviceContext* g_D3d11Context = NULL;

	ID3D11Texture2D* backGroundTexture = nullptr;
	ID3D11ShaderResourceView* backGroundTextureSRV = nullptr;
	D3D11_TEXTURE2D_DESC backGroundTextureDesc;

public:
	DistortingCallbackDX11(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		g_D3d11Device = device;
		g_D3d11Context = context;
	}

	virtual ~DistortingCallbackDX11()
	{
		ReleaseTexture();
	}

	void ReleaseTexture()
	{
		ES_SAFE_RELEASE(backGroundTextureSRV);
		ES_SAFE_RELEASE(backGroundTexture);
	}

	void PrepareTexture(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		ReleaseTexture();

		ZeroMemory(&backGroundTextureDesc, sizeof(backGroundTextureDesc));
		backGroundTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		backGroundTextureDesc.Format = format;
		backGroundTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		backGroundTextureDesc.Width = width;
		backGroundTextureDesc.Height = height;
		backGroundTextureDesc.CPUAccessFlags = 0;
		backGroundTextureDesc.MipLevels = 1;
		backGroundTextureDesc.ArraySize = 1;
		backGroundTextureDesc.SampleDesc.Count = 1;
		backGroundTextureDesc.SampleDesc.Quality = 0;

		HRESULT hr = S_OK;
		hr = g_D3d11Device->CreateTexture2D(&backGroundTextureDesc, nullptr, &backGroundTexture);
		if (FAILED(hr))
		{
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		default:
			srvDesc.Format = format;
			break;
		}
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = g_D3d11Device->CreateShaderResourceView(backGroundTexture, &srvDesc, &backGroundTextureSRV);
		if (FAILED(hr))
		{
			return;
		}
	}

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{
		HRESULT hr = S_OK;

		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11Texture2D* renderTexture = nullptr;

		g_D3d11Context->OMGetRenderTargets(1, &renderTargetView, nullptr);
		renderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&renderTexture));

		D3D11_TEXTURE2D_DESC renderTextureDesc;
		renderTexture->GetDesc(&renderTextureDesc);

		if (backGroundTextureSRV == nullptr || backGroundTextureDesc.Width != renderTextureDesc.Width ||
			backGroundTextureDesc.Height != renderTextureDesc.Height || backGroundTextureDesc.Format != renderTextureDesc.Format)
		{
			PrepareTexture(renderTextureDesc.Width, renderTextureDesc.Height, renderTextureDesc.Format);
		}

		g_D3d11Context->CopyResource(backGroundTexture, renderTexture);
		ES_SAFE_RELEASE(renderTexture);
		ES_SAFE_RELEASE(renderTargetView);

		reinterpret_cast<EffekseerRendererDX11::Renderer*>(renderer)->SetBackground(backGroundTextureSRV);

		return true;
	}
};

void EffectPlatformDX11::CreateCheckedTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = initParam_.WindowSize[0];
	desc.Height = initParam_.WindowSize[1];
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	auto hr = device_->CreateTexture2D(&desc, 0, &checkedTexture_);
	if (FAILED(hr))
	{
		throw "Failed : CreateTexture";
	}

	D3D11_MAPPED_SUBRESOURCE mr;
	UINT sr = D3D11CalcSubresource(0, 0, 0);
	hr = context_->Map(checkedTexture_, sr, D3D11_MAP_READ_WRITE, 0, &mr);

	std::vector<uint8_t> data;

	data.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);

	for (int32_t h = 0; h < initParam_.WindowSize[1]; h++)
	{
		auto src_ = &(checkeredPattern_[h * initParam_.WindowSize[0]]);
		auto dst_ = &(((uint8_t*)mr.pData)[h * mr.RowPitch]);
		memcpy(dst_, src_, initParam_.WindowSize[0] * 4);
	}

	context_->Unmap(checkedTexture_, sr);
}

void EffectPlatformDX11::UpdateBackgroundTexture()
{
	ES_SAFE_RELEASE(checkedTexture_);
	CreateCheckedTexture();
}

bool EffectPlatformDX11::CreateGroundResources()
{
	if (groundDepthTexture_ != nullptr)
	{
		return true;
	}

	ID3DBlob* vs = nullptr;
	ID3DBlob* ps = nullptr;
	ID3DBlob* depthPs = nullptr;
	const auto& shaderCode = Effekseer::ToolRuntime::GetGroundShaderCode(Effekseer::ToolRuntime::GroundShaderBackend::DirectX11);
	if (!CompileGroundShader(shaderCode.Vertex, shaderCode.VertexProfile, &vs) ||
		!CompileGroundShader(shaderCode.Pixel, shaderCode.PixelProfile, &ps) ||
		!CompileGroundShader(shaderCode.DepthPixel, shaderCode.PixelProfile, &depthPs))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	auto hr = device_->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, &groundVertexShader_);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	hr = device_->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, &groundPixelShader_);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	hr = device_->CreatePixelShader(depthPs->GetBufferPointer(), depthPs->GetBufferSize(), nullptr, &groundDepthPixelShader_);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = device_->CreateInputLayout(inputElements, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &groundInputLayout_);
	ES_SAFE_RELEASE(vs);
	ES_SAFE_RELEASE(ps);
	ES_SAFE_RELEASE(depthPs);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC vbDesc{};
	vbDesc.ByteWidth = sizeof(GroundPlaneVertex) * 4;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hr = device_->CreateBuffer(&vbDesc, nullptr, &groundVertexBuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	const auto indices = CreateGroundPlaneIndices();
	D3D11_BUFFER_DESC ibDesc{};
	ibDesc.ByteWidth = sizeof(indices);
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA ibData{};
	ibData.pSysMem = indices.data();
	hr = device_->CreateBuffer(&ibDesc, &ibData, &groundIndexBuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthColorDesc{};
	depthColorDesc.Width = initParam_.WindowSize[0];
	depthColorDesc.Height = initParam_.WindowSize[1];
	depthColorDesc.MipLevels = 1;
	depthColorDesc.ArraySize = 1;
	depthColorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthColorDesc.SampleDesc.Count = 1;
	depthColorDesc.Usage = D3D11_USAGE_DEFAULT;
	depthColorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	hr = device_->CreateTexture2D(&depthColorDesc, nullptr, &groundDepthTexture_);
	if (FAILED(hr))
	{
		depthColorDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		hr = device_->CreateTexture2D(&depthColorDesc, nullptr, &groundDepthTexture_);
	}
	if (FAILED(hr))
	{
		return false;
	}

	hr = device_->CreateRenderTargetView(groundDepthTexture_, nullptr, &groundDepthRenderTargetView_);
	if (FAILED(hr))
	{
		return false;
	}
	hr = device_->CreateShaderResourceView(groundDepthTexture_, nullptr, &groundDepthShaderResourceView_);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = initParam_.WindowSize[0];
	depthDesc.Height = initParam_.WindowSize[1];
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = device_->CreateTexture2D(&depthDesc, nullptr, &groundDepthBuffer_);
	if (FAILED(hr))
	{
		return false;
	}
	hr = device_->CreateDepthStencilView(groundDepthBuffer_, nullptr, &groundDepthStencilView_);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	hr = device_->CreateRasterizerState(&rasterizerDesc, &groundRasterizerState_);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device_->CreateDepthStencilState(&depthStencilDesc, &groundDepthStencilState_);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device_->CreateBlendState(&blendDesc, &groundBlendState_);
	return SUCCEEDED(hr);
}

void EffectPlatformDX11::ReleaseGroundResources()
{
	groundDepthTextureForEffekseer_.Reset();
	ES_SAFE_RELEASE(groundBlendState_);
	ES_SAFE_RELEASE(groundDepthStencilState_);
	ES_SAFE_RELEASE(groundRasterizerState_);
	ES_SAFE_RELEASE(groundIndexBuffer_);
	ES_SAFE_RELEASE(groundVertexBuffer_);
	ES_SAFE_RELEASE(groundInputLayout_);
	ES_SAFE_RELEASE(groundDepthPixelShader_);
	ES_SAFE_RELEASE(groundPixelShader_);
	ES_SAFE_RELEASE(groundVertexShader_);
	ES_SAFE_RELEASE(groundDepthStencilView_);
	ES_SAFE_RELEASE(groundDepthBuffer_);
	ES_SAFE_RELEASE(groundDepthShaderResourceView_);
	ES_SAFE_RELEASE(groundDepthRenderTargetView_);
	ES_SAFE_RELEASE(groundDepthTexture_);
	usesGpuGroundDepth_ = false;
}

void EffectPlatformDX11::UpdateGroundVertexBuffer(ID3D11DeviceContext* context)
{
	const auto vertices = CreateGroundPlaneVertices();
	context->UpdateSubresource(groundVertexBuffer_, 0, nullptr, vertices.data(), 0, 0);
}

void EffectPlatformDX11::DrawGround(ID3D11DeviceContext* context, Effekseer::ToolRuntime::GroundRenderPass pass)
{
	if (groundVertexBuffer_ == nullptr || groundIndexBuffer_ == nullptr)
	{
		return;
	}

	const UINT stride = sizeof(GroundPlaneVertex);
	const UINT offset = 0;
	context->IASetInputLayout(groundInputLayout_);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, &groundVertexBuffer_, &stride, &offset);
	context->IASetIndexBuffer(groundIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);
	context->VSSetShader(groundVertexShader_, nullptr, 0);
	context->PSSetShader(pass == Effekseer::ToolRuntime::GroundRenderPass::Depth ? groundDepthPixelShader_ : groundPixelShader_, nullptr, 0);
	context->RSSetState(groundRasterizerState_);
	context->OMSetDepthStencilState(groundDepthStencilState_, 0);
	const float blendFactor[4] = {};
	context->OMSetBlendState(groundBlendState_, blendFactor, 0xffffffff);
	context->DrawIndexed(6, 0, 0);
}

EffekseerRenderer::RendererRef EffectPlatformDX11::CreateRenderer()
{
	ID3D11DeviceContext* context = nullptr;

	if (isDefferedContextMode_)
	{
		context = defferedContext_;
	}
	else
	{
		context = context_;
	}

	auto ret = EffekseerRendererDX11::Renderer::Create(device_, context, initParam_.SpriteCount);

	ret->SetDistortingCallback(new DistortingCallbackDX11(device_, context));

	return ret;
}

EffectPlatformDX11::~EffectPlatformDX11()
{
	ReleaseGroundResources();
	ES_SAFE_RELEASE(checkedTexture_);
	ES_SAFE_RELEASE(renderTargetView_);
	ES_SAFE_RELEASE(backBuffer_);
	ES_SAFE_RELEASE(depthStencilView_);
	ES_SAFE_RELEASE(depthBuffer_);
	ES_SAFE_RELEASE(swapChain_);
	ES_SAFE_RELEASE(dxgiFactory_);
	ES_SAFE_RELEASE(adapter_);
	ES_SAFE_RELEASE(dxgiDevice_);
	ES_SAFE_RELEASE(context_);
	ES_SAFE_RELEASE(defferedContext_);
	ES_SAFE_RELEASE(device_);
}

void EffectPlatformDX11::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	UINT debugFlag = 0;
#if _DEBUG
	debugFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, debugFlag, NULL, 0, D3D11_SDK_VERSION, &device_, NULL, &context_);

	if (FAILED(hr))
	{
		throw "Failed : D3D11CreateDevice";
	}

	if (FAILED(device_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice_)))
	{
		throw "Failed : QueryInterface";
	}

	if (FAILED(dxgiDevice_->GetAdapter(&adapter_)))
	{
		throw "Failed : GetAdapter";
	}

	adapter_->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory_);
	if (dxgiFactory_ == NULL)
	{
		throw "Failed : GetParent";
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = initParam_.WindowSize[0];
	hDXGISwapChainDesc.BufferDesc.Height = initParam_.WindowSize[1];
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)GetNativePtr(0);
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(dxgiFactory_->CreateSwapChain(device_, &hDXGISwapChainDesc, &swapChain_)))
	{
		throw "Failed : CreateSwapChain";
	}

	if (FAILED(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer_)))
	{
		throw "Failed : GetBuffer";
	}

	if (FAILED(device_->CreateRenderTargetView(backBuffer_, NULL, &renderTargetView_)))
	{
		throw "Failed : CreateRenderTargetView";
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	if (FAILED(device_->CreateTexture2D(&hTexture2dDesc, NULL, &depthBuffer_)))
	{
		throw "Failed : CreateRenderTargetView";
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(device_->CreateDepthStencilView(depthBuffer_, &hDepthStencilViewDesc, &depthStencilView_)))
	{
		throw "Failed : CreateDepthStencilView";
	}

	context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)initParam_.WindowSize[0];
	vp.Height = (float)initParam_.WindowSize[1];
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context_->RSSetViewports(1, &vp);

	CreateCheckedTexture();

	if (FAILED(device_->CreateDeferredContext(0, &defferedContext_)))
	{
		throw "Failed : CreateDeferredContext";
	}
}

void EffectPlatformDX11::BeginRendering()
{
	ID3D11DeviceContext* context = nullptr;

	if (isDefferedContextMode_)
	{
		GetRenderer().DownCast<EffekseerRendererDX11::Renderer>()->ResetStateForDefferedContext();

		context = defferedContext_;

		context->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)initParam_.WindowSize[0];
		vp.Height = (float)initParam_.WindowSize[1];
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		context->RSSetViewports(1, &vp);
	}
	else
	{
		context = context_;
	}

	if (usesGpuGroundDepth_)
	{
		UpdateGroundVertexBuffer(context);

		ID3D11ShaderResourceView* nullSRVs[8] = {};
		context->PSSetShaderResources(0, 8, nullSRVs);

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)initParam_.WindowSize[0];
		vp.Height = (float)initParam_.WindowSize[1];
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		context->OMSetRenderTargets(1, &groundDepthRenderTargetView_, groundDepthStencilView_);
		context->RSSetViewports(1, &vp);
		float DepthClearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		context->ClearRenderTargetView(groundDepthRenderTargetView_, DepthClearColor);
		context->ClearDepthStencilView(groundDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		DrawGround(context, Effekseer::ToolRuntime::GroundRenderPass::Depth);

		context->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
		context->RSSetViewports(1, &vp);
		float ClearColor[] = {22.0f / 255.0f, 34.0f / 255.0f, 48.0f / 255.0f, 1.0f};
		context->ClearRenderTargetView(renderTargetView_, ClearColor);
		context->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		DrawGround(context, Effekseer::ToolRuntime::GroundRenderPass::Color);
	}
	else
	{
		float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
		context->ClearRenderTargetView(renderTargetView_, ClearColor);
		context->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		context->CopyResource(backBuffer_, checkedTexture_);
	}
}

void EffectPlatformDX11::EndRendering()
{
	if (isDefferedContextMode_)
	{
		ID3D11CommandList* cl = nullptr;
		defferedContext_->FinishCommandList(false, &cl);
		context_->ExecuteCommandList(cl, false);
		cl->Release();
	}
}

void EffectPlatformDX11::Present()
{
	swapChain_->Present(1, 0);
}

bool EffectPlatformDX11::TakeScreenshot(const char* path)
{

	ID3D11Texture2D* cpuTexture = nullptr;

	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = initParam_.WindowSize[0];
	desc.Height = initParam_.WindowSize[1];
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	hr = device_->CreateTexture2D(&desc, 0, &cpuTexture);
	if (FAILED(hr))
	{
		throw "Failed : CreateTexture";
	}

	context_->CopyResource(cpuTexture, backBuffer_);

	D3D11_MAPPED_SUBRESOURCE mr;
	UINT sr = D3D11CalcSubresource(0, 0, 0);
	hr = context_->Map(cpuTexture, sr, D3D11_MAP_READ_WRITE, 0, &mr);

	std::vector<uint8_t> data;

	data.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);

	for (int32_t h = 0; h < initParam_.WindowSize[1]; h++)
	{
		auto dst_ = &(data[h * initParam_.WindowSize[0] * 4]);
		auto src_ = &(((uint8_t*)mr.pData)[h * mr.RowPitch]);
		memcpy(dst_, src_, initParam_.WindowSize[0] * 4);
	}

	context_->Unmap(cpuTexture, sr);

	cpuTexture->Release();

	// HACK for intel
	for (int32_t i = 0; i < initParam_.WindowSize[0] * initParam_.WindowSize[1]; i++)
	{
		data[i * 4 + 3] = 255;
	}

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
}

void EffectPlatformDX11::ResetBackgroundPattern()
{
	usesGpuGroundDepth_ = false;
	EffectPlatform::ResetBackgroundPattern();
}

void EffectPlatformDX11::GenerateGroundDepth()
{
	isGroundDepthEnabled_ = true;
	usesGpuGroundDepth_ = false;

	if (!CreateGroundResources())
	{
		ReleaseGroundResources();
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		groundDepthTextureForEffekseer_ =
			EffekseerRendererDX11::CreateTexture(GetRenderer()->GetGraphicsDevice(), groundDepthShaderResourceView_, nullptr, nullptr);
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		ReleaseGroundResources();
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	usesGpuGroundDepth_ = true;
	GetRenderer()->SetDepth(groundDepthTextureForEffekseer_, CreateGroundDepthReconstructionParameter());
}
