
#ifndef	__EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__
#define	__EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
//#define MODEL_SOFTWARE_INSTANCING

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
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
public:
	enum eShaderAttrib
	{
		AttribPosition,
		AttribNormal,
		AttribBinormal,
		AttribTangent,
		AttribTexCoord,
		
#if defined(MODEL_SOFTWARE_INSTANCING)
		AttribInstanceID,
		AttribUVOffset,
		AttribModelColor,
#endif
		NumAttribs,
		NumVertexAttribs = AttribTexCoord + 1,
	};
	enum eShaderUniform
	{
		UniformProjectionMatrix,
		UniformModelMatrix,
#if !defined(MODEL_SOFTWARE_INSTANCING)
		UniformUVOffset,
		UniformModelColor,
#endif
		UniformColorTexture,
		UniformNormalTexture,
		UniformLightDirection,
		UniformLightColor,
		UniformLightAmbient,
		UniformTextureEnable,
		UniformLightingEnable,
		UniformNormalMapEnable,
		NumUniforms,
	};

	static const int32_t MaxInstanced = 20;

private:
	RendererImplemented*				m_renderer;

	Shader*								m_shader_lighting_texture_normal;
	Shader*								m_shader_lighting_normal;

	Shader*								m_shader_lighting_texture;
	Shader*								m_shader_lighting;

	Shader*								m_shader_texture;
	Shader*								m_shader;

	GLint								m_uniformLoc[6][NumUniforms];

	ModelRenderer( 
		RendererImplemented* renderer,
		Shader* shader_lighting_texture_normal,
		Shader* shader_lighting_normal,
		Shader* shader_lighting_texture,
		Shader* shader_lighting,
		Shader* shader_texture,
		Shader* shader);
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
#endif	// __EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__