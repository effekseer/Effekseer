
#include "Graphics/efk.AVIExporter.h"
#include "Graphics/efk.GifHelper.h"
#include "Graphics/efk.PNGHelper.h"
#include "RenderedEffectGenerator.h"

#ifdef _WIN32
#include "Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif
#include "Graphics/Platform/GL/efk.GraphicsGL.h"

#ifdef _WIN32
#include "3rdParty/imgui_platform/imgui_impl_dx11.h"
#include <filesystem>
#endif

#include "dll.h"
#include <IO/IO.h>
#include <unordered_map>
#include <unordered_set>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

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

const float DistanceBase = 15.0f;
const float OrthoScaleBase = 16.0f;
const float ZoomDistanceFactor = 1.125f;
const float MaxZoom = 40.0f;
const float MinZoom = -40.0f;

static float g_RotX = 30.0f;
static float g_RotY = -30.0f;
static Effekseer::Vector3D g_lightDirection = Effekseer::Vector3D(1, 1, 1);
static float g_Zoom = 0.0f;
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

void SetZoom(float zoom)
{
	g_Zoom = Effekseer::Max(MinZoom, Effekseer::Min(MaxZoom, zoom));
}

float GetDistance()
{
	return DistanceBase * powf(ZoomDistanceFactor, g_Zoom);
}

float GetOrthoScale()
{
	return OrthoScaleBase / powf(ZoomDistanceFactor, g_Zoom);
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

	, Distortion(Effekseer::Tool::DistortionType::Current)
	, RenderingMode(RenderMode::Normal)
	, ViewerMode(ViewMode::_3D)

{
}

static Effekseer::Manager::DrawParameter drawParameter;

static ::EffekseerTool::Renderer* renderer_ = NULL;
static ::Effekseer::Effect* effect_ = NULL;
static ::EffekseerTool::Sound* sound_ = NULL;
static std::map<std::u16string, Effekseer::TextureData*> m_textures;
static std::map<std::u16string, Effekseer::Model*> m_models;
static std::map<std::u16string, std::shared_ptr<efk::ImageResource>> g_imageResources;
static std::map<std::u16string, Effekseer::MaterialData*> g_materials_;

static ::Effekseer::Vector3D g_focus_position;

static ::Effekseer::Client* g_client = NULL;

static efk::DeviceType g_deviceType = efk::DeviceType::OpenGL;

Native::TextureLoader::TextureLoader(efk::Graphics* graphics, Effekseer::ColorSpaceType colorSpaceType)
{
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		m_originalTextureLoader = EffekseerRendererGL::CreateTextureLoader(nullptr, colorSpaceType);
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto g = static_cast<efk::GraphicsDX11*>(graphics);
		m_originalTextureLoader = EffekseerRendererDX11::CreateTextureLoader(g->GetDevice(), g->GetContext(), nullptr, colorSpaceType);
	}
	else
	{
		assert(0);
	}
#endif
}

Native::TextureLoader::~TextureLoader()
{
	ES_SAFE_DELETE(m_originalTextureLoader);
}

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

Native::SoundLoader::SoundLoader(Effekseer::SoundLoader* loader)
	: m_loader(loader)
{
}

Native::SoundLoader::~SoundLoader()
{
}

void* Native::SoundLoader::Load(const EFK_CHAR* path)
{
	EFK_CHAR dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, (char16_t*)dst, 260);

	return m_loader->Load(dst);
}

void Native::SoundLoader::Unload(void* handle)
{
	m_loader->Unload(handle);
}

Native::ModelLoader::ModelLoader(efk::Graphics* graphics)
	: graphics_(graphics)
{
}

Native::ModelLoader::~ModelLoader()
{
}

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
			auto g = static_cast<efk::GraphicsDX11*>(graphics_);
			auto loader = ::EffekseerRendererDX11::CreateModelLoader(g->GetDevice());
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
			assert(0);
			return nullptr;
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

Native::MaterialLoader::MaterialLoader(EffekseerRenderer::Renderer* renderer)
{
	loader_ = renderer->CreateMaterialLoader();
}

Native::MaterialLoader::~MaterialLoader()
{
	ES_SAFE_DELETE(loader_);
}

Effekseer::MaterialData* Native::MaterialLoader::Load(const EFK_CHAR* path)
{
	if (loader_ == nullptr)
	{
		return nullptr;
	}

	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, dst, 260);

	std::u16string key(dst);

	if (g_materials_.count(key) > 0)
	{
		return g_materials_[key];
	}
	else
	{
		std::shared_ptr<Effekseer::StaticFile> staticFile;
		::Effekseer::MaterialData* t = nullptr;

		if (staticFile == nullptr)
		{
			staticFile = ::Effekseer::IO::GetInstance()->LoadIPCFile(dst);
		}

		if (staticFile == nullptr)
		{
			staticFile = ::Effekseer::IO::GetInstance()->LoadFile(dst);
		}

		if (staticFile != nullptr)
		{
			t = loader_->Load(staticFile->GetData(), staticFile->GetSize(), ::Effekseer::MaterialFileType::Code);
			materialFiles_[dst] = staticFile;
		}

		if (t != nullptr)
		{
			g_materials_[key] = t;
		}

		return t;
	}

	return nullptr;
}

void Native::MaterialLoader::ReleaseAll()
{
	for (auto it : g_materials_)
	{
		loader_->Unload(it.second);
	}
	g_materials_.clear();
	materialFiles_.clear();
}

::Effekseer::Effect* Native::GetEffect()
{
	return effect_;
}

Native::Native()
	: m_time(0)
	, m_step(1)
{
	g_client = Effekseer::Client::Create();

	commandQueueToMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueToMaterialEditor_->Start("EfkCmdToMatEdit", 1024 * 1024);

	commandQueueFromMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueFromMaterialEditor_->Start("EfkCmdFromMatEdit", 1024 * 1024);
}

Native::~Native()
{
	spdlog::trace("Begin Native::~Native()");

	ES_SAFE_DELETE(g_client);
	ES_SAFE_RELEASE(setting_);

	commandQueueToMaterialEditor_->Stop();
	commandQueueToMaterialEditor_.reset();

	commandQueueFromMaterialEditor_->Stop();
	commandQueueFromMaterialEditor_.reset();

	spdlog::trace("End Native::~Native()");
}

bool Native::CreateWindow_Effekseer(void* pHandle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	spdlog::trace("Begin Native::CreateWindow_Effekseer");
	m_isSRGBMode = isSRGBMode;
	g_deviceType = deviceType;

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	if (deviceType == efk::DeviceType::OpenGL)
	{
		graphics_ = new efk::GraphicsGL();
	}
#ifdef _WIN32
	else if (deviceType == efk::DeviceType::DirectX11)
	{
		graphics_ = new efk::GraphicsDX11();
	}
	else
	{
		assert(0);
	}
#endif
	spdlog::trace("OK new ::efk::Graphics");

	if (!graphics_->Initialize(pHandle, width, height, m_isSRGBMode))
	{
		spdlog::trace("Graphics::Initialize(false)");
		ES_SAFE_DELETE(graphics_);
		return false;
	}

	renderer_ = new ::EffekseerTool::Renderer(g_deviceType);

	if (renderer_->Initialize(width, height))
	{
		spdlog::trace("OK : ::EffekseerTool::Renderer::Initialize");

		//manager_ = ::Effekseer::Manager::Create(spriteCount);
		spdlog::trace("OK : ::Effekseer::Manager::Create");

		{
			spdlog::trace("OK : SetRenderers");

			{
				setting_ = Effekseer::Setting::Create();

				m_textureLoader = new TextureLoader(graphics_,
													isSRGBMode ? ::Effekseer::ColorSpaceType::Linear : ::Effekseer::ColorSpaceType::Gamma);

				setting_->SetTextureLoader(m_textureLoader);

				// TODO : refactor
				setting_->SetModelLoader(new ModelLoader(graphics_));
			}

			//manager_->SetSetting(setting_);

			spdlog::trace("OK : SetLoaders");
		}

		spdlog::trace("OK : AssignFunctions");

		mainScreen_ = std::make_shared<EffekseerTool::MainScreenRenderedEffectGenerator>();
		mainScreen_->Initialize(graphics_, setting_, spriteCount, isSRGBMode);
		mainScreen_->InitializedPrePost();
	}
	else
	{
		ES_SAFE_DELETE(renderer_);
		ES_SAFE_DELETE(graphics_);
		spdlog::trace("End Native::CreateWindow_Effekseer (false)");

		return false;
	}

	sound_ = new ::EffekseerTool::Sound();
	if (sound_->Initialize())
	{
		mainScreen_->GetMamanager()->SetSoundPlayer(sound_->GetSound()->CreateSoundPlayer());

		setting_->SetSoundLoader(new SoundLoader(sound_->GetSound()->CreateSoundLoader()));
	}

	if (mainScreen_ != nullptr && sound_ != nullptr)
	{
		mainScreen_->SetSound(sound_);
	}

	// TODO : refactor
	mainScreen_->GetMamanager()->GetSetting()->SetMaterialLoader(new MaterialLoader(mainScreen_->GetRenderer()));

	spdlog::trace("End Native::CreateWindow_Effekseer (true)");

	return true;
}

void Native::ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	graphics_->Clear(Effekseer::Color(r, g, b, a));
}

bool Native::UpdateWindow()
{
	//assert(manager_ != NULL);
	assert(renderer_ != NULL);

	::Effekseer::Vector3D position(0, 0, GetDistance());
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);

	if (renderer_->IsRightHand)
	{
		mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		Effekseer::Vector3D::Normal(drawParameter.CameraDirection, position);

		position.X += g_focus_position.X;
		position.Y += g_focus_position.Y;
		position.Z += g_focus_position.Z;

		::Effekseer::Matrix44 cameraMat;
		renderer_->SetCameraMatrix(
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
		renderer_->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtLH(position, temp_focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		drawParameter.CameraPosition = position;
	}

	renderer_->SetOrthographicScale(GetOrthoScale());

	sound_->SetListener(position, g_focus_position, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
	sound_->Update();

	// command
	if (commandQueueFromMaterialEditor_ != nullptr)
	{
		IPC::CommandData commandDataFromMaterialEditor;
		while (commandQueueFromMaterialEditor_->Dequeue(&commandDataFromMaterialEditor))
		{
			if (commandDataFromMaterialEditor.Type == IPC::CommandType::NotifyUpdate)
			{
				isUpdateMaterialRequired_ = true;
			}
		}
	}

	return true;
}

void Native::RenderWindow()
{
	graphics_->BeginScene();

	mainScreen_->Render();

	graphics_->EndScene();
}

void Native::Present()
{
	graphics_->Present();
}

bool Native::ResizeWindow(int width, int height)
{
	graphics_->Resize(width, height);
	return true;
}

bool Native::DestroyWindow()
{
	assert(renderer_ != NULL);

	InvalidateTextureCache();

	g_imageResources.clear();

	ES_SAFE_RELEASE(effect_);

	//manager_->Destroy();
	ES_SAFE_DELETE(renderer_);

	mainScreen_.reset();

	ES_SAFE_DELETE(graphics_);

	return true;
}

bool Native::LoadEffect(void* pData, int size, const char16_t* Path)
{
	assert(effect_ == NULL);

	//handles_.clear();

	((TextureLoader*)setting_->GetTextureLoader())->RootPath = std::u16string(Path);
	((ModelLoader*)setting_->GetModelLoader())->RootPath = std::u16string(Path);
	((MaterialLoader*)setting_->GetMaterialLoader())->RootPath = std::u16string(Path);

	SoundLoader* soundLoader = (SoundLoader*)setting_->GetSoundLoader();
	if (soundLoader)
	{
		soundLoader->RootPath = std::u16string(Path);
	}

	effect_ = Effekseer::Effect::Create(setting_, pData, size);
	assert(effect_ != NULL);

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(effect_);
	}
	return true;
}

bool Native::RemoveEffect()
{
	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(effect_);
		mainScreen_->Reset();
	}

	ES_SAFE_RELEASE(effect_);
	return true;
}

bool Native::PlayEffect()
{
	mainScreen_->PlayEffect();
	return true;
}

bool Native::StopEffect()
{
	mainScreen_->Reset();
	return true;
}

bool Native::StepEffect(int frame)
{
	mainScreen_->Update(frame);
	return true;
}

bool Native::StepEffect()
{
	mainScreen_->Update(1);
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

	float moveFactor = powf(ZoomDistanceFactor, g_Zoom);
	g_focus_position.X += v.X * moveFactor;
	g_focus_position.Y += v.Y * moveFactor;
	g_focus_position.Z += v.Z * moveFactor;

	return true;
}

bool Native::Zoom(float zoom)
{
	SetZoom(g_Zoom - zoom);

	return true;
}

bool Native::SetRandomSeed(int seed)
{
	srand(seed);

	return true;
}

void* Native::RenderView(int32_t width, int32_t height)
{
	renderer_->SetScreenSize(width, height);
	mainScreenConfig_.DrawParameter = drawParameter;
	mainScreenConfig_.CameraMatrix = renderer_->GetCameraMatrix();
	mainScreenConfig_.ProjectionMatrix = renderer_->GetProjectionMatrix();
	mainScreenConfig_.Distortion = (Effekseer::Tool::DistortionType)renderer_->Distortion;
	mainScreenConfig_.BackgroundColor = renderer_->BackgroundColor;
	mainScreen_->SetConfig(mainScreenConfig_);
	mainScreen_->Resize(Effekseer::Tool::Vector2DI(width, height));
	mainScreen_->Render();
	return (void*)mainScreen_->GetView()->GetViewID();
}

class RecorderCallback
{
private:
public:
	RecorderCallback() = default;
	virtual ~RecorderCallback() = default;

	virtual bool OnBeginRecord()
	{
		return false;
	}

	virtual void OnEndRecord()
	{
	}

	virtual void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels)
	{
	}
};

class RecorderCallbackSprite : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2DI imageSize_;

public:
	RecorderCallbackSprite(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2DI imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackSprite() = default;

	bool OnBeginRecord() override
	{
		return true;
	}

	void OnEndRecord() override
	{
	}

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

		spdlog::trace("RecorderCallbackSprite : {}", path8_dst);

		efk::PNGHelper pngHelper;
		pngHelper.Save((char16_t*)path_, imageSize_.X, imageSize_.Y, pixels.data());
	}
};

class RecorderCallbackSpriteSheet : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2DI imageSize_;
	int yCount = 0;
	std::vector<Effekseer::Color> pixels_out;

public:
	RecorderCallbackSpriteSheet(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2DI imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackSpriteSheet() = default;

	bool OnBeginRecord() override
	{
		yCount = recordingParameter_.Count / recordingParameter_.HorizontalCount;
		if (recordingParameter_.Count > recordingParameter_.HorizontalCount * yCount)
			yCount++;

		pixels_out.resize((imageSize_.X * recordingParameter_.HorizontalCount) * (imageSize_.Y * yCount));

		if (recordingParameter_.Transparence == TransparenceType::None)
		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 255);
			}
		}
		else

		{
			for (auto& p : pixels_out)
			{
				p = Effekseer::Color(0, 0, 0, 0);
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

		spdlog::trace("RecorderCallbackSpriteSheet : {}", path8_dst);

		efk::PNGHelper pngHelper;
		pngHelper.Save(
			path_, imageSize_.X * recordingParameter_.HorizontalCount, imageSize_.Y * yCount, pixels_out.data());
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		auto x = index % recordingParameter_.HorizontalCount;
		auto y = index / recordingParameter_.HorizontalCount;

		for (int32_t y_ = 0; y_ < imageSize_.Y; y_++)
		{
			for (int32_t x_ = 0; x_ < imageSize_.X; x_++)
			{
				pixels_out[x * imageSize_.X + x_ +
						   (imageSize_.X * recordingParameter_.HorizontalCount) * (imageSize_.Y * y + y_)] =
					pixels[x_ + y_ * imageSize_.X];
			}
		}
	}
};

class RecorderCallbackGif : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2DI imageSize_;
	efk::GifHelper helper;

public:
	RecorderCallbackGif(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2DI imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

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

		spdlog::trace("RecorderCallbackGif : {}", path8_dst);

		helper.Initialize(path_, imageSize_.X, imageSize_.Y, recordingParameter_.Freq);
		return true;
	}

	void OnEndRecord() override
	{
	}

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels) override
	{
		helper.AddImage(pixels);
	}
};

class RecorderCallbackAvi : public RecorderCallback
{
private:
	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2DI imageSize_;
	efk::AVIExporter exporter;
	std::vector<uint8_t> d;
	FILE* fp = nullptr;

public:
	RecorderCallbackAvi(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2DI imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

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

		spdlog::trace("RecorderCallbackAvi : {}", path8_dst);

		Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 260, (const int8_t*)path8_dst);

#ifdef _WIN32
		_wfopen_s(&fp, (const wchar_t*)path_, L"wb");
#else
		fp = fopen(path8_dst, "wb");
#endif

		if (fp == nullptr)
			return false;

		exporter.Initialize(
			imageSize_.X, imageSize_.Y, (int32_t)(60.0f / (float)recordingParameter_.Freq), recordingParameter_.Count);

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

class Native::Recorder
{
private:
	std::shared_ptr<RecorderCallback> recorderCallback;
	std::shared_ptr<RecorderCallback> recorderCallback2;
	Effekseer::Tool::Vector2DI imageSize_;

	bool isRecording = false;
	bool isRecordCompleted = false;
	RecordingParameter recordingParameter_ = {};
	RecordingParameter recordingParameter2_ = {};
	::Effekseer::Handle handle = 0;
	int iteratorCount = 1;
	int32_t currentTime = 0;
	int recordedCount = 0;
	bool completed = false;

	std::shared_ptr<Effekseer::Tool::RenderedEffectGenerator> generator_;
	std::shared_ptr<EffekseerTool::MainScreenRenderedEffectGenerator> mainScreen_;
	efk::Graphics* graphics_ = nullptr;

public:
	bool Begin(std::shared_ptr<EffekseerTool::MainScreenRenderedEffectGenerator> mainScreen,
			   efk::Graphics* graphics,
			   Effekseer::Setting* setting,
			   const RecordingParameter& recordingParameter,
			   Effekseer::Tool::Vector2DI imageSize,
			   bool isSRGBMode,
			   Effekseer::Tool::ViewerEffectBehavior behavior)
	{
		mainScreen_ = mainScreen;
		graphics_ = graphics;
		recordingParameter_ = recordingParameter;
		int recScale = 1;
		imageSize_ = Effekseer::Tool::Vector2DI(imageSize.X * recScale, imageSize.Y * recScale);

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
		{
			recordingParameter2_ = recordingParameter_;
			auto path = recordingParameter_.GetPath();

			char pathWOE[256];
			char path8_dst[256];
			char16_t* path_[256];
			Effekseer::ConvertUtf16ToUtf8((int8_t*)pathWOE, 256, (const int16_t*)path);
			sprintf(path8_dst, "%s_add", pathWOE);
			Effekseer::ConvertUtf8ToUtf16((int16_t*)path_, 256, (const int8_t*)path8_dst);
			recordingParameter2_.SetPath((const char16_t*)path_);
		}

		if (recordingParameter_.RecordingMode == RecordingModeType::Sprite)
		{
			recorderCallback = std::make_shared<RecorderCallbackSprite>(recordingParameter_, imageSize_);

			if (recordingParameter_.Transparence == TransparenceType::Generate2)
			{
				recorderCallback2 = std::make_shared<RecorderCallbackSprite>(recordingParameter2_, imageSize_);
			}
		}
		else if (recordingParameter_.RecordingMode == RecordingModeType::SpriteSheet)
		{
			recorderCallback = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter_, imageSize_);

			if (recordingParameter_.Transparence == TransparenceType::Generate2)
			{
				recorderCallback2 = std::make_shared<RecorderCallbackSpriteSheet>(recordingParameter2_, imageSize_);
			}
		}
		else if (recordingParameter_.RecordingMode == RecordingModeType::Gif)
		{
			recorderCallback = std::make_shared<RecorderCallbackGif>(recordingParameter_, imageSize_);

			if (recordingParameter_.Transparence == TransparenceType::Generate2)
			{
				recorderCallback2 = std::make_shared<RecorderCallbackGif>(recordingParameter2_, imageSize_);
			}
		}
		else if (recordingParameter_.RecordingMode == RecordingModeType::Avi)
		{
			recorderCallback = std::make_shared<RecorderCallbackAvi>(recordingParameter_, imageSize_);

			if (recordingParameter_.Transparence == TransparenceType::Generate2)
			{
				recorderCallback2 = std::make_shared<RecorderCallbackAvi>(recordingParameter2_, imageSize_);
			}
		}

		if (recordingParameter_.Transparence == TransparenceType::Generate2)
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

		generator_ = std::make_shared<Effekseer::Tool::RenderedEffectGenerator>();

		if (!generator_->Initialize(graphics_, setting, mainScreen_->GetRenderer()->GetSquareMaxCount(), isSRGBMode))
		{
			return false;
		}

		generator_->Resize(imageSize_);

		Effekseer::Tool::RenderedEffectGeneratorConfig config = mainScreen_->GetConfig();
		config.DrawParameter = drawParameter;
		config.Distortion = (Effekseer::Tool::DistortionType)renderer_->Distortion;
		auto screenSize = renderer_->GetScreenSize();

		::Effekseer::Matrix44 mat;
		mat.Values[0][0] = (float)screenSize.X / (float)imageSize.X;
		mat.Values[1][1] = (float)screenSize.Y / (float)imageSize.Y;
		::Effekseer::Matrix44::Mul(config.ProjectionMatrix, config.ProjectionMatrix, mat);

		generator_->SetConfig(config);
		generator_->SetEffect(effect_);

		generator_->SetBehavior(behavior);

		float bloomIntencity = 0.0f;
		float bloomTh = 0.0f;
		float bloomK = 0.0f;

		mainScreen_->GetBloomEffect()->GetParameters(bloomIntencity, bloomTh, bloomK);
		generator_->GetBloomEffect()->SetParameters(bloomIntencity, bloomTh, bloomK);
		generator_->GetBloomEffect()->SetEnabled(mainScreen_->GetBloomEffect()->GetEnabled());

		efk::TonemapEffect::Algorithm toneA;
		float toneE = 0.0f;
		mainScreen_->GetTonemapEffect()->GetParameters(toneA, toneE);
		generator_->GetTonemapEffect()->SetParameters(toneA, toneE);
		generator_->GetTonemapEffect()->SetEnabled(mainScreen_->GetTonemapEffect()->GetEnabled());

		generator_->PlayEffect();
		generator_->Update(recordingParameter_.OffsetFrame);

		return true;
	}

	bool Step(Native* native, int frames)
	{
		for (int32_t i = 0; i < frames; i++)
		{
			if (IsCompleted())
			{
				break;
			}

			std::vector<std::vector<Effekseer::Color>> pixelss;
			pixelss.resize(iteratorCount);

			for (int32_t loop = 0; loop < iteratorCount; loop++)
			{
				auto colorValue = Effekseer::Clamp(32 * loop, 255, 0);

				auto config = generator_->GetConfig();
				config.BackgroundColor = Effekseer::Color(colorValue, colorValue, colorValue, 255);
				generator_->SetConfig(config);

				generator_->Render();
				graphics_->SaveTexture(generator_->GetView().get(), pixelss[loop]);

				auto generateAlpha = recordingParameter_.Transparence == TransparenceType::Generate;
				auto removeAlpha = recordingParameter_.Transparence == TransparenceType::None;

				auto& pixels = pixelss[loop];

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

				for (int32_t i = 0; i < imageSize_.X * imageSize_.Y; i++)
				{
					if (generateAlpha)
					{
						auto rf = b2f(pixels[i].R);
						auto gf = b2f(pixels[i].G);
						auto bf = b2f(pixels[i].B);
						auto oaf = b2f(pixels[i].A);

						rf = rf * oaf;
						gf = gf * oaf;
						bf = bf * oaf;

						auto af = rf;
						af = Effekseer::Max(af, gf);
						af = Effekseer::Max(af, bf);

						if (af > 0.0f)
						{
							pixels[i].R = f2b(rf / af);
							pixels[i].G = f2b(gf / af);
							pixels[i].B = f2b(bf / af);
						}

						pixels[i].A = f2b(af);
					}

					if (removeAlpha)
					{
						pixels[i].A = 255;
					}
				}
			}

			generator_->Update(recordingParameter_.Freq);

			if (recordingParameter_.Transparence == TransparenceType::Generate2)
			{
				std::vector<std::vector<Effekseer::Color>> pixels_out;
				pixels_out.resize(2);
				GenerateExportedImageWithBlendAndAdd(pixels_out[0], pixels_out[1], pixelss);

				recorderCallback->OnEndFrameRecord(recordedCount, pixels_out[0]);

				if (recorderCallback2 != nullptr)
				{
					recorderCallback2->OnEndFrameRecord(recordedCount, pixels_out[1]);
				}
			}
			else
			{
				recorderCallback->OnEndFrameRecord(recordedCount, pixelss[0]);
			}

			recordedCount++;
		}

		return true;
	}

	bool End(Native* native)
	{
		generator_->Reset();

		recorderCallback->OnEndRecord();

		if (recorderCallback2 != nullptr)
		{
			recorderCallback2->OnEndRecord();
		}

		generator_ = nullptr;

		return true;
	}

	bool IsCompleted() const
	{
		return recordedCount >= recordingParameter_.Count;
	}

	float GetProgress() const
	{
		return static_cast<float>(recordedCount) / static_cast<float>(recordingParameter_.Count);
	}
};

bool Native::BeginRecord(const RecordingParameter& recordingParameter)
{
	if (effect_ == nullptr)
		return false;

	recorder.reset(new Recorder());
	return recorder->Begin(mainScreen_, graphics_, setting_, recordingParameter, Effekseer::Tool::Vector2DI(mainScreen_->GuideWidth, mainScreen_->GuideHeight), m_isSRGBMode, behavior_);
}

bool Native::StepRecord(int frames)
{
	if (recorder == nullptr)
	{
		return false;
	}
	return recorder->Step(this, frames);
}

bool Native::EndRecord()
{
	if (recorder == nullptr)
	{
		return false;
	}
	bool result = recorder->End(this);
	recorder.reset();
	return true;
}

bool Native::IsRecording() const
{
	return recorder != nullptr;
}

float Native::GetRecordingProgress() const
{
	return (recorder) ? recorder->GetProgress() : 0.0f;
}

bool Native::IsRecordCompleted() const
{
	return (recorder) ? recorder->IsCompleted() : false;
}

bool Native::Record(const RecordingParameter& recordingParameter)
{
	if (BeginRecord(recordingParameter) == false)
	{
		return false;
	}

	while (!IsRecordCompleted())
	{
		if (StepRecord(1) == false)
		{
			break;
		}
	}

	if (EndRecord() == false)
	{
		return false;
	}

	return true;
}

ViewerParamater Native::GetViewerParamater()
{
	assert(renderer_ != NULL);

	ViewerParamater paramater;

	paramater.GuideWidth = mainScreen_->GuideWidth;
	paramater.GuideHeight = mainScreen_->GuideHeight;
	paramater.ClippingStart = renderer_->ClippingStart;
	paramater.ClippingEnd = renderer_->ClippingEnd;
	paramater.IsPerspective = renderer_->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE;
	paramater.IsOrthographic = renderer_->GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC;
	paramater.FocusX = g_focus_position.X;
	paramater.FocusY = g_focus_position.Y;
	paramater.FocusZ = g_focus_position.Z;
	paramater.AngleX = g_RotX;
	paramater.AngleY = g_RotY;
	paramater.Distance = GetDistance();
	paramater.RendersGuide = mainScreen_->RendersGuide;
	paramater.RateOfMagnification = renderer_->RateOfMagnification;

	paramater.Distortion = (Effekseer::Tool::DistortionType)renderer_->Distortion;
	paramater.RenderingMode = (RenderMode)renderer_->RenderingMode;

	return paramater;
}

void Native::SetViewerParamater(ViewerParamater& paramater)
{
	assert(renderer_ != NULL);

	renderer_->ClippingStart = paramater.ClippingStart;
	renderer_->ClippingEnd = paramater.ClippingEnd;
	mainScreen_->GuideWidth = paramater.GuideWidth;
	mainScreen_->GuideHeight = paramater.GuideHeight;

	renderer_->RateOfMagnification = paramater.RateOfMagnification;

	if (paramater.IsPerspective)
	{
		renderer_->SetProjectionType(::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE);
	}

	if (paramater.IsOrthographic)
	{
		renderer_->SetProjectionType(::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC);
	}

	g_focus_position.X = paramater.FocusX;
	g_focus_position.Y = paramater.FocusY;
	g_focus_position.Z = paramater.FocusZ;
	g_RotX = paramater.AngleX;
	g_RotY = paramater.AngleY;

	SetZoom(logf(Effekseer::Max(FLT_MIN, paramater.Distance / DistanceBase)) / logf(ZoomDistanceFactor));

	mainScreen_->RendersGuide = paramater.RendersGuide;
	renderer_->Distortion = (Effekseer::Tool::DistortionType)paramater.Distortion;
	renderer_->RenderingMode = (::Effekseer::RenderMode)paramater.RenderingMode;
}

Effekseer::Tool::ViewerEffectBehavior Native::GetEffectBehavior()
{
	return behavior_;
}

void Native::SetViewerEffectBehavior(Effekseer::Tool::ViewerEffectBehavior& behavior)
{
	behavior_ = behavior;
	if (behavior_.CountX <= 0)
		behavior_.CountX = 1;
	if (behavior_.CountY <= 0)
		behavior_.CountY = 1;
	if (behavior_.CountZ <= 0)
		behavior_.CountZ = 1;

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetBehavior(behavior_);
	}
}

bool Native::SetSoundMute(bool mute)
{
	sound_->SetMute(mute);
	return true;
}

bool Native::SetSoundVolume(float volume)
{
	sound_->SetVolume(volume);
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

	{
		((MaterialLoader*)setting_->GetMaterialLoader())->ReleaseAll();
	}

	return true;
}

void Native::SetIsGridShown(bool value, bool xy, bool xz, bool yz)
{
	mainScreen_->IsGridShown = value;
	mainScreen_->IsGridXYShown = xy;
	mainScreen_->IsGridXZShown = xz;
	mainScreen_->IsGridYZShown = yz;
}

void Native::SetGridLength(float length)
{
	mainScreen_->GridLength = length;
}

void Native::SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b)
{
	renderer_->BackgroundColor.R = r;
	renderer_->BackgroundColor.G = g;
	renderer_->BackgroundColor.B = b;
}

void Native::SetBackgroundImage(const char16_t* path)
{
	mainScreen_->LoadBackgroundImage(path);
}

void Native::SetGridColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	mainScreen_->GridColor = Effekseer::Color(r, g, b, a);
}

void Native::SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
{
	g_mouseRotDirectionInvX = rotX;
	g_mouseRotDirectionInvY = rotY;

	g_mouseSlideDirectionInvX = slideX;
	g_mouseSlideDirectionInvY = slideY;
}

void Native::SetStep(int32_t step)
{
	m_step = step;
}

bool Native::StartNetwork(const char* host, uint16_t port)
{
	return g_client->Start((char*)host, port);
}

void Native::StopNetwork()
{
	g_client->Stop();
}

bool Native::IsConnectingNetwork()
{
	return g_client->IsConnected();
}

void Native::SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path)
{
	g_client->Reload((const EFK_CHAR*)key, data, size);
}

void Native::SetLightDirection(float x, float y, float z)
{
	g_lightDirection = Effekseer::Vector3D(x, y, z);

	Effekseer::Vector3D temp = g_lightDirection;

	if (!renderer_->IsRightHand)
	{
		temp.Z = -temp.Z;
	}

	mainScreenConfig_.LightDirection = temp;
}

void Native::SetLightColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	mainScreenConfig_.LightColor = Effekseer::Color(r, g, b, a);
}

void Native::SetLightAmbientColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	mainScreenConfig_.LightAmbientColor = Effekseer::Color(r, g, b, a);
}

void Native::SetIsRightHand(bool value)
{
	renderer_->IsRightHand = value;
	if (renderer_->IsRightHand)
	{
		setting_->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
	}
	else
	{
		setting_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	}

	renderer_->RecalcProjection();

	{
		Effekseer::Vector3D temp = mainScreenConfig_.LightDirection;

		if (!renderer_->IsRightHand)
		{
			temp.Z = -temp.Z;
		}

		mainScreenConfig_.LightDirection = temp;
	}
}

void Native::SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ)
{
	mainScreen_->IsCullingShown = isCullingShown;
	mainScreen_->CullingRadius = cullingRadius;
	mainScreen_->CullingPosition.X = cullingX;
	mainScreen_->CullingPosition.Y = cullingY;
	mainScreen_->CullingPosition.Z = cullingZ;
}

efk::ImageResource* Native::LoadImageResource(const char16_t* path)
{
	auto it = g_imageResources.find(path);
	if (it != g_imageResources.end())
	{
		return it->second.get();
	}

	std::shared_ptr<Effekseer::TextureLoader> loader = nullptr;
	if (g_deviceType == efk::DeviceType::OpenGL)
	{
		auto r = (EffekseerRendererGL::Renderer*)mainScreen_->GetRenderer();
		loader = std::shared_ptr<Effekseer::TextureLoader>(EffekseerRendererGL::CreateTextureLoader());
	}
#ifdef _WIN32
	else if (g_deviceType == efk::DeviceType::DirectX11)
	{
		auto r = (EffekseerRendererDX11::Renderer*)mainScreen_->GetRenderer();
		loader = std::shared_ptr<Effekseer::TextureLoader>(EffekseerRendererDX11::CreateTextureLoader(r->GetDevice(), r->GetContext()));
	}
	else
	{
		assert(0);
	}
#endif

	auto resource = std::make_shared<efk::ImageResource>(loader);
	resource->SetPath(path);

	if (resource->Validate())
	{
		g_imageResources[path] = resource;
		return resource.get();
	}

	return nullptr;
}

int32_t Native::GetAndResetDrawCall()
{
	auto call = mainScreen_->GetRenderer()->GetDrawCallCount();
	mainScreen_->GetRenderer()->ResetDrawCallCount();
	return call;
}

int32_t Native::GetAndResetVertexCount()
{
	auto call = mainScreen_->GetRenderer()->GetDrawVertexCount();
	mainScreen_->GetRenderer()->ResetDrawVertexCount();
	return call;
}

int32_t Native::GetInstanceCount()
{
	return mainScreen_->GetInstanceCount();
}

float Native::GetFPS()
{
	return 60.0;
}

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
	if (mainScreen_ != nullptr)
	{
		auto bloom = mainScreen_->GetBloomEffect();
		if (bloom)
		{
			bloom->SetEnabled(enabled);
			bloom->SetParameters(intensity, threshold, softKnee);
		}
	}
}

void Native::SetTonemapParameters(int32_t algorithm, float exposure)
{
	if (mainScreen_ != nullptr)
	{
		auto tonemap = mainScreen_->GetTonemapEffect();
		if (tonemap)
		{
			tonemap->SetEnabled(algorithm != 0);
			tonemap->SetParameters((efk::TonemapEffect::Algorithm)algorithm, exposure);
		}
	}
}

void Native::OpenOrCreateMaterial(const char16_t* path)
{
	if (commandQueueToMaterialEditor_ == nullptr)
		return;

	char u8path[260];

	Effekseer::ConvertUtf16ToUtf8((int8_t*)u8path, 260, (int16_t*)path);

	IPC::CommandData commandData;
	commandData.Type = IPC::CommandType::OpenOrCreateMaterial;
	commandData.SetStr(u8path);
	commandQueueToMaterialEditor_->Enqueue(&commandData);

	spdlog::trace("ICP - Send - OpenOrCreateMaterial : {}", u8path);
}

void Native::TerminateMaterialEditor()
{
	if (commandQueueToMaterialEditor_ == nullptr)
		return;

	IPC::CommandData commandData;
	commandData.Type = IPC::CommandType::Terminate;
	commandQueueToMaterialEditor_->Enqueue(&commandData);

	spdlog::trace("ICP - Send - Terminate");
}

bool Native::GetIsUpdateMaterialRequiredAndReset()
{
	return false;
	auto ret = isUpdateMaterialRequired_;
	isUpdateMaterialRequired_ = false;
	return ret;
}

void Native::SetFileLogger(const char16_t* path)
{
	spdlog::flush_on(spdlog::level::trace);
	spdlog::set_level(spdlog::level::trace);
	spdlog::trace("Begin Native::SetFileLogger");

#if defined(_WIN32)
	auto wpath = std::filesystem::path(reinterpret_cast<const wchar_t*>(path));
	auto fileLogger = spdlog::basic_logger_mt("logger", wpath.generic_string().c_str());
#else
	char cpath[512];
	Effekseer::ConvertUtf16ToUtf8(reinterpret_cast<int8_t*>(cpath), 512, reinterpret_cast<const int16_t*>(path));
	auto fileLogger = spdlog::basic_logger_mt("logger", cpath);
#endif

	spdlog::set_default_logger(fileLogger);

#if defined(_WIN32)
	spdlog::trace("Native::SetFileLogger : {}", wpath.generic_string().c_str());
#else
	spdlog::trace("Native::SetFileLogger : {}", cpath);
#endif

	spdlog::trace("End Native::SetFileLogger");
}

EffekseerRenderer::Renderer* Native::GetRenderer()
{
	return mainScreen_->GetRenderer();
}
