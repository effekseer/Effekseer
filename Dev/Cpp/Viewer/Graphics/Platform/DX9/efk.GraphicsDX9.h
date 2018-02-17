
#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>

#include "../../efk.Graphics.h"

#include <functional>

namespace efk
{
	class GraphicsDX9
		: public Graphics
	{
	private:
		void*	windowHandle = nullptr;
		int32_t	windowWidth = 0;
		int32_t	windowHeight = 0;

		LPDIRECT3D9			d3d = nullptr;
		LPDIRECT3DDEVICE9	d3d_device = nullptr;
		bool				isSRGBMode = false;

		IDirect3DSurface9*	renderDefaultTarget = nullptr;
		IDirect3DSurface9*	renderDefaultDepth = nullptr;

		IDirect3DSurface9*	recordingTarget = nullptr;
		IDirect3DTexture9*	recordingTargetTexture = nullptr;
		IDirect3DSurface9*	recordingDepth = nullptr;
		int32_t				recordingWidth = 0;
		int32_t				recordingHeight = 0;

		EffekseerRendererDX9::Renderer*	renderer = nullptr;

	public:
		GraphicsDX9();
		virtual ~GraphicsDX9();

		bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount);

		void Resize(int32_t width, int32_t height);

		bool Present();

		void BeginScene();

		void EndScene();

		void BeginRecord(int32_t width, int32_t height);

		void EndRecord(std::vector<Effekseer::Color>& pixels);

		void Clear(Effekseer::Color color);

		void ResetDevice();

		EffekseerRenderer::Renderer* GetRenderer();

		/**
		Called when device is losted.
		*/
		std::function<void()>	LostedDevice;

		/**
		Called when device is resetted.
		*/
		std::function<void()>	ResettedDevice;
	};
}