#pragma once

#include "PostProcess.h"
#include <memory>

namespace Effekseer::Tool
{

class BloomPostEffect
{
public:
	static const int BlurBuffers = 2;
	static const int BlurIterations = 4;

	BloomPostEffect(Backend::GraphicsDeviceRef graphicsDevice);

	void Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src);

	bool GetEnabled() const
	{
		return enabled_;
	}

	void SetEnabled(bool enabled)
	{
		this->enabled_ = enabled;
	}

	void GetParameters(float& intensity, float& threshold, float& softKnee)
	{
		intensity = intensity_;
		threshold = threshold_;
		softKnee = softKnee_;
	}

	void SetParameters(float intensity, float threshold, float softKnee)
	{
		intensity_ = intensity;
		threshold_ = threshold;
		softKnee_ = softKnee;
	}

	bool GetIsValid() const;

private:
	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::RenderPassRef renderPass_;
	Effekseer::Backend::TextureRef currentRenderTarget_;

	std::unique_ptr<PostProcess> extract_;
	std::unique_ptr<PostProcess> downsample_;
	std::unique_ptr<PostProcess> blend_;
	std::unique_ptr<PostProcess> blurH_;
	std::unique_ptr<PostProcess> blurV_;

	float intensity_ = 1.0f;
	float threshold_ = 1.0f;
	float softKnee_ = 0.5f;
	bool enabled_ = true;

	int32_t renderTextureWidth = 0;
	int32_t renderTextureHeight = 0;
	Effekseer::Backend::TextureRef extractBuffer;
	Effekseer::Backend::TextureRef lowresBuffers[BlurBuffers][BlurIterations];

	Backend::RenderPassRef renderPassExtract_;
	Backend::RenderPassRef renderPassDownscales_[BlurIterations];
	Backend::RenderPassRef renderPassBlurs_[BlurBuffers][BlurIterations];
	Backend::RenderPassRef renderPassBlend_;

	void SetupBuffers(int32_t width, int32_t height);
	void ReleaseBuffers();
};

class TonemapPostEffect
{
public:
	enum class Algorithm
	{
		Off,
		Reinhard,
	};

	TonemapPostEffect(Backend::GraphicsDeviceRef graphicsDevice);

	void Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src);

	bool GetIsValid() const;

	void GetParameters(Algorithm& algorithm, float& exposure)
	{
		algorithm = this->algorithm_;
		exposure = this->exposure_;
	}

	void SetParameters(Algorithm algorithm, float exposure)
	{
		this->algorithm_ = algorithm;
		this->exposure_ = exposure;
	}

	bool GetEnabled() const
	{
		return enabled_;
	}

	void SetEnabled(bool enabled)
	{
		this->enabled_ = enabled;
	}

private:
	Backend::GraphicsDeviceRef graphicsDevice_;
	std::unique_ptr<PostProcess> postProcessCopy_;
	std::unique_ptr<PostProcess> postProcessTone_;
	Backend::RenderPassRef renderPass_;
	Effekseer::Backend::TextureRef currentRenderTarget_;

	Algorithm algorithm_ = Algorithm::Reinhard;
	float exposure_ = 1.0f;
	bool enabled_ = true;
};

class LinearToSRGBPostEffect
{
private:
	Backend::GraphicsDeviceRef graphicsDevice_;
	std::unique_ptr<PostProcess> postProcess_;
	Backend::RenderPassRef renderPass_;
	Effekseer::Backend::TextureRef currentRenderTarget_;

public:
	LinearToSRGBPostEffect(Backend::GraphicsDeviceRef graphicsDevice);

	void Render(Effekseer::Backend::TextureRef dst, Effekseer::Backend::TextureRef src);

	bool GetIsValid() const;
};

} // namespace Effekseer::Tool