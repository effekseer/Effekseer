
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
//#include "EffekseerRendererGL.SpriteRenderer.h"
//#include "EffekseerRendererGL.RibbonRenderer.h"
//#include "EffekseerRendererGL.RingRenderer.h"
//#include "EffekseerRendererGL.TrackRenderer.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.TextureLoader.h"
#include "EffekseerRendererGL.ModelLoader.h"

#include "EffekseerRendererGL.GLExtension.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"

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
R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;

uniform sampler2D uTexture0;

void main()
{
	FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);
}
)";

static const char g_sprite_fs_no_texture_src[] =
R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;

void main()
{
	FRAGCOLOR = vaColor;
}
)";


static const char g_sprite_distortion_vs_src [] =
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
	vec4 color = TEX2D(uTexture0, vaTexCoord.xy);
	color.w = color.w * vaColor.w;

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (color.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (color.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
	//uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	color.xyz = TEX2D(uBackTexture0, uv).xyz;
	
	FRAGCOLOR = color;
}
)";

static const char g_sprite_fs_no_texture_distortion_src [] =
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
	vec4 color = vaColor;
	color.xyz = vec3(1.0,1.0,1.0);

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (color.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (color.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
	//uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	color.xyz = TEX2D(uBackTexture0, uv).xyz;
	
	FRAGCOLOR = color;
}
)";

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(fileInterface);
#else
	return NULL;
#endif
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer* Renderer::Create(int32_t squareMaxCount, OpenGLDeviceType deviceType)
{
	GLExt::Initialize(deviceType);

	RendererImplemented* renderer = new RendererImplemented( squareMaxCount, deviceType );
	if( renderer->Initialize() )
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount, OpenGLDeviceType deviceType)
	: m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_indexBufferForWireframe	( NULL )
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
	, m_distortingCallback(nullptr)

	, m_deviceType(deviceType)
{
	::Effekseer::Vector3D direction( 1.0f, 1.0f, 1.0f );
	SetLightDirection( direction );
	::Effekseer::Color lightColor( 255, 255, 255, 255 );
	SetLightColor( lightColor );
	::Effekseer::Color lightAmbient( 40, 40, 40, 255 );
	SetLightAmbientColor( lightAmbient );

	m_background.UserID = 0;

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

	assert( GetRef() == 0 );

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
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	if (isVaoEnabled)
	{
		assert(GetRef() == -11);
	}
	else
	{
		assert(GetRef() == -6);
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

	GenerateIndexData();
}

//----------------------------------------------------------------------------------
// インデックスデータの生成
//----------------------------------------------------------------------------------
void RendererImplemented::GenerateIndexData()
{
	// インデックスの生成
	if( m_indexBuffer != NULL )
	{
		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*) m_indexBuffer->GetBufferDirect(6);
			buf[0] = (uint16_t) (3 + 4 * i);
			buf[1] = (uint16_t) (1 + 4 * i);
			buf[2] = (uint16_t) (0 + 4 * i);
			buf[3] = (uint16_t) (3 + 4 * i);
			buf[4] = (uint16_t) (0 + 4 * i);
			buf[5] = (uint16_t) (2 + 4 * i);
		}

		m_indexBuffer->Unlock();
	}

	// ワイヤーフレーム用インデックスの生成
	if( m_indexBufferForWireframe != NULL )
	{
		m_indexBufferForWireframe->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBufferForWireframe->GetBufferDirect( 8 );
			buf[0] = (uint16_t)(0 + 4 * i);
			buf[1] = (uint16_t)(1 + 4 * i);
			buf[2] = (uint16_t)(2 + 4 * i);
			buf[3] = (uint16_t)(3 + 4 * i);
			buf[4] = (uint16_t)(0 + 4 * i);
			buf[5] = (uint16_t)(2 + 4 * i);
			buf[6] = (uint16_t)(1 + 4 * i);
			buf[7] = (uint16_t)(3 + 4 * i);
		}

		m_indexBufferForWireframe->Unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize()
{
	SetSquareMaxCount( m_squareMaxCount );

	m_renderState = new RenderState( this );

	m_shader = Shader::Create(this,
		g_sprite_vs_src, sizeof(g_sprite_vs_src), 
		g_sprite_fs_texture_src, sizeof(g_sprite_fs_texture_src), 
		"Standard Tex");
	if (m_shader == nullptr) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture = Shader::Create(this,
		g_sprite_vs_src, sizeof(g_sprite_vs_src), 
		g_sprite_fs_no_texture_src, sizeof(g_sprite_fs_no_texture_src), 
		"Standard NoTex");
	if (m_shader_no_texture == nullptr)
	{
		return false;
	}

	// 参照カウントの調整
	Release();

	m_shader_distortion = Shader::Create(this,
		g_sprite_distortion_vs_src, sizeof(g_sprite_distortion_vs_src), 
		g_sprite_fs_texture_distortion_src, sizeof(g_sprite_fs_texture_distortion_src), 
		"Standard Distortion Tex");
	if (m_shader_distortion == nullptr) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture_distortion = Shader::Create(this,
		g_sprite_distortion_vs_src, sizeof(g_sprite_distortion_vs_src), 
		g_sprite_fs_no_texture_distortion_src, sizeof(g_sprite_fs_no_texture_distortion_src), 
		"Standard Distortion NoTex");
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

	m_vao_wire_frame = VertexArray::Create(this, m_shader_no_texture, GetVertexBuffer(), m_indexBufferForWireframe);
	if (m_vao_wire_frame != nullptr) Release();

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(this, m_shader, m_shader_no_texture, m_shader_distortion, m_shader_no_texture_distortion);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destroy()
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

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			m_originalState.texture = glIsEnabled(GL_TEXTURE_2D);
		}

		glGetBooleanv(GL_DEPTH_WRITEMASK, &m_originalState.depthWrite);
		glGetIntegerv(GL_DEPTH_FUNC, &m_originalState.depthFunc);
		glGetIntegerv(GL_CULL_FACE_MODE, &m_originalState.cullFaceMode);
		glGetIntegerv(GL_BLEND_SRC_RGB, &m_originalState.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &m_originalState.blendDst);
		glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);

		if (GLExt::IsSupportedVertexArray())
		{
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_originalState.vao);
		}
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

	// reset renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	// restore states
	if(m_restorationOfStates)
	{
		if (m_originalState.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (m_originalState.cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (m_originalState.depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		
		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			if (m_originalState.texture) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
		}
		
		glDepthFunc(m_originalState.depthFunc);
		glDepthMask(m_originalState.depthWrite);
		glCullFace(m_originalState.cullFaceMode);
		glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
		GLExt::glBlendEquation(m_originalState.blendEquation);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3)
		{
			for( int32_t i = 0; i < 4; i++ )
			{
				GLExt::glBindSampler(i, 0);
			}
		}

		if (GLExt::IsSupportedVertexArray())
		{
			GLExt::glBindVertexArray(m_originalState.vao);
		}
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
void RendererImplemented::SetSquareMaxCount(int32_t count)
{
	m_squareMaxCount = count;

	if (m_vertexBuffer != nullptr) AddRef();
	if (m_indexBuffer != nullptr) AddRef();
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);

	// 頂点の生成
	{
		// 最大でfloat * 10 と仮定
		m_vertexBuffer = VertexBuffer::Create(this, sizeof(Vertex) * m_squareMaxCount * 4, true);
		if (m_vertexBuffer == NULL) return;
	}

	// 参照カウントの調整
	Release();


	// インデックスの生成
	{
		m_indexBuffer = IndexBuffer::Create(this, m_squareMaxCount * 6, false);
		if (m_indexBuffer == NULL) return;
	}

	// 参照カウントの調整
	Release();

	// ワイヤーフレーム用インデックスの生成
	{
		m_indexBufferForWireframe = IndexBuffer::Create( this, m_squareMaxCount * 8, false );
		if( m_indexBufferForWireframe == NULL ) return;
	}

	// 参照カウントの調整
	Release();

	// インデックスデータの生成
	GenerateIndexData();
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
void RendererImplemented::SetLightDirection( const ::Effekseer::Vector3D& direction )
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
void RendererImplemented::SetLightColor( const ::Effekseer::Color& color )
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
void RendererImplemented::SetLightAmbientColor( const ::Effekseer::Color& color )
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
	m_cameraFrontDirection = ::Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);

	auto localPos = ::Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
	auto f = m_cameraFrontDirection;
	auto r = ::Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
	auto u = ::Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

	m_cameraPosition = r * localPos.X + u * localPos.Y + f * localPos.Z;

	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

::Effekseer::Vector3D RendererImplemented::GetCameraFrontDirection() const
{
	return m_cameraFrontDirection;
}

::Effekseer::Vector3D RendererImplemented::GetCameraPosition() const
{
	return m_cameraPosition;
}

void RendererImplemented::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	m_cameraFrontDirection = front;
	m_cameraPosition = position;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
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
	return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
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
	m_background.UserID = background;
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
	if (m_currentVertexArray == nullptr ||
		m_currentVertexArray->GetVertexBuffer() == nullptr)
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
	
	if (m_currentVertexArray == nullptr || 
		m_currentVertexArray->GetVertexBuffer() == nullptr)
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

	drawcallCount++;
	drawvertexCount += spriteCount * 4;

	if( m_renderMode == ::Effekseer::RenderMode::Normal )
	{
		glDrawElements(GL_TRIANGLES, spriteCount * 6, GL_UNSIGNED_SHORT, (void*) (vertexOffset / 4 * 6 * sizeof(GLushort)));
	}
	else if( m_renderMode == ::Effekseer::RenderMode::Wireframe )
	{
		glDrawElements(GL_LINES, spriteCount * 8, GL_UNSIGNED_SHORT, (void*) (vertexOffset / 4 * 8 * sizeof(GLushort)));
	}
	
	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon( int32_t vertexCount, int32_t indexCount)
{
	GLCheckError();

	drawcallCount++;
	drawvertexCount += vertexCount;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Shader* RendererImplemented::GetShader(bool useTexture, bool useDistortion) const
{
	if( useDistortion )
	{
		if( useTexture && m_renderMode == Effekseer::RenderMode::Normal )
		{
			return m_shader_distortion;
		}
		else
		{
			return m_shader_no_texture_distortion;
		}
	}
	else
	{
		if( useTexture && m_renderMode == Effekseer::RenderMode::Normal )
		{
			return m_shader;
		}
		else
		{
			return m_shader_no_texture;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	// VAOの切り替え
	if (m_renderMode == ::Effekseer::RenderMode::Wireframe)
	{
		SetVertexArray(m_vao_wire_frame);
	}
	else if (shader == m_shader)
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

	assert(currentShader == nullptr);
	currentShader = shader;

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	assert(currentShader == shader);
	currentShader = nullptr;

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

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size)
{
	assert(currentShader != nullptr);
	memcpy(currentShader->GetVertexConstantBuffer(), data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size)
{
	assert(currentShader != nullptr);
	memcpy(currentShader->GetPixelConstantBuffer(), data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	GLCheckError();

	m_currentTextures.clear();
	m_currentTextures.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		auto id = 0;
		if (textures[i] != nullptr)
		{
			id = textures[i]->UserID;
		}

		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, id);
		
		m_currentTextures[i] = id;

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

int32_t RendererImplemented::GetDrawCallCount() const
{
	return drawcallCount;
}

int32_t RendererImplemented::GetDrawVertexCount() const
{
	return drawvertexCount;
}

void RendererImplemented::ResetDrawCallCount()
{
	drawcallCount = 0;
}

void RendererImplemented::ResetDrawVertexCount()
{
	drawvertexCount = 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

Model::InternalModel::InternalModel()
{
	VertexBuffer = 0;
	IndexBuffer = 0;
}

Model::InternalModel::~InternalModel()
{
	GLExt::glDeleteBuffers(1, &IndexBuffer);
	GLExt::glDeleteBuffers(1, &VertexBuffer);
}

Model::Model(void* data, int32_t size)
	: ::Effekseer::Model(data, size)
	, InternalModels(nullptr)
	, ModelCount(1)
{
	InternalModels = new Model::InternalModel[GetFrameCount()];

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		auto vertexData = GetVertexes(f);
		auto vertexCount = GetVertexCount(f);
		auto faceData = GetFaces(f);
		auto faceCount = GetFaceCount(f);

		InternalModels[f].VertexCount = vertexCount;
		InternalModels[f].IndexCount = faceCount * 3;

		GLExt::glGenBuffers(1, &InternalModels[f].VertexBuffer);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, InternalModels[f].VertexBuffer);
		size_t vertexSize = vertexCount * sizeof(::Effekseer::Model::Vertex);
		GLExt::glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLExt::glGenBuffers(1, &InternalModels[f].IndexBuffer);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, InternalModels[f].IndexBuffer);
		size_t indexSize = faceCount * sizeof(::Effekseer::Model::Face);
		GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, faceData, GL_STATIC_DRAW);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

Model::~Model()
{
	ES_SAFE_DELETE_ARRAY(InternalModels);

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
