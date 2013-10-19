
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.RenderState.h"

#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.RingRenderer.h"
#include "EffekseerRenderer.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
static const char g_ring_vs_src[] = 
#if defined(__EFFEKSEER_RENDERER_GLES2__)
#else
	"#version 110\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
#endif
	"attribute vec4 atPosition;\n"
	"attribute vec4 atColor;\n"
	"attribute vec4 atTexCoord;\n"
	"varying vec4 vaColor;\n"
	"varying vec4 vaTexCoord;\n"
	"uniform mat4 uMatProjection;\n"
	"void main() {\n"
	"	gl_Position = uMatProjection * atPosition;\n"
	"	vaColor = atColor;\n"
	"	vaTexCoord = atTexCoord;\n"
	"}\n";

static const char g_ring_fs_src[] = 
#if defined(__EFFEKSEER_RENDERER_GLES2__)
	"precision mediump float;\n"
#else
	"#version 110\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
#endif
	"varying lowp vec4 vaColor;\n"
	"varying mediump vec4 vaTexCoord;\n"
	"uniform sampler2D uTexture0;\n"
	"uniform bool uTexEnable;\n"
	"void main() {\n"
	"gl_FragColor = vaColor * texture2D(uTexture0, vaTexCoord.xy);\n"
	"}\n";

static const char g_ring_fs_no_texture_src [] =
#if defined(__EFFEKSEER_RENDERER_GLES2__)
	"precision mediump float;\n"
#else
	"#version 110\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
#endif
	"varying lowp vec4 vaColor;\n"
	"varying mediump vec4 vaTexCoord;\n"
	"uniform sampler2D uTexture0;\n"
	"uniform bool uTexEnable;\n"
	"void main() {\n"
	"gl_FragColor = vaColor;\n"
	"}\n";

static ShaderAttribInfo g_sprite_attribs[3] = {
	{"atPosition", GL_FLOAT, 3, 0, false},
	{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
	{"atTexCoord", GL_FLOAT, 2, 16, false}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer(RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture)
	: m_renderer(renderer)
	, m_shader(shader)
	, m_shader_no_texture(shader_no_texture)

{
	// ’¸“_‘®«ID‚ğæ“¾
	shader->GetAttribIdList(3, g_sprite_attribs);
	shader->SetVertexSize(sizeof(Vertex));
	shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		shader->GetUniformId("uMatProjection"),
		0
		);
	shader->SetTextureSlot(0, shader->GetUniformId("uTexture0"));


	shader_no_texture->GetAttribIdList(3, g_sprite_attribs);
	shader_no_texture->SetVertexSize(sizeof(Vertex));
	shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	shader_no_texture->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		shader_no_texture->GetUniformId("uMatProjection"),
		0
		);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::~RingRenderer()
{
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_no_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* RingRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	Shader* shader = Shader::Create(renderer,
		g_ring_vs_src, strlen(g_ring_vs_src), g_ring_fs_src, strlen(g_ring_fs_src), "RingRenderer");
	if (shader == NULL) return NULL;

	Shader* shader_no_texture = Shader::Create(renderer,
		g_ring_vs_src, strlen(g_ring_vs_src), g_ring_fs_no_texture_src, strlen(g_ring_fs_no_texture_src), "RingRenderer");
	if (shader_no_texture == NULL)
	{
		ES_SAFE_DELETE(shader);
		return NULL;
	}
	return new RingRenderer(renderer, shader, shader_no_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::BeginRendering( const efkRingNodeParam& parameter, int32_t count, void* userData )
{
	/*
	m_spriteCount = 0;
	m_renderer->GetVertexBuffer()->Lock();
	m_instanceCount = count;
	*/

	m_spriteCount = 0;
	
	int32_t vertexCount = parameter.VertexCount * 8;

	if( ! m_renderer->GetVertexBuffer()->RingBufferLock( count * sizeof(Vertex) * vertexCount, m_ringBufferOffset, (void*&)m_ringBufferData ) )
	{
		m_ringBufferOffset = 0;
		m_ringBufferData = NULL;
	}

	m_instanceCount = count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::Rendering( const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData )
{
	if( m_spriteCount == m_renderer->GetSquareMaxCount() ) return;
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::EndRendering( const efkRingNodeParam& parameter, void* userData )
{
	m_renderer->GetVertexBuffer()->Unlock();

	if( m_spriteCount == 0 ) return;
	
	EndRendering_<RendererImplemented, Shader, GLuint>(m_renderer, m_shader, m_shader_no_texture, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
