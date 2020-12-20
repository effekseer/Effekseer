﻿#if 0

#ifndef __EFFEKSEERRENDERER_SWITCH_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_SWITCH_RENDERER_IMPLEMENTED_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererDX12.Base.h"
#include "EffekseerRendererDX12.Renderer.h"

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererDX12
{

class RendererImplemented : public Renderer, public ::EffekseerRendererLLGI::RendererImplemented
{
	friend class ::EffekseerRendererDX12::Renderer;

private:
	EffekseerRendererLLGI::FixedShader fixedShader;

public:
	void NewFrame() override;

	Effekseer::Backend::TextureRef CreateTexture(ID3D12Resource* texture) override;

	RendererImplemented(int32_t squareMaxCount);
	virtual ~RendererImplemented();

	void OnLostDevice() override;

	void OnResetDevice() override;

	void Destroy() override;

	void SetRestorationOfStatesFlag(bool flag) override;

	bool BeginRendering() override;

	bool EndRendering() override;

	::Effekseer::Vector3D GetLightDirection() const override;

	void SetLightDirection(const ::Effekseer::Vector3D& direction) override;

	const ::Effekseer::Color& GetLightColor() const override;

	void SetLightColor(const ::Effekseer::Color& color) override;

	const ::Effekseer::Color& GetLightAmbientColor() const override;

	void SetLightAmbientColor(const ::Effekseer::Color& color) override;

	int32_t GetSquareMaxCount() const override;

	::Effekseer::Matrix44 GetProjectionMatrix() const override;

	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat) override;

	::Effekseer::Matrix44 GetCameraMatrix() const override;

	void SetCameraMatrix(const ::Effekseer::Matrix44& mat) override;

	::Effekseer::Matrix44& GetCameraProjectionMatrix() override;

	::Effekseer::Vector3D GetCameraFrontDirection() const override;

	::Effekseer::Vector3D GetCameraPosition() const override;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position) override;

	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;

	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;

	::Effekseer::RingRendererRef CreateRingRenderer() override;

	::Effekseer::ModelRendererRef CreateModelRenderer() override;

	::Effekseer::TrackRendererRef CreateTrackRenderer() override;

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	void ResetRenderState() override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	int32_t GetDrawCallCount() const override;

	int32_t GetDrawVertexCount() const override;

	void ResetDrawCallCount() override;

	void ResetDrawVertexCount() override;

	void SetRenderMode(Effekseer::RenderMode renderMode) override;

	Effekseer::RenderMode GetRenderMode() override;

	virtual int GetRef() { return ::Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() { return ::Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() { return ::Effekseer::ReferenceObject::Release(); }
};

} // namespace EffekseerRendererDX12

#endif

#endif