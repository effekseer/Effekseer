
#ifndef	__EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__
#define	__EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"

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
typedef ::Effekseer::Vector3D efkVector3D;

class ModelRenderer
	: public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplemented*			m_renderer;
	Shader*							m_shader_lighting_texture_normal;
	Shader*							m_shader_lighting_normal;

	Shader*							m_shader_lighting_texture;
	Shader*							m_shader_lighting;

	Shader*							m_shader_texture;
	Shader*							m_shader;

	Shader*							m_shader_distortion_texture;
	Shader*							m_shader_distortion;

	ModelRenderer( RendererImplemented* renderer,
		Shader* shader_lighting_texture_normal,
		Shader* shader_lighting_normal,
		Shader* shader_lighting_texture,
		Shader* shader_lighting,
		Shader* shader_texture,
		Shader* shader,
		Shader* shader_distortion_texture,
		Shader* shader_distortion);
public:

	virtual ~ModelRenderer();

	static ModelRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);
	void EndRendering( const efkModelNodeParam& parameter, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_MODEL_RENDERER_H__