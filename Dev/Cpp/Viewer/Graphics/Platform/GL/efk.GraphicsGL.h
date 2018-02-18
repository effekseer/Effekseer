
#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

#include "../../efk.Graphics.h"

namespace efk
{
	class GraphicsGL
		: public Graphics
	{
	private:
		void*	windowHandle = nullptr;
		int32_t	windowWidth = 0;
		int32_t	windowHeight = 0;

		bool				isSRGBMode = false;

		/*
		LPDIRECT3D9			d3d = nullptr;
		LPDIRECT3DDEVICE9	d3d_device = nullptr;
		
		IDirect3DSurface9*	renderDefaultTarget = nullptr;
		IDirect3DSurface9*	renderDefaultDepth = nullptr;

		IDirect3DSurface9*	recordingTarget = nullptr;
		IDirect3DTexture9*	recordingTargetTexture = nullptr;
		IDirect3DSurface9*	recordingDepth = nullptr;
		int32_t				recordingWidth = 0;
		int32_t				recordingHeight = 0;

		IDirect3DSurface9*	backTarget = nullptr;
		IDirect3DTexture9*	backTargetTexture = nullptr;
		*/
		EffekseerRendererGL::Renderer*	renderer = nullptr;

	public:
		GraphicsGL();
		virtual ~GraphicsGL();

		bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount) override;

		void CopyToBackground() override;

		void Resize(int32_t width, int32_t height) override;

		bool Present() override;

		void BeginScene() override;

		void EndScene() override;

		void BeginRecord(int32_t width, int32_t height) override;

		void EndRecord(std::vector<Effekseer::Color>& pixels) override;

		void Clear(Effekseer::Color color) override;

		void ResetDevice() override;

		void* GetBack() override;

		EffekseerRenderer::Renderer* GetRenderer() override;

		DeviceType GetDeviceType() const override { return DeviceType::OpenGL; }
	};
}