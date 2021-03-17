
#ifdef _WIN32
#include <windows.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")

#include <Wingdi.h>

#endif

#include <assert.h>
#include <stdio.h>
#include <string>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>

#include "picojson.h"

typedef int(APIENTRY* PFNWGLSWAPINTERVALEXTPROC)(int);

static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::ManagerRef g_manager;
static ::EffekseerRenderer::RendererRef g_renderer = NULL;
static ::Effekseer::EffectRef g_effect;
static ::Effekseer::Handle g_handle = -1;
static ::Effekseer::Vector3D g_position;

static HDC g_hDC = NULL;
static HGLRC g_hGLRC = NULL;

static void WaitFrame()
{
	static DWORD beforeTime = timeGetTime();
	DWORD currentTime = timeGetTime();

	DWORD elapsedTime = currentTime - beforeTime;
	if (elapsedTime < 16)
	{
		Sleep(16 - elapsedTime);
	}
	beforeTime = timeGetTime();
}

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

	// Initialize COM
	// COMを初期化する
	CoInitializeEx(NULL, NULL);

	// Initialize OpenGL
	// OpenGLの初期化する
	g_hDC = GetDC(g_window_handle);

	PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
								 1,
								 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
								 PFD_TYPE_RGBA,
								 24,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 0,
								 32,
								 0,
								 0,
								 PFD_MAIN_PLANE,
								 0,
								 0,
								 0,
								 0};

	int pxfm = ChoosePixelFormat(g_hDC, &pfd);

	SetPixelFormat(g_hDC, pxfm, &pfd);

	g_hGLRC = wglCreateContext(g_hDC);

	wglMakeCurrent(g_hDC, g_hGLRC);

	auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);

	glViewport(0, 0, g_window_width, g_window_height);
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
			// Move an effect
			// エフェクトを移動させる
			g_manager->AddLocation(g_handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

			// Update effects
			// エフェクトを更新する
			g_manager->Update();

			wglMakeCurrent(g_hDC, g_hGLRC);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Begin to draw effects
			// エフェクトの描画を開始する
			g_renderer->BeginRendering();

			// Draw effects
			// エフェクトを描画する
			g_manager->Draw();

			// Finish to draw effects
			// エフェクトの描画を終了する
			g_renderer->EndRendering();

			glFlush();
			wglMakeCurrent(0, 0);
			WaitFrame();
			SwapBuffers(g_hDC);
		}
	}
}

/**
	a loader class to load effects from glb(textures are only supported)
	glbからエフェクトを読み込むローダークラス(テクスチャのみサポート)
*/
class glmEffectFactory : public Effekseer::EffectFactory
{
private:
public:
	bool OnCheckIsBinarySupported(const void* data, int32_t size) override
	{
		int head = 0;
		memcpy(&head, data, sizeof(int));
		if (memcmp(&head, "glTF", 4) != 0)
			return false;
		return true;
	}

	bool OnCheckIsReloadSupported() override { return false; }

	struct BufferView
	{
		int buffer = 0;
		int byteLength = 0;
		int byteOffset = 0;
	};

	static BufferView CreateBufferView(picojson::object& o)
	{
		BufferView bv;
		bv.buffer = o["buffer"].get<double>();
		bv.byteOffset = o["byteOffset"].get<double>();
		bv.byteLength = o["byteLength"].get<double>();
		return bv;
	}

	struct glbData
	{
		uint8_t* bin = nullptr;
		BufferView body;
		std::vector<BufferView> images;
		std::vector<BufferView> normalImages;
		std::vector<BufferView> distortionImages;

		bool Load(const void* data, int32_t size)
		{
			uint8_t* p = static_cast<uint8_t*>(const_cast<void*>(data));
			char ascii[4];
			memcpy(ascii, p, sizeof(int));
			p += sizeof(int);

			int version = 0;
			memcpy(&version, p, sizeof(int));
			p += sizeof(int);

			int filesize = 0;
			memcpy(&filesize, p, sizeof(int));
			p += sizeof(int);

			filesize -= 12;

			uint8_t* p_json = nullptr;
			uint8_t* p_bin = nullptr;

			while (filesize > 0)
			{
				int chunksize = 0;
				memcpy(&chunksize, p, sizeof(int));
				p += sizeof(int);
				filesize -= sizeof(int);

				char chunktype[4];
				memcpy(&chunktype, p, sizeof(int));
				p += sizeof(int);
				filesize -= sizeof(int);

				if (memcmp(chunktype, "JSON", 4) == 0)
				{
					p_json = p;
				}

				if (memcmp(chunktype, "BIN", 4) == 0)
				{
					p_bin = p;
				}

				p += chunksize;
				filesize -= chunksize;
			}

			std::string json_str = reinterpret_cast<char*>(p_json);

			picojson::value json;
			const std::string err = picojson::parse(json, json_str);
			if (!err.empty())
			{
				std::cerr << err << std::endl;
				return false;
			}

			bin = p_bin;

			picojson::object& root = json.get<picojson::object>();
			auto& extensions = root["extensions"].get<picojson::object>();
			auto& buffers = root["buffers"].get<picojson::array>();
			auto& bufferViews = root["bufferViews"].get<picojson::object>();

			auto& effekseer = extensions["Effekseer"].get<picojson::object>();
			auto& effects = effekseer["effects"].get<picojson::array>();
			auto& effect1 = effects.begin()->get<picojson::object>();
			auto& body_bufferview_name = effect1["body"].get<picojson::object>()["bufferview"].get<std::string>();
			auto& images_bufferviews = effect1["images"].get<picojson::array>();
			auto& normalImages_bufferviews = effect1["normalImages"].get<picojson::array>();
			auto& distortionImages_bufferviews = effect1["distortionImages"].get<picojson::array>();

			body = CreateBufferView(bufferViews[body_bufferview_name].get<picojson::object>());

			for (auto& image : images_bufferviews)
			{
				auto bufferview_name = image.get<picojson::object>()["bufferview"].get<std::string>();
				images.push_back(CreateBufferView(bufferViews[bufferview_name].get<picojson::object>()));
			}

			for (auto& image : normalImages_bufferviews)
			{
				auto bufferview_name = image.get<picojson::object>()["bufferview"].get<std::string>();
				normalImages.push_back(CreateBufferView(bufferViews[bufferview_name].get<picojson::object>()));
			}

			for (auto& image : distortionImages_bufferviews)
			{
				auto bufferview_name = image.get<picojson::object>()["bufferview"].get<std::string>();
				distortionImages.push_back(CreateBufferView(bufferViews[bufferview_name].get<picojson::object>()));
			}

			return true;
		}
	};

	bool OnLoading(Effekseer::Effect* effect, const void* data, int32_t size, float magnification, const EFK_CHAR* materialPath) override
	{
		glbData glbData_;
		if (!glbData_.Load(data, size))
			return false;

		return LoadBody(effect, glbData_.bin + glbData_.body.byteOffset, glbData_.body.byteLength, magnification, materialPath);
	}

	void OnLoadingResource(Effekseer::Effect* effect, const void* data, int32_t size, const EFK_CHAR* materialPath)
	{
		glbData glbData_;
		if (!glbData_.Load(data, size))
			return;
		
		auto textureLoader = effect->GetSetting()->GetTextureLoader();
		auto soundLoader = effect->GetSetting()->GetSoundLoader();
		auto modelLoader = effect->GetSetting()->GetModelLoader();

		if (textureLoader != nullptr)
		{
			for (auto i = 0; i < effect->GetColorImageCount(); i++)
			{
				char path[260];
				Effekseer::ConvertUtf16ToUtf8(path, 260, effect->GetColorImagePath(i));
				auto buf = glbData_.images[i];
				auto resource = textureLoader->Load((const void*)(glbData_.bin + buf.byteOffset), buf.byteLength, Effekseer::TextureType::Color, true);
				SetTexture(effect, i, Effekseer::TextureType::Color, resource);
			}

			for (auto i = 0; i < effect->GetNormalImageCount(); i++)
			{
				char path[260];
				Effekseer::ConvertUtf16ToUtf8(path, 260, effect->GetColorImagePath(i));
				auto buf = glbData_.normalImages[i];
				auto resource =
					textureLoader->Load((const void*)(glbData_.bin + buf.byteOffset), buf.byteLength, Effekseer::TextureType::Normal, true);
				SetTexture(effect, i, Effekseer::TextureType::Normal, resource);
			}

			for (auto i = 0; i < effect->GetDistortionImageCount(); i++)
			{
				char path[260];
				Effekseer::ConvertUtf16ToUtf8(path, 260, effect->GetColorImagePath(i));
				auto buf = glbData_.distortionImages[i];
				auto resource = textureLoader->Load(
					(const void*)(glbData_.bin + buf.byteOffset), buf.byteLength, Effekseer::TextureType::Distortion, true);
				SetTexture(effect, i, Effekseer::TextureType::Distortion, resource);
			}
		}

		if (soundLoader != nullptr)
		{
			// Need to implement
		}

		if (modelLoader != nullptr)
		{
			// Need to implement
		}
	}

	void OnUnloadingResource(Effekseer::Effect* effect)
	{
		auto textureLoader = effect->GetSetting()->GetTextureLoader();
		auto soundLoader = effect->GetSetting()->GetSoundLoader();
		auto modelLoader = effect->GetSetting()->GetModelLoader();

		if (textureLoader != nullptr)
		{
			for (auto i = 0; i < effect->GetColorImageCount(); i++)
			{
				textureLoader->Unload(effect->GetColorImage(i));
				SetTexture(effect, i, Effekseer::TextureType::Color, nullptr);
			}

			for (auto i = 0; i < effect->GetNormalImageCount(); i++)
			{
				textureLoader->Unload(effect->GetNormalImage(i));
				SetTexture(effect, i, Effekseer::TextureType::Normal, nullptr);
			}

			for (auto i = 0; i < effect->GetDistortionImageCount(); i++)
			{
				textureLoader->Unload(effect->GetDistortionImage(i));
				SetTexture(effect, i, Effekseer::TextureType::Distortion, nullptr);
			}
		}

		if (soundLoader != nullptr)
		{
			for (auto i = 0; i < effect->GetWaveCount(); i++)
			{
				soundLoader->Unload(effect->GetWave(i));
				SetSound(effect, i, nullptr);
			}
		}

		if (modelLoader != nullptr)
		{
			for (auto i = 0; i < effect->GetModelCount(); i++)
			{
				modelLoader->Unload(effect->GetModel(i));
				SetModel(effect, i, nullptr);
			}
		}
	}
};

#if _WIN32
#include <Windows.h>
std::wstring ToWide(const char* pText);
void GetDirectoryName(char* dst, char* src);
#endif

int main(int argc, char** argv)
{
#if _WIN32
	char current_path[MAX_PATH + 1];
	GetDirectoryName(current_path, argv[0]);
	SetCurrentDirectoryA(current_path);
#endif

	InitWindow();

	// Create an instance for drawing
	// 描画用インスタンスを生成する
	g_renderer = ::EffekseerRendererGL::Renderer::Create(2000);

	// Create an instance for managing effects
	// エフェクト管理用インスタンスを生成する
	g_manager = ::Effekseer::Manager::Create(2000);

	// Extend a format which can be loaded
	// 読み込めるフォーマットを拡張する
	g_manager->GetSetting()->AddEffectFactory(Effekseer::MakeRefPtr<glmEffectFactory>());

	// Specify functions to draw from the instance
	// 描画用インスタンスから描画機能を設定する
	g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

	// Specify methods to load textures from the instance
	// You can extend this methods. it loads from only files in this time.
	// 描画用インスタンスからテクスチャの読込機能を設定する
	// この機能は拡張できる。現在はファイルのみから読み込んでいる。
	g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
	g_manager->SetModelLoader(g_renderer->CreateModelLoader());

	// Specify a position of eye
	// 視点位置を設定する
	g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// Specify a projection matrix
	// 投影行列を設定する
	g_renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(
		90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f));

	// Specify a camera matrix
	// カメラ行列を設定する
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// Load an effect
	// エフェクトを読み込む
	g_effect = Effekseer::Effect::Create(g_manager, u"laser.glb");

	// Playback an effect
	// エフェクトを再生する
	g_handle = g_manager->Play(g_effect, 0, 0, 0);

	MainLoop();

	// Stop to playback the effect
	// エフェクトの再生を停止する
	g_manager->StopEffect(g_handle);

	// To finalize OpenGL, make context enabled
	// OpenGLを終了するために、コンテキストを有効にする
	wglMakeCurrent(g_hDC, g_hGLRC);

	// First dispose the instance for managing effects
	// 先にエフェクト管理用インスタンスを破棄する
	g_manager.Reset();

	// Next dispose the instance for drawing effects
	// 次に描画用インスタンスを破棄する
	g_renderer.Reset();

	// Finish OpenGL
	// OpenGLを終了させる
	wglMakeCurrent(0, 0);
	wglDeleteContext(g_hGLRC);
	timeEndPeriod(1);

	// COMを終了させる
	// Finish COM
	CoUninitialize();

	return 0;
}

#if _WIN32
static std::wstring ToWide(const char* pText)
{
	int Len = ::MultiByteToWideChar(CP_ACP, 0, pText, -1, NULL, 0);

	wchar_t* pOut = new wchar_t[Len + 1];
	::MultiByteToWideChar(CP_ACP, 0, pText, -1, pOut, Len);
	std::wstring Out(pOut);
	delete[] pOut;

	return Out;
}

void GetDirectoryName(char* dst, char* src)
{
	auto Src = std::string(src);
	int pos = 0;
	int last = 0;
	while (Src.c_str()[pos] != 0)
	{
		dst[pos] = Src.c_str()[pos];

		if (Src.c_str()[pos] == L'\\' || Src.c_str()[pos] == L'/')
		{
			last = pos;
		}

		pos++;
	}

	dst[pos] = 0;
	dst[last] = 0;
}
#endif
