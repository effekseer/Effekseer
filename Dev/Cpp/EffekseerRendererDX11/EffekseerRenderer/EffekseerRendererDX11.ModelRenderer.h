
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
typedef ::Effekseer::Vec3f efkVector3D;

class ModelRenderer
	: public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplemented* m_renderer;
	Shader* m_shader_lighting_texture_normal;
	Shader* m_shader_texture;
	Shader* m_shader_distortion_texture;

	ModelRenderer(RendererImplemented* renderer,
				  Shader* shader_lighting_texture_normal,
				  Shader* shader_texture,
				  Shader* shader_distortion_texture);

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