
#include "Graphics/efk.AVIExporter.h"
#include "Graphics/efk.GifHelper.h"
#include "Graphics/efk.PNGHelper.h"
#include "Recorder/Recorder.h"
#include "RenderedEffectGenerator.h"
#define NOMINMAX

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

#include "Effekseer/Effekseer.EffectImplemented.h"
#include "Effekseer/Effekseer.EffectNode.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class EditorEffectNodeUserData : public ::Effekseer::RenderingUserData
{
public:
	// Identifier to use when referring to a node from the editor.
	int32_t editorNodeId_ = 0;


	static ::Effekseer::EffectNodeImplemented* FindNodeByEditorNodeId(::Effekseer::EffectImplemented* effect, int32_t editorNodeId)
	{
		auto* root = effect->GetRoot();
		if (!root) return nullptr;

		::Effekseer::EffectNodeImplemented* result = nullptr;

		const auto& visitor = [&](::Effekseer::EffectNodeImplemented* node) -> bool {
			const auto userData = node->GetRenderingUserData();
			if (userData != nullptr) {
				const auto* editorUserData = static_cast<EditorEffectNodeUserData*>(userData.Get());

				if (editorUserData->editorNodeId_ == editorNodeId) {
					result = node;
					return false;
				}
			}
			return true;
		};

		static_cast<::Effekseer::EffectNodeImplemented*>(root)->Traverse(visitor);

		return result;
	}
};

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

void Native::SetZoom(float zoom)
{
	g_Zoom = Effekseer::Max(MinZoom, Effekseer::Min(MaxZoom, zoom));
}

float Native::GetDistance()
{
	return DistanceBase * powf(ZoomDistanceFactor, g_Zoom);
}

float Native::GetOrthoScale()
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
	, RenderingMode(Effekseer::Tool::RenderingMethodType::Normal)
	, ViewerMode(ViewMode::_3D)

{
}

static Effekseer::Manager::DrawParameter drawParameter;

static ::EffekseerTool::Sound* sound_ = nullptr;
static std::map<std::u16string, Effekseer::TextureRef> m_textures;
static std::map<std::u16string, Effekseer::ModelRef> m_models;
static std::map<std::u16string, std::shared_ptr<efk::ImageResource>> g_imageResources;
static std::map<std::u16string, Effekseer::MaterialRef> g_materials_;

static ::Effekseer::Vector3D g_focus_position;

static ::Effekseer::Client* g_client = nullptr;

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
	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, dst, 260);

	std::u16string key(dst);

	if (m_textures.count(key) > 0)
	{
		return m_textures[key];
	}
	else
	{
		auto t = m_originalTextureLoader->Load((char16_t*)dst, textureType);

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
	// m_originalTextureLoader->Unload(data);
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
	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, (char16_t*)dst, 260);

	return m_loader->Load(dst);
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
	char16_t dst[260];
	Combine(RootPath.c_str(), (const char16_t*)path, dst, 260);

	std::u16string key(dst);
	Effekseer::ModelRef model = nullptr;

	if (m_models.count(key) > 0)
	{
		return m_models[key];
	}
	else
	{
		auto loader = ::EffekseerRenderer::CreateModelLoader(graphics_->GetGraphicsDevice());
		auto m = loader->Load((const char16_t*)dst);

		if (m != nullptr)
		{
			m_models[key] = m;
		}

		return m;
	}
}

void Native::ModelLoader::Unload(Effekseer::ModelRef data)
{
	/*
	if( data != nullptr )
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
		texture->Release();
	}
	*/
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
		::Effekseer::MaterialRef t = nullptr;

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

::Effekseer::EffectRef Native::GetEffect()
{
	return effect_;
}

Native::Native()
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

	if (!graphics_->Initialize(pHandle, width, height))
	{
		spdlog::trace("Graphics::Initialize(false)");
		ES_SAFE_DELETE(graphics_);
		return false;
	}

	viewPointCtrl_.Initialize(deviceType, width, height);
	{
		spdlog::trace("OK : ::EffekseerTool::Renderer::Initialize");

		//manager_ = ::Effekseer::Manager::Create(spriteCount);
		spdlog::trace("OK : ::Effekseer::Manager::Create");

		{
			spdlog::trace("OK : SetRenderers");

			{
				setting_ = Effekseer::Setting::Create();

				textureLoader_ = Effekseer::RefPtr<TextureLoader>(new TextureLoader(graphics_,
																					isSRGBMode ? ::Effekseer::ColorSpaceType::Linear : ::Effekseer::ColorSpaceType::Gamma));

				setting_->SetTextureLoader(textureLoader_);

				modelLoader_ = Effekseer::RefPtr<ModelLoader>(new ModelLoader(graphics_));

				setting_->SetModelLoader(modelLoader_);

				setting_->SetCurveLoader(Effekseer::CurveLoaderRef(new ::Effekseer::CurveLoader()));
			}

			//manager_->SetSetting(setting_);

			spdlog::trace("OK : SetLoaders");
		}

		spdlog::trace("OK : AssignFunctions");

		mainScreen_ = std::make_shared<EffekseerTool::MainScreenRenderedEffectGenerator>();
		if (!mainScreen_->Initialize(graphics_, setting_, spriteCount, isSRGBMode))
		{
			ES_SAFE_DELETE(graphics_);
			spdlog::trace("End Native::CreateWindow_Effekseer (false)");

			return false;
		}

		if (!mainScreen_->InitializedPrePost())
		{
			ES_SAFE_DELETE(graphics_);
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
	mainScreen_->GetMamanager()->GetSetting()->SetMaterialLoader(materialLoader_);

	spdlog::trace("End Native::CreateWindow_Effekseer (true)");

	return true;
}

void Native::ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	graphics_->Clear(Effekseer::Color(r, g, b, a));
}

bool Native::UpdateWindow()
{
	::Effekseer::Vector3D position(0, 0, GetDistance());
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);

	drawParameter.IsSortingEffectsEnabled = true;

	if (viewPointCtrl_.IsRightHand)
	{
		mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		Effekseer::Vector3D::Normal(drawParameter.CameraFrontDirection, -position);

		position.X += g_focus_position.X;
		position.Y += g_focus_position.Y;
		position.Z += g_focus_position.Z;

		::Effekseer::Matrix44 cameraMat;
		viewPointCtrl_.SetCameraMatrix(
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

		Effekseer::Vector3D::Normal(drawParameter.CameraFrontDirection, -position);

		position.X += temp_focus.X;
		position.Y += temp_focus.Y;
		position.Z += temp_focus.Z;

		::Effekseer::Matrix44 cameraMat;
		viewPointCtrl_.SetCameraMatrix(
			::Effekseer::Matrix44().LookAtLH(position, temp_focus, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		drawParameter.CameraPosition = position;
	}

	viewPointCtrl_.SetOrthographicScale(GetOrthoScale());

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
	InvalidateTextureCache();

	g_imageResources.clear();

	mainScreen_.reset();

	ES_SAFE_DELETE(graphics_);

	textureLoader_.Reset();
	effect_.Reset();
	setting_.Reset();
	materialLoader_.Reset();

	return true;
}

bool Native::LoadEffect(void* pData, int size, const char16_t* Path)
{
	assert(effect_ == nullptr);

	//handles_.clear();

	if (textureLoader_ != nullptr)
		textureLoader_->RootPath = std::u16string(Path);
	if (modelLoader_ != nullptr)
		modelLoader_->RootPath = std::u16string(Path);
	if (materialLoader_ != nullptr)
		materialLoader_->RootPath = std::u16string(Path);
	if (soundLoader_ != nullptr)
		soundLoader_->RootPath = std::u16string(Path);

	// To release caches
	if (effect_ != nullptr)
	{
		effect_->UnloadResources();
	}

	effect_ = Effekseer::Effect::Create(setting_, pData, size);
	assert(effect_ != nullptr);

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(effect_);
	}

	// Create UserData while assigning NodeId.
	{
		int nextEditorNodeId = 1;
		const auto& visitor = [&](::Effekseer::EffectNodeImplemented* node) {
			auto userData = ::Effekseer::MakeRefPtr<EditorEffectNodeUserData>();
			userData->editorNodeId_ = nextEditorNodeId;
			node->SetRenderingUserData(userData);
			nextEditorNodeId++;
			return true;
		};
		static_cast<::Effekseer::EffectNodeImplemented*>(effect_->GetRoot())->Traverse(visitor);
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

	// To release caches
	if (effect_ != nullptr)
	{
		effect_->UnloadResources();
	}

	effect_.Reset();

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
	if (g_mouseSlideDirectionInvX)
	{
		x = -x;
	}

	if (g_mouseSlideDirectionInvY)
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
	viewPointCtrl_.SetScreenSize(width, height);
	mainScreenConfig_.DrawParameter = drawParameter;
	mainScreenConfig_.CameraMatrix = viewPointCtrl_.GetCameraMatrix();
	mainScreenConfig_.ProjectionMatrix = viewPointCtrl_.GetProjectionMatrix();
	mainScreenConfig_.RenderingMethod = viewPointCtrl_.RenderingMode;
	mainScreen_->SetConfig(mainScreenConfig_);
	mainScreen_->Resize(Effekseer::Tool::Vector2DI(width, height));
	mainScreen_->Render();
	return (void*)mainScreen_->GetView()->GetViewID();
}

bool Native::BeginRecord(const RecordingParameter& recordingParameter)
{
	if (effect_ == nullptr)
		return false;

	recorder.reset(new Effekseer::Tool::Recorder());
	return recorder->Begin(mainScreen_, graphics_, setting_, recordingParameter, Effekseer::Tool::Vector2DI(mainScreen_->GuideWidth, mainScreen_->GuideHeight), mainScreen_->GetIsSRGBMode(), behavior_, effect_);
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
	ViewerParamater paramater;

	paramater.GuideWidth = mainScreen_->GuideWidth;
	paramater.GuideHeight = mainScreen_->GuideHeight;
	paramater.ClippingStart = viewPointCtrl_.ClippingStart;
	paramater.ClippingEnd = viewPointCtrl_.ClippingEnd;
	paramater.IsPerspective = viewPointCtrl_.GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE;
	paramater.IsOrthographic = viewPointCtrl_.GetProjectionType() == ::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC;
	paramater.FocusX = g_focus_position.X;
	paramater.FocusY = g_focus_position.Y;
	paramater.FocusZ = g_focus_position.Z;
	paramater.AngleX = g_RotX;
	paramater.AngleY = g_RotY;
	paramater.Distance = GetDistance();
	paramater.RendersGuide = mainScreen_->RendersGuide;
	paramater.RateOfMagnification = viewPointCtrl_.RateOfMagnification;

	paramater.Distortion = (Effekseer::Tool::DistortionType)mainScreenConfig_.Distortion;
	paramater.RenderingMode = viewPointCtrl_.RenderingMode;

	return paramater;
}

void Native::SetViewerParamater(ViewerParamater& paramater)
{
	viewPointCtrl_.ClippingStart = paramater.ClippingStart;
	viewPointCtrl_.ClippingEnd = paramater.ClippingEnd;
	mainScreen_->GuideWidth = paramater.GuideWidth;
	mainScreen_->GuideHeight = paramater.GuideHeight;

	viewPointCtrl_.RateOfMagnification = paramater.RateOfMagnification;

	if (paramater.IsPerspective)
	{
		viewPointCtrl_.SetProjectionType(::EffekseerTool::PROJECTION_TYPE_PERSPECTIVE);
	}

	if (paramater.IsOrthographic)
	{
		viewPointCtrl_.SetProjectionType(::EffekseerTool::PROJECTION_TYPE_ORTHOGRAPHIC);
	}

	g_focus_position.X = paramater.FocusX;
	g_focus_position.Y = paramater.FocusY;
	g_focus_position.Z = paramater.FocusZ;
	g_RotX = paramater.AngleX;
	g_RotY = paramater.AngleY;

	SetZoom(logf(Effekseer::Max(FLT_MIN, paramater.Distance / DistanceBase)) / logf(ZoomDistanceFactor));

	mainScreen_->RendersGuide = paramater.RendersGuide;
	mainScreenConfig_.Distortion = (Effekseer::Tool::DistortionType)paramater.Distortion;
	viewPointCtrl_.RenderingMode = paramater.RenderingMode;
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

void Native::SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
{
	g_mouseRotDirectionInvX = rotX;
	g_mouseRotDirectionInvY = rotY;

	g_mouseSlideDirectionInvX = slideX;
	g_mouseSlideDirectionInvY = slideY;
}

void Native::SetStep(int32_t step)
{
	mainScreen_->SetStep(step);
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
	g_client->Reload((const char16_t*)key, data, size);
}

void Native::SetLightDirection(float x, float y, float z)
{
	Effekseer::Vector3D temp = Effekseer::Vector3D(x, y, z);

	if (!viewPointCtrl_.IsRightHand)
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
	viewPointCtrl_.IsRightHand = value;
	if (viewPointCtrl_.IsRightHand)
	{
		setting_->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
	}
	else
	{
		setting_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	}

	viewPointCtrl_.RecalcProjection();

	{
		Effekseer::Vector3D temp = mainScreenConfig_.LightDirection;

		if (!viewPointCtrl_.IsRightHand)
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

	auto loader = EffekseerRenderer::CreateTextureLoader(graphics_->GetGraphicsDevice());
	auto resource = std::make_shared<efk::ImageResource>(g_deviceType, loader);
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

bool Native::GetNodeLifeTimes(int32_t nodeId, int32_t* frameMin, int32_t* frameMax)
{
	if (!effect_.Get()) return false;

	if (auto* effect = dynamic_cast<Effekseer::EffectImplemented*>(effect_.Get())) {
		if (auto* node = EditorEffectNodeUserData::FindNodeByEditorNodeId(effect, nodeId)) {
			Effekseer::EffectInstanceTerm term;
			auto cterm = node->CalculateInstanceTerm(term);
			*frameMin = cterm.FirstInstanceStartMin;
			*frameMax = cterm.LastInstanceEndMax;
			return true;
		}
	}

	return false;
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
