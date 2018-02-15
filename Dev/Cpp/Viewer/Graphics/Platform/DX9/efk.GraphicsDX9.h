
#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>

#include "../../efk.Graphics.h"

namespace efk
{
	class GraphicsDX9
		: public Graphics
	{
	private:
		LPDIRECT3D9			d3d = nullptr;
		LPDIRECT3DDEVICE9	d3d_device = nullptr;
		bool				isSRGBMode = false;

	public:
		GraphicsDX9();
		virtual ~GraphicsDX9();

		bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode);

		bool Present();
	};
}