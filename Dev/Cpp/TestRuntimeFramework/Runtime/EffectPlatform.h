#pragma once

#include <Effekseer.h>
#include <array>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

enum class BackgroundPatternType
{
	Checkered,
	NonPeriodicGradient,
};

struct EffectPlatformInitializingParameter
{
	bool VSync = true;
	bool IsUpdatedByHandle = false;
	bool IsCullingCreated = true;
	int InstanceCount = 8000;
	int SpriteCount = 2000;
	::Effekseer::CoordinateSystem CoordinateSyatem = ::Effekseer::CoordinateSystem::RH;
	std::array<int32_t, 2> WindowSize = {320, 240};
	BackgroundPatternType BackgroundPattern = BackgroundPatternType::Checkered;
};

struct EffectPlatformMeasuredTime
{
	int32_t ManagerUpdate = 0;
	int32_t Compute = 0;
	int32_t PlatformBeginRendering = 0;
	int32_t RendererBegin = 0;
	int32_t ManagerDraw = 0;
	int32_t ManagerDrawWorkerThreadWait = 0;
	int32_t ManagerDrawMutexLock = 0;
	int32_t ManagerDrawCulling = 0;
	int32_t ManagerDrawSorting = 0;
	int32_t ManagerDrawDrawSets = 0;
	int32_t ManagerDrawGpuParticles = 0;
	int32_t ManagerDrawTotal = 0;
	int32_t RendererEnd = 0;
	int32_t PlatformEndRendering = 0;
	int32_t FrameWithoutPresent = 0;
	int32_t Present = 0;
	int32_t Frame = 0;
};

class EffectPlatform
{
private:
	bool isInitialized_ = false;
	bool isTerminated_ = false;
	float time_ = 0;

	Effekseer::ManagerRef manager_;
	EffekseerRenderer::RendererRef renderer_ = nullptr;
	std::vector<Effekseer::Handle> effectHandles_;

	void CreateBackgroundPattern(int width, int height, uint32_t* pixels);

protected:
	bool isOpenGLMode_ = false;
	EffectPlatformInitializingParameter initParam_;

protected:
	std::vector<Effekseer::EffectRef> effects_;
	std::vector<std::vector<uint8_t>> buffers_;
	std::vector<uint32_t> checkeredPattern_;
	bool isBackgroundFlipped_ = false;
	bool isGroundDepthEnabled_ = false;

	struct GroundPlaneVertex
	{
		std::array<float, 4> Pos;
		std::array<float, 2> WorldXZ;
	};

	std::array<GroundPlaneVertex, 4> CreateGroundPlaneVertices() const;
	std::array<uint16_t, 6> CreateGroundPlaneIndices() const;
	std::array<std::array<float, 4>, 4> CreateGroundViewProjectionColumns() const;
	EffekseerRenderer::DepthReconstructionParameter CreateGroundDepthReconstructionParameter(float depthBufferScale = 1.0f, float depthBufferOffset = 0.0f) const;

	virtual void UpdateBackgroundTexture()
	{
	}

	virtual void* GetNativePtr(int32_t index)
	{
		return nullptr;
	}
	virtual EffekseerRenderer::RendererRef CreateRenderer()
	{
		return nullptr;
	}

	virtual void InitializeWindow()
	{
	}

	virtual void InitializeDevice(const EffectPlatformInitializingParameter& param)
	{
	}
	virtual void PreDestroyDevice()
	{
	}
	virtual void DestroyDevice()
	{
	}
	virtual void BeginCompute()
	{
	}
	virtual void EndCompute()
	{
	}
	virtual void BeginRendering()
	{
	}
	virtual void EndRendering()
	{
	}
	virtual void Present()
	{
	}
	virtual bool DoEvent()
	{
		return true;
	}

public:
	EffectPlatform();
	virtual ~EffectPlatform();

	void Initialize(const EffectPlatformInitializingParameter& param);
	void Terminate();
	virtual void ResetBackgroundPattern();

	Effekseer::Handle Play(const char16_t* path, Effekseer::Vector3D position = Effekseer::Vector3D(), int32_t startFrame = 0);

	bool Update(EffectPlatformMeasuredTime* measuredTime = nullptr);

	bool Draw();

	void StopAllEffects();
	void ClearLoadedEffects();

	virtual bool TakeScreenshot(const char* path)
	{
		return false;
	}

	virtual bool SetFullscreen(bool isFullscreen)
	{
		return false;
	}

	Effekseer::ManagerRef GetManager() const
	{
		return manager_;
	}

	EffekseerRenderer::RendererRef GetRenderer() const;

	virtual void GenerateGroundDepth();

	const std::vector<Effekseer::EffectRef>& GetEffects() const
	{
		return effects_;
	}
};
