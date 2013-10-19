
#ifndef	__EFFEKSEERRENDERER_MODEL_RENDERER_H__
#define	__EFFEKSEERRENDERER_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class ModelRenderer
	: public ModelRendererBase
{
private:
	RendererImplemented*			m_renderer;
	Shader*							m_shader_lighting_texture_normal;
	Shader*							m_shader_lighting_normal;

	Shader*							m_shader_lighting_texture;
	Shader*							m_shader_lighting;

	Shader*							m_shader_texture;
	Shader*							m_shader;

	ModelRenderer( RendererImplemented* renderer,
		Shader* shader_lighting_texture_normal,
		Shader* shader_lighting_normal,
		Shader* shader_lighting_texture,
		Shader* shader_lighting,
		Shader* shader_texture,
		Shader* shader );
public:

	virtual ~ModelRenderer();

	static ModelRenderer* Create( RendererImplemented* renderer );

public:
	void EndRendering( const efkModelNodeParam& parameter, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_MODEL_RENDERER_H__