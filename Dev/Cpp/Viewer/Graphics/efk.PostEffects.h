
#pragma once

#include <Effekseer.h>
#include "efk.Graphics.h"

namespace efk
{
	class PostEffect
	{
	protected:
		Graphics* graphics;

	public:
		PostEffect(Graphics* graphics): graphics(graphics) {}
		virtual ~PostEffect() {}

		virtual void Render() = 0;

		virtual void OnLostDevice() = 0;

		virtual void OnResetDevice() = 0;

		static PostEffect* CreateBloom(Graphics* graphics);
	};
}