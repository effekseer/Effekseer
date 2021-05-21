
#pragma once

#include "efk.Graphics.h"
#include <Effekseer.h>

namespace efk
{
class BloomEffect;
class TonemapEffect;
class LinearToSRGBEffect;

class PostEffect
{
protected:
	bool enabled = true;
	Graphics* graphics;
	bool isValid_ = true;

public:
	PostEffect(Graphics* graphics)
		: graphics(graphics)
	{
	}
	virtual ~PostEffect() = default;

	virtual void Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) = 0;

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;

	bool GetEnabled() const
	{
		return enabled;
	}

	virtual void SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	bool GetIsValid() const
	{
		return isValid_;
	}

	static BloomEffect* CreateBloom(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);

	static TonemapEffect* CreateTonemap(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);

	static LinearToSRGBEffect* CreateLinearToSRGB(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
};

class BloomEffect : public PostEffect
{
protected:
	float intensity = 1.0f;
	float threshold = 1.0f;
	float softKnee = 0.5f;

public:
	BloomEffect(Graphics* graphics)
		: PostEffect(graphics)
	{
	}
	virtual ~BloomEffect() = default;

	void GetParameters(float& intensity, float& threshold, float& softKnee)
	{
		intensity = this->intensity;
		threshold = this->threshold;
		softKnee = this->softKnee;
	}

	virtual void SetParameters(float intensity, float threshold, float softKnee)
	{
		this->intensity = intensity;
		this->threshold = threshold;
		this->softKnee = softKnee;
	}
};

class TonemapEffect : public PostEffect
{
public:
	enum class Algorithm
	{
		Off,
		Reinhard,
	};

	TonemapEffect(Graphics* graphics)
		: PostEffect(graphics)
	{
	}
	virtual ~TonemapEffect() = default;

	void GetParameters(Algorithm& algorithm, float& exposure)
	{
		algorithm = this->algorithm;
		exposure = this->exposure;
	}

	virtual void SetParameters(Algorithm algorithm, float exposure)
	{
		this->algorithm = algorithm;
		this->exposure = exposure;
	}

protected:
	Algorithm algorithm = Algorithm::Reinhard;
	float exposure = 1.0f;
};

class LinearToSRGBEffect : public PostEffect
{
public:
	LinearToSRGBEffect(Graphics* graphics)
		: PostEffect(graphics)
	{
	}
	virtual ~LinearToSRGBEffect() = default;
};
} // namespace efk