
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RenderState.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.VertexBuffer.h"
//#include "EffekseerRendererGL.SpriteRenderer.h"
//#include "EffekseerRendererGL.RibbonRenderer.h"
//#include "EffekseerRendererGL.RingRenderer.h"
//#include "EffekseerRendererGL.TrackRenderer.h"
#include "EffekseerRendererGL.MaterialLoader.h"
#include "EffekseerRendererGL.ModelLoader.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.TextureLoader.h"

#include "EffekseerRendererGL.GLExtension.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
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
static const char g_sprite_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
)"

	R"(
CENTROID OUT vec4 vaColor;
CENTROID OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;

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

	vaTexCoord.y = mUVInversed.x + mUVInversed.y * vaTexCoord.y;
}

)";

static const char g_sprite_fs_texture_src[] =
	R"(
CENTROID IN lowp vec4 vaColor;
CENTROID IN mediump vec4 vaTexCoord;

uniform sampler2D uTexture0;

void main()
{
	FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);

	if(FRAGCOLOR.w <= 0.0) discard;
}
)";

static const char g_sprite_fs_no_texture_src[] =
	R"(
CENTROID IN lowp vec4 vaColor;
CENTROID IN mediump vec4 vaTexCoord;

void main()
{
	FRAGCOLOR = vaColor;

	if(FRAGCOLOR.w <= 0.0) discard;
}
)";

static const char g_sprite_distortion_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
IN vec4 atBinormal;
IN vec4 atTangent;

)"

	R"(
CENTROID OUT vec4 vaColor;
CENTROID OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"
	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;

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

	vaTexCoord.y = mUVInversed.x + mUVInversed.y * vaTexCoord.y;
}

)";

static const char g_sprite_fs_texture_distortion_src[] =
	R"(
CENTROID IN lowp vec4 vaColor;
CENTROID IN mediump vec4 vaTexCoord;
IN mediump vec4 vaPos;
IN mediump vec4 vaPosR;
IN mediump vec4 vaPosU;
)"

	R"(
uniform sampler2D uTexture0;
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;
uniform	vec4	mUVInversedBack;

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

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	color.xyz = TEX2D(uBackTexture0, uv).xyz;
	
	FRAGCOLOR = color;

	if(FRAGCOLOR.w <= 0.0) discard;
}
)";

static const char g_sprite_fs_no_texture_distortion_src[] =
	R"(
CENTROID IN lowp vec4 vaColor;
CENTROID IN mediump vec4 vaTexCoord;
IN mediump vec4 vaPos;
IN mediump vec4 vaPosR;
IN mediump vec4 vaPosU;
)"

	R"(
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;
uniform	vec4	mUVInversedBack;

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

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	color.xyz = TEX2D(uBackTexture0, uv).xyz;
	
	FRAGCOLOR = color;

	if(FRAGCOLOR.w <= 0.0) discard;
}
)";

static const char g_sprite_vs_lighting_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec3 atNormal;
IN vec3 atTangent;
IN vec2 atTexCoord;
IN vec2 atTexCoord2;
)"

	R"(
CENTROID OUT lowp vec4 v_VColor;
CENTROID OUT mediump vec2 v_UV1;
OUT mediump vec2 v_UV2;
OUT mediump vec3 v_WorldP;
OUT mediump vec3 v_WorldN;
OUT mediump vec3 v_WorldT;
OUT mediump vec3 v_WorldB;
OUT mediump vec2 v_ScreenUV;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;

)"

	R"(
void main() {
	vec3 worldPos = atPosition.xyz;

	// UV
	vec2 uv1 = atTexCoord.xy;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = atTexCoord2.xy;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	vec3 worldNormal = (atNormal - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldTangent = (atTangent - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldBinormal = cross(worldNormal, worldTangent);

	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
	vec3 pixelNormalDir = vec3(0.5, 0.5, 1.0);

	vec4 cameraPos = uMatCamera * vec4(worldPos, 1.0);
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	v_WorldP = worldPos;
	v_VColor = atColor;

	v_UV1 = uv1;
	v_UV2 = uv2;
	v_ScreenUV.xy = gl_Position.xy / gl_Position.w;
	v_ScreenUV.xy = vec2(v_ScreenUV.x + 1.0, v_ScreenUV.y + 1.0) * 0.5;
}

)";

static const char g_sprite_fs_lighting_src[] =
	R"(

CENTROID IN lowp vec4 v_VColor;
CENTROID IN mediump vec2 v_UV1;
IN mediump vec2 v_UV2;
IN mediump vec3 v_WorldP;
IN mediump vec3 v_WorldN;
IN mediump vec3 v_WorldT;
IN mediump vec3 v_WorldB;
IN mediump vec2 v_ScreenUV;

uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;

void main()
{
	vec3 texNormal = (TEX2D(NormalTexture, v_UV1.xy).xyz - 0.5) * 2.0;
	mat3 normalMatrix = mat3(v_WorldT.xyz, v_WorldB.xyz, v_WorldN.xyz );
	vec3 localNormal = normalize( normalMatrix * texNormal );
	float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
	
	FRAGCOLOR = v_VColor * TEX2D(ColorTexture, v_UV1.xy);
	FRAGCOLOR.xyz = FRAGCOLOR.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);

	if(FRAGCOLOR.w <= 0.0) discard;
}


)";

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface, ::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(fileInterface, colorSpaceType);
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

Renderer* Renderer::Create(int32_t squareMaxCount, OpenGLDeviceType deviceType, DeviceObjectCollection* deviceObjectCollection)
{
	GLExt::Initialize(deviceType);

	RendererImplemented* renderer = new RendererImplemented(squareMaxCount, deviceType, deviceObjectCollection);
	if (renderer->Initialize())
	{
		return renderer;
	}
	return NULL;
}

int32_t RendererImplemented::GetIndexSpriteCount() const
{
	int vsSize = EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4;
	return (vsSize / sizeof(Vertex) / 4 + 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount,
										 OpenGLDeviceType deviceType,
										 DeviceObjectCollection* deviceObjectCollection)
	: m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_indexBufferForWireframe(NULL)
	, m_squareMaxCount(squareMaxCount)
	, m_renderState(NULL)
	, m_restorationOfStates(true)
	, m_currentVertexArray(NULL)

	, m_shader(nullptr)
	, m_shader_distortion(nullptr)
	, m_standardRenderer(nullptr)

	, m_vao(nullptr)
	, m_vao_distortion(nullptr)
	, m_vao_wire_frame(nullptr)
	, m_distortingCallback(nullptr)

	, m_deviceType(deviceType)
	, deviceObjectCollection_(deviceObjectCollection)
{
	m_background.UserID = 0;
	m_background.HasMipmap = false;

	if (deviceObjectCollection == nullptr)
	{
		deviceObjectCollection_ = new DeviceObjectCollection();
	}
	else
	{
		ES_SAFE_ADDREF(deviceObjectCollection_);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	GetImpl()->DeleteProxyTextures(this);

	ES_SAFE_DELETE(m_distortingCallback);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_distortion);
	ES_SAFE_DELETE(m_shader_lighting);

	auto isVaoEnabled = m_vao != nullptr;

	ES_SAFE_DELETE(m_vao);
	ES_SAFE_DELETE(m_vao_distortion);
	ES_SAFE_DELETE(m_vao_wire_frame);
	ES_SAFE_DELETE(m_vao_lighting);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	ES_SAFE_RELEASE(deviceObjectCollection_);

	if (GLExt::IsSupportedVertexArray() && defaultVertexArray_ > 0)
	{
		GLExt::glDeleteVertexArrays(1, &defaultVertexArray_);
		defaultVertexArray_ = 0;
	}
}

void RendererImplemented::OnLostDevice()
{
	if (deviceObjectCollection_ != nullptr)
		deviceObjectCollection_->OnLostDevice();
}

void RendererImplemented::OnResetDevice()
{
	if (deviceObjectCollection_ != nullptr)
		deviceObjectCollection_->OnResetDevice();

	GenerateIndexData();
}

void RendererImplemented::GenerateIndexData()
{
	if (indexBufferStride_ == 2)
	{
		GenerateIndexDataStride<uint16_t>();
	}
	else if (indexBufferStride_ == 4)
	{
		GenerateIndexDataStride<uint32_t>();
	}
}

template <typename T>
void RendererImplemented::GenerateIndexDataStride()
{
	// generate an index buffer
	if (m_indexBuffer != nullptr)
	{
		m_indexBuffer->Lock();

		for (int i = 0; i < GetIndexSpriteCount(); i++)
		{
			std::array<T, 6> buf;
			buf[0] = (T)(3 + 4 * i);
			buf[1] = (T)(1 + 4 * i);
			buf[2] = (T)(0 + 4 * i);
			buf[3] = (T)(3 + 4 * i);
			buf[4] = (T)(0 + 4 * i);
			buf[5] = (T)(2 + 4 * i);
			memcpy(m_indexBuffer->GetBufferDirect(6), buf.data(), sizeof(T) * 6);
		}

		m_indexBuffer->Unlock();
	}

	// generate an index buffer for a wireframe
	if (m_indexBufferForWireframe != nullptr)
	{
		m_indexBufferForWireframe->Lock();

		for (int i = 0; i < GetIndexSpriteCount(); i++)
		{
			std::array<T, 8> buf;
			buf[0] = (T)(0 + 4 * i);
			buf[1] = (T)(1 + 4 * i);
			buf[2] = (T)(2 + 4 * i);
			buf[3] = (T)(3 + 4 * i);
			buf[4] = (T)(0 + 4 * i);
			buf[5] = (T)(2 + 4 * i);
			buf[6] = (T)(1 + 4 * i);
			buf[7] = (T)(3 + 4 * i);
			memcpy(m_indexBufferForWireframe->GetBufferDirect(8), buf.data(), sizeof(T) * 8);
		}

		m_indexBufferForWireframe->Unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize()
{
	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	}

	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	if (GetIndexSpriteCount() * 4 > 65536)
	{
		indexBufferStride_ = 4;
	}

	SetSquareMaxCount(m_squareMaxCount);

	m_renderState = new RenderState(this);

	m_shader = Shader::Create(this->GetDeviceType(),
							  this->GetDeviceObjectCollection(),
							  g_sprite_vs_src,
							  sizeof(g_sprite_vs_src),
							  g_sprite_fs_texture_src,
							  sizeof(g_sprite_fs_texture_src),
							  "Standard Tex",
							  false);
	if (m_shader == nullptr)
		return false;

	m_shader_distortion = Shader::Create(this->GetDeviceType(),
										 this->GetDeviceObjectCollection(),
										 g_sprite_distortion_vs_src,
										 sizeof(g_sprite_distortion_vs_src),
										 g_sprite_fs_texture_distortion_src,
										 sizeof(g_sprite_fs_texture_distortion_src),
										 "Standard Distortion Tex",
										 false);
	if (m_shader_distortion == nullptr)
		return false;

	static ShaderAttribInfo sprite_attribs[3] = {
		{"atPosition", GL_FLOAT, 3, 0, false},
		{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
		{"atTexCoord", GL_FLOAT, 2, 16, false}};

	static ShaderAttribInfo sprite_attribs_distortion[5] = {
		{"atPosition", GL_FLOAT, 3, 0, false},
		{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
		{"atTexCoord", GL_FLOAT, 2, 16, false},
		{"atBinormal", GL_FLOAT, 3, 24, false},
		{"atTangent", GL_FLOAT, 3, 36, false},
	};

	// 頂点属性IDを取得
	m_shader->GetAttribIdList(3, sprite_attribs);
	m_shader->SetVertexSize(sizeof(Vertex));
	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);

	m_shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader->GetUniformId("uMatCamera"),
		0);

	m_shader->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44));

	m_shader->AddVertexConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader->GetUniformId("mUVInversed"),
		sizeof(Effekseer::Matrix44) * 2);

	m_shader->SetTextureSlot(0, m_shader->GetUniformId("uTexture0"));

	m_vao = VertexArray::Create(this, m_shader, GetVertexBuffer(), GetIndexBuffer(), false);

	// Distortion
	m_shader_distortion->GetAttribIdList(5, sprite_attribs_distortion);
	m_shader_distortion->SetVertexSize(sizeof(VertexDistortion));
	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);

	m_shader_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_distortion->GetUniformId("uMatCamera"),
		0);

	m_shader_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44,
		m_shader_distortion->GetUniformId("uMatProjection"),
		sizeof(Effekseer::Matrix44));

	m_shader_distortion->AddVertexConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader_distortion->GetUniformId("mUVInversed"),
		sizeof(Effekseer::Matrix44) * 2);

	m_shader_distortion->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader_distortion->GetUniformId("g_scale"),
		0);

	m_shader_distortion->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4,
		m_shader_distortion->GetUniformId("mUVInversedBack"),
		sizeof(float) * 4);

	m_shader_distortion->SetTextureSlot(0, m_shader_distortion->GetUniformId("uTexture0"));
	m_shader_distortion->SetTextureSlot(1, m_shader_distortion->GetUniformId("uBackTexture0"));

	m_vao_distortion = VertexArray::Create(this, m_shader_distortion, GetVertexBuffer(), GetIndexBuffer(), false);

	// Lighting
	EffekseerRendererGL::ShaderAttribInfo sprite_attribs_lighting[6] = {
		{"atPosition", GL_FLOAT, 3, 0, false},
		{"atColor", GL_UNSIGNED_BYTE, 4, 12, true},
		{"atNormal", GL_UNSIGNED_BYTE, 4, 16, true},
		{"atTangent", GL_UNSIGNED_BYTE, 4, 20, true},
		{"atTexCoord", GL_FLOAT, 2, 24, false},
		{"atTexCoord2", GL_FLOAT, 2, 32, false},
	};

	m_shader_lighting = Shader::Create(this->GetDeviceType(),
									   this->GetDeviceObjectCollection(),
									   g_sprite_vs_lighting_src,
									   sizeof(g_sprite_vs_lighting_src),
									   g_sprite_fs_lighting_src,
									   sizeof(g_sprite_fs_lighting_src),
									   "Standard Lighting Tex",
									   false);

	m_shader_lighting->GetAttribIdList(5, sprite_attribs_lighting);
	m_shader_lighting->SetVertexSize(sizeof(EffekseerRenderer::DynamicVertex));
	m_shader_lighting->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader_lighting->SetPixelConstantBufferSize(sizeof(float) * 4 * 3);

	m_shader_lighting->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, m_shader_lighting->GetUniformId("uMatCamera"), 0);

	m_shader_lighting->AddVertexConstantLayout(
		CONSTANT_TYPE_MATRIX44, m_shader_lighting->GetUniformId("uMatProjection"), sizeof(Effekseer::Matrix44));

	m_shader_lighting->AddVertexConstantLayout(
		CONSTANT_TYPE_VECTOR4, m_shader_lighting->GetUniformId("mUVInversed"), sizeof(Effekseer::Matrix44) * 2);

	m_shader_lighting->AddPixelConstantLayout(
		CONSTANT_TYPE_VECTOR4, m_shader_lighting->GetUniformId("LightDirection"), sizeof(float[4]) * 0);
	m_shader_lighting->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, m_shader_lighting->GetUniformId("LightColor"), sizeof(float[4]) * 1);
	m_shader_lighting->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, m_shader_lighting->GetUniformId("LightAmbient"), sizeof(float[4]) * 2);

	m_shader_lighting->SetTextureSlot(0, m_shader_lighting->GetUniformId("ColorTexture"));
	m_shader_lighting->SetTextureSlot(1, m_shader_lighting->GetUniformId("NormalTexture"));

	m_vao_lighting = VertexArray::Create(this, m_shader_lighting, GetVertexBuffer(), GetIndexBuffer(), false);

	m_vao_wire_frame = VertexArray::Create(this, m_shader, GetVertexBuffer(), m_indexBufferForWireframe, false);

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(this, m_shader, m_shader_distortion);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(currentVAO);
	}

	GetImpl()->CreateProxyTextures(this);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glGenVertexArrays(1, &defaultVertexArray_);
	}

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

	impl->CalculateCameraProjectionMatrix();

	// store state
	if (m_restorationOfStates)
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
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_originalState.arrayBufferBinding);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_originalState.elementArrayBufferBinding);

		for (size_t i = 0; i < m_originalState.boundTextures.size(); i++)
		{
			GLint bound = 0;
			GLExt::glActiveTexture(GL_TEXTURE0 + i);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
			m_originalState.boundTextures[i] = bound;
		}

		if (GLExt::IsSupportedVertexArray())
		{
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_originalState.vao);
		}
	}

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	currentTextures_.clear();
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	m_renderState->GetActiveState().TextureIDs.fill(0);

	// reset renderer
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
	if (m_restorationOfStates)
	{
		if (GLExt::IsSupportedVertexArray())
		{
			GLExt::glBindVertexArray(m_originalState.vao);
		}

		for (size_t i = 0; i < m_originalState.boundTextures.size(); i++)
		{
			GLExt::glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_originalState.boundTextures[i]);
		}
		GLExt::glActiveTexture(GL_TEXTURE0);

		if (m_originalState.blend)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		if (m_originalState.cullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		if (m_originalState.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL2)
		{
			if (m_originalState.texture)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		}

		glDepthFunc(m_originalState.depthFunc);
		glDepthMask(m_originalState.depthWrite);
		glCullFace(m_originalState.cullFaceMode);
		glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
		GLExt::glBlendEquation(m_originalState.blendEquation);

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_originalState.arrayBufferBinding);
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_originalState.elementArrayBufferBinding);

		if (GetDeviceType() == OpenGLDeviceType::OpenGL3 || GetDeviceType() == OpenGLDeviceType::OpenGLES3)
		{
			for (int32_t i = 0; i < 4; i++)
			{
				GLExt::glBindSampler(i, 0);
			}
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
	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	m_squareMaxCount = count;

	if (m_vertexBuffer != nullptr)
		AddRef();
	if (m_indexBuffer != nullptr)
		AddRef();
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);

	// generate a vertex buffer
	{
		m_vertexBuffer =
			VertexBuffer::Create(this, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true, false);
		if (m_vertexBuffer == NULL)
			return;
	}

	// generate an index buffer
	{
		m_indexBuffer = IndexBuffer::Create(this, GetIndexSpriteCount() * 6, false, indexBufferStride_, false);
		if (m_indexBuffer == nullptr)
			return;
	}

	// generate an index buffer for a wireframe
	{
		m_indexBufferForWireframe = IndexBuffer::Create(this, GetIndexSpriteCount() * 8, false, indexBufferStride_, false);
		if (m_indexBufferForWireframe == nullptr)
			return;
	}

	// generate index data
	GenerateIndexData();

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
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
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
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
	return ModelRenderer::Create(this);
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
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(fileInterface);
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new MaterialLoader(this->GetDeviceType(), this, this->GetDeviceObjectCollection(), fileInterface);
#else
	return nullptr;
#endif
}

void RendererImplemented::SetBackground(GLuint background, bool hasMipmap)
{
	m_background.UserID = background;
	m_background.HasMipmap = hasMipmap;
}

void RendererImplemented::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	m_background = *textureData;
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
void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
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
void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	if (m_currentVertexArray == nullptr || m_currentVertexArray->GetIndexBuffer() == nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetInterface());
		indexBufferCurrentStride_ = indexBuffer->GetStride();
	}
	else
	{
		indexBufferCurrentStride_ = m_currentVertexArray->GetIndexBuffer()->GetStride();
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
		indexBufferCurrentStride_ = 4;
	}
	else
	{
		indexBufferCurrentStride_ = m_currentVertexArray->GetIndexBuffer()->GetStride();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexArray(VertexArray* vertexArray)
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
void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	GLsizei stride = GL_UNSIGNED_SHORT;
	if (indexBufferCurrentStride_ == 4)
	{
		stride = GL_UNSIGNED_INT;
	}

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		glDrawElements(GL_TRIANGLES, spriteCount * 6, stride, reinterpret_cast<GLvoid*>(static_cast<size_t>(vertexOffset / 4 * 6 * indexBufferCurrentStride_)));
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		glDrawElements(GL_LINES, spriteCount * 8, stride, reinterpret_cast<GLvoid*>(static_cast<size_t>(vertexOffset / 4 * 8 * indexBufferCurrentStride_)));
	}

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	GLCheckError();

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Shader* RendererImplemented::GetShader(bool useTexture, ::Effekseer::RendererMaterialType materialType) const
{
	if (materialType == ::Effekseer::RendererMaterialType::BackDistortion)
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader_distortion;
		}
		else
		{
			return m_shader_distortion;
		}
	}
	else if (materialType == ::Effekseer::RendererMaterialType::Lighting)
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader_lighting;
		}
		else
		{
			return m_shader_lighting;
		}
	}
	else
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader;
		}
		else
		{
			return m_shader;
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	// change VAO with shader
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		SetVertexArray(m_vao_wire_frame);
	}
	else if (shader == m_shader)
	{
		SetVertexArray(m_vao);
	}
	else if (shader == m_shader_distortion)
	{
		SetVertexArray(m_vao_distortion);
	}
	else if (shader == m_shader_lighting)
	{
		SetVertexArray(m_vao_lighting);
	}
	else if (m_currentVertexArray != nullptr)
	{
		SetVertexArray(m_currentVertexArray);
	}
	else
	{
		m_currentVertexArray = nullptr;

		if (defaultVertexArray_ > 0)
		{
			GLExt::glBindVertexArray(defaultVertexArray_);
		}
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

		if (defaultVertexArray_ > 0)
		{
			GLExt::glBindVertexArray(0);
		}
	}

	shader->EndScene();
	GLCheckError();
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	GLCheckError();

	for (int i = count; i < currentTextures_.size(); i++)
	{
		m_renderState->GetActiveState().TextureIDs[i] = 0;
	}

	currentTextures_.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		GLuint id = 0;
		if (textures[i] != nullptr)
		{
			id = (GLuint)textures[i]->UserID;
		}

		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, id);

		if (textures[i] != nullptr)
		{
			m_renderState->GetActiveState().TextureIDs[i] = textures[i]->UserID;
			currentTextures_[i] = *textures[i];
		}
		else
		{
			currentTextures_[i].UserID = 0;
			currentTextures_[i].UserPtr = nullptr;
			m_renderState->GetActiveState().TextureIDs[i] = 0;
		}

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
	m_renderState->Update(true);
}

Effekseer::TextureData* RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{

	GLint bound = 0;
	GLExt::glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

	std::array<uint8_t, 4> buf;

	if (type == EffekseerRenderer::ProxyTextureType::White)
	{
		buf.fill(255);
	}
	else if (type == EffekseerRenderer::ProxyTextureType::Normal)
	{
		buf.fill(127);
		buf[2] = 255;
		buf[3] = 255;
	}
	else
	{
		assert(0);
	}

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 1,
				 1,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 buf.data());

	// Generate mipmap
	GLExt::glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bound);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = nullptr;
	textureData->UserID = texture;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 1;
	textureData->Height = 1;
	return textureData;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
	if (data != nullptr)
	{
		GLuint texture = (GLuint)data->UserID;
		glDeleteTextures(1, &texture);
		delete data;
	}
}

bool RendererImplemented::IsVertexArrayObjectSupported() const
{
	return GLExt::IsSupportedVertexArray();
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
	if (VertexBuffer > 0)
	{
		GLExt::glDeleteBuffers(1, &IndexBuffer);
	}

	if (IndexBuffer > 0)
	{
		GLExt::glDeleteBuffers(1, &VertexBuffer);
	}
}

bool Model::InternalModel::TryDelayLoad()
{
	if (VertexBuffer > 0)
		return false;

	int arrayBufferBinding = 0;
	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	GLExt::glGenBuffers(1, &VertexBuffer);
	if (VertexBuffer > 0)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		GLExt::glBufferData(GL_ARRAY_BUFFER, delayVertexBuffer.size(), delayVertexBuffer.data(), GL_STATIC_DRAW);
	}

	GLExt::glGenBuffers(1, &IndexBuffer);
	if (IndexBuffer > 0)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
		GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, delayIndexBuffer.size(), delayIndexBuffer.data(), GL_STATIC_DRAW);
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

	return true;
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

		//GLExt::glGenBuffers(1, &InternalModels[f].VertexBuffer);
		size_t vertexSize = vertexCount * sizeof(::Effekseer::Model::Vertex);

		//int arrayBufferBinding = 0;
		//int elementArrayBufferBinding = 0;
		//glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);
		//glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

		//if (InternalModels[f].VertexBuffer > 0)
		//{
		//	GLExt::glBindBuffer(GL_ARRAY_BUFFER, InternalModels[f].VertexBuffer);
		//	GLExt::glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
		//}
		InternalModels[f].delayVertexBuffer.resize(vertexSize);
		memcpy(InternalModels[f].delayVertexBuffer.data(), vertexData, vertexSize);

		//GLExt::glGenBuffers(1, &InternalModels[f].IndexBuffer);
		size_t indexSize = faceCount * sizeof(::Effekseer::Model::Face);

		//if (InternalModels[f].IndexBuffer > 0)
		//{
		//	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, InternalModels[f].IndexBuffer);
		//	GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, faceData, GL_STATIC_DRAW);
		//}

		//GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
		//GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

		InternalModels[f].delayIndexBuffer.resize(indexSize);
		memcpy(InternalModels[f].delayIndexBuffer.data(), faceData, indexSize);
	}
}

Model::~Model()
{
	ES_SAFE_DELETE_ARRAY(InternalModels);
}

bool Model::LoadToGPU()
{
	if (IsLoadedOnGPU)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		if (!InternalModels[f].TryDelayLoad())
		{
			return false;
		}
	}

	IsLoadedOnGPU = true;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
