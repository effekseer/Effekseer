
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
#include "EffekseerRendererDX9.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderLightingTextureNormal_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTextureNormal_PS.h"

}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderLightingNormal_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingNormal_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingNormal_PS.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderLightingTexture_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTexture_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLightingTexture_PS.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderLighting_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLighting_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderLighting_PS.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderTexture_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderTexture_PS.h"
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Shader_
{
static
#include "Shader/EffekseerRenderer.ModelRenderer.Shader_VS.h"

static
#include "Shader/EffekseerRenderer.ModelRenderer.Shader_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderDistortionTexture_
{
	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"

	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortionTexture_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace ShaderDistortion_
{
	static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortion_VS.h"

		static
#include "Shader/EffekseerRenderer.ModelRenderer.ShaderDistortion_PS.h"
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::ModelRenderer( 
	RendererImplemented* renderer,
	Shader* shader_lighting_texture_normal,
	Shader* shader_lighting_normal,
	Shader* shader_lighting_texture, 
	Shader* shader_lighting, 
	Shader* shader_texture, 
	Shader* shader,
	Shader* shader_distortion_texture,
	Shader* shader_distortion)
	: m_renderer	( renderer )
	, m_shader_lighting_texture_normal		( shader_lighting_texture_normal )
	, m_shader_lighting_normal		( shader_lighting_normal )
	, m_shader_lighting_texture		( shader_lighting_texture )
	, m_shader_lighting		( shader_lighting )
	, m_shader_texture		( shader_texture )
	, m_shader		( shader )
	, m_shader_distortion_texture(shader_distortion_texture)
	, m_shader_distortion(shader_distortion)

{
	Shader* shaders[6];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_lighting_normal;
	shaders[2] = m_shader_lighting_texture;
	shaders[3] = m_shader_lighting;
	shaders[4] = m_shader_texture;
	shaders[5] = m_shader;
	
	for( int32_t i = 0; i < 6; i++ )
	{
		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
		shaders[i]->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>)/(sizeof(float)*4));
		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->SetPixelRegisterCount(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer)/(sizeof(float)*4));
	}

	m_shader_distortion_texture->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
	m_shader_distortion_texture->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>) / (sizeof(float) * 4));
	m_shader_distortion_texture->SetPixelConstantBufferSize(sizeof(float) * 4);
	m_shader_distortion_texture->SetPixelRegisterCount(1);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>));
	m_shader_distortion->SetVertexRegisterCount(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<40>) / (sizeof(float) * 4));
	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1);

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
	ES_SAFE_DELETE( m_shader_lighting_texture_normal );
	ES_SAFE_DELETE( m_shader_lighting_normal );
	ES_SAFE_DELETE( m_shader_lighting_texture );
	ES_SAFE_DELETE( m_shader_lighting );
	ES_SAFE_DELETE( m_shader_texture );
	ES_SAFE_DELETE( m_shader );

	ES_SAFE_DELETE(m_shader_distortion_texture);
	ES_SAFE_DELETE(m_shader_distortion);

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ModelRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	// 座標(3) 法線(3)*3 UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{0,	24,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	1},
		{0,	36,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	2},
		{0,	48,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		{0, 56, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 3 },
		{0,	60,	D3DDECLTYPE_UBYTE4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0},
		D3DDECL_END()
	};

	Shader* shader_lighting_texture_normal = Shader::Create( 
		renderer, 
		ShaderLightingTextureNormal_::g_vs20_VS,
		sizeof(ShaderLightingTextureNormal_::g_vs20_VS),
		ShaderLightingTextureNormal_::g_ps20_PS,
		sizeof(ShaderLightingTextureNormal_::g_ps20_PS),
		"ModelRendererLightingTextureNormal", 
		decl );

	Shader* shader_lighting_normal = Shader::Create( 
		renderer, 
		ShaderLightingNormal_::g_vs20_VS,
		sizeof(ShaderLightingNormal_::g_vs20_VS),
		ShaderLightingNormal_::g_ps20_PS,
		sizeof(ShaderLightingNormal_::g_ps20_PS),
		"ModelRendererLightingNormal", 
		decl );


	Shader* shader_lighting_texture = Shader::Create( 
		renderer, 
		ShaderLightingTexture_::g_vs20_VS,
		sizeof(ShaderLightingTexture_::g_vs20_VS),
		ShaderLightingTexture_::g_ps20_PS,
		sizeof(ShaderLightingTexture_::g_ps20_PS),
		"ModelRendererLightingTexture", 
		decl );

	Shader* shader_lighting = Shader::Create( 
		renderer, 
		ShaderLighting_::g_vs20_VS,
		sizeof(ShaderLighting_::g_vs20_VS),
		ShaderLighting_::g_ps20_PS,
		sizeof(ShaderLighting_::g_ps20_PS),
		"ModelRendererLighting", 
		decl );

	Shader* shader_texture = Shader::Create( 
		renderer, 
		ShaderTexture_::g_vs20_VS,
		sizeof(ShaderTexture_::g_vs20_VS),
		ShaderTexture_::g_ps20_PS,
		sizeof(ShaderTexture_::g_ps20_PS),
		"ModelRendererTexture", 
		decl );

	Shader* shader = Shader::Create( 
		renderer, 
		Shader_::g_vs20_VS,
		sizeof(Shader_::g_vs20_VS),
		Shader_::g_ps20_PS,
		sizeof(Shader_::g_ps20_PS),
		"ModelRenderer", 
		decl );

	auto shader_distortion_texture = Shader::Create(
		renderer,
		ShaderDistortionTexture_::g_vs20_VS,
		sizeof(ShaderDistortionTexture_::g_vs20_VS),
		ShaderDistortionTexture_::g_ps20_PS,
		sizeof(ShaderDistortionTexture_::g_ps20_PS),
		"ModelRendererDistortionTexture",
		decl);

	auto shader_distortion = Shader::Create(
		renderer,
		ShaderDistortion_::g_vs20_VS,
		sizeof(ShaderDistortion_::g_vs20_VS),
		ShaderDistortion_::g_ps20_PS,
		sizeof(ShaderDistortion_::g_ps20_PS),
		"ModelRendererDistortion",
		decl);

	if( shader_lighting_texture_normal == NULL ||
		shader_lighting_normal == NULL ||
		shader_lighting_texture == NULL ||
		shader_lighting == NULL ||
		shader_texture == NULL ||
		shader == NULL ||
		shader_distortion_texture == NULL ||
		shader_distortion == NULL)
	{
		ES_SAFE_DELETE( shader_lighting_texture_normal );
		ES_SAFE_DELETE( shader_lighting_normal );
		ES_SAFE_DELETE( shader_lighting_texture );
		ES_SAFE_DELETE( shader_lighting );
		ES_SAFE_DELETE( shader_texture );
		ES_SAFE_DELETE( shader );
		ES_SAFE_DELETE(shader_distortion_texture);
		ES_SAFE_DELETE(shader_distortion);
	}

	return new ModelRenderer( renderer, 
		shader_lighting_texture_normal,
		shader_lighting_normal,
		shader_lighting_texture, 
		shader_lighting,
		shader_texture, 
		shader,
		shader_distortion_texture,
		shader_distortion);
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelRenderer::EndRendering( const efkModelNodeParam& parameter, void* userData )
{
	EndRendering_<
		RendererImplemented,
		Shader,
		Model,
		true,
		40>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_lighting_normal,
		m_shader_lighting_texture,
		m_shader_lighting,
		m_shader_texture,
		m_shader,
		m_shader_distortion_texture,
		m_shader_distortion,
		parameter );
	/*
	if (m_matrixes.size() == 0) return;
	if (parameter.ModelIndex < 0) return;

	Model* model = (Model*) parameter.EffectPointer->GetModel(parameter.ModelIndex);
	if (model == NULL) return;
	
	RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
	state.DepthTest = parameter.ZTest;
	state.DepthWrite = parameter.ZWrite;
	state.AlphaBlend = parameter.AlphaBlend;
	state.CullingType = parameter.Culling;

	Shader* shader_ = NULL;
	if (parameter.Lighting)
	{
		if (parameter.NormalTextureIndex >= 0)
		{
			if (parameter.ColorTextureIndex >= 0)
			{
				shader_ = m_shader_lighting_texture_normal;
			}
			else
			{
				shader_ = m_shader_lighting_normal;
			}
		}
		else
		{
			if (parameter.ColorTextureIndex >= 0)
			{
				shader_ = m_shader_lighting_texture;
			}
			else
			{
				shader_ = m_shader_lighting;
			}
		}
	}
	else
	{
		if (parameter.ColorTextureIndex >= 0)
		{
			shader_ = m_shader_texture;
		}
		else
		{
			shader_ = m_shader;
		}
	}

	m_renderer->BeginShader(shader_);

	IDirect3DTexture9* textures[2];
	textures[0] = NULL;
	textures[1] = NULL;

	if (parameter.ColorTextureIndex >= 0)
	{
		textures[0] = (IDirect3DTexture9*) parameter.EffectPointer->GetImage(parameter.ColorTextureIndex);
	}

	if (parameter.NormalTextureIndex >= 0)
	{
		textures[1] = (IDirect3DTexture9*) parameter.EffectPointer->GetImage(parameter.NormalTextureIndex);
	}
	
	m_renderer->SetTextures(shader_, textures, 2);

	state.TextureFilterTypes[0] = parameter.TextureFilter;
	state.TextureWrapTypes[0] = parameter.TextureWrap;
	state.TextureFilterTypes[1] = parameter.TextureFilter;
	state.TextureWrapTypes[1] = parameter.TextureWrap;

	m_renderer->GetRenderState()->Update(false);

	ModelRendererVertexConstantBuffer<40>* vcb = (ModelRendererVertexConstantBuffer<40>*)shader_->GetVertexConstantBuffer();
	ModelRendererPixelConstantBuffer* pcb = (ModelRendererPixelConstantBuffer*)shader_->GetPixelConstantBuffer();
	
	// 固定値設定
	if (parameter.Lighting)
	{
		{
			float f[4];
			::Effekseer::Vector3D lightDirection = m_renderer->GetLightDirection();
			::Effekseer::Vector3D::Normal(lightDirection, lightDirection);
			VectorToFloat4(lightDirection, vcb->LightDirection);
			VectorToFloat4(lightDirection, pcb->LightDirection);
		}

		{
			ColorToFloat4(m_renderer->GetLightColor(), vcb->LightColor);
			ColorToFloat4(m_renderer->GetLightColor(), pcb->LightColor);
		}

		{
			ColorToFloat4(m_renderer->GetLightAmbientColor(), vcb->LightAmbientColor);
			ColorToFloat4(m_renderer->GetLightAmbientColor(), pcb->LightAmbientColor);
		}
	}
	vcb->CameraMatrix = m_renderer->GetCameraProjectionMatrix();

	m_renderer->SetLayout(shader_);

	m_renderer->SetVertexBuffer(model->VertexBufferPtr, sizeof(Effekseer::Model::VertexWithIndex));
	m_renderer->SetIndexBuffer(model->IndexBufferPtr);

	for( size_t loop = 0; loop < m_matrixes.size(); )
	{
		int32_t modelCount = Effekseer::Min( m_matrixes.size() - loop, model->ModelCount );
		
		for( int32_t num = 0; num < modelCount; num++ )
		{
			vcb->ModelMatrix[num] = m_matrixes[loop+num];

			vcb->ModelUV[num][0] = m_uv[loop+num].X;
			vcb->ModelUV[num][1] = m_uv[loop+num].Y;
			vcb->ModelUV[num][2] = m_uv[loop+num].Width;
			vcb->ModelUV[num][3] = m_uv[loop+num].Height;

			ColorToFloat4(m_colors[loop+num],vcb->ModelColor[num]);
		}

		shader_->SetConstantBuffer();

		m_renderer->DrawPolygon( model->VertexCount * modelCount, model->IndexCount * modelCount );

		loop += modelCount;
	}

	m_renderer->EndShader(shader_);

	m_renderer->GetRenderState()->Pop();
	*/
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
