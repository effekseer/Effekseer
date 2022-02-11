
#include "3D/EffectRenderer.h"
#include "Recorder/EffectRecorder.h"

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

#include "GUI/Image.h"
#include "GUI/ReloadableImage.h"
#include "GUI/RenderImage.h"

#include "3D/Effect.h"
#include "3D/EffectSetting.h"

#include <EffekseerSoundOSMixer.h>

bool Native::CreateWindow_Effekseer(
	std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice,
	std::shared_ptr<Effekseer::Tool::SoundDevice> soundDevice,
	std::shared_ptr<Effekseer::Tool::EffectSetting> effectSetting)
{
	spdlog::trace("Begin Native::CreateWindow_Effekseer");

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	graphicsDevice_ = graphicsDevice;
	setting_ = effectSetting->GetSetting();

	{
		mainScreen_ = std::make_shared<Effekseer::Tool::MainScreenEffectRenderer>();
		if (!mainScreen_->Initialize(graphicsDevice_->GetGraphics(), setting_, spriteCount, graphicsDevice->GetIsSRGBMode()))
		{
			spdlog::trace("End Native::CreateWindow_Effekseer (false)");
			return false;
		}
	}

	if (soundDevice != nullptr)
	{
		mainScreen_->GetMamanager()->SetSoundPlayer(soundDevice->GetSound()->CreateSoundPlayer());
	}

	spdlog::trace("End Native::CreateWindow_Effekseer (true)");

	return true;
}

bool Native::DestroyWindow()
{
	mainScreen_.reset();
	setting_.Reset();

	return true;
}

bool Native::LoadEffect(std::shared_ptr<Effekseer::Tool::Effect> effect)
{
	mainScreen_->SetEffect(nullptr);

	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(effect);
	}

	return true;
}

bool Native::RemoveEffect()
{
	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetEffect(nullptr);
		mainScreen_->ResetEffect();
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
	mainScreen_->ResetEffect();
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
	mainScreenConfig_.CameraMatrix = viewPointCtrl->GetCameraMatrix();
	mainScreenConfig_.ProjectionMatrix = viewPointCtrl->GetProjectionMatrix();
	mainScreenConfig_.RenderingMethod = renderingMode_;
	mainScreenConfig_.CameraPosition = ray.Origin;
	mainScreenConfig_.CameraFrontDirection = ray.Direction;

	mainScreen_->SetConfig(mainScreenConfig_);
	mainScreen_->ResizeScreen(Effekseer::Tool::Vector2I(width, height));

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

	const auto screenSize = mainScreen_->GetScreenSize();

	auto recorder = std::make_shared<Effekseer::Tool::EffectRecorder>();
	if (recorder->Begin(
			mainScreen_->GetRenderer()->GetSquareMaxCount(),
			mainScreen_->GetConfig(),
			screenSize,
			graphicsDevice_,
			setting_,
			recordingParameter,
			Effekseer::Tool::Vector2I(mainScreen_->GuideWidth, mainScreen_->GuideHeight),
			mainScreen_->GetIsSRGBMode(),
			mainScreen_->GetBehavior(),
			mainScreen_->GetPostEffectParameter(),
			mainScreen_->GetEffect()))
	{
		return recorder;
	}

	return nullptr;
}

Effekseer::Tool::ViewerParamater Native::GetViewerParamater()
{
	Effekseer::Tool::ViewerParamater paramater;
	paramater.RendersGuide = mainScreen_->RendersGuide;
	paramater.GuideWidth = mainScreen_->GuideWidth;
	paramater.GuideHeight = mainScreen_->GuideHeight;
	paramater.Distortion = (Effekseer::Tool::DistortionType)mainScreenConfig_.Distortion;
	paramater.RenderingMode = renderingMode_;

	return paramater;
}

void Native::SetViewerParamater(Effekseer::Tool::ViewerParamater& paramater)
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
	return 60.0f;
}

bool Native::IsDebugMode()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

void Native::SetPostEffectParameter(const Effekseer::Tool::PostEffectParameter& parameter)
{
	if (mainScreen_ != nullptr)
	{
		mainScreen_->SetPostEffectParameter(parameter);
	}
}
