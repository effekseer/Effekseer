
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
#include "efk.Base.h"

enum class DistortionType
{
	Current,
	Effekseer120,
	Disabled,
};

enum class RenderMode
{
	Normal,
	Wireframe,
};

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

	DistortionType Distortion;
	RenderMode RenderingMode;
	ViewMode ViewerMode;

	ViewerParamater();
};

class ViewerEffectBehavior
{
public:
	int32_t CountX;
	int32_t CountY;
	int32_t CountZ;

	int32_t TimeSpan = 0;

	uint8_t AllColorR = 255;
	uint8_t AllColorG = 255;
	uint8_t AllColorB = 255;
	uint8_t AllColorA = 255;

	float Distance;

	int32_t RemovedTime;

	float PositionX;
	float PositionY;
	float PositionZ;

	float RotationX;
	float RotationY;
	float RotationZ;

	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float PositionVelocityX;
	float PositionVelocityY;
	float PositionVelocityZ;

	float RotationVelocityX;
	float RotationVelocityY;
	float RotationVelocityZ;

	float ScaleVelocityX;
	float ScaleVelocityY;
	float ScaleVelocityZ;

	float TargetPositionX;
	float TargetPositionY;
	float TargetPositionZ;

	float DynamicInput1 = 0.0f;
	float DynamicInput2 = 0.0f;
	float DynamicInput3 = 0.0f;
	float DynamicInput4 = 0.0f;

	float PlaybackSpeed = 1.0f;

	ViewerEffectBehavior();
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
	const char16_t* GetPath() const { return path.c_str(); }
	const char16_t* GetExt() const { return ext.c_str(); }
	void SetPath(const char16_t* value) { path = value; }
	void SetExt(const char16_t* value) { ext = value; }

	RecordingModeType RecordingMode;
	int32_t Count = 0;
	int32_t HorizontalCount = 0;
	int32_t OffsetFrame;
	int32_t Freq;
	TransparenceType Transparence;
};

class Native
{
private:
	class TextureLoader : public ::Effekseer::TextureLoader
	{
	private:
		EffekseerRenderer::Renderer* m_renderer;
		Effekseer::TextureLoader* m_originalTextureLoader;

	public:
		TextureLoader(EffekseerRenderer::Renderer* renderer, Effekseer::ColorSpaceType colorSpaceType);
		virtual ~TextureLoader();

	public:
		Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;

		void Unload(Effekseer::TextureData* data) override;

		Effekseer::TextureLoader* GetOriginalTextureLoader() const { return m_originalTextureLoader; }
		std::u16string RootPath;
	};

	class SoundLoader : public ::Effekseer::SoundLoader
	{
	private:
		::Effekseer::SoundLoader* m_loader;

	public:
		SoundLoader(Effekseer::SoundLoader* loader);
		virtual ~SoundLoader();

	public:
		void* Load(const EFK_CHAR* path);

		void Unload(void* data);

		std::u16string RootPath;
	};

	class ModelLoader : public ::Effekseer::ModelLoader
	{
	private:
		EffekseerRenderer::Renderer* m_renderer;

	public:
		ModelLoader(EffekseerRenderer::Renderer* renderer);
		virtual ~ModelLoader();

	public:
		void* Load(const EFK_CHAR* path);

		void Unload(void* data);

		std::u16string RootPath;
	};

	class MaterialLoader : public ::Effekseer::MaterialLoader
	{
	private:
		EffekseerRenderer::Renderer* renderer_ = nullptr;
		Effekseer::MaterialLoader* loader_ = nullptr;
		std::unordered_map<std::u16string, std::shared_ptr<Effekseer::StaticFile>> materialFiles_;

	public:
		MaterialLoader(EffekseerRenderer::Renderer* renderer);
		virtual ~MaterialLoader();

	public:
		Effekseer::MaterialData* Load(const EFK_CHAR* path) override;
		std::u16string RootPath;

		::Effekseer::MaterialLoader* GetOriginalLoader() { return loader_; }

		void ReleaseAll();
	};

	ViewerEffectBehavior m_effectBehavior;
	TextureLoader* m_textureLoader;

	int32_t m_time;

	int m_step;

	bool m_isSRGBMode = false;

	::Effekseer::Vector3D m_rootLocation;
	::Effekseer::Vector3D m_rootRotation;
	::Effekseer::Vector3D m_rootScale;

	std::shared_ptr<IPC::CommandQueue> commandQueueToMaterialEditor_;
	std::shared_ptr<IPC::CommandQueue> commandQueueFromMaterialEditor_;

	bool isUpdateMaterialRequired_ = false;

	::Effekseer::Effect* GetEffect();

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

	bool Record(RecordingParameter& recordingParameter);

	ViewerParamater GetViewerParamater();

	void SetViewerParamater(ViewerParamater& paramater);

	ViewerEffectBehavior GetEffectBehavior();

	void SetViewerEffectBehavior(ViewerEffectBehavior& behavior);

	bool SetSoundMute(bool mute);

	bool SetSoundVolume(float volume);

	bool InvalidateTextureCache();

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

	static void SetFileLogger(const char16_t* path);

#if !SWIG
	EffekseerRenderer::Renderer* GetRenderer();
#endif
};
