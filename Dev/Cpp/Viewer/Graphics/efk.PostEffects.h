
#pragma once

#include <Effekseer.h>
#include "efk.Graphics.h"

namespace efk
{
	class BloomEffect;
	class TonemapEffect;

	class PostEffect
	{
	protected:
		bool enabled = true;
		Graphics* graphics;

	public:
		PostEffect(Graphics* graphics): graphics(graphics) {}
		virtual ~PostEffect() = default;

		virtual void Render(RenderTexture* src, RenderTexture* dest) = 0;

		virtual void OnLostDevice() = 0;

		virtual void OnResetDevice() = 0;

		virtual void SetEnabled(bool enabled) { this->enabled = enabled; }

		static BloomEffect* CreateBloom(Graphics* graphics);

		static TonemapEffect* CreateTonemap(Graphics* graphics);
	};

	class BloomEffect : public PostEffect
	{
	protected:
		float intensity = 1.0f;
		float threshold = 1.0f;
		float softKnee = 0.5f;

	public:
		BloomEffect(Graphics* graphics): PostEffect(graphics) {}
		virtual ~BloomEffect() = default;

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
		enum class Algorithm {
			Off,
			Reinhard,
		};

		TonemapEffect(Graphics* graphics): PostEffect(graphics) {}
		virtual ~TonemapEffect() = default;

		virtual void SetParameters(Algorithm algorithm, float exposure)
		{
			this->algorithm = algorithm;
			this->exposure = exposure;
		}

	protected:
		Algorithm algorithm = Algorithm::Reinhard;
		float exposure = 1.0f;
	};
}