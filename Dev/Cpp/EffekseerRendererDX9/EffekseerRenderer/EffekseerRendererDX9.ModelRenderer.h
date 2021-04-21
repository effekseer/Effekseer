
#ifndef __EFFEKSEERRENDERER_DX9_MODEL_RENDERER_H__
#define __EFFEKSEERRENDERER_DX9_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class ModelRenderer;
typedef ::Effekseer::RefPtr<ModelRenderer> ModelRendererRef;

class ModelRenderer : public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplementedRef m_renderer;
	Shader* shader_advanced_lit_ = nullptr;
	Shader* shader_advanced_unlit_ = nullptr;
	Shader* shader_advanced_distortion_ = nullptr;
	Shader* shader_lit_ = nullptr;
	Shader* shader_unlit_ = nullptr;
	Shader* shader_distortion_ = nullptr;
	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;

	ModelRenderer(const RendererImplementedRef& renderer,
				  Shader* shader_advanced_lit,
				  Shader* shader_advanced_unlit,
				  Shader* shader_advanced_distortion,
				  Shader* shader_lit,
				  Shader* shader_unlit,
				  Shader* shader_distortion);

public:
	virtual ~ModelRenderer();

	static ModelRendererRef Create(const RendererImplementedRef& renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);

	virtual void Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_MODEL_RENDERER_H__