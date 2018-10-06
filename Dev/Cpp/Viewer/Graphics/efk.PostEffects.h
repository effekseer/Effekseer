
#pragma once

#include <Effekseer.h>
#include "efk.Graphics.h"

namespace efk
{
	class BloomEffect;

	class PostEffect
	{
	protected:
		bool enabled = true;
		Graphics* graphics;

	public:
		PostEffect(Graphics* graphics): graphics(graphics) {}
		virtual ~PostEffect() {};

		virtual void Render() = 0;

		virtual void OnLostDevice() = 0;

		virtual void OnResetDevice() = 0;

		virtual void SetEnabled(bool enabled) { this->enabled = enabled; }

		static BloomEffect* CreateBloom(Graphics* graphics);
	};

	class BloomEffect : public PostEffect
	{
	protected:
		float intensity = 1.0f;
		float threshold = 1.0f;
		float softKnee = 0.5f;

	public:
		BloomEffect(Graphics* graphics): PostEffect(graphics) {}
		virtual ~BloomEffect() {};

		virtual void SetParameters(float intensity, float threshold, float softKnee)
		{
			this->intensity = intensity;
			this->threshold = threshold;
			this->softKnee = softKnee;
		}
	};
}