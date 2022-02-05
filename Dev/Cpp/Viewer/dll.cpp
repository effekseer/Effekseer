
#include "Recorder/EffectRecorder.h"
#include "RenderedEffectGenerator.h"

#ifdef _WIN32
#include "Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif
#include "Graphics/Platform/GL/efk.GraphicsGL.h"

#ifdef _WIN32
#include "3rdParty/imgui_platform/imgui_impl_dx11.h"
#include <filesystem>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif

#include "dll.h"
#include <IO/IO.h>
#include <unordered_map>
#include <unordered_set>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "Effekseer/Effekseer.EffectImplemented.h"
#include "Effekseer/Effekseer.EffectNode.h"

#include "GUI/Image.h"
#include "GUI/ReloadableImage.h"
#include "GUI/RenderImage.h"

#include "3D/Effect.h"

ViewerParamater::ViewerParamater()
	: GuideWidth(0)
	, GuideHeight(0)
	, RendersGuide(false)

	, IsCullingShown(false)
	, CullingRadius(0)
	, CullingX(0)
	, CullingY(0)
	, CullingZ(0)

	, Distortion(Effekseer::Tool::DistortionType::Current)
	, RenderingMode(Effekseer::Tool::RenderingMethodType::Normal)
{
}

static Effekseer::Manager::DrawParameter drawParameter;

static ::EffekseerTool::Sound* sound_ = nullptr;
static std::map<std::u16string, Effekseer::TextureRef> m_textures;
static std::map<std::u16string, Effekseer::ModelRef> m_models;
static std::map<std::u16string, Effekseer::MaterialRef> g_materials_;

static efk::DeviceType g_deviceType = efk::DeviceType::OpenGL;

Native::TextureLoader::TextureLoader(efk::Graphics* graphics, Effekseer::ColorSpaceType colorSpaceType)
{
	m_originalTextureLoader = EffekseerRenderer::CreateTextureLoader(graphics->GetGraphicsDevice(), nullptr, colorSpaceType);
}

Native::TextureLoader::~TextureLoader()
{
}

Effekseer::TextureRef Native::TextureLoader::Load(const char16_t* path, ::Effekseer::TextureType textureType)
{
	std::u16string key(path);

	if (m_textures.count(key) > 0)
	{
		return m_textures[key];
	}
	else
	{
		auto t = m_originalTextureLoader->Load(path, textureType);

		if (t != nullptr)
		{
			m_textures[key] = t;
		}

		return t;
	}

	return nullptr;
}

void Native::TextureLoader::Unload(Effekseer::TextureRef data)
{
}

Native::SoundLoader::SoundLoader(Effekseer::SoundLoaderRef loader)
	: m_loader(loader)
{
}

Native::SoundLoader::~SoundLoader()
{
}

::Effekseer::SoundDataRef Native::SoundLoader::Load(const char16_t* path)
{
	return m_loader->Load(path);
}

void Native::SoundLoader::Unload(::Effekseer::SoundDataRef soundData)
{
	m_loader->Unload(soundData);
}

Native::ModelLoader::ModelLoader(efk::Graphics* graphics)
	: graphics_(graphics)
{
}

Native::ModelLoader::~ModelLoader()
{
}

Effekseer::ModelRef Native::ModelLoader::Load(const char16_t* path)
{
	std::u16string key(path);
	Effekseer::ModelRef model = nullptr;

	if (m_models.count(key) > 0)
	{
		return m_models[key];
	}
	else
	{
		auto loader = ::EffekseerRenderer::CreateModelLoader(graphics_->GetGraphicsDevice());
		auto m = loader->Load(path);

		if (m != nullptr)
		{
			m_models[key] = m;
		}

		return m;
	}
}

void Native::ModelLoader::Unload(Effekseer::ModelRef data)
{
}

Native::MaterialLoader::MaterialLoader(const EffekseerRenderer::RendererRef& renderer)
{
	loader_ = renderer->CreateMaterialLoader();
}

Native::MaterialLoader::~MaterialLoader()
{
}

Effekseer::MaterialRef Native::MaterialLoader::Load(const char16_t* path)
{
	if (loader_ == nullptr)
	{
		return nullptr;
	}

	std::u16string key(path);

	if (g_materials_.count(key) > 0)
	{
		return g_materials_[key];
	}
	else
	{
		std::shared_ptr<Effekseer::StaticFile> staticFile;
		::Effekseer::MaterialRef t = nullptr;

		if (staticFile == nullptr)
		{
			staticFile = ::Effekseer::IO::GetInstance()->LoadIPCFile(path);
		}

		if (staticFile == nullptr)
		{
			staticFile = ::Effekseer::IO::GetInstance()->LoadFile(path);
		}

		if (staticFile != nullptr)
		{
			t = loader_->Load(staticFile->GetData(), staticFile->GetSize(), ::Effekseer::MaterialFileType::Code);
			materialFiles_[path] = staticFile;
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

Native::Native()
{
	commandQueueToMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueToMaterialEditor_->Start("EfkCmdToMatEdit", 1024 * 1024);

	commandQueueFromMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueFromMaterialEditor_->Start("EfkCmdFromMatEdit", 1024 * 1024);
}

Native::~Native()
{
	spdlog::trace("Begin Native::~Native()");

	commandQueueToMaterialEditor_->Stop();
	commandQueueToMaterialEditor_.reset();

	commandQueueFromMaterialEditor_->Stop();
	commandQueueFromMaterialEditor_.reset();

	spdlog::trace("End Native::~Native()");
}

bool Native::CreateWindow_Effekseer(void* pHandle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	spdlog::trace("Begin Native::CreateWindow_Effekseer");
	g_deviceType = deviceType;

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	if (deviceType == efk::DeviceType::OpenGL)
	{
		graphics_ = std::make_shared<efk::GraphicsGL>();
	}
#ifdef _WIN32
	else if (deviceType == efk::DeviceType::DirectX11)
	{
		graphics_ = std::make_shared<efk::GraphicsDX11>();
	}
	else
	{
		assert(0);
	}
#endif
	spdlog::trace("OK new ::efk::Graphics");

	if (!graphics_->Initialize(pHandle, width, height))
	{
		spdlog::trace("Graphics::Initialize(false)");
		graphics_.reset();
		return false;
	}

	{
		spdlog::trace("OK : ::EffekseerTool::Renderer::Initialize");

		//manager_ = ::Effekseer::Manager::Create(spriteCount);
		spdlog::trace("OK : ::Effekseer::Manager::Create");

		{
			spdlog::trace("OK : SetRenderers");

			{
				setting_ = Effekseer::Setting::Create();

				textureLoader_ = Effekseer::RefPtr<TextureLoader>(new TextureLoader(graphics_.get(),
																					isSRGBMode ? ::Effekseer::ColorSpaceType::Linear : ::Effekseer::ColorSpaceType::Gamma));

				setting_->SetTextureLoader(textureLoader_);

				modelLoader_ = Effekseer::RefPtr<ModelLoader>(new ModelLoader(graphics_.get()));

				setting_->SetModelLoader(modelLoader_);

				setting_->SetCurveLoader(Effekseer::CurveLoaderRef(new ::Effekseer::CurveLoader()));
			}

			spdlog::trace("OK : SetLoaders");
		}

		spdlog::trace("OK : AssignFunctions");

		mainScreen_ = std::make_shared<EffekseerTool::MainScreenRenderedEffectGenerator>();
		if (!mainScreen_->Initialize(graphics_, setting_, spriteCount, isSRGBMode))
		{
			graphics_.reset();
			spdlog::trace("End Native::CreateWindow_Effekseer (false)");

			return false;
		}

		if (!mainScreen_->InitializedPrePost())
		{
			graphics_.reset();
			spdlog::trace("End Native::CreateWindow_Effekseer (false)");

			return false;
		}
	}

	sound_ = new ::EffekseerTool::Sound();
	if (sound_->Initialize())
	{
		mainScreen_->GetMamanager()->SetSoundPlayer(sound_->GetSound()->CreateSoundPlayer());

		soundLoader_ = Effekseer::RefPtr<SoundLoader>(new SoundLoader(sound_->GetSound()->CreateSoundLoader()));
		setting_->SetSoundLoader(soundLoader_);
	}

	if (mainScreen_ != nullptr && sound_ != nullptr)
	{
		mainScreen_->SetSound(sound_);
	}

	materialLoader_ = Effekseer::RefPtr<MaterialLoader>(new MaterialLoader(mainScreen_->GetRenderer()));
	setting_->SetMaterialLoader(materialLoader_);

	spdlog::trace("End Native::CreateWindow_Effekseer (true)");

	return true;
}

void Native::ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	graphics_->Clear(Effekseer::Color(r, g, b, a));
}

bool Native::UpdateWindow(std::shared_ptr<Effekseer::Tool::ViewPointController> viewPointCtrl)
{
	const auto ray = viewPointCtrl->GetCameraRay();

	sound_->SetListener(ray.Origin, ray.Origin + ray.Direction, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
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
	InvalidateTextureCache();

	mainScreen_.reset();
	graphics_.reset();
	textureLoader_.Reset();
	setting_.Reset();
	materialLoader_.Reset();

	return true;
}

bool Native::LoadEffect(std::shared_ptr<Effekseer::Tool::Effect> effect)
{
	mainScreen_->SetEffect(nullptr);

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(effect->GetEffect());
	}

	return true;
}

bool Native::RemoveEffect()
{
	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(nullptr);
		mainScreen_->Reset();
	}

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

bool Native::SetRandomSeed(int seed)
{
	srand(seed);

	return true;
}

void Native::RenderView(int32_t width, int32_t height, std::shared_ptr<Effekseer::Tool::ViewPointController> viewPointCtrl, std::shared_ptr<Effekseer::Tool::RenderImage> renderImage)
{
	viewPointCtrl->SetScreenSize(width, height);
	viewPointCtrl->Update();

	const auto ray = viewPointCtrl->GetCameraRay();
	drawParameter.IsSortingEffectsEnabled = true;
	drawParameter.CameraPosition = ray.Origin;
	drawParameter.CameraFrontDirection = ray.Direction;

	Effekseer::Matrix44 vpm;
	Effekseer::Matrix44::Mul(vpm, viewPointCtrl->GetCameraMatrix(), viewPointCtrl->GetProjectionMatrix());
	drawParameter.ViewProjectionMatrix = vpm;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;

	mainScreenConfig_.DrawParameter = drawParameter;
	mainScreenConfig_.CameraMatrix = viewPointCtrl->GetCameraMatrix();
	mainScreenConfig_.ProjectionMatrix = viewPointCtrl->GetProjectionMatrix();
	mainScreenConfig_.RenderingMethod = renderingMode_;
	mainScreen_->SetConfig(mainScreenConfig_);
	mainScreen_->Resize(Effekseer::Tool::Vector2I(width, height));

	if (renderImage != nullptr)
	{
		renderImage->Resize(width, height);
	}

	mainScreen_->Render(renderImage);
}

std::shared_ptr<Effekseer::Tool::EffectRecorder> Native::CreateRecorder(const Effekseer::Tool::RecordingParameter& recordingParameter)
{
	if (mainScreen_->GetEffect() == nullptr)
		return nullptr;

	Effekseer::Tool::Vector2I screenSize = {mainScreen_->GetView()->GetParameter().Size[0], mainScreen_->GetView()->GetParameter().Size[1]};

	auto recorder = std::make_shared<Effekseer::Tool::EffectRecorder>();
	if (recorder->Begin(
			mainScreen_,
			mainScreen_->GetConfig(),
			screenSize,
			graphics_,
			setting_,
			recordingParameter,
			Effekseer::Tool::Vector2I(mainScreen_->GuideWidth, mainScreen_->GuideHeight),
			mainScreen_->GetIsSRGBMode(),
			mainScreen_->GetBehavior(),
			mainScreen_->GetEffect()))
	{
		return recorder;
	}

	return nullptr;
}

ViewerParamater Native::GetViewerParamater()
{
	ViewerParamater paramater;
	paramater.RendersGuide = mainScreen_->RendersGuide;
	paramater.GuideWidth = mainScreen_->GuideWidth;
	paramater.GuideHeight = mainScreen_->GuideHeight;
	paramater.Distortion = (Effekseer::Tool::DistortionType)mainScreenConfig_.Distortion;
	paramater.RenderingMode = renderingMode_;

	return paramater;
}

void Native::SetViewerParamater(ViewerParamater& paramater)
{
	mainScreen_->GuideWidth = paramater.GuideWidth;
	mainScreen_->GuideHeight = paramater.GuideHeight;
	mainScreen_->RendersGuide = paramater.RendersGuide;
	mainScreenConfig_.Distortion = (Effekseer::Tool::DistortionType)paramater.Distortion;
	renderingMode_ = paramater.RenderingMode;
}

Effekseer::Tool::ViewerEffectBehavior Native::GetEffectBehavior()
{
	return mainScreen_->GetBehavior();
}

void Native::SetViewerEffectBehavior(Effekseer::Tool::ViewerEffectBehavior behavior)
{
	if (behavior.CountX <= 0)
		behavior.CountX = 1;
	if (behavior.CountY <= 0)
		behavior.CountY = 1;
	if (behavior.CountZ <= 0)
		behavior.CountZ = 1;

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetBehavior(behavior);
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
			textureLoader_->GetOriginalTextureLoader()->Unload((*it).second);
			++it;
		}
		m_textures.clear();
	}

	{
		auto it = m_models.begin();
		auto it_end = m_models.end();
		while (it != it_end)
		{
			modelLoader_->Unload((*it).second);
			++it;
		}
		m_models.clear();
	}

	if (materialLoader_ != nullptr)
	{
		materialLoader_->ReleaseAll();
	}

	return true;
}

void Native::SetGroundParameters(bool shown, float height, int32_t extent)
{
	mainScreenConfig_.IsGroundShown = shown;
	mainScreenConfig_.GroundHeight = height;
	mainScreenConfig_.GroundExtent = extent;
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
	mainScreenConfig_.BackgroundColor = Effekseer::Color(r, g, b, 255);
}

void Native::SetBackgroundImage(const char16_t* path)
{
	mainScreen_->LoadBackgroundImage(path);
}

void Native::SetGridColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	mainScreen_->GridColor = Effekseer::Color(r, g, b, a);
}

void Native::SetStep(int32_t step)
{
	mainScreen_->SetStep(step);
}

void Native::SetLightDirection(float x, float y, float z)
{
	Effekseer::Vector3D temp = Effekseer::Vector3D(x, y, z);

	if (setting_->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH)
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

void Native::SetCoordinateSystem(Effekseer::Tool::CoordinateSystemType coordinateSystem)
{
	setting_->SetCoordinateSystem(coordinateSystem == Effekseer::Tool::CoordinateSystemType::RH ? Effekseer::CoordinateSystem::RH : Effekseer::CoordinateSystem::LH);

	{
		Effekseer::Vector3D temp = mainScreenConfig_.LightDirection;

		if (setting_->GetCoordinateSystem() == Effekseer::CoordinateSystem::LH)
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
			tonemap->SetParameters((Effekseer::Tool::TonemapPostEffect::Algorithm)algorithm, exposure);
		}
	}
}

void Native::OpenOrCreateMaterial(const char16_t* path)
{
	if (commandQueueToMaterialEditor_ == nullptr)
		return;

	char u8path[260];

	Effekseer::ConvertUtf16ToUtf8(u8path, 260, path);

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

std::shared_ptr<Effekseer::Tool::ReloadableImage> Native::CreateReloadableImage(const char16_t* path)
{
	auto loader = EffekseerRenderer::CreateTextureLoader(graphics_->GetGraphicsDevice());
	return std::make_shared<Effekseer::Tool::ReloadableImage>(path, loader);
}

std::shared_ptr<Effekseer::Tool::RenderImage> Native::CreateRenderImage()
{
	return std::make_shared<Effekseer::Tool::RenderImage>(graphics_->GetGraphicsDevice());
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
	Effekseer::ConvertUtf16ToUtf8(cpath, 512, path);
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

const EffekseerRenderer::RendererRef& Native::GetRenderer()
{
	return mainScreen_->GetRenderer();
}
