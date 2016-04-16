
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.SpriteRenderer.h"
#include "EffekseerRendererGL.RibbonRenderer.h"
#include "EffekseerRendererGL.RingRenderer.h"
#include "EffekseerRendererGL.TrackRenderer.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.TextureLoader.h"
#include "EffekseerRendererGL.ModelLoader.h"

#include "EffekseerRendererGL.GLExtension.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

	//-----------------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------------
	static const char g_sprite_vs_src [] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
#define OUT out
)"
#else
R"(
#define IN attribute
#define OUT varying
)"
#endif


R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
)"

R"(
OUT vec4 vaColor;
OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"

R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;

void main() {
	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;

	vec4 cameraPosU = cameraPos + vec4(0.0, 1.0, 0.0, 0.0);
	vec4 cameraPosR = cameraPos + vec4(1.0, 0.0, 0.0, 0.0);

	vaPosR = uMatProjection * cameraPosR;
	vaPosU = uMatProjection * cameraPosU;
	
	vaPos = vaPos / vaPos.w;
	vaPosR = vaPosR / vaPosR.w;
	vaPosU = vaPosU / vaPosU.w;

	vaColor = atColor;
	vaTexCoord = atTexCoord;
}

)";

	static const char g_sprite_fs_texture_src[] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
		R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
		R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
		R"(
precision mediump float;
)"
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
layout (location = 0) out vec4 FRAGCOLOR;
)"
#else
R"(
#define FRAGCOLOR gl_FragColor
)"
#endif

#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
#define TEX2D texture
)"
#else
R"(
#define IN varying
#define TEX2D texture2D
)"
#endif

"IN lowp vec4 vaColor;\n"
"IN mediump vec4 vaTexCoord;\n"

"uniform sampler2D uTexture0;\n"

"void main() {\n"
"FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);\n"
"}\n";

	static const char g_sprite_fs_no_texture_src[] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
		R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
		R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
		R"(
precision mediump float;
)"
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
layout (location = 0) out vec4 FRAGCOLOR;
)"
#else
R"(
#define FRAGCOLOR gl_FragColor
)"
#endif

#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
)"
#else
R"(
#define IN varying
)"
#endif

"IN lowp vec4 vaColor;\n"
"IN mediump vec4 vaTexCoord;\n"

"void main() {\n"
"FRAGCOLOR = vaColor;\n"
"}\n";


	static const char g_sprite_distortion_vs_src [] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
		R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
		R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
#define OUT out
)"
#else
R"(
#define IN attribute
#define OUT varying
)"
#endif


		R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
IN vec4 atBinormal;
IN vec4 atTangent;

)"

R"(
OUT vec4 vaColor;
OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"
R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;

void main() {

	vec4 localBinormal = vec4( atPosition.x + atBinormal.x, atPosition.y + atBinormal.y, atPosition.z + atBinormal.z, 1.0 );
	vec4 localTangent = vec4( atPosition.x + atTangent.x, atPosition.y + atTangent.y, atPosition.z + atTangent.z, 1.0 );
	localBinormal = uMatCamera * localBinormal;
	localTangent = uMatCamera * localTangent;

	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	localBinormal = localBinormal / localBinormal.w;
	localTangent = localTangent / localTangent.w;

	localBinormal = cameraPos + normalize(localBinormal - cameraPos);
	localTangent = cameraPos + normalize(localTangent - cameraPos);

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;


	vaPosR = uMatProjection * localTangent;
	vaPosU = uMatProjection * localBinormal;
	
	vaPos = vaPos / vaPos.w;
	vaPosR = vaPosR / vaPosR.w;
	vaPosU = vaPosU / vaPosU.w;

	vaColor = atColor;
	vaTexCoord = atTexCoord;
}

)";

	static const char g_sprite_fs_texture_distortion_src [] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
		R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
		R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
		R"(
precision mediump float;
)"
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
layout (location = 0) out vec4 FRAGCOLOR;
)"
#else
R"(
#define FRAGCOLOR gl_FragColor
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
#define TEX2D texture
)"
#else
R"(
#define IN varying
#define TEX2D texture2D
)"
#endif

		R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;
IN mediump vec4 vaPos;
IN mediump vec4 vaPosR;
IN mediump vec4 vaPosU;
)"

R"(
uniform sampler2D uTexture0;
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;
)"

R"(
void main() {
	vec4 output = TEX2D(uTexture0, vaTexCoord.xy);
	output.w = output.w * vaColor.w;

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (output.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (output.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	vec3 color = TEX2D(uBackTexture0, uv).xyz;
	output.xyz = color;

	FRAGCOLOR = output;
}
)";

	static const char g_sprite_fs_no_texture_distortion_src [] =
#if  defined(__EFFEKSEER_RENDERER_GL3__) 
		R"(
#version 330
#define lowp
#define mediump
#define highp
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
		R"(
#version 300 es
)"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
		R"(
precision mediump float;
)"
#else
		R"(
#version 110
#define lowp
#define mediump
#define highp
)"
#endif


#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
layout (location = 0) out vec4 FRAGCOLOR;
)"
#else
R"(
#define FRAGCOLOR gl_FragColor
)"
#endif

#if  defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
R"(
#define IN in
#define TEX2D texture
)"
#else
R"(
#define IN varying
#define TEX2D texture2D
)"
#endif


		R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;
IN mediump vec4 vaPos;
IN mediump vec4 vaPosR;
IN mediump vec4 vaPosU;
)"

R"(
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;

)"

R"(
void main() {
	vec4 output = vaColor;
	output.xyz = vec3(1.0,1.0,1.0);

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (output.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (output.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	vec3 color = TEX2D(uBackTexture0, uv).xyz;
	output.xyz = color;

	FRAGCOLOR = output;
}
)";

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer* Renderer::Create( int32_t squareMaxCount )
{
	GLExt::Initialize();

	RendererImplemented* renderer = new RendererImplemented( squareMaxCount );
	if( renderer->Initialize() )
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented( int32_t squareMaxCount )
	: m_reference	( 1 )
	, m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_renderState		( NULL )
	, m_restorationOfStates(true)
	, m_currentVertexArray( NULL )

	, m_shader(nullptr)
	, m_shader_no_texture(nullptr)
	, m_shader_distortion(nullptr)
	, m_shader_no_texture_distortion(nullptr)
	, m_standardRenderer(nullptr)

	, m_vao(nullptr)
	, m_vao_no_texture(nullptr)
	, m_vao_distortion(nullptr)
	, m_vao_no_texture_distortion(nullptr)

	, m_background(0)
	, m_distortingCallback(nullptr)
{
	::Effekseer::Vector3D direction( 1.0f, 1.0f, 1.0f );
	SetLightDirection( direction );
	::Effekseer::Color lightColor( 255, 255, 255, 255 );
	SetLightColor( lightColor );
	::Effekseer::Color lightAmbient( 40, 40, 40, 255 );
	SetLightAmbientColor( lightAmbient );

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Finalize();
#endif

	assert( m_reference == 0 );

	ES_SAFE_DELETE(m_distortingCallback);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_no_texture);
	ES_SAFE_DELETE(m_shader_distortion);
	ES_SAFE_DELETE(m_shader_no_texture_distortion);

	auto isVaoEnabled = m_vao != nullptr;

	ES_SAFE_DELETE(m_vao);
	ES_SAFE_DELETE(m_vao_no_texture);
	ES_SAFE_DELETE(m_vao_distortion);
	ES_SAFE_DELETE(m_vao_no_texture_distortion);

	ES_SAFE_DELETE( m_renderState );
	ES_SAFE_DELETE( m_vertexBuffer );
	ES_SAFE_DELETE( m_indexBuffer );

	if (isVaoEnabled)
	{
		assert(m_reference == -10);
	}
	else
	{
		assert(m_reference == -6);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
	for (auto& device : m_deviceObjects)
	{
		device->OnLostDevice();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnResetDevice()
{
	for (auto& device : m_deviceObjects)
	{
		device->OnResetDevice();
	}

	{
		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
			buf[0] = 3 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 0 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
		}

		m_indexBuffer->Unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize()
{
	// 頂点の生成
	{
		// 最大でfloat * 10 と仮定
		m_vertexBuffer = VertexBuffer::Create( this, sizeof(Vertex) * m_squareMaxCount * 4, true );
		if( m_vertexBuffer == NULL ) return false;
	}

	// 参照カウントの調整
	Release();


	// インデックスの生成
	{
		m_indexBuffer = IndexBuffer::Create( this, m_squareMaxCount * 6, false );
		if( m_indexBuffer == NULL ) return false;

		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
			buf[0] = (uint16_t)(3 + 4 * i);
			buf[1] = (uint16_t)(1 + 4 * i);
			buf[2] = (uint16_t)(0 + 4 * i);
			buf[3] = (uint16_t)(3 + 4 * i);
			buf[4] = (uint16_t)(0 + 4 * i);
			buf[5] = (uint16_t)(2 + 4 * i);
		}

		m_indexBuffer->Unlock();
	}

	// 参照カウントの調整
	Release();

	m_renderState = new RenderState( this );

	m_shader = Shader::Create(this,
		g_sprite_vs_src, strlen(g_sprite_vs_src), g_sprite_fs_texture_src, strlen(g_sprite_fs_texture_src), "SpriteRenderer");
	if (m_shader == nullptr) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture = Shader::Create(this,
		g_sprite_vs_src, strlen(g_sprite_vs_src), g_sprite_fs_no_texture_src, strlen(g_sprite_fs_no_texture_src), "SpriteRenderer");
	if (m_shader_no_texture == nullptr)
	{
		return false;
	}

	// 参照カウントの調整
	Release();

	m_shader_distortion = Shader::Create(this,
		g_sprite_distortion_vs_src, strlen(g_sprite_distortion_vs_src), g_sprite_fs_texture_distortion_src, strlen(g_sprite_fs_texture_distortion_src), "SpriteRenderer");
	if (m_shader_distortion == nullptr) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture_distortion = Shader::Create(this,
		g_sprite_distortion_vs_src, strlen(g_sprite_distortion_vs_src), g_sprite_fs_no_texture_distortion_src, strlen(g_sprite_fs_no_texture_distortion_src), "SpriteRenderer");
	if (m_shader_no_texture_distortion == nullptr)
	{
		return false;
	}

	// 参照カウントの調整
	Release();

	static ShaderAttribInfo sprite_attribs[3] = {
		{ "atPosition", GL_FLOAT, 3, 0, false },
		{ "atColor", GL_UNSIGNED_BYTE, 4, 12, true },
		{ "atTexCoord", GL_FLOAT, 2, 16, false }
	};

	static ShaderAttribInfo sprite_attribs_distortion[5] = {
		{ "atPosition", GL_FLOAT, 3, 0, false },
		{ "atColor", GL_UNSIGNED_BYTE, 4, 12, true },
		{ "atTexCoord", GL_FLOAT, 2, 16, false },
		{ "atBinormal", GL_FLOAT, 3, 24, false },
		{ "atTangent", GL_FLOAT, 3, 36, false },
	};

	// 頂点属性IDを取得
	m_shader->GetAttribIdList(3, sprite_attribs);
	m_shader->SetVertexSize(sizeof(Vertex));
	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	
	m_shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader->GetUniformId("uMatCamera"),
		0
		);

	m_shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44)
		);

	m_shader->SetTextureSlot(0, m_shader->GetUniformId("uTexture0"));

	m_shader_no_texture->GetAttribIdList(3, sprite_attribs);
	m_shader_no_texture->SetVertexSize(sizeof(Vertex));
	m_shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	
	m_shader_no_texture->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_no_texture->GetUniformId("uMatCamera"),
		0
		);

	m_shader_no_texture->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_no_texture->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44)
		);

	m_vao = VertexArray::Create(this, m_shader, GetVertexBuffer(), GetIndexBuffer());
	// 参照カウントの調整
	if (m_vao != nullptr) Release();

	m_vao_no_texture = VertexArray::Create(this, m_shader_no_texture, GetVertexBuffer(), GetIndexBuffer());
	// 参照カウントの調整
	if (m_vao_no_texture != nullptr) Release();

	// Distortion
	m_shader_distortion->GetAttribIdList(5, sprite_attribs_distortion);
	m_shader_distortion->SetVertexSize(sizeof(VertexDistortion));
	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4);

	m_shader_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_distortion->GetUniformId("uMatCamera"),
		0
		);

	m_shader_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_distortion->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44)
		);

	m_shader_distortion->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader_distortion->GetUniformId("g_scale"),
		0
		);

	m_shader_distortion->SetTextureSlot(0, m_shader_distortion->GetUniformId("uTexture0"));
	m_shader_distortion->SetTextureSlot(1, m_shader_distortion->GetUniformId("uBackTexture0"));

	m_shader_no_texture_distortion->GetAttribIdList(5, sprite_attribs_distortion);
	m_shader_no_texture_distortion->SetVertexSize(sizeof(VertexDistortion));
	m_shader_no_texture_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader_no_texture_distortion->SetPixelConstantBufferSize(sizeof(float) * 4);

	m_shader_no_texture_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_no_texture_distortion->GetUniformId("uMatCamera"),
		0
		);

	m_shader_no_texture_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_no_texture_distortion->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44)
		);

	m_shader_no_texture_distortion->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader_distortion->GetUniformId("g_scale"),
		0
		);

	m_shader_no_texture_distortion->SetTextureSlot(1, m_shader_no_texture_distortion->GetUniformId("uBackTexture0"));


	m_vao_distortion = VertexArray::Create(this, m_shader_distortion, GetVertexBuffer(), GetIndexBuffer());
	
	// 参照カウントの調整
	if (m_vao_distortion != nullptr) Release();

	m_vao_no_texture_distortion = VertexArray::Create(this, m_shader_no_texture_distortion, GetVertexBuffer(), GetIndexBuffer());
	
	// 参照カウントの調整
	if (m_vao_no_texture_distortion != nullptr) Release();

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, GLuint, Vertex, VertexDistortion>(this, m_shader, m_shader_no_texture, m_shader_distortion, m_shader_no_texture_distortion);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destory()
{
	Release();
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	m_restorationOfStates = flag;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	GLCheckError();

	::Effekseer::Matrix44::Mul( m_cameraProj, m_camera, m_proj );

	// ステートを保存する
	if(m_restorationOfStates)
	{
		m_originalState.blend = glIsEnabled(GL_BLEND);
		m_originalState.cullFace = glIsEnabled(GL_CULL_FACE);
		m_originalState.depthTest = glIsEnabled(GL_DEPTH_TEST);
		m_originalState.texture = glIsEnabled(GL_TEXTURE_2D);
		glGetBooleanv(GL_DEPTH_WRITEMASK, &m_originalState.depthWrite);
		glGetIntegerv(GL_DEPTH_FUNC, &m_originalState.depthFunc);
		glGetIntegerv(GL_BLEND_SRC_RGB, &m_originalState.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &m_originalState.blendDst);
		glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);
	}

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	m_renderState->GetActiveState().Reset();
	m_renderState->Update( true );
	m_currentTextures.clear();

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	GLCheckError();

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	// ステートを復元する
	if(m_restorationOfStates)
	{
		if (m_originalState.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (m_originalState.cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (m_originalState.depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);

#if !defined(__EFFEKSEER_RENDERER_GL3__)
		if (m_originalState.texture) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
#endif
		
		glDepthFunc(m_originalState.depthFunc);
		glDepthMask(m_originalState.depthWrite);
		glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
		GLExt::glBlendEquation(m_originalState.blendEquation);
	}

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	return m_indexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
{
	return m_lightDirection;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightDirection( ::Effekseer::Vector3D& direction )
{
	m_lightDirection = direction;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightColor() const
{
	return m_lightColor;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightColor( ::Effekseer::Color& color )
{
	m_lightColor = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
{
	return m_lightAmbient;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightAmbientColor( ::Effekseer::Color& color )
{
	m_lightAmbient = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
{
	return m_proj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetProjectionMatrix( const ::Effekseer::Matrix44& mat )
{
	m_proj = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
{
	return m_camera;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetCameraMatrix( const ::Effekseer::Matrix44& mat )
{
	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return SpriteRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return RibbonRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return RingRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return TrackRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader( fileInterface );
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader( fileInterface );
#else
	return NULL;
#endif
}

void RendererImplemented::SetBackground(GLuint background)
{
	m_background = background;
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return m_distortingCallback;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(m_distortingCallback);
	m_distortingCallback = callback;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer( VertexBuffer* vertexBuffer, int32_t size )
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetInterface());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(GLuint vertexBuffer, int32_t size)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer( IndexBuffer* indexBuffer )
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetIndexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetInterface());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(GLuint indexBuffer)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetIndexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexArray( VertexArray* vertexArray )
{
	m_currentVertexArray = vertexArray;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	GLCheckError();
	
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetVertexBuffer() == nullptr)
	{
		shader->EnableAttribs();
		shader->SetVertex();
		GLCheckError();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites( int32_t spriteCount, int32_t vertexOffset )
{
	GLCheckError();

	//assert( vertexOffset == 0 );

	// 全てがスプライトであること前提
	auto triangles = vertexOffset / 4 * 2;

	glDrawElements(GL_TRIANGLES, spriteCount * 6, GL_UNSIGNED_SHORT, (void*) (triangles * 3 * sizeof(GLushort)));
	
	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon( int32_t vertexCount, int32_t indexCount)
{
	GLCheckError();

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	// VAOの切り替え
	if (shader == m_shader)
	{
		SetVertexArray(m_vao);
	}
	else if (shader == m_shader_no_texture)
	{
		SetVertexArray(m_vao_no_texture);
	}
	else if (shader == m_shader_distortion)
	{
		SetVertexArray(m_vao_distortion);
	}
	else if (shader == m_shader_no_texture_distortion)
	{
		SetVertexArray(m_vao_no_texture_distortion);
	}

	shader->BeginScene();

	if (m_currentVertexArray)
	{
		GLExt::glBindVertexArray(m_currentVertexArray->GetInterface());
	}

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	GLCheckError();
	
	if (m_currentVertexArray)
	{
		if (m_currentVertexArray->GetVertexBuffer() == nullptr)
		{
			shader->DisableAttribs();
			GLCheckError();

			GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			GLCheckError();

			GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
			GLCheckError();
		}

		GLExt::glBindVertexArray(0);
		GLCheckError();
		m_currentVertexArray = nullptr;
	}
	else
	{
		shader->DisableAttribs();
		GLCheckError();

		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		GLCheckError();

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLCheckError();
	}

	shader->EndScene();
	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, GLuint* textures, int32_t count)
{
	GLCheckError();

	m_currentTextures.clear();
	m_currentTextures.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		
		m_currentTextures[i] = textures[i];

		if (shader->GetTextureSlotEnable(i))
		{
			GLExt::glUniform1i(shader->GetTextureSlot(i), i);
		}
	}
	GLExt::glActiveTexture(GL_TEXTURE0);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update( true );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Model::Model(::Effekseer::Model::Vertex vertexData [], int32_t vertexCount,
	::Effekseer::Model::Face faceData [], int32_t faceCount)
	: VertexBuffer(0)
	, IndexBuffer(0)
	, VertexCount(vertexCount)
	, IndexCount(faceCount * 3)
	, ModelCount(1)
{
	GLExt::glGenBuffers(1, &VertexBuffer);
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	size_t vertexSize = vertexCount * sizeof(::Effekseer::Model::Vertex);
	GLExt::glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLExt::glGenBuffers(1, &IndexBuffer);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	size_t indexSize = faceCount * sizeof(::Effekseer::Model::Face);
	GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, faceData, GL_STATIC_DRAW);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Model::~Model()
{
	GLExt::glDeleteBuffers( 1, &IndexBuffer );
	GLExt::glDeleteBuffers( 1, &VertexBuffer );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
