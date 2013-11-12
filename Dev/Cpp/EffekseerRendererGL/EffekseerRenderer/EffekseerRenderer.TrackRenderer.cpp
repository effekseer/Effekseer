
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.RenderState.h"

#include "EffekseerRenderer.VertexBuffer.h"
#include "EffekseerRenderer.IndexBuffer.h"
#include "EffekseerRenderer.TrackRenderer.h"
#include "EffekseerRenderer.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
static const char g_ribbon_vs_src[] = (
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
	"}\n"
);

static const char g_ribbon_fs_src[] = (
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
	"}\n"
);


static const char g_ribbon_fs_no_texture_src [] = (
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
	"}\n"
	);

static ShaderAttribInfo g_ribbon_attribs[3] = {
	{"atPosition", GL_FLOAT, 3, 0, false},
	{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
	{"atTexCoord", GL_FLOAT, 2, 16, false}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer::TrackRenderer(RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture)
	: m_renderer(renderer)
	, m_shader(shader)
	, m_shader_no_texture(shader_no_texture)
{
	// ’¸“_‘®«ID‚ðŽæ“¾
	shader->GetAttribIdList(3, g_ribbon_attribs);
	shader->SetVertexSize(sizeof(Vertex));
	shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		shader->GetUniformId("uMatProjection"),
		0
		);
	shader->SetTextureSlot(0, shader->GetUniformId("uTexture0"));

	shader_no_texture->GetAttribIdList(3, g_ribbon_attribs);
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
TrackRenderer::~TrackRenderer()
{
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_no_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer* TrackRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	Shader* shader = Shader::Create(renderer, g_ribbon_vs_src, strlen(g_ribbon_vs_src), g_ribbon_fs_src, strlen(g_ribbon_fs_src), "TrackRenderer");
	if (shader == NULL) return NULL;

	Shader* shader_no_texture = Shader::Create(renderer, g_ribbon_vs_src, strlen(g_ribbon_vs_src), g_ribbon_fs_no_texture_src, strlen(g_ribbon_fs_no_texture_src), "TrackRenderer");
	if (shader_no_texture == NULL)
	{
		ES_SAFE_DELETE(shader);
		return NULL;
	}

	return new TrackRenderer(renderer, shader, shader_no_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::BeginRendering( const efkTrackNodeParam& parameter, int32_t count, void* userData )
{
	BeginRendering_<Vertex>( m_renderer, parameter, count, userData );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::Rendering( const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData )
{
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::EndRendering(const efkTrackNodeParam& parameter, void* userData)
{
	if (m_ringBufferData == NULL) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if (m_ribbonCount <= 1) return;

	EndRendering_<RendererImplemented, Shader, GLuint, Vertex>(m_renderer, m_shader, m_shader_no_texture, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
