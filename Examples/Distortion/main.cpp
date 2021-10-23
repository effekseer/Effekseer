
#include <stdio.h>
#include <string>
#include <windows.h>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <Effekseer.h>
#include <EffekseerRendererDX9.h>

static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::ManagerRef g_manager;
static ::EffekseerRendererDX9::RendererRef g_renderer;
static ::Effekseer::Vector3D g_position;
static ::Effekseer::EffectRef g_effect;
static ::Effekseer::Handle g_handle = -1;

static LPDIRECT3D9 g_d3d = NULL;
static LPDIRECT3DDEVICE9 g_d3d_device = NULL;

static int32_t g_timer = 0;

static IDirect3DSurface9* g_backgroundSurface = nullptr;
static IDirect3DTexture9* g_backgroundTexture = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

bool CopyRenderTargetToBackground()
{
	bool ret = false;

	HRESULT hr;
	IDirect3DSurface9* tempRender = nullptr;
	IDirect3DSurface9* tempDepth = nullptr;

	hr = g_d3d_device->GetRenderTarget(0, &tempRender);
	if (FAILED(hr))
	{
		goto Exit;
	}

	hr = g_d3d_device->GetDepthStencilSurface(&tempDepth);
	if (FAILED(hr))
	{
		goto Exit;
	}

	g_d3d_device->SetRenderTarget(0, g_backgroundSurface);
	g_d3d_device->SetDepthStencilSurface(nullptr);

	D3DSURFACE_DESC desc;
	tempRender->GetDesc(&desc);

	g_d3d_device->StretchRect(tempRender, nullptr, g_backgroundSurface, nullptr, D3DTEXF_POINT);

	g_d3d_device->SetRenderTarget(0, tempRender);
	g_d3d_device->SetDepthStencilSurface(tempDepth);

	ret = true;

Exit:;
	ES_SAFE_RELEASE(tempRender);
	ES_SAFE_RELEASE(tempDepth);

	return ret;
}

class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
public:
	DistortingCallback() {}

	virtual ~DistortingCallback() {}

	bool OnDistorting()
	{
		if (g_backgroundTexture == NULL)
		{
			g_renderer->SetBackground(NULL);
			return false;
		}

		CopyRenderTargetToBackground();
		g_renderer->SetBackground(g_backgroundTexture);
		return true;
	}
};

void InitWindow()
{
	WNDCLASS wndClass;
	wchar_t szClassNme[] = L"RuntimeSample";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(0);
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szClassNme;
	RegisterClass(&wndClass);
	g_window_handle = CreateWindow(szClassNme,
								   L"RuntimeSample",
								   WS_SYSMENU,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   g_window_width,
								   g_window_height,
								   NULL,
								   NULL,
								   GetModuleHandle(0),
								   NULL);
	ShowWindow(g_window_handle, true);
	UpdateWindow(g_window_handle);

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = g_window_width;
	d3dp.BackBufferHeight = g_window_height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = g_window_handle;
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_window_handle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &g_d3d_device);
}

void MainLoop()
{
	for (;;)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return;
			}
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_manager->Update();

			g_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			g_d3d_device->BeginScene();

			g_renderer->BeginRendering();

			// Render rear effects
			// 背面のエフェクトの描画を行う。
			g_manager->DrawBack();

			g_renderer->EndRendering();

			// Distort background and rear effects.
			// 背景と背面のエフェクトを歪ませる。
			DistortingCallback distoring;
			distoring.OnDistorting();

			g_renderer->BeginRendering();

			// Render front effects
			// 前面のエフェクトの描画を行う。
			g_manager->DrawFront();

			g_renderer->EndRendering();

			g_d3d_device->EndScene();

			g_timer++;

			{
				HRESULT hr;
				hr = g_d3d_device->Present(NULL, NULL, NULL, NULL);

				switch (hr)
				{
				case D3DERR_DEVICELOST:
					while (FAILED(hr = g_d3d_device->TestCooperativeLevel()))
					{
						switch (hr)
						{
						case D3DERR_DEVICELOST:
							::SleepEx(1000, true);
							break;

						case D3DERR_DEVICENOTRESET:

							g_renderer->OnLostDevice();

							D3DPRESENT_PARAMETERS d3dp;
							ZeroMemory(&d3dp, sizeof(d3dp));
							d3dp.BackBufferWidth = g_window_width;
							d3dp.BackBufferHeight = g_window_height;
							d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
							d3dp.BackBufferCount = 1;
							d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
							d3dp.Windowed = TRUE;
							d3dp.hDeviceWindow = g_window_handle;
							d3dp.EnableAutoDepthStencil = TRUE;
							d3dp.AutoDepthStencilFormat = D3DFMT_D16;

							g_d3d_device->Reset(&d3dp);

							g_renderer->OnResetDevice();

							break;
						}
					}
					break;
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	InitWindow();

	g_renderer = ::EffekseerRendererDX9::Renderer::Create(g_d3d_device, 2000);

	// Specify a distortion function
	// 歪み機能を設定

	// If you'd like to distort background and particles by rendering, it need to specify it.
	// It is a bit heavy
	// もし、描画するごとに背景とパーティクルを歪ませたい場合、設定する必要がある
	// やや重い
	// g_renderer->SetDistortingCallback(new DistortingCallback());

	// Create a background buffer
	g_d3d_device->CreateTexture(640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_backgroundTexture, NULL);

	g_backgroundTexture->GetSurfaceLevel(0, &g_backgroundSurface);

	g_manager = ::Effekseer::Manager::Create(2000);

	g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

	g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
	g_manager->SetModelLoader(g_renderer->CreateModelLoader());

	g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f) * 0.25;

	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f));

	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	g_effect = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_ASSETS_DIR_U16 u"distortion.efk");

	g_handle = g_manager->Play(g_effect, 0, 0, 0);

	MainLoop();

	g_manager->StopEffect(g_handle);

	g_manager.Reset();

	g_renderer.Reset();

	ES_SAFE_RELEASE(g_backgroundTexture);
	ES_SAFE_RELEASE(g_backgroundSurface);

	ES_SAFE_RELEASE(g_d3d_device);
	ES_SAFE_RELEASE(g_d3d);

	return 0;
}
