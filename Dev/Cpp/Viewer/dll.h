
#pragma once

/**
	@file
	@brief	DLL export for tool
*/

#include "EffekseerTool/EffekseerTool.Renderer.h"
#include "EffekseerTool/EffekseerTool.Sound.h"
#include "Recorder/RecordingParameter.h"
#include <Effekseer.h>
#include <IO/IO.h>
#include <unordered_map>
#include <unordered_set>

#include "../IPC/IPC.h"
#include "3D/ViewPointController.h"
#include "ViewerEffectBehavior.h"
#include "efk.Base.h"

namespace Effekseer
{
namespace Tool
{
class ReloadableImage;
class RenderImage;
class EffectRecorder;
class Effect;
} // namespace Tool
} // namespace Effekseer

enum class ViewMode
{
	_3D,
	_2D,
};

class ViewerParamater
{
public:
	int32_t GuideWidth;
	int32_t GuideHeight;
	bool RendersGuide;

	bool IsCullingShown;
	float CullingRadius;
	float CullingX;
	float CullingY;
	float CullingZ;

	Effekseer::Tool::DistortionType Distortion;
	Effekseer::Tool::RenderingMethodType RenderingMode;

	ViewerParamater();
};

class Native
{
private:
	std::shared_ptr<IPC::CommandQueue> commandQueueToMaterialEditor_;
	std::shared_ptr<IPC::CommandQueue> commandQueueFromMaterialEditor_;

	bool isUpdateMaterialRequired_ = false;

	std::shared_ptr<efk::Graphics> graphics_ = nullptr;

	Effekseer::SettingRef setting_;

	std::shared_ptr<EffekseerTool::MainScreenRenderedEffectGenerator> mainScreen_;

	Effekseer::Tool::RenderedEffectGeneratorConfig mainScreenConfig_;

	Effekseer::Tool::RenderingMethodType renderingMode_ = Effekseer::Tool::RenderingMethodType::Normal;

public:
	Native();

	~Native();

	bool CreateWindow_Effekseer(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType);

	bool UpdateWindow(std::shared_ptr<Effekseer::Tool::ViewPointController> viewPointCtrl);

	void ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void Present();

	bool ResizeWindow(int width, int height);

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

	ViewerParamater GetViewerParamater();

	void SetViewerParamater(ViewerParamater& paramater);

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

	void SetBloomParameters(bool enabled, float intensity, float threshold, float softKnee);

	void SetTonemapParameters(int32_t algorithm, float exposure);

	void OpenOrCreateMaterial(const char16_t* path);

	void TerminateMaterialEditor();

	bool GetIsUpdateMaterialRequiredAndReset();

	std::shared_ptr<Effekseer::Tool::ReloadableImage> CreateReloadableImage(const char16_t* path);

	std::shared_ptr<Effekseer::Tool::RenderImage> CreateRenderImage();

	static void SetFileLogger(const char16_t* path);

#if !SWIG
	Effekseer::SettingRef GetSetting()
	{
		return setting_;
	}

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice()
	{
		return graphics_->GetGraphicsDevice();
	}
#endif
};
