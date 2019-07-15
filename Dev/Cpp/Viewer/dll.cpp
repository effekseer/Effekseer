
#include "Graphics/efk.AVIExporter.h"
#include "Graphics/efk.GifHelper.h"
#include "Graphics/efk.PNGHelper.h"

#ifdef _WIN32
#include "3rdParty/imgui_platform/imgui_impl_dx11.h"
#include "3rdParty/imgui_platform/imgui_impl_dx9.h"
#endif

#include "dll.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define NONDLL 1

#ifdef _WIN32
#define MSWIN32 1
#define BGDWIN32 1
#endif
#include <gd.h>
#include <gdfontmb.h>

#ifndef __EFFEKSEER_VIEWER_BUILD_AS_CMAKE__

#ifdef _WIN32
#if _DEBUG
#pragma comment(lib, "x86/Debug/libgd_static.lib")
#else
#pragma comment(lib, "x86/Release/libgd_static.lib")
#endif
#endif

#ifdef _WIN32

#ifdef _DEBUG
#pragma comment(lib, "x86/Debug/libglew32d.lib")
#else
#pragma comment(lib, "x86/Release/libglew32.lib")
#endif

#endif

#if _DEBUG
#pragma comment(lib, "x86/Debug/glfw3.lib")
#else
#pragma comment(lib, "x86/Release/glfw3.lib")
#endif

#if _DEBUG
#pragma comment(lib, "x86/Debug/OpenSoundMixer.lib")
#else
#pragma comment(lib, "x86/Release/OpenSoundMixer.lib")
#endif

#endif

static float g_RotX = 30.0f;
static float g_RotY = -30.0f;
static Effekseer::Vector3D g_lightDirection = Effekseer::Vector3D(1, 1, 1);
static float g_Distance = 15.0f;
const float PI = 3.14159265f;

static bool g_mouseRotDirectionInvX = false;
static bool g_mouseRotDirectionInvY = false;

static bool g_mouseSlideDirectionInvX = false;
static bool g_mouseSlideDirectionInvY = false;

static int g_lastViewWidth = 0;
static int g_lastViewHeight = 0;

bool Combine(const char16_t* rootPath, const char16_t* treePath, char16_t* dst, int dst_length)
{
	int rootPathLength = 0;
	while (rootPath[rootPathLength] != 0)
	{
		rootPathLength++;
	}

	int treePathLength = 0;
	while (treePath[treePathLength] != 0)
	{
		treePathLength++;
	}

	// both pathes are none
	if (rootPathLength == 0 && treePathLength == 0)
	{
		return false;
	}

	// either path is none
	if (rootPathLength == 0)
	{
		if (treePathLength < dst_length)
		{
			memcpy(dst, treePath, sizeof(char16_t) * (treePathLength + 1));
			return true;
		}
		else
		{
			return false;
		}
	}

	if (treePathLength == 0)
	{
		if (rootPathLength < dst_length)
		{
			memcpy(dst, rootPath, sizeof(char16_t) * (rootPathLength + 1));
			return true;
		}
		else
		{
			return false;
		}
	}

	// both exists

	// back to a directory separator
	int PathPosition = rootPathLength;
	while (PathPosition > 0)
	{
		if (rootPath[PathPosition - 1] == u'/' || rootPath[PathPosition - 1] == u'\\')
		{
			break;
		}
		PathPosition--;
	}

	// copy
	memcpy(dst, rootPath, sizeof(char16_t) * PathPosition);
	dst[PathPosition] = 0;

	// connect forcely
	if (PathPosition + treePathLength > dst_length)
	{
		return false;
	}

	memcpy(&(dst[PathPosition]), treePath, sizeof(char16_t) * treePathLength);
	PathPosition = PathPosition + treePathLength;
	dst[PathPosition] = 0;

	// execute ..\ or ../
	for (int i = 0; i < PathPosition - 2; i++)
	{
		if (dst[i] == L'.' && dst[i + 1] == L'.' && (dst[i + 2] == L'/' || dst[i + 2] == L'\\'))
		{
			int pos = 0;

			if (i > 1 && dst[i - 2] == L'.')
			{
			}
			else
			{
				for (pos = i - 2; pos > 0; pos--)
				{
					if (dst[pos - 1] == L'/' || dst[pos - 1] == L'\\')
					{
						break;
					}
				}

				for (int k = pos; k < PathPosition; k++)
				{
					dst[k] = dst[k + (i + 3) - pos];
				}
				PathPosition = PathPosition - (i + 3 - pos);
				i = pos - 1;
			}
		}
	}
	dst[PathPosition] = 0;
	return true;
}

void GenerateExportedImageWithBlendAndAdd(std::vector<Effekseer::Color>& pixelsBlend,
										  std::vector<Effekseer::Color>& pixelsAdd,
										  std::vector<std::vector<Effekseer::Color>>& pixels)
{
	assert(pixels.size() == 9);

	auto f2b = [](float v) -> uint8_t {
		auto v_ = v * 255;
		if (v_ > 255)
			v_ = 255;
		if (v_ < 0)
			v_ = 0;
		return v_;
	};

	auto b2f = [](uint8_t v) -> float {
		auto v_ = (float)v / 255.0f;
		return v_;
	};

	pixelsAdd.resize(pixels[0].size());
	pixelsBlend.resize(pixels[0].size());

	for (auto i = 0; i < pixels[0].size(); i++)
	{
		float colors[4][9];

		for (auto j = 0; j < 9; j++)
		{
			colors[0][j] = pixels[j][i].R / 255.0;
			colors[1][j] = pixels[j][i].G / 255.0;
			colors[2][j] = pixels[j][i].B / 255.0;
			colors[3][j] = pixels[j][i].A / 255.0;

			colors[0][j] *= colors[3][j];
			colors[1][j] *= colors[3][j];
			colors[2][j] *= colors[3][j];
		}

		std::array<float, 3> gradients;
		gradients.fill(0.0f);

		for (auto c = 0; c < 3; c++)
		{
			bool found = false;

			for (auto j = 0; j < 9; j++)
			{
				if (colors[c][j] >= 1.0f)
				{
					gradients[c] = (colors[c][j] - colors[c][0]) / ((j + 1) / 9.0f);
					found = true;
					break;
				}
			}

			if (!found)
			{
				gradients[c] = (colors[c][8] - colors[c][0]);
			}
		}

		float blendAlpha = (3.0f - (gradients[0] + gradients[1] + gradients[2])) / 3.0f;

		pixelsBlend[i].R = 0.0f;
		pixelsBlend[i].G = 0.0f;
		pixelsBlend[i].B = 0.0f;
		pixelsBlend[i].A = Effekseer::Clamp(blendAlpha * 255.0f, 255.0f, 0.0f);

		pixelsAdd[i].R = Effekseer::Clamp(colors[0][0] * 255.0f, 255.0f, 0.0f);
		pixelsAdd[i].G = Effekseer::Clamp(colors[1][0] * 255.0f, 255.0f, 0.0f);
		pixelsAdd[i].B = Effekseer::Clamp(colors[2][0] * 255.0f, 255.0f, 0.0f);
		pixelsAdd[i].A = 255.0f;
	}
}

ViewerParamater::ViewerParamater()
	: GuideWidth(0)
	, GuideHeight(0)
	, IsPerspective(false)
	, IsOrthographic(false)
	, FocusX(0)
	, FocusY(0)
	, FocusZ(0)
	, AngleX(0)
	, AngleY(0)
	, Distance(0)
	, RendersGuide(false)

	, IsCullingShown(false)
	, CullingRadius(0)
	, CullingX(0)
	, CullingY(0)
	, CullingZ(0)

	, Distortion(DistortionType::Current)
	, RenderingMode(RenderMode::Normal)

{
}

ViewerEffectBehavior::ViewerEffectBehavior()
	: CountX(1)
	, CountY(1)
	, CountZ(1)
	, Distance(5.0f)
	, RemovedTime(0xffff)
	, PositionX(0.0f)
	, PositionY(0.0f)
	, PositionZ(0.0f)
	, RotationX(0.0f)
	, RotationY(0.0f)
	, RotationZ(0.0f)
	, ScaleX(1.0f)
	, ScaleY(1.0f)
	, ScaleZ(1.0f)

	, PositionVelocityX(0.0f)
	, PositionVelocityY(0.0f)
	, PositionVelocityZ(0.0f)
	, RotationVelocityX(0.0f)
	, RotationVelocityY(0.0f)
	, RotationVelocityZ(0.0f)
	, ScaleVelocityX(0.0f)
	, ScaleVelocityY(0.0f)
	, ScaleVelocityZ(0.0f)

{
}

struct HandleHolder
{
	::Effekseer::Handle Handle = 0;
	int32_t Time = 0;
	bool IsRootStopped = false;

	HandleHolder() : Handle(0), Time(0) {}

	HandleHolder(::Effekseer::Handle handle, int32_t time = 0) : Handle(handle), Time(time) {}
};

static ::Effekseer::Manager* g_manager = NULL;
static Effekseer::Manager::DrawParameter drawParameter;

static ::EffekseerTool::Renderer* g_renderer = NULL;
static ::Effekseer::Effect* g_effect = NULL;
static ::EffekseerTool::Sound* g_sound = NULL;
static std::map<std::u16string, Effekseer::TextureData*> m_textures;
static std::map<std::u16string, Effekseer::Model*> m_models;
static std::map<std::u16string, std::shared_ptr<efk::ImageResource>> g_imageResources;

static std::vector<HandleHolder> g_handles;

static ::Effekseer::Vector3D g_focus_position;

static ::Effekseer::Client* g_client = NULL;

static efk::DeviceType g_deviceType = efk::DeviceType::OpenGL;

Native::TextureLoader::TextureLoader(EffekseerRenderer::Renderer* renderer) : m_renderer(renderer)
{
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		auto r = (EffekseerRendererGL::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererGL::CreateTextureLoader();
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto r = (EffekseerRendererDX11::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
	}
	else
	{
		auto r = (EffekseerRendererDX9::Renderer*)m_renderer;
		m_originalTextureLoader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
	}
#endif
}

Native::TextureLoader::~TextureLoader() { ES_SAFE_DELETE(m_originalTextureLoader); }

Effekseer::TextureData* Native::TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, dst, 260);

	std::u16string key(dst);

	if (m_textures.count(key) > 0)
	{
		return m_textures[key];
	}
	else
	{
		auto t = m_originalTextureLoader->Load((EFK_CHAR*)dst, textureType);

		if (t != nullptr)
		{
			m_textures[key] = t;
		}

		return t;
	}

	return nullptr;
}

void Native::TextureLoader::Unload(Effekseer::TextureData* data)
{
	// m_originalTextureLoader->Unload(data);
}

Native::SoundLoader::SoundLoader(Effekseer::SoundLoader* loader) : m_loader(loader) {}

Native::SoundLoader::~SoundLoader() {}

void* Native::SoundLoader::Load(const EFK_CHAR* path)
{
	EFK_CHAR dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, (char16_t*)dst, 260);

	return m_loader->Load(dst);
}

void Native::SoundLoader::Unload(void* handle) { m_loader->Unload(handle); }

Native::ModelLoader::ModelLoader(EffekseerRenderer::Renderer* renderer) : m_renderer(renderer) {}

Native::ModelLoader::~ModelLoader() {}

void* Native::ModelLoader::Load(const EFK_CHAR* path)
{
	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, dst, 260);

	std::u16string key(dst);
	Effekseer::Model* model = NULL;

	if (m_models.count(key) > 0)
	{
		return m_models[key];
	}
	else
	{
		if (g_deviceType == efk::DeviceType::OpenGL)
		{
			auto r = (EffekseerRendererGL::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererGL::CreateModelLoader();
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
#ifdef _WIN32
		else if (g_deviceType == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererDX11::CreateModelLoader(r->GetDevice());
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
		else
		{
			auto r = (EffekseerRendererDX9::Renderer*)m_renderer;
			auto loader = ::EffekseerRendererDX9::CreateModelLoader(r->GetDevice());
			auto m = (Effekseer::Model*)loader->Load((const EFK_CHAR*)dst);

			if (m != nullptr)
			{
				m_models[key] = m;
			}

			ES_SAFE_DELETE(loader);

			return m;
		}
#endif
	}
}

void Native::ModelLoader::Unload(void* data)
{
	/*
	if( data != NULL )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
	*/
}

::Effekseer::Effect* Native::GetEffect() { return g_effect; }

Native::Native() : m_time(0), m_step(1) { g_client = Effekseer::Client::Create(); }

Native::~Native() { ES_SAFE_DELETE(g_client); }

bool Native::CreateWindow_Effekseer(void* pHandle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	m_isSRGBMode = isSRGBMode;
	g_deviceType = deviceType;

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	g_renderer = new ::EffekseerTool::Renderer(spriteCount, isSRGBMode, g_deviceType);
	if (g_renderer->Initialize(pHandle, width, height))
	{
		// 関数追加
		//::Effekseer::ScriptRegister::SetExternalFunction(0, print);

		g_manager = ::Effekseer::Manager::Create(spriteCount);

		{
			::Effekseer::SpriteRenderer* sprite_renderer = g_renderer->GetRenderer()->CreateSpriteRenderer();
			::Effekseer::RibbonRenderer* ribbon_renderer = g_renderer->GetRenderer()->CreateRibbonRenderer();
			::Effekseer::RingRenderer* ring_renderer = g_renderer->GetRenderer()->CreateRingRenderer();
			::Effekseer::ModelRenderer* model_renderer = g_renderer->GetRenderer()->CreateModelRenderer();
			::Effekseer::TrackRenderer* track_renderer = g_renderer->GetRenderer()->CreateTrackRenderer();

			if (sprite_renderer == NULL)
			{
				printf("Failed create Renderer\n");
				g_manager->Destroy();
				g_manager = NULL;
				ES_SAFE_DELETE(g_renderer);
				return false;
			}
			g_manager->SetSpriteRenderer(sprite_renderer);
			g_manager->SetRibbonRenderer(ribbon_renderer);
			g_manager->SetRingRenderer(ring_renderer);
			g_manager->SetModelRenderer(model_renderer);
			g_manager->SetTrackRenderer(track_renderer);

			m_textureLoader = new TextureLoader((EffekseerRenderer::Renderer*)g_renderer->GetRenderer());
			g_manager->SetTextureLoader(m_textureLoader);
			g_manager->SetModelLoader(new ModelLoader((EffekseerRenderer::Renderer*)g_renderer->GetRenderer()));

			// temp
			g_manager->SetMaterialLoader(g_renderer->GetRenderer()->CreateMaterialLoader());
		}

		// Assign device lost events.
		g_renderer->LostedDevice = [this]() -> void {
			this->InvalidateTextureCache();
			auto e = this->GetEffect();
			if (e != nullptr)
			{
				e->UnloadResources();
			}

			{
				Effekseer::TextureLoader* loader = nullptr;
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
					auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererGL::CreateTextureLoader();
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
					auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
				}
				else
				{
					auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
				}
#endif
				for (auto& resource : g_imageResources)
				{
					loader->Unload(resource.second->GetTextureData());
					resource.second->GetTextureData() = nullptr;
				}

				delete loader;
			}

			{
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
				}
				else
				{
					ImGui_ImplDX9_InvalidateDeviceObjects();
				}
#endif
			}
		};

		g_renderer->ResettedDevice = [this]() -> void {
			auto e = this->GetEffect();
			if (e != nullptr)
			{
				e->ReloadResources();
			}

			{
				Effekseer::TextureLoader* loader = nullptr;
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
					auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererGL::CreateTextureLoader();
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
					auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
				}
				else
				{
					auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
					loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
				}
#endif
				for (auto& resource : g_imageResources)
				{
					resource.second->GetTextureData() = loader->Load(resource.second->GetPath(), Effekseer::TextureType::Color);
				}

				delete loader;
			}

			{
				if (g_deviceType == efk::DeviceType::OpenGL)
				{
				}
#ifdef _WIN32
				else if (g_deviceType == efk::DeviceType::DirectX11)
				{
				}
				else
				{
					ImGui_ImplDX9_CreateDeviceObjects();
				}
#endif
			}
		};
	}
	else
	{
		ES_SAFE_DELETE(g_renderer);
		return false;
	}

	g_sound = new ::EffekseerTool::Sound();
	if (g_sound->Initialize())
	{
		g_manager->SetSoundPlayer(g_sound->GetSound()->CreateSoundPlayer());
		g_manager->SetSoundLoader(new SoundLoader(g_sound->GetSound()->CreateSoundLoader()));
	}

	return true;
}

void Native::ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { g_renderer->GetGraphics()->Clear(Effekseer::Color(r, g, b, a)); }

bool Native::UpdateWindow()
{
	assert(g_manager != NULL);
	assert(g_renderer != NULL);

	::Effekseer::Vector3D position(0, 0, g_Distance);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);

	if (g_renderer->IsRightHand)
	{
		mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		Effekseer::Vector3D::Normal(drawParameter.CameraDirection, position);

		position.X += g_focus_position.X;
		position.Y += g_focus_position.Y;
		position.Z += g_focus_position.Z;

		::Effekseer::Matrix44 cameraMat;
		g_renderer->GetRenderer()->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtRH(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		drawParameter.CameraPosition = position;
	}
	else
	{
		mat_rot_y.RotationY((g_RotY + 180.0f) / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		::Effekseer::Vector3D temp_focus = g_focus_position;
		temp_focus.Z = -temp_focus.Z;

		Effekseer::Vector3D::Normal(drawParameter.CameraDirection, position);

		position.X += temp_focus.X;
		position.Y += temp_focus.Y;
		position.Z += temp_focus.Z;

		::Effekseer::Matrix44 cameraMat;
		g_renderer->GetRenderer()->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtLH(position, temp_focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		drawParameter.CameraPosition = position;
	}

	g_sound->SetListener(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
	g_sound->Update();

	return true;
}

void Native::RenderWindow()
{
	g_renderer->BeginRendering();

	if (g_renderer->Distortion == EffekseerTool::eDistortionType::DistortionType_Current)
	{
		g_manager->DrawBack(drawParameter);

		// HACK
		g_renderer->GetRenderer()->EndRendering();

		g_renderer->CopyToBackground();

		// HACK
		g_renderer->GetRenderer()->BeginRendering();
		g_manager->DrawFront(drawParameter);
	}
	else
	{
		g_manager->Draw(drawParameter);
	}

	g_renderer->EndRendering();

	g_renderer->RenderPostEffect();
}

void Native::Present() { g_renderer->Present(); }

bool Native::ResizeWindow(int width, int height)
{
	g_renderer->Resize(width, height);
	return true;
}

bool Native::DestroyWindow()
{
	assert(g_renderer != NULL);
	assert(g_manager != NULL);

	for (size_t i = 0; i < g_handles.size(); i++)
	{
		g_manager->StopEffect(g_handles[i].Handle);
	}
	g_handles.clear();

	InvalidateTextureCache();

	{
		Effekseer::TextureLoader* loader = nullptr;
		if (g_deviceType == efk::DeviceType::OpenGL)
		{
			auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererGL::CreateTextureLoader();
		}
#ifdef _WIN32
		else if (g_deviceType == efk::DeviceType::DirectX11)
		{
			auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
		}
		else
		{
			auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
			loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
		}
#endif
		for (auto& resource : g_imageResources)
		{
			loader->Unload(resource.second->GetTextureData());
		}

		delete loader;
	}

	ES_SAFE_RELEASE(g_effect);

	g_manager->Destroy();
	ES_SAFE_DELETE(g_renderer);

	return true;
}

bool Native::LoadEffect(void* pData, int size, const char16_t* Path)
{
	assert(g_effect == NULL);

	g_handles.clear();

	((TextureLoader*)g_manager->GetTextureLoader())->RootPath = std::u16string(Path);
	((ModelLoader*)g_manager->GetModelLoader())->RootPath = std::u16string(Path);

	SoundLoader* soundLoader = (SoundLoader*)g_manager->GetSoundLoader();
	if (soundLoader)
	{
		soundLoader->RootPath = std::u16string(Path);
	}

	g_effect = Effekseer::Effect::Create(g_manager, pData, size);
	assert(g_effect != NULL);
	return true;
}

bool Native::RemoveEffect()
{
	ES_SAFE_RELEASE(g_effect);
	g_handles.clear();
	return true;
}

bool Native::PlayEffect()
{
	assert(g_effect != NULL);

	for (int32_t z = 0; z < m_effectBehavior.CountZ; z++)
	{
		for (int32_t y = 0; y < m_effectBehavior.CountY; y++)
		{
			for (int32_t x = 0; x < m_effectBehavior.CountX; x++)
			{
				auto lenX = m_effectBehavior.Distance * (m_effectBehavior.CountX - 1);
				auto lenY = m_effectBehavior.Distance * (m_effectBehavior.CountY - 1);
				auto lenZ = m_effectBehavior.Distance * (m_effectBehavior.CountZ - 1);

				auto posX = m_effectBehavior.Distance * x - lenX / 2.0f;
				auto posY = m_effectBehavior.Distance * y - lenY / 2.0f;
				auto posZ = m_effectBehavior.Distance * z - lenZ / 2.0f;

				posX += m_effectBehavior.PositionX;
				posY += m_effectBehavior.PositionY;
				posZ += m_effectBehavior.PositionZ;

				HandleHolder handleHolder(g_manager->Play(g_effect, posX, posY, posZ));

				Effekseer::Matrix43 mat, matTra, matRot, matScale;
				matTra.Translation(posX, posY, posZ);
				matRot.RotationZXY(m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y);
				matScale.Scaling(m_rootScale.X, m_rootScale.Y, m_rootScale.Z);

				mat.Indentity();
				Effekseer::Matrix43::Multiple(mat, mat, matScale);
				Effekseer::Matrix43::Multiple(mat, mat, matRot);
				Effekseer::Matrix43::Multiple(mat, mat, matTra);

				g_manager->SetMatrix(handleHolder.Handle, mat);

				g_handles.push_back(handleHolder);

				if (m_effectBehavior.AllColorR != 255 || m_effectBehavior.AllColorG != 255 || m_effectBehavior.AllColorB != 255 ||
					m_effectBehavior.AllColorA != 255)
				{
					g_manager->SetAllColor(handleHolder.Handle,
										   Effekseer::Color(m_effectBehavior.AllColorR,
															m_effectBehavior.AllColorG,
															m_effectBehavior.AllColorB,
															m_effectBehavior.AllColorA));
				}
			}
		}
	}

	m_time = 0;
	m_rootLocation.X = m_effectBehavior.PositionX;
	m_rootLocation.Y = m_effectBehavior.PositionY;
	m_rootLocation.Z = m_effectBehavior.PositionZ;
	m_rootRotation.X = m_effectBehavior.RotationX;
	m_rootRotation.Y = m_effectBehavior.RotationY;
	m_rootRotation.Z = m_effectBehavior.RotationZ;
	m_rootScale.X = m_effectBehavior.ScaleX;
	m_rootScale.Y = m_effectBehavior.ScaleY;
	m_rootScale.Z = m_effectBehavior.ScaleZ;

	return true;
}

bool Native::StopEffect()
{
	if (g_handles.size() <= 0)
		return false;

	for (size_t i = 0; i < g_handles.size(); i++)
	{
		g_manager->StopEffect(g_handles[i].Handle);
	}
	g_handles.clear();

	g_manager->Update();

	return true;
}

bool Native::StepEffect(int frame)
{
	if (frame <= 0)
	{
	}
	else if (frame == 1)
	{
		StepEffect();
	}
	else
	{
		bool mute = g_sound->GetMute();
		g_sound->SetMute(true);

		for (size_t i = 0; i < g_handles.size(); i++)
		{
			g_manager->SetShown(g_handles[i].Handle, false);
		}

		for (int i = 0; i < frame - 1; i++)
		{
			StepEffect();
		}

		for (size_t i = 0; i < g_handles.size(); i++)
		{
			g_manager->SetShown(g_handles[i].Handle, true);
		}

		g_sound->SetMute(mute);
		g_manager->Update();
		g_renderer->GetRenderer()->SetTime(m_time / 60.0f);
	}
	return true;
}

bool Native::StepEffect()
{
	if (m_effectBehavior.TimeSpan > 0 && m_time > 0 && m_time % m_effectBehavior.TimeSpan == 0)
	{
		PlayEffect();
	}

	// dynamic parameter
	for (auto h : g_handles)
	{
		g_manager->SetDynamicInput(h.Handle, 0, m_effectBehavior.DynamicInput1);
		g_manager->SetDynamicInput(h.Handle, 1, m_effectBehavior.DynamicInput2);
		g_manager->SetDynamicInput(h.Handle, 2, m_effectBehavior.DynamicInput3);
		g_manager->SetDynamicInput(h.Handle, 3, m_effectBehavior.DynamicInput4);
	}

	if (m_time % m_step == 0)
	{
		m_rootLocation.X += m_effectBehavior.PositionVelocityX;
		m_rootLocation.Y += m_effectBehavior.PositionVelocityY;
		m_rootLocation.Z += m_effectBehavior.PositionVelocityZ;

		m_rootRotation.X += m_effectBehavior.RotationVelocityX;
		m_rootRotation.Y += m_effectBehavior.RotationVelocityY;
		m_rootRotation.Z += m_effectBehavior.RotationVelocityZ;

		m_rootScale.X += m_effectBehavior.ScaleVelocityX;
		m_rootScale.Y += m_effectBehavior.ScaleVelocityY;
		m_rootScale.Z += m_effectBehavior.ScaleVelocityZ;

		int32_t index = 0;

		for (int32_t z = 0; z < m_effectBehavior.CountZ && index < g_handles.size(); z++)
		{
			for (int32_t y = 0; y < m_effectBehavior.CountY && index < g_handles.size(); y++)
			{
				for (int32_t x = 0; x < m_effectBehavior.CountX && index < g_handles.size(); x++)
				{
					auto lenX = m_effectBehavior.Distance * (m_effectBehavior.CountX - 1);
					auto lenY = m_effectBehavior.Distance * (m_effectBehavior.CountY - 1);
					auto lenZ = m_effectBehavior.Distance * (m_effectBehavior.CountZ - 1);

					auto posX = m_effectBehavior.Distance * x - lenX / 2.0f;
					auto posY = m_effectBehavior.Distance * y - lenY / 2.0f;
					auto posZ = m_effectBehavior.Distance * z - lenZ / 2.0f;

					posX += m_rootLocation.X;
					posY += m_rootLocation.Y;
					posZ += m_rootLocation.Z;

					Effekseer::Matrix43 mat, matTra, matRot, matScale;
					matTra.Translation(posX, posY, posZ);
					matRot.RotationZXY(m_rootRotation.Z, m_rootRotation.X, m_rootRotation.Y);
					matScale.Scaling(m_rootScale.X, m_rootScale.Y, m_rootScale.Z);

					mat.Indentity();
					Effekseer::Matrix43::Multiple(mat, mat, matScale);
					Effekseer::Matrix43::Multiple(mat, mat, matRot);
					Effekseer::Matrix43::Multiple(mat, mat, matTra);

					g_manager->SetMatrix(g_handles[index].Handle, mat);

					g_manager->SetTargetLocation(g_handles[index].Handle,
												 m_effectBehavior.TargetPositionX,
												 m_effectBehavior.TargetPositionY,
												 m_effectBehavior.TargetPositionZ);

					index++;
				}
			}
		}

		for (size_t i = 0; i < g_handles.size(); i++)
		{
			if (g_handles[i].Time >= m_effectBehavior.RemovedTime)
			{
				g_manager->StopRoot(g_handles[i].Handle);
				g_handles[i].IsRootStopped = true;
			}
		}

		g_manager->Update((float)m_step);
		g_renderer->GetRenderer()->SetTime(m_time / 60.0f);

		for (size_t i = 0; i < g_handles.size(); i++)
		{
			g_handles[i].Time++;
		}
	}

	m_time++;

	return true;
}

bool Native::Rotate(float x, float y)
{
	if (g_mouseRotDirectionInvX)
	{
		x = -x;
	}

	if (g_mouseRotDirectionInvY)
	{
		y = -y;
	}

	g_RotY += x;
	g_RotX += y;

	while (g_RotY >= 180.0f)
	{
		g_RotY -= 180.0f * 2.0f;
	}

	while (g_RotY <= -180.0f)
	{
		g_RotY += 180.0f * 2.0f;
	}

	if (g_RotX > 180.0f / 2.0f)
	{
		g_RotX = 180.0f / 2.0f;
	}

	if (g_RotX < -180.0f / 2.0f)
	{
		g_RotX = -180.0f / 2.0f;
	}

	return true;
}

bool Native::Slide(float x, float y)
{
	if (::g_mouseSlideDirectionInvX)
	{
		x = -x;
	}

	if (::g_mouseSlideDirectionInvY)
	{
		y = -y;
	}

	::Effekseer::Vector3D up(0, 1, 0);
	::Effekseer::Vector3D right(1, 0, 0);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);
	mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
	::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
	::Effekseer::Vector3D::Transform(up, up, mat);
	::Effekseer::Vector3D::Transform(right, right, mat);

	up.X = up.X * y;
	up.Y = up.Y * y;
	up.Z = up.Z * y;
	right.X = right.X * (-x);
	right.Y = right.Y * (-x);
	right.Z = right.Z * (-x);

	::Effekseer::Vector3D v;
	v.X = up.X + right.X;
	v.Y = up.Y + right.Y;
	v.Z = up.Z + right.Z;
	g_focus_position.X += v.X;
	g_focus_position.Y += v.Y;
	g_focus_position.Z += v.Z;

	return true;
}

bool Native::Zoom(float zoom)
{
	g_Distance -= zoom;

	return true;
}

bool Native::SetRandomSeed(int seed)
{
	srand(seed);

	return true;
}

void* Native::RenderView(int32_t width, int32_t height)
{
	g_lastViewWidth = width;
	g_lastViewHeight = height;

	g_renderer->BeginRenderToView(width, height);
	RenderWindow();
	g_renderer->EndRenderToView();
	return (void*)g_renderer->GetViewID();
}

class RecorderCallback
{
private:
public:
	RecorderCallback() = default;
	virtual ~RecorderCallback() = default;

	virtual bool OnBeginRecord() { return false; }

	virtual void OnEndRecord() {}

	virtual void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) {}
};

class RecorderCallbackSprite : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;

public:
	RecorderCallbackSprite(RecordingParameter& recordingParameter) : recordingParameter_(recordingParameter) {}

	virtual ~RecorderCallbackSprite() = default;

	bool OnBeginRecord() override { return true; }

	void OnEndRecord() override {}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		char16_t path_[260];
		auto pathWithoutExt = recordingParameter_.GetPath();
		auto ext = recordingParameter_.GetExt();

		char pathWOE[256];
		char ext_[256];
		char path8_dst[256];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)pathWithoutExt);
		Effekseer::ConvertUtf16ToUtf8((int8_t*)ext_, 256, (const int16_t*)ext);
		sprintf(path8_dst, "%s.%d%s", pathWOE, index, ext_);
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 260, (const int8_t*)path8_dst);


		efk::PNGHelper pngHelper;
		pngHelper.Save((char16_t*)path_, g_renderer->GuideWidth, g_renderer->GuideHeight, pixels.data());
	}
};

class RecorderCallbackSpriteSheet : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	int yCount = 0;
	std::vector<Effekseer::Color> pixels_out;

public:
	RecorderCallbackSpriteSheet(RecordingParameter& recordingParameter) : recordingParameter_(recordingParameter) {}

	virtual ~RecorderCallbackSpriteSheet() = default;

	bool OnBeginRecord() override
	{
		yCount = recordingParameter_.Count / recordingParameter_.HorizontalCount;
		if (recordingParameter_.Count > recordingParameter_.HorizontalCount * yCount)
			yCount++;

		pixels_out.resize((g_renderer->GuideWidth * recordingParameter_.HorizontalCount) * (g_renderer->GuideHeight * yCount));

		if (recordingParameter_.Transparence == TransparenceType::Original)
		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 0);
			}
		}
		else

		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 255);
			}
		}

		return true;
	}

	void OnEndRecord() override
	{
		char16_t path_[260];
		auto pathWithoutExt = recordingParameter_.GetPath();
		auto ext = recordingParameter_.GetExt();

		char pathWOE[256];
		char ext_[256];
		char path8_dst[256];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)pathWithoutExt);
		Effekseer::ConvertUtf16ToUtf8((int8_t*)ext_, 256, (const int16_t*)ext);
		sprintf(path8_dst, "%s%s", pathWOE, ext_);
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 260, (const int8_t*)path8_dst);

		efk::PNGHelper pngHelper;
		pngHelper.Save(path_,
					   g_renderer->GuideWidth * recordingParameter_.HorizontalCount,
					   g_renderer->GuideHeight * yCount,
					   pixels_out.data());
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		auto x = index % recordingParameter_.HorizontalCount;
		auto y = index / recordingParameter_.HorizontalCount;

		for (int32_t y_ = 0; y_ < g_renderer->GuideHeight; y_++)
		{
			for (int32_t x_ = 0; x_ < g_renderer->GuideWidth; x_++)
			{
				pixels_out[x * g_renderer->GuideWidth + x_ +
						   (g_renderer->GuideWidth * recordingParameter_.HorizontalCount) * (g_renderer->GuideHeight * y + y_)] =
					pixels[x_ + y_ * g_renderer->GuideWidth];
			}
		}
	}
};

class RecorderCallbackGif : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	efk::GifHelper helper;

public:
	RecorderCallbackGif(RecordingParameter& recordingParameter) : recordingParameter_(recordingParameter) {}

	virtual ~RecorderCallbackGif() = default;

	bool OnBeginRecord() override
	{
		char16_t path_[260];
		auto pathWithoutExt = recordingParameter_.GetPath();
		auto ext = recordingParameter_.GetExt();

		char pathWOE[256];
		char ext_[256];
		char path8_dst[256];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)pathWithoutExt);
		Effekseer::ConvertUtf16ToUtf8((int8_t*)ext_, 256, (const int16_t*)ext);
		sprintf(path8_dst, "%s%s", pathWOE, ext_);
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 260, (const int8_t*)path8_dst);

		helper.Initialize(path_, g_renderer->GuideWidth, g_renderer->GuideHeight, recordingParameter_.Freq);
		return true;
	}

	void OnEndRecord() override {}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override { helper.AddImage(pixels); }
};

class RecorderCallbackAvi : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	efk::AVIExporter exporter;
	std::vector<uint8_t> d;
	FILE* fp = nullptr;

public:
	RecorderCallbackAvi(RecordingParameter& recordingParameter) : recordingParameter_(recordingParameter) {}

	virtual ~RecorderCallbackAvi() = default;

	bool OnBeginRecord() override
	{
		char16_t path_[260];
		auto pathWithoutExt = recordingParameter_.GetPath();
		auto ext = recordingParameter_.GetExt();

		char pathWOE[256];
		char ext_[256];
		char path8_dst[256];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)pathWithoutExt);
		Effekseer::ConvertUtf16ToUtf8((int8_t*)ext_, 256, (const int16_t*)ext);
		sprintf(path8_dst, "%s%s", pathWOE, ext_);
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 260, (const int8_t*)path8_dst);

#ifdef _WIN32
		_wfopen_s(&fp, (const wchar_t*)path_, L"wb");
#else
		fp = fopen(path8_dst, "wb");
#endif

		if (fp == nullptr)
			return false;

		exporter.Initialize(
			g_renderer->GuideWidth, g_renderer->GuideHeight, (int32_t)(60.0f / (float)recordingParameter_.Freq), recordingParameter_.Count);

		exporter.BeginToExportAVI(d);
		fwrite(d.data(), 1, d.size(), fp);

		return true;
	}

	void OnEndRecord() override
	{
		exporter.FinishToExportAVI(d);
		fwrite(d.data(), 1, d.size(), fp);
		fclose(fp);
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		exporter.ExportFrame(d, pixels);
		fwrite(d.data(), 1, d.size(), fp);
	}
};

bool Native::Record(RecordingParameter& recordingParameter)
{

	bool isBehaviorEnabled = true;

	if (g_effect == nullptr)
		return false;

	std::shared_ptr<RecorderCallback> recorderCallback;
	std::shared_ptr<RecorderCallback> recorderCallback2;

	RecordingParameter recordingParameter2;
	int32_t currentTime = 0;

	if (recordingParameter.Transparence == TransparenceType::Generate2)
	{
		recordingParameter2 = recordingParameter;
		auto path = recordingParameter2.GetPath();

		char pathWOE[256];
		char ext_[256];
		char path8_dst[256];
		char16_t* path_[256];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)path);
		sprintf(path8_dst, "%s_add%s", pathWOE, ext_);
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 256, (const int8_t*)path8_dst);
		recordingParameter2.SetPath((const char16_t*)path_);
	}

	if (recordingParameter.RecordingMode == RecordingModeType::Sprite)
	{
		recorderCallback = std::make_shared<RecorderCallbackSprite>(recordingParameter);

		if (recordingParameter.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackSprite>(recordingParameter2);
		}
	}
	else if (recordingParameter.RecordingMode == RecordingModeType::SpriteSheet)
	{
		recorderCallback = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter);

		if (recordingParameter.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter2);
		}
	}
	else if (recordingParameter.RecordingMode == RecordingModeType::Gif)
	{
		recorderCallback = std::make_shared<RecorderCallbackGif>(recordingParameter);

		if (recordingParameter.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackGif>(recordingParameter2);
		}
	}
	else if (recordingParameter.RecordingMode == RecordingModeType::Avi)
	{
		recorderCallback = std::make_shared<RecorderCallbackAvi>(recordingParameter);

		if (recordingParameter.Transparence == TransparenceType::Generate2)
		{
			recorderCallback2 = std::make_shared<RecorderCallbackAvi>(recordingParameter2);
		}
	}

	int iteratorCount = 1;

	if (recordingParameter.Transparence == TransparenceType::Generate2)
	{
		iteratorCount = 9;
	}

	if (!recorderCallback->OnBeginRecord())
	{
		return false;
	}

	if (recorderCallback2 != nullptr && !recorderCallback2->OnBeginRecord())
	{
		return false;
	}

	g_renderer->IsBackgroundTranslucent = recordingParameter.Transparence == TransparenceType::Original;

	::Effekseer::Vector3D position(0, 0, g_Distance);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);
	mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
	::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
	::Effekseer::Vector3D::Transform(position, position, mat);
	position.X += g_focus_position.X;
	position.Y += g_focus_position.Y;
	position.Z += g_focus_position.Z;

	g_renderer->GetRenderer()->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	StopEffect();

	::Effekseer::Handle handle;
	if (isBehaviorEnabled)
	{
		PlayEffect();
	}
	else
	{
		handle = g_manager->Play(g_effect, 0, 0, 0);
		g_manager->SetTargetLocation(
			handle, m_effectBehavior.TargetPositionX, m_effectBehavior.TargetPositionY, m_effectBehavior.TargetPositionZ);
	}

	if (isBehaviorEnabled)
	{
		StepEffect(recordingParameter.OffsetFrame);
	}
	else
	{
		for (int i = 0; i < recordingParameter.OffsetFrame; i++)
		{
			g_manager->Update();
			g_renderer->GetRenderer()->SetTime(currentTime / 60.0f);
			currentTime++;
		}
	}

	g_renderer->BeginRenderToView(g_lastViewWidth, g_lastViewHeight);

	for (int32_t i = 0; i < recordingParameter.Count; i++)
	{
		std::vector<std::vector<Effekseer::Color>> pixels;
		pixels.resize(iteratorCount);

		for (int32_t loop = 0; loop < iteratorCount; loop++)
		{
			auto colorValue = Effekseer::Clamp(32 * loop, 255, 0);
			g_renderer->BackgroundColor = Effekseer::Color(colorValue, colorValue, colorValue, 255);

			if (!g_renderer->BeginRecord(g_renderer->GuideWidth, g_renderer->GuideHeight))
				goto Exit;

			RenderWindow();

			g_renderer->EndRecord(pixels[loop],
								  recordingParameter.Transparence == TransparenceType::Generate,
								  recordingParameter.Transparence == TransparenceType::None);
		}

		if (isBehaviorEnabled)
		{
			StepEffect(recordingParameter.Freq);
		}
		else
		{
			for (int j = 0; j < recordingParameter.Freq; j++)
			{
				g_manager->Update();
				g_renderer->GetRenderer()->SetTime(currentTime / 60.0f);
				currentTime++;
			}
		}

		if (recordingParameter.Transparence == TransparenceType::Generate2)
		{
			std::vector<std::vector<Effekseer::Color>> pixels_out;
			pixels_out.resize(2);
			GenerateExportedImageWithBlendAndAdd(pixels_out[0], pixels_out[1], pixels);

			recorderCallback->OnEndFrameRecord(i, pixels_out[0]);

			if (recorderCallback2 != nullptr)
			{
				recorderCallback2->OnEndFrameRecord(i, pixels_out[1]);
			}
		}
		else
		{
			recorderCallback->OnEndFrameRecord(i, pixels[0]);
		}
	}

Exit:;

	if (isBehaviorEnabled)
	{
		StopEffect();
	}
	else
	{
		g_manager->StopEffect(handle);
	}

	g_manager->Update();

	g_renderer->EndRenderToView();

	recorderCallback->OnEndRecord();

	if (recorderCallback2 != nullptr)
	{
		recorderCallback2->OnEndRecord();
	}

	return true;
}

ViewerParamater Native::GetViewerParamater()
{
	assert(g_renderer != NULL);

	ViewerParamater paramater;

	paramater.GuideWidth = g_renderer->GuideWidth;
	paramater.GuideHeight = g_renderer->GuideHeight;
	paramater.ClippingStart = g_renderer->ClippingStart;
	paramater.ClippingEnd = g_renderer->ClippingEnd;
	paramater.IsPerspective = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE;
	paramater.IsOrthographic = g_renderer->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC;
	paramater.FocusX = g_focus_position.X;
	paramater.FocusY = g_focus_position.Y;
	paramater.FocusZ = g_focus_position.Z;
	paramater.AngleX = g_RotX;
	paramater.AngleY = g_RotY;
	paramater.Distance = g_Distance;
	paramater.RendersGuide = g_renderer->RendersGuide;
	paramater.RateOfMagnification = g_renderer->RateOfMagnification;

	paramater.Distortion = (DistortionType)g_renderer->Distortion;
	paramater.RenderingMode = (RenderMode)g_renderer->RenderingMode;

	return paramater;
}

void Native::SetViewerParamater(ViewerParamater& paramater)
{
	assert(g_renderer != NULL);

	g_renderer->ClippingStart = paramater.ClippingStart;
	g_renderer->ClippingEnd = paramater.ClippingEnd;
	g_renderer->GuideWidth = paramater.GuideWidth;
	g_renderer->GuideHeight = paramater.GuideHeight;

	g_renderer->RateOfMagnification = paramater.RateOfMagnification;

	if (paramater.IsPerspective)
	{
		g_renderer->SetProjectionType(::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE);
	}

	if (paramater.IsOrthographic)
	{
		g_renderer->SetProjectionType(::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC);
	}

	g_focus_position.X = paramater.FocusX;
	g_focus_position.Y = paramater.FocusY;
	g_focus_position.Z = paramater.FocusZ;
	g_RotX = paramater.AngleX;
	g_RotY = paramater.AngleY;
	g_Distance = paramater.Distance;
	g_renderer->RendersGuide = paramater.RendersGuide;

	g_renderer->Distortion = (::EffekseerTool::eDistortionType)paramater.Distortion;
	g_renderer->RenderingMode = (::Effekseer::RenderMode)paramater.RenderingMode;
}

ViewerEffectBehavior Native::GetEffectBehavior() { return m_effectBehavior; }

void Native::SetViewerEffectBehavior(ViewerEffectBehavior& behavior)
{
	m_effectBehavior = behavior;
	if (m_effectBehavior.CountX <= 0)
		m_effectBehavior.CountX = 1;
	if (m_effectBehavior.CountY <= 0)
		m_effectBehavior.CountY = 1;
	if (m_effectBehavior.CountZ <= 0)
		m_effectBehavior.CountZ = 1;
}

bool Native::SetSoundMute(bool mute)
{
	g_sound->SetMute(mute);
	return true;
}

bool Native::SetSoundVolume(float volume)
{
	g_sound->SetVolume(volume);
	return true;
}

bool Native::InvalidateTextureCache()
{
	{
		auto it = m_textures.begin();
		auto it_end = m_textures.end();
		while (it != it_end)
		{
			m_textureLoader->GetOriginalTextureLoader()->Unload((*it).second);
			++it;
		}
		m_textures.clear();
	}

	{
		auto it = m_models.begin();
		auto it_end = m_models.end();
		while (it != it_end)
		{
			ES_SAFE_DELETE((*it).second);
			++it;
		}
		m_models.clear();
	}

	return true;
}

void Native::SetIsGridShown(bool value, bool xy, bool xz, bool yz)
{
	g_renderer->IsGridShown = value;
	g_renderer->IsGridXYShown = xy;
	g_renderer->IsGridXZShown = xz;
	g_renderer->IsGridYZShown = yz;
}

void Native::SetGridLength(float length) { g_renderer->GridLength = length; }

void Native::SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b)
{
	g_renderer->BackgroundColor.R = r;
	g_renderer->BackgroundColor.G = g;
	g_renderer->BackgroundColor.B = b;
}

void Native::SetBackgroundImage(const char16_t* path) { g_renderer->LoadBackgroundImage(path); }

void Native::SetGridColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	g_renderer->GridColor.R = r;
	g_renderer->GridColor.G = g;
	g_renderer->GridColor.B = b;
	g_renderer->GridColor.A = a;
}

void Native::SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
{
	g_mouseRotDirectionInvX = rotX;
	g_mouseRotDirectionInvY = rotY;

	g_mouseSlideDirectionInvX = slideX;
	g_mouseSlideDirectionInvY = slideY;
}

void Native::SetStep(int32_t step) { m_step = step; }

bool Native::StartNetwork(const char* host, uint16_t port) { return g_client->Start((char*)host, port); }

void Native::StopNetwork() { g_client->Stop(); }

bool Native::IsConnectingNetwork() { return g_client->IsConnected(); }

void Native::SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path)
{
	g_client->Reload((const EFK_CHAR*)key, data, size);
}

void Native::SetLightDirection(float x, float y, float z)
{
	g_lightDirection = Effekseer::Vector3D(x, y, z);

	Effekseer::Vector3D temp = g_lightDirection;

	if (!g_renderer->IsRightHand)
	{
		temp.Z = -temp.Z;
	}

	g_renderer->GetRenderer()->SetLightDirection(temp);
}

void Native::SetLightColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	g_renderer->GetRenderer()->SetLightColor(Effekseer::Color(r, g, b, a));
}

void Native::SetLightAmbientColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	g_renderer->GetRenderer()->SetLightAmbientColor(Effekseer::Color(r, g, b, a));
}

void Native::SetIsRightHand(bool value)
{
	g_renderer->IsRightHand = value;
	if (g_renderer->IsRightHand)
	{
		g_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
	}
	else
	{
		g_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	}

	g_renderer->RecalcProjection();

	{
		Effekseer::Vector3D temp = g_lightDirection;

		if (!g_renderer->IsRightHand)
		{
			temp.Z = -temp.Z;
		}

		g_renderer->GetRenderer()->SetLightDirection(temp);
	}
}

void Native::SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ)
{
	g_renderer->IsCullingShown = isCullingShown;
	g_renderer->CullingRadius = cullingRadius;
	g_renderer->CullingPosition.X = cullingX;
	g_renderer->CullingPosition.Y = cullingY;
	g_renderer->CullingPosition.Z = cullingZ;
}

efk::ImageResource* Native::LoadImageResource(const char16_t* path)
{
	Effekseer::TextureLoader* loader = nullptr;
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		auto r = (EffekseerRendererGL::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererGL::CreateTextureLoader();
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto r = (EffekseerRendererDX11::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext());
	}
	else
	{
		auto r = (EffekseerRendererDX9::Renderer*)g_renderer->GetRenderer();
		loader = EffekseerRendererDX9::CreateTextureLoader(r->GetDevice());
	}
#endif

auto textureData = loader->Load(path, Effekseer::TextureType::Color);

	if (textureData != nullptr)
	{
		auto resource = std::make_shared<efk::ImageResource>();

		resource->GetTextureData() = textureData;
		resource->SetPath(path);

		if (g_imageResources[path] != nullptr)
		{
			loader->Unload(g_imageResources[path]->GetTextureData());
		}

		g_imageResources[path] = resource;

		delete loader;

		return resource.get();
	}
	else
	{
		delete loader;
		return nullptr;
	}
}

int32_t Native::GetAndResetDrawCall()
{
	auto call = g_renderer->GetRenderer()->GetDrawCallCount();
	g_renderer->GetRenderer()->ResetDrawCallCount();
	return call;
}

int32_t Native::GetAndResetVertexCount()
{
	auto call = g_renderer->GetRenderer()->GetDrawVertexCount();
	g_renderer->GetRenderer()->ResetDrawVertexCount();
	return call;
}

int32_t Native::GetInstanceCount()
{
	if (m_time == 0)
		return 0;

	int32_t sum = 0;
	for (int i = 0; i < g_handles.size(); i++)
	{
		auto count = g_manager->GetInstanceCount(g_handles[i].Handle);

		// Root
		if (!g_handles[i].IsRootStopped)
			count--;

		if (!g_manager->Exists(g_handles[i].Handle))
			count = 0;

		sum += count;
	}

	return sum;
}

float Native::GetFPS() { return 60.0; }

bool Native::IsDebugMode()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

void Native::SetBloomParameters(bool enabled, float intensity, float threshold, float softKnee)
{
	auto bloom = g_renderer->GetBloomEffect();
	if (bloom)
	{
		bloom->SetEnabled(enabled);
		bloom->SetParameters(intensity, threshold, softKnee);
	}
}

void Native::SetTonemapParameters(int32_t algorithm, float exposure)
{
	auto tonemap = g_renderer->GetTonemapEffect();
	if (tonemap)
	{
		tonemap->SetEnabled(algorithm != 0);
		tonemap->SetParameters((efk::TonemapEffect::Algorithm)algorithm, exposure);
	}
}

EffekseerRenderer::Renderer* Native::GetRenderer() { return g_renderer->GetRenderer(); }
