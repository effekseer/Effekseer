#pragma once

#include <Effekseer.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"

struct EffectPlatformInitializingParameter
{
	bool VSync = true;
	bool IsUpdatedByHandle = false;
	bool IsCullingCreated = true;
	int InstanceCount = 8000;
	std::array<int32_t, 2> WindowSize = {320, 240};
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

	void CreateCheckeredPattern(int width, int height, uint32_t* pixels);

protected:
	bool isOpenGLMode_ = false;
	EffectPlatformInitializingParameter initParam_;

protected:
	std::vector<Effekseer::EffectRef> effects_;
	std::vector<std::vector<uint8_t>> buffers_;
	std::vector<uint32_t> checkeredPattern_;
	bool isBackgroundFlipped_ = false;

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

	Effekseer::Handle Play(const char16_t* path, Effekseer::Vector3D position = Effekseer::Vector3D(), int32_t startFrame = 0);

	bool Update();

	bool Draw();

	void StopAllEffects();

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

	void GenerateDepth()
	{
		auto projMat = renderer_->GetProjectionMatrix();
		auto cameraMat = renderer_->GetCameraMatrix();

		Effekseer::Vector3D posMiddle{0.0f, 0.0f, 0.0f};

		Effekseer::Vector3D::TransformWithW(posMiddle, posMiddle, cameraMat);
		Effekseer::Vector3D::TransformWithW(posMiddle, posMiddle, projMat);

		std::array<float, 4> posMiddleArray;
		posMiddleArray.fill(1.0f);
		posMiddleArray[0] = posMiddle.Z;

		Effekseer::Vector3D posNear{0.0f, 0.0f, 1.0f};

		Effekseer::Vector3D::TransformWithW(posNear, posNear, cameraMat);
		Effekseer::Vector3D::TransformWithW(posNear, posNear, projMat);

		std::array<float, 4> posNearArray;
		posNearArray.fill(1.0f);
		posNearArray[0] = posNear.Z;

		{
			const size_t heightSize = 10;

			Effekseer::Backend::TextureParameter texParam;
			texParam.InitialData.resize(sizeof(float) * 4 * heightSize);
			texParam.Format = Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT;
			texParam.Size = {1, heightSize};

			for (size_t i = 0; i < heightSize; i++)
			{
				if ((i % 2 == 0 && !isBackgroundFlipped_) || (i % 2 == 1 && isBackgroundFlipped_))
				{
					memcpy(texParam.InitialData.data() + sizeof(float) * 4 * i, posMiddleArray.data(), sizeof(float) * 4);
				}
				else
				{
					memcpy(texParam.InitialData.data() + sizeof(float) * 4 * i, posNearArray.data(), sizeof(float) * 4);
				}
			}

			auto depth = GetRenderer()->GetGraphicsDevice()->CreateTexture(texParam);

			EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
			reconstructionParam.DepthBufferScale = 1.0f;
			reconstructionParam.DepthBufferOffset = 0.0f;
			reconstructionParam.ProjectionMatrix33 = projMat.Values[2][2];
			reconstructionParam.ProjectionMatrix43 = projMat.Values[2][3];
			reconstructionParam.ProjectionMatrix34 = projMat.Values[3][2];
			reconstructionParam.ProjectionMatrix44 = projMat.Values[3][3];
			GetRenderer()->SetDepth(depth, reconstructionParam);
		}
	}

	const std::vector<Effekseer::EffectRef>& GetEffects() const
	{
		return effects_;
	}
};