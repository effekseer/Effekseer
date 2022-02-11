
#pragma once

#include "3D/MainScreenEffectRenderer.h"
#include "Recorder/RecordingParameter.h"
#include <Effekseer.h>
#include <IO/IO.h>
#include <unordered_map>
#include <unordered_set>

#include "../IPC/IPC.h"
#include "3D/ViewPointController.h"
#include "Graphics/GraphicsDevice.h"
#include "Parameters.h"
#include "Sound/SoundDevice.h"

namespace Effekseer
{
namespace Tool
{
class ReloadableImage;
class RenderImage;
class EffectRecorder;
class Effect;
class EffectSetting;
class GraphicsDevice;
class SoundDevice;
} // namespace Tool
} // namespace Effekseer

class Native
{
private:
	std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice_ = nullptr;

	std::shared_ptr<Effekseer::Tool::EffectSetting> setting_;

	std::shared_ptr<Effekseer::Tool::MainScreenEffectRenderer> mainScreen_;

	Effekseer::Tool::EffectRendererParameter mainScreenConfig_;

	Effekseer::Tool::RenderingMethodType renderingMode_ = Effekseer::Tool::RenderingMethodType::Normal;

public:
	bool CreateWindow_Effekseer(
		std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice,
		std::shared_ptr<Effekseer::Tool::SoundDevice> soundDevice,
		std::shared_ptr<Effekseer::Tool::EffectSetting> effectSetting);

	bool DestroyWindow();

	bool LoadEffect(std::shared_ptr<Effekseer::Tool::Effect> effect);

	bool RemoveEffect();

	bool PlayEffect();

	bool StopEffect();

	bool StepEffect(int frame);

	bool StepEffect();

	bool SetRandomSeed(int seed);

	void RenderView(int32_t width, int32_t height, std::shared_ptr<Effekseer::Tool::ViewPointController> viewPointCtrl, std::shared_ptr<Effekseer::Tool::RenderImage> renderImage);

	std::shared_ptr<Effekseer::Tool::EffectRecorder> CreateRecorder(const Effekseer::Tool::RecordingParameter& recordingParameter);

	Effekseer::Tool::ViewerParamater GetViewerParamater();

	void SetViewerParamater(Effekseer::Tool::ViewerParamater& paramater);

	Effekseer::Tool::ViewerEffectBehavior GetEffectBehavior();

	void SetViewerEffectBehavior(Effekseer::Tool::ViewerEffectBehavior behavior);

	void SetGroundParameters(bool shown, float height, int32_t extent);

	void SetIsGridShown(bool value, bool xy, bool xz, bool yz);

	void SetGridLength(float length);

	void SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b);

	void SetBackgroundImage(const char16_t* path);

	void SetGridColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetStep(int32_t step);

	void SetLightDirection(float x, float y, float z);

	void SetLightColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetLightAmbientColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetCoordinateSystem(Effekseer::Tool::CoordinateSystemType coordinateSystem);

	void SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ);

	int32_t GetAndResetDrawCall();

	int32_t GetAndResetVertexCount();

	int32_t GetInstanceCount();

	float GetFPS();

	static bool IsDebugMode();

	void SetPostEffectParameter(const Effekseer::Tool::PostEffectParameter& parameter);

#if !SWIG
	std::shared_ptr<Effekseer::Tool::EffectSetting> GetSetting()
	{
		return setting_;
	}

	std::shared_ptr<Effekseer::Tool::GraphicsDevice> GetGraphicsDevice()
	{
		return graphicsDevice_;
	}
#endif
};
