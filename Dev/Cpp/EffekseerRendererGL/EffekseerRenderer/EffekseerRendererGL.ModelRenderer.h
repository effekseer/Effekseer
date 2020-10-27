
#ifndef __EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__
#define __EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"
#include "EffekseerRendererGL.RendererImplemented.h"

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
		AttribColor,
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
		Scale,
		UVInversed,
		UVInversedBack,
		NumUniforms,
	};

	static const int32_t MaxInstanced = 20;

private:
	RendererImplemented* m_renderer;

	Shader* m_shader_lighting_texture_normal;
	Shader* m_shader_texture;
	Shader* m_shader_distortion_texture;

	GLint m_uniformLoc[8][NumUniforms];

	VertexArray* m_va[8];

	ModelRenderer(
		RendererImplemented* renderer,
		Shader* shader_lighting_texture_normal,
		Shader* shader_texture,
		Shader* shader_distortion_texture);

public:
	virtual ~ModelRenderer();

	static ModelRenderer* Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData) override;

	virtual void Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData) override;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__