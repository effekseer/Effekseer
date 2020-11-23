﻿
#ifndef __EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__
#define __EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::SIMD::Vec3f efkVector3D;

class ModelRenderer : public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplemented* m_renderer;
	Shader* shader_advanced_lit_;
	Shader* shader_advanced_unlit_;
	Shader* shader_advanced_distortion_;
	Shader* shader_lit_;
	Shader* shader_unlit_;
	Shader* shader_distortion_;
	Backend::GraphicsDevice* graphicsDevice_ = nullptr;

	ModelRenderer(RendererImplemented* renderer,
				  Shader* shader_advanced_lit,
				  Shader* shader_advanced_unlit,
				  Shader* shader_advanced_distortion,
				  Shader* shader_lit,
				  Shader* shader_unlit,
				  Shader* shader_distortion);

public:
	virtual ~ModelRenderer();

	static ModelRenderer* Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);

	virtual void Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__