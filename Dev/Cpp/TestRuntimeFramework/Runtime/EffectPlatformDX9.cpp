#include "EffectPlatformDX9.h"
#include "../../EffekseerRendererDX9/EffekseerRendererDX9/GraphicsDevice.h"
#include "../../3rdParty/stb/stb_image_write.h"
#include <assert.h>
#include <d3dcompiler.h>
#include <cstring>

#pragma comment(lib, "d3dcompiler.lib")

namespace
{

const auto ground_vs_dx9 = R"(
struct VS_INPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION0;
	float2 WorldXZ : TEXCOORD0;
	float4 Clip : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.Position = input.Position;
	output.WorldXZ = input.WorldXZ;
	output.Clip = input.Position;
	return output;
}
)";

const auto ground_ps_dx9 = R"(
struct PS_INPUT
{
	float2 WorldXZ : TEXCOORD0;
};

float4 main(PS_INPUT input) : COLOR0
{
	float checker = frac((floor(input.WorldXZ.x) + floor(input.WorldXZ.y)) * 0.5);
	float3 darkColor = float3(0.24, 0.32, 0.27);
	float3 brightColor = float3(0.39, 0.50, 0.42);
	float3 color = lerp(darkColor, brightColor, checker >= 0.5 ? 1.0 : 0.0);
	float distanceFade = saturate(length(input.WorldXZ) * 0.025);
	color *= 1.0 - distanceFade * 0.35;
	return float4(color, 1.0);
}
)";

const auto ground_depth_ps_dx9 = R"(
struct PS_INPUT
{
	float4 Clip : TEXCOORD1;
};

float4 main(PS_INPUT input) : COLOR0
{
	return float4(input.Clip.z / input.Clip.w, 1.0, 1.0, 1.0);
}
)";

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

void UnbindTextures(IDirect3DDevice9* device)
{
	for (int32_t i = 0; i < 8; i++)
	{
		device->SetTexture(i, nullptr);
	}
}

} // namespace

DistortingCallbackDX9::DistortingCallbackDX9(::EffekseerRendererDX9::RendererRef renderer,
											 LPDIRECT3DDEVICE9 device,
											 int texWidth,
											 int texHeight)
	: device(device)
	, texWidth_(texWidth)
	, texHeight_(texHeight)
{
	device->CreateTexture(texWidth, texHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
}

DistortingCallbackDX9::~DistortingCallbackDX9()
{
	ES_SAFE_RELEASE(texture);
}

bool DistortingCallbackDX9::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	IDirect3DSurface9* targetSurface = nullptr;
	IDirect3DSurface9* texSurface = nullptr;
	HRESULT hr = S_OK;

	hr = texture->GetSurfaceLevel(0, &texSurface);
	assert(SUCCEEDED(hr));

	hr = device->GetRenderTarget(0, &targetSurface);
	assert(SUCCEEDED(hr));

	hr = device->StretchRect(targetSurface, NULL, texSurface, NULL, D3DTEXF_NONE);
	assert(SUCCEEDED(hr));

	ES_SAFE_RELEASE(texSurface);
	ES_SAFE_RELEASE(targetSurface);

	reinterpret_cast<EffekseerRendererDX9::Renderer*>(renderer)->SetBackground(texture);

	return true;
}

void DistortingCallbackDX9::Lost()
{
	ES_SAFE_RELEASE(texture);
}

void DistortingCallbackDX9::ChangeDevice(LPDIRECT3DDEVICE9 device)
{
	Effekseer::SafeAddRef(device);
	Effekseer::SafeRelease(this->device);
	this->device = device;
}

void DistortingCallbackDX9::Reset()
{
	device->CreateTexture(texWidth_, texHeight_, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
}

void EffectPlatformDX9::CreateCheckedSurface()
{
	device_->CreateOffscreenPlainSurface(initParam_.WindowSize[0], initParam_.WindowSize[1], D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &checkedSurface_, nullptr);
	D3DLOCKED_RECT lockedRect;
	checkedSurface_->LockRect(&lockedRect, nullptr, 0);

	for (int32_t y = 0; y < initParam_.WindowSize[1]; y++)
	{
		auto dst = static_cast<uint8_t*>(lockedRect.pBits) + lockedRect.Pitch * y;
		auto src = reinterpret_cast<const uint8_t*>(checkeredPattern_.data() + initParam_.WindowSize[0] * y);

		for (int32_t x = 0; x < initParam_.WindowSize[0]; x++)
		{
			dst[x * 4 + 0] = src[x * 4 + 2];
			dst[x * 4 + 1] = src[x * 4 + 1];
			dst[x * 4 + 2] = src[x * 4 + 0];
			dst[x * 4 + 3] = src[x * 4 + 3];
		}
	}

	checkedSurface_->UnlockRect();
}

void EffectPlatformDX9::UpdateBackgroundTexture()
{
	ES_SAFE_RELEASE(checkedSurface_);
	CreateCheckedSurface();
}

bool EffectPlatformDX9::CreateGroundResources()
{
	if (groundDepthTexture_ != nullptr)
	{
		return true;
	}

	ID3DBlob* vs = nullptr;
	ID3DBlob* ps = nullptr;
	ID3DBlob* depthPs = nullptr;
	if (!CompileGroundShader(ground_vs_dx9, "vs_3_0", &vs) ||
		!CompileGroundShader(ground_ps_dx9, "ps_3_0", &ps) ||
		!CompileGroundShader(ground_depth_ps_dx9, "ps_3_0", &depthPs))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	auto hr = device_->CreateVertexShader(reinterpret_cast<const DWORD*>(vs->GetBufferPointer()), &groundVertexShader_);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	hr = device_->CreatePixelShader(reinterpret_cast<const DWORD*>(ps->GetBufferPointer()), &groundPixelShader_);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(vs);
		ES_SAFE_RELEASE(ps);
		ES_SAFE_RELEASE(depthPs);
		return false;
	}

	hr = device_->CreatePixelShader(reinterpret_cast<const DWORD*>(depthPs->GetBufferPointer()), &groundDepthPixelShader_);
	ES_SAFE_RELEASE(vs);
	ES_SAFE_RELEASE(ps);
	ES_SAFE_RELEASE(depthPs);
	if (FAILED(hr))
	{
		return false;
	}

	const D3DVERTEXELEMENT9 elements[] = {
		{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, sizeof(float) * 4, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()};
	hr = device_->CreateVertexDeclaration(elements, &groundVertexDeclaration_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device_->CreateTexture(
		initParam_.WindowSize[0],
		initParam_.WindowSize[1],
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A32B32G32R32F,
		D3DPOOL_DEFAULT,
		&groundDepthTexture_,
		nullptr);
	if (FAILED(hr))
	{
		hr = device_->CreateTexture(
			initParam_.WindowSize[0],
			initParam_.WindowSize[1],
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A16B16G16R16F,
			D3DPOOL_DEFAULT,
			&groundDepthTexture_,
			nullptr);
	}
	if (FAILED(hr))
	{
		return false;
	}

	hr = groundDepthTexture_->GetSurfaceLevel(0, &groundDepthSurface_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = device_->CreateDepthStencilSurface(
		initParam_.WindowSize[0],
		initParam_.WindowSize[1],
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&groundDepthStencilSurface_,
		nullptr);
	return SUCCEEDED(hr);
}

void EffectPlatformDX9::ReleaseGroundResources()
{
	groundDepthTextureForEffekseer_.Reset();
	ES_SAFE_RELEASE(groundDepthPixelShader_);
	ES_SAFE_RELEASE(groundPixelShader_);
	ES_SAFE_RELEASE(groundVertexShader_);
	ES_SAFE_RELEASE(groundVertexDeclaration_);
	ES_SAFE_RELEASE(groundDepthStencilSurface_);
	ES_SAFE_RELEASE(groundDepthSurface_);
	ES_SAFE_RELEASE(groundDepthTexture_);
	usesGpuGroundDepth_ = false;
}

void EffectPlatformDX9::DrawGround(bool writesDepthTexture)
{
	const auto vertices = CreateGroundPlaneVertices();
	const auto indices = CreateGroundPlaneIndices();

	device_->SetVertexDeclaration(groundVertexDeclaration_);
	device_->SetVertexShader(groundVertexShader_);
	device_->SetPixelShader(writesDepthTexture ? groundDepthPixelShader_ : groundPixelShader_);
	device_->SetTexture(0, nullptr);
	device_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device_->SetRenderState(D3DRS_ZENABLE, TRUE);
	device_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	device_->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device_->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		4,
		2,
		indices.data(),
		D3DFMT_INDEX16,
		vertices.data(),
		sizeof(GroundPlaneVertex));
}

EffekseerRenderer::RendererRef EffectPlatformDX9::CreateRenderer()
{
	auto ret = EffekseerRendererDX9::Renderer::Create(device_, initParam_.SpriteCount);
	distorting_ = new DistortingCallbackDX9((EffekseerRendererDX9::RendererRef)ret, device_, initParam_.WindowSize[0], initParam_.WindowSize[1]);
	ret->SetDistortingCallback(distorting_);
	return ret;
}

EffectPlatformDX9::~EffectPlatformDX9()
{
	ReleaseGroundResources();
	ES_SAFE_RELEASE(checkedSurface_);
	ES_SAFE_RELEASE(d3d_);
	ES_SAFE_RELEASE(device_);
}

void EffectPlatformDX9::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = initParam_.WindowSize[0];
	d3dp.BackBufferHeight = initParam_.WindowSize[1];
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = (HWND)GetNativePtr(0);
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	vsync_ = param.VSync;
	if (!vsync_)
	{
		d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	d3d_ = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d_ == nullptr)
	{
		throw "Failed : Direct3DCreate9";
	}

	hr =
		d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &device_);

	if (FAILED(hr))
	{
		throw "Failed : CreateDevice";
	}

	CreateCheckedSurface();
}

void EffectPlatformDX9::BeginRendering()
{
	if (usesGpuGroundDepth_)
	{
		LPDIRECT3DSURFACE9 targetSurface = nullptr;
		LPDIRECT3DSURFACE9 depthSurface = nullptr;
		device_->GetRenderTarget(0, &targetSurface);
		device_->GetDepthStencilSurface(&depthSurface);

		device_->BeginScene();

		UnbindTextures(device_);
		device_->SetRenderTarget(0, groundDepthSurface_);
		device_->SetDepthStencilSurface(groundDepthStencilSurface_);
		device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
		DrawGround(true);

		UnbindTextures(device_);
		device_->SetRenderTarget(0, targetSurface);
		device_->SetDepthStencilSurface(depthSurface);
		device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(22, 34, 48), 1.0f, 0);
		DrawGround(false);

		ES_SAFE_RELEASE(depthSurface);
		ES_SAFE_RELEASE(targetSurface);
	}
	else
	{
		device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		LPDIRECT3DSURFACE9 targetSurface = nullptr;
		device_->GetRenderTarget(0, &targetSurface);
		device_->UpdateSurface(checkedSurface_, NULL, targetSurface, NULL);
		ES_SAFE_RELEASE(targetSurface);

		device_->BeginScene();
	}
}

void EffectPlatformDX9::EndRendering()
{
	device_->EndScene();
}

void EffectPlatformDX9::Present()
{
	auto hr = device_->Present(nullptr, nullptr, nullptr, nullptr);

	switch (hr)
	{
		// cause an unknown error
	case D3DERR_DRIVERINTERNALERROR:
		throw "Failed : D3DERR_DRIVERINTERNALERROR";

		// device lost
	case D3DERR_DEVICELOST:
		while (FAILED(hr = device_->TestCooperativeLevel()))
		{
			switch (hr)
			{
				// device lost
			case D3DERR_DEVICELOST:
				::SleepEx(1000, true);
				break;
				// device lost : reset
			case D3DERR_DEVICENOTRESET:
				ResetDevice();
				break;
			}
		}
		break;
	}
}

bool EffectPlatformDX9::TakeScreenshot(const char* path)
{

	IDirect3DSurface9* surface = nullptr;
	device_->CreateOffscreenPlainSurface(initParam_.WindowSize[0], initParam_.WindowSize[1], D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);

	LPDIRECT3DSURFACE9 backBuf;
	device_->GetRenderTarget(0, &backBuf);

	device_->GetRenderTargetData(backBuf, surface);
	backBuf->Release();

	D3DLOCKED_RECT locked;
	RECT rect;
	rect.left = 0;
	rect.bottom = initParam_.WindowSize[1];
	rect.top = 0;
	rect.right = initParam_.WindowSize[0];
	surface->LockRect(&locked, &rect, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);

	std::vector<uint8_t> data;

	data.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);

	for (int32_t h = 0; h < initParam_.WindowSize[1]; h++)
	{
		auto dst_ = &(data[h * initParam_.WindowSize[0] * 4]);
		auto src_ = &(((uint8_t*)locked.pBits)[h * locked.Pitch]);
		memcpy(dst_, src_, initParam_.WindowSize[0] * 4);
	}

	// HACK for Geforce
	for (int32_t i = 0; i < initParam_.WindowSize[0] * initParam_.WindowSize[1]; i++)
	{
		data[i * 4 + 3] = 255;
		std::swap(data[i * 4 + 0], data[i * 4 + 2]);
	}

	surface->UnlockRect();
	surface->Release();

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
}

void EffectPlatformDX9::ResetBackgroundPattern()
{
	usesGpuGroundDepth_ = false;
	EffectPlatform::ResetBackgroundPattern();
}

void EffectPlatformDX9::GenerateGroundDepth()
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
		auto graphicsDevice = GetRenderer()->GetGraphicsDevice().DownCast<EffekseerRendererDX9::Backend::GraphicsDevice>();
		if (graphicsDevice != nullptr)
		{
			groundDepthTextureForEffekseer_ = graphicsDevice->CreateTexture(groundDepthTexture_, nullptr, nullptr);
		}
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

bool EffectPlatformDX9::SetFullscreen(bool isFullscreen)
{
	fullscreen_ = !fullscreen_;
	ResetDevice();
	return true;
}

void EffectPlatformDX9::ResetDevice()
{
	const auto wasGroundDepthEnabled = isGroundDepthEnabled_;
	ES_SAFE_RELEASE(checkedSurface_);
	ReleaseGroundResources();

	distorting_->Lost();

	auto renderer = static_cast<EffekseerRendererDX9::Renderer*>(GetRenderer().Get());

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->UnloadResources();
	}

	renderer->OnLostDevice();

	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = initParam_.WindowSize[0];
	d3dp.BackBufferHeight = initParam_.WindowSize[1];
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = fullscreen_ ? FALSE : TRUE;
	d3dp.hDeviceWindow = (HWND)GetNativePtr(0);
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;
	if (!vsync_)
	{
		d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	bool newDevice = true;

	if (newDevice)
	{
		distorting_->ChangeDevice(nullptr);
		renderer->ChangeDevice(nullptr);
		device_->Release();

		hr =
			d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &device_);

		if (FAILED(hr))
		{
			throw "Failed : CreateDevice";
		}

		renderer->ChangeDevice(device_);
		distorting_->ChangeDevice(device_);
	}
	else
	{
		hr = device_->Reset(&d3dp);

		if (FAILED(hr))
		{
			throw "Failed : ResetDevice";
			return;
		}
	}

	distorting_->Reset();

	renderer->OnResetDevice();

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->ReloadResources(buffers_[i].data(), static_cast<int32_t>(buffers_[i].size()));
	}

	CreateCheckedSurface();
	if (wasGroundDepthEnabled)
	{
		GenerateGroundDepth();
	}
}
