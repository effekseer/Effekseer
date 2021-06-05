
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

class ModelRenderer;
typedef ::Effekseer::RefPtr<ModelRenderer> ModelRendererRef;

const int OpenGLInstancingCount = 10;

class ModelRenderer : public ::EffekseerRenderer::ModelRendererBase
{
public:
	static const int32_t MaxInstanced = 20;

private:
	RendererImplemented* m_renderer;

	VertexArray* m_va[6];

	Shader* shader_ad_lit_ = nullptr;
	Shader* shader_ad_unlit_ = nullptr;
	Shader* shader_ad_distortion_ = nullptr;

	Shader* shader_lit_ = nullptr;
	Shader* shader_unlit_ = nullptr;
	Shader* shader_distortion_ = nullptr;

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;

	template <int N>
	void InitRenderer();

	ModelRenderer(RendererImplemented* renderer,
				  Shader* shader_ad_lit,
				  Shader* shader_ad_unlit,
				  Shader* shader_ad_distortion,
				  Shader* shader_lighting_texture_normal,
				  Shader* shader_texture,
				  Shader* shader_distortion_texture);

public:
	virtual ~ModelRenderer();

	static ModelRendererRef Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData) override;

	virtual void Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData) override;
};

void AddModelVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, bool isAd, bool isInstancing, int N);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_MODEL_RENDERER_H__