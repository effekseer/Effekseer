
#pragma once

/**
	@file
	@brief	DLL export for tool
*/

#include "EffekseerTool/EffekseerTool.Renderer.h"
#include "EffekseerTool/EffekseerTool.Sound.h"
#include <Effekseer.h>
#include <IO/IO.h>
#include <unordered_map>
#include <unordered_set>

#include "../IPC/IPC.h"
#include "GUI/efk.ImageResource.h"
#include "ViewerEffectBehavior.h"
#include "efk.Base.h"

namespace Effekseer
{
namespace Tool
{
class Recorder;
}
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
	float RateOfMagnification;
	bool IsPerspective;
	bool IsOrthographic;
	float FocusX;
	float FocusY;
	float FocusZ;
	float AngleX;
	float AngleY;
	float Distance;
	float ClippingStart;
	float ClippingEnd;
	bool RendersGuide;

	bool IsCullingShown;
	float CullingRadius;
	float CullingX;
	float CullingY;
	float CullingZ;

	Effekseer::Tool::DistortionType Distortion;
	Effekseer::Tool::RenderingMethodType RenderingMode;
	ViewMode ViewerMode;

	ViewerParamater();
};

enum class RecordingModeType
{
	Sprite,
	SpriteSheet,
	Gif,
	Avi,
};

enum class TransparenceType
{
	None = 0,
	Original = 1,
	Generate = 2,
	Generate2 = 3,
};

class RecordingParameter
{
	std::u16string path;
	std::u16string ext;

public:
	const char16_t* GetPath() const
	{
		return path.c_str();
	}
	const char16_t* GetExt() const
	{
		return ext.c_str();
	}
	void SetPath(const char16_t* value)
	{
		path = value;
	}
	void SetExt(const char16_t* value)
	{
		ext = value;
	}

	RecordingModeType RecordingMode;
	int32_t Count = 0;
	int32_t HorizontalCount = 0;
	int32_t OffsetFrame;
	int32_t Freq;
	TransparenceType Transparence;
	int32_t Scale = 1;
};

class Native
{
private:
	class TextureLoader : public ::Effekseer::TextureLoader
	{
	private:
		Effekseer::TextureLoaderRef m_originalTextureLoader;

	public:
		TextureLoader(efk::Graphics* graphics, Effekseer::ColorSpaceType colorSpaceType);
		virtual ~TextureLoader();

	public:
		Effekseer::TextureRef Load(const char16_t* path, ::Effekseer::TextureType textureType) override;

		void Unload(Effekseer::TextureRef data) override;

		Effekseer::TextureLoaderRef GetOriginalTextureLoader() const
		{
			return m_originalTextureLoader;
		}
		std::u16string RootPath;
	};

	class SoundLoader : public ::Effekseer::SoundLoader
	{
	private:
		::Effekseer::SoundLoaderRef m_loader;

	public:
		SoundLoader(Effekseer::SoundLoaderRef loader);
		virtual ~SoundLoader();

	public:
		::Effekseer::SoundDataRef Load(const char16_t* path) override;

		void Unload(::Effekseer::SoundDataRef soundData) override;

		std::u16string RootPath;
	};

	class ModelLoader : public ::Effekseer::ModelLoader
	{
	private:
		efk::Graphics* graphics_ = nullptr;

	public:
		ModelLoader(efk::Graphics* graphics);
		virtual ~ModelLoader();

	public:
		Effekseer::ModelRef Load(const char16_t* path);

		void Unload(Effekseer::ModelRef data);

		std::u16string RootPath;
	};

	class MaterialLoader : public ::Effekseer::MaterialLoader
	{
	private:
		Effekseer::MaterialLoaderRef loader_ = nullptr;
		std::unordered_map<std::u16string, std::shared_ptr<Effekseer::StaticFile>> materialFiles_;

	public:
		MaterialLoader(const EffekseerRenderer::RendererRef& renderer);
		virtual ~MaterialLoader();

	public:
		Effekseer::MaterialRef Load(const char16_t* path) override;
		std::u16string RootPath;

		::Effekseer::MaterialLoaderRef GetOriginalLoader()
		{
			return loader_;
		}

		void ReleaseAll();
	};

	const float DistanceBase = 15.0f;
	const float OrthoScaleBase = 16.0f;
	const float ZoomDistanceFactor = 1.125f;
	const float MaxZoom = 40.0f;
	const float MinZoom = -40.0f;
	const float PI = 3.14159265f;

	float g_RotX = 30.0f;
	float g_RotY = -30.0f;
	float g_Zoom = 0.0f;

	bool g_mouseRotDirectionInvX = false;
	bool g_mouseRotDirectionInvY = false;

	bool g_mouseSlideDirectionInvX = false;
	bool g_mouseSlideDirectionInvY = false;

	Effekseer::Tool::ViewerEffectBehavior behavior_;

	::Effekseer::EffectRef effect_ = nullptr;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;

	std::shared_ptr<IPC::CommandQueue> commandQueueToMaterialEditor_;
	std::shared_ptr<IPC::CommandQueue> commandQueueFromMaterialEditor_;

	bool isUpdateMaterialRequired_ = false;

	std::unique_ptr<Effekseer::Tool::Recorder> recorder;

	efk::Graphics* graphics_ = nullptr;

	Effekseer::RefPtr<Effekseer::Setting> setting_;

	Effekseer::RefPtr<TextureLoader> textureLoader_;

	Effekseer::RefPtr<MaterialLoader> materialLoader_;

	Effekseer::RefPtr<ModelLoader> modelLoader_;

	Effekseer::RefPtr<SoundLoader> soundLoader_;

	std::shared_ptr<EffekseerTool::MainScreenRenderedEffectGenerator> mainScreen_;

	Effekseer::Tool::RenderedEffectGeneratorConfig mainScreenConfig_;

	EffekseerTool::ViewPointController viewPointCtrl_;

	::Effekseer::EffectRef GetEffect();

	void SetZoom(float zoom);

	float GetDistance();

	float GetOrthoScale();

public:
	Native();

	~Native();

	bool CreateWindow_Effekseer(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType);

	bool UpdateWindow();

	void ClearWindow(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void RenderWindow();

	void Present();

	bool ResizeWindow(int width, int height);

	bool DestroyWindow();

	bool LoadEffect(void* data, int size, const char16_t* path);

	bool RemoveEffect();

	bool PlayEffect();

	bool StopEffect();

	bool StepEffect(int frame);

	bool StepEffect();

	bool Rotate(float x, float y);

	bool Slide(float x, float y);

	bool Zoom(float zoom);

	bool SetRandomSeed(int seed);

	void* RenderView(int32_t width, int32_t height);

	bool BeginRecord(const RecordingParameter& recordingParameter);

	bool StepRecord(int frames);

	bool EndRecord();

	bool IsRecording() const;

	float GetRecordingProgress() const;

	bool IsRecordCompleted() const;

	bool Record(const RecordingParameter& recordingParameter);

	ViewerParamater GetViewerParamater();

	void SetViewerParamater(ViewerParamater& paramater);

	Effekseer::Tool::ViewerEffectBehavior GetEffectBehavior();

	void SetViewerEffectBehavior(Effekseer::Tool::ViewerEffectBehavior& behavior);

	bool SetSoundMute(bool mute);

	bool SetSoundVolume(float volume);

	bool InvalidateTextureCache();

	void SetGroundParameters(bool shown, float height, int32_t extent);

	void SetIsGridShown(bool value, bool xy, bool xz, bool yz);

	void SetGridLength(float length);

	void SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b);

	void SetBackgroundImage(const char16_t* path);

	void SetGridColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY);

	void SetStep(int32_t step);

	bool StartNetwork(const char* host, uint16_t port);

	void StopNetwork();

	bool IsConnectingNetwork();

	void SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path);

	void SetLightDirection(float x, float y, float z);

	void SetLightColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetLightAmbientColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void SetIsRightHand(bool value);

	void SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ);

	efk::ImageResource* LoadImageResource(const char16_t* path);

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

	bool GetNodeLifeTimes(int32_t nodeId, int32_t* frameMin, int32_t* frameMax);

	static void SetFileLogger(const char16_t* path);

#if !SWIG
	const EffekseerRenderer::RendererRef& GetRenderer();
#endif
};
