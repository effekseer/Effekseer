
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.GLExtension.h"
#include <string>

#include "Shader/FlipbookInterpolationUtils_VS.h"
#include "Shader/FlipbookInterpolationUtils_PS.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
static std::string Replace( std::string target, std::string from_, std::string to_ )
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

static const char g_model_vs_src[] =
R"(
IN vec4 a_Position;
IN vec4 a_Normal;
IN vec4 a_Binormal;
IN vec4 a_Tangent;
IN vec4 a_TexCoord;
IN vec4 a_Color;
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
R"(
IN float a_InstanceID;
IN vec4 a_UVOffset;
IN vec4 a_ModelColor;
)"
#endif

R"(
OUT mediump vec4 v_Normal;
OUT mediump vec4 v_Binormal;
OUT mediump vec4 v_Tangent;
OUT mediump vec4 v_TexCoord;
OUT lowp vec4 v_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
OUT vec2 vaAlphaUV;
OUT float vaFlipbookRate;
OUT vec2 vaFlipbookNextIndexUV;
OUT float vaAlphaThreshold;
)"
#endif

#if defined(MODEL_SOFTWARE_INSTANCING)
R"(
uniform mat4 ModelMatrix[20];
uniform vec4 UVOffset[20];
uniform vec4 ModelColor[20];
)"
#else
R"(
uniform mat4 ModelMatrix;
uniform vec4 UVOffset;
uniform vec4 ModelColor;
)"
#endif

R"(
uniform mat4 ProjectionMatrix;
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;
uniform vec4 mUVInversed;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform vec4 atAlphaUV;
uniform vec4 atFlipbookIndexAndNextRate;
uniform vec4 atAlphaThreshold;
)"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
uniform vec4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
)"
#endif

R"(
void main()
{
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
R"(
	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];
	vec4 uvOffset = a_UVOffset;
	vec4 modelColor = a_ModelColor;
)"
#else
R"(
	mat4 modelMatrix = ModelMatrix;
	vec4 uvOffset = UVOffset;
	vec4 modelColor = ModelColor * a_Color;
)"
#endif
R"(
	vec4 localPosition = modelMatrix * a_Position;
	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;

	if (LightingEnable)
	{
		mat3 lightMatrix = mat3(modelMatrix[0].xyz, modelMatrix[1].xyz, modelMatrix[2].xyz);
		vec3 localNormal = normalize( lightMatrix * a_Normal.xyz );

		v_Normal = vec4(localNormal, 1.0);
		v_Binormal = vec4(normalize( lightMatrix * a_Binormal.xyz ), 1.0);
		v_Tangent = vec4(normalize( lightMatrix * a_Tangent.xyz ), 1.0);
	}

	v_Color = modelColor;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
    // alpha texture
    vaAlphaUV.x = v_TexCoord.x * atAlphaUV.z + atAlphaUV.x;
	vaAlphaUV.y = v_TexCoord.y * atAlphaUV.w + atAlphaUV.y;
	vaAlphaUV.y = mUVInversed.x + mUVInversed.y * vaAlphaUV.y;

	vaFlipbookRate = 0.0;
	vaFlipbookNextIndexUV = vec2(0.0, 0.0);

	ApplyFlipbookVS(vaFlipbookRate, vaFlipbookNextIndexUV, mflipbookParameter, atFlipbookIndexAndNextRate.x, v_TexCoord.xy);
	vaAlphaThreshold = atAlphaThreshold.x;
)"
#endif

R"(
	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
)";

static const char g_model_fs_src[] = 
R"(

IN mediump vec4 v_Normal;
IN mediump vec4 v_Binormal;
IN mediump vec4 v_Tangent;
IN mediump vec4 v_TexCoord;
IN lowp vec4 v_Color;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
IN mediump vec2 vaAlphaUV;
IN mediump float vaFlipbookRate;
IN mediump vec2 vaFlipbookNextIndexUV;
IN mediump float vaAlphaThreshold;
)"
#endif

R"(

uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
uniform sampler2D uAlphaTexture;
uniform float4 flipbookParameter; // x:enable, y:interpolationType
)"
#endif

R"(

void main()
{
	FRAGCOLOR = v_Color * TEX2D(ColorTexture, v_TexCoord.xy);

	if (LightingEnable)
	{
		vec3 texNormal = (TEX2D(NormalTexture, v_TexCoord.xy).xyz - 0.5) * 2.0;
		mat3 normalMatrix = mat3(v_Tangent.xyz, v_Binormal.xyz, v_Normal.xyz );
		vec3 localNormal = normalize( normalMatrix * texNormal );
		float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
		FRAGCOLOR.xyz = FRAGCOLOR.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);
	}

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, ColorTexture, flipbookParameter, v_Color, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

R"(

}

)";


static const char g_model_distortion_vs_src [] =
"IN vec4 a_Position;\n"
"IN vec4 a_Normal;\n"
"IN vec4 a_Binormal;\n"
"IN vec4 a_Tangent;\n"
"IN vec4 a_TexCoord;\n"
"IN vec4 a_Color;\n"
#if defined(MODEL_SOFTWARE_INSTANCING)
"IN float a_InstanceID;\n"
"IN vec4 a_UVOffset;\n"
"IN vec4 a_ModelColor;\n"
#endif

R"(
OUT mediump vec4 v_Normal;
OUT mediump vec4 v_Binormal;
OUT mediump vec4 v_Tangent;
OUT mediump vec4 v_TexCoord;
OUT mediump vec4 v_Pos;
OUT lowp vec4 v_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
OUT vec2 vaAlphaUV;
OUT float vaFlipbookRate;
OUT vec2 vaFlipbookNextIndexUV;
OUT float vaAlphaThreshold;
)"
#endif

#if defined(MODEL_SOFTWARE_INSTANCING)
"uniform mat4 ModelMatrix[20];\n"
"uniform vec4 UVOffset[20];\n"
"uniform vec4 ModelColor[20];\n"
#else
"uniform mat4 ModelMatrix;\n"
"uniform vec4 UVOffset;\n"
"uniform vec4 ModelColor;\n"
#endif
"uniform mat4 ProjectionMatrix;\n"
"uniform vec4 mUVInversed;\n"

#ifdef __EFFEKSEER_BUILD_VERSION16__
												R"(
uniform vec4 atAlphaUV;
uniform vec4 atFlipbookIndexAndNextRate;
uniform vec4 atAlphaThreshold;
)"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
uniform vec4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
)"
#endif

"void main() {\n"
#if defined(MODEL_SOFTWARE_INSTANCING)
"	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];\n"
"	vec4 uvOffset = a_UVOffset;\n"
"	vec4 modelColor = a_ModelColor;\n"
#else
"	mat4 modelMatrix = ModelMatrix;\n"
"	vec4 uvOffset = UVOffset;\n"
"	vec4 modelColor = ModelColor;\n"
#endif

R"(
	vec4 localPosition = vec4( a_Position.x, a_Position.y, a_Position.z, 1.0 );
	vec4 localNormal = vec4( a_Position.x + a_Normal.x, a_Position.y + a_Normal.y, a_Position.z + a_Normal.z, 1.0 );
	vec4 localBinormal = vec4( a_Position.x + a_Binormal.x, a_Position.y + a_Binormal.y, a_Position.z + a_Binormal.z, 1.0 );
	vec4 localTangent = vec4( a_Position.x + a_Tangent.x, a_Position.y + a_Tangent.y, a_Position.z + a_Tangent.z, 1.0 );


	localPosition = modelMatrix * localPosition;
	localNormal = modelMatrix * localNormal;
	localBinormal = modelMatrix * localBinormal;
	localTangent = modelMatrix * localTangent;

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;

	v_Normal = ProjectionMatrix * localNormal;
	v_Binormal = ProjectionMatrix * localBinormal;
	v_Tangent = ProjectionMatrix * localTangent;
	v_Pos = gl_Position;

	v_Color = modelColor * a_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
    vaAlphaUV.x = v_TexCoord.x * atAlphaUV.z + atAlphaUV.x;
	vaAlphaUV.y = v_TexCoord.y * atAlphaUV.w + atAlphaUV.y;
	vaAlphaUV.y = mUVInversed.x + mUVInversed.y * vaAlphaUV.y;

	vaFlipbookRate = 0.0;
	vaFlipbookNextIndexUV = vec2(0.0, 0.0);

	ApplyFlipbookVS(vaFlipbookRate, vaFlipbookNextIndexUV, mflipbookParameter, atFlipbookIndexAndNextRate.x, v_TexCoord.xy);
	vaAlphaThreshold = atAlphaThreshold.x;
)"
#endif

R"(
	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
)";

static const char g_model_distortion_fs_src [] =
"IN mediump vec4 v_Normal;\n"
"IN mediump vec4 v_Binormal;\n"
"IN mediump vec4 v_Tangent;\n"
"IN mediump vec4 v_TexCoord;\n"
"IN mediump vec4 v_Pos;\n"
"IN lowp vec4 v_Color;\n"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
IN mediump vec2 vaAlphaUV;
IN mediump float vaFlipbookRate;
IN mediump vec2 vaFlipbookNextIndexUV;
IN mediump float vaAlphaThreshold;
)"
#endif

R"(
uniform sampler2D uTexture0;
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;
uniform	vec4	mUVInversedBack;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
R"(
uniform sampler2D uAlphaTexture;
uniform float4 flipbookParameter; // x:enable, y:interpolationType
)"
#endif

R"(

void main() {
	if (TextureEnable)
	{
		FRAGCOLOR = TEX2D(uTexture0, v_TexCoord.xy);
	}
	else
	{
		FRAGCOLOR = vec4(1.0, 1.0, 1.0, 1.0);
	}

	FRAGCOLOR.a = FRAGCOLOR.a * v_Color.a;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
												R"(
	ApplyFlipbook(FRAGCOLOR, uTexture0, flipbookParameter, v_Color, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

												R"(

	vec2 pos = v_Pos.xy / v_Pos.w;
	vec2 posU = v_Tangent.xy / v_Tangent.w;
	vec2 posR = v_Binormal.xy / v_Binormal.w;

	float xscale = (FRAGCOLOR.x * 2.0 - 1.0) * v_Color.x * g_scale.x;
	float yscale = (FRAGCOLOR.y * 2.0 - 1.0) * v_Color.y * g_scale.x;

	vec2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
//	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	vec3 color = TEX2D(uBackTexture0, uv).xyz;
	FRAGCOLOR.xyz = color;
}
)";

#ifdef __EFFEKSEER_BUILD_VERSION16__
static const int NumAttribs_Model = 6;

static ShaderAttribInfo g_model_attribs[NumAttribs_Model] = {
	{"a_Position", GL_FLOAT, 3, 0, false},
	{"a_Normal", GL_FLOAT, 3, 12, false},
	{"a_Binormal", GL_FLOAT, 3, 24, false},
	{"a_Tangent", GL_FLOAT, 3, 36, false},
	{"a_TexCoord", GL_FLOAT, 2, 48, false},
	{"a_Color", GL_UNSIGNED_BYTE, 4, 56, true},
};

#else
static const int NumAttribs_Model = 6;

static ShaderAttribInfo g_model_attribs[NumAttribs_Model] = {
	{"a_Position", GL_FLOAT, 3, 0, false},
	{"a_Normal", GL_FLOAT, 3, 12, false},
	{"a_Binormal", GL_FLOAT, 3, 24, false},
	{"a_Tangent", GL_FLOAT, 3, 36, false},
	{"a_TexCoord", GL_FLOAT, 2, 48, false},
	{"a_Color", GL_UNSIGNED_BYTE, 4, 56, true},
#if defined(MODEL_SOFTWARE_INSTANCING)
	{"a_InstanceID", GL_FLOAT, 1, 0, false},
	{"a_UVOffset", GL_FLOAT, 4, 0, false},
	{"a_ModelColor", GL_FLOAT, 4, 0, false},
#endif
};

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::ModelRenderer(
	RendererImplemented* renderer,
	Shader* shader_lighting_texture_normal,
	Shader* shader_texture,
	Shader* shader_distortion_texture)
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_texture(shader_texture)
	, m_shader_distortion_texture(shader_distortion_texture)
{
	for (size_t i = 0; i < 8; i++)
	{
		m_va[i] = nullptr;
	}

	shader_lighting_texture_normal->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_lighting_texture_normal->SetTextureSlot(0, shader_lighting_texture_normal->GetUniformId("ColorTexture"));
	shader_lighting_texture_normal->SetTextureSlot(1, shader_lighting_texture_normal->GetUniformId("NormalTexture"));

	shader_texture->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_texture->SetTextureSlot(0, shader_texture->GetUniformId("ColorTexture"));
	
	shader_distortion_texture->GetAttribIdList(NumAttribs_Model, g_model_attribs);
	shader_distortion_texture->SetTextureSlot(0, shader_distortion_texture->GetUniformId("uTexture0"));
	shader_distortion_texture->SetTextureSlot(1, shader_distortion_texture->GetUniformId("uBackTexture0"));

	Shader* shaders[2];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_texture;
	
	for( int32_t i = 0; i < 2; i++ )
	{
		shaders[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));

		int vsOffset = 0;
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("ProjectionMatrix"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders[i]->GetUniformId("ModelMatrix"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("UVOffset"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("atAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("mflipbookParameter"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("atFlipbookIndexAndNextRate"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("atAlphaThreshold"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;
#endif
		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("ModelColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("LightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("LightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("LightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders[i]->SetPixelConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererPixelConstantBuffer));
		shaders[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightDirection"),
			sizeof(float[4]) * 0
			);
		shaders[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightColor"),
			sizeof(float[4]) * 1
			);
		shaders[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightAmbient"),
			sizeof(float[4]) * 2
			);

	#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders[i]->SetTextureSlot(2, shaders[i]->GetUniformId("uAlphaTexture"));
		shaders[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders[i]->GetUniformId("flipbookParameter"), sizeof(float) * 4 * 3);
	#endif
	}

	Shader* shaders_d[1];
	shaders_d[0] = shader_distortion_texture;

	for (int32_t i = 0; i < 1; i++)
	{
		shaders_d[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders_d[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
		
		int vsOffset = 0;
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("ProjectionMatrix"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_MATRIX44, shaders_d[i]->GetUniformId("ModelMatrix"), vsOffset);

		vsOffset += sizeof(Effekseer::Matrix44);

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("UVOffset"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("atAlphaUV"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("mflipbookParameter"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("atFlipbookIndexAndNextRate"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("atAlphaThreshold"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;
#endif
		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("ModelColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("LightDirection"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("LightColor"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("LightAmbient"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

		shaders_d[i]->AddVertexConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("mUVInversed"), vsOffset);

		vsOffset += sizeof(float[4]) * 1;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->SetPixelConstantBufferSize(sizeof(float) * 4 * 3);
#else
		shaders_d[i]->SetPixelConstantBufferSize(sizeof(float) * 4 * 2);
#endif
		shaders_d[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("g_scale"),
			sizeof(float[4]) * 0
			);


		shaders_d[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("mUVInversedBack"),
			sizeof(float[4]) * 1
			);

#ifdef __EFFEKSEER_BUILD_VERSION16__
		shaders_d[i]->SetTextureSlot(2, shaders_d[i]->GetUniformId("uAlphaTexture"));
		shaders_d[i]->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, shaders_d[i]->GetUniformId("flipbookParameter"), sizeof(float[4]) * 2);
#endif
	}

	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	}

	m_va[0] = VertexArray::Create(renderer, m_shader_lighting_texture_normal, nullptr, nullptr, true);
	m_va[4] = VertexArray::Create(renderer, m_shader_texture, nullptr, nullptr, true);
	m_va[6] = VertexArray::Create(renderer, m_shader_distortion_texture, nullptr, nullptr, true);

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(currentVAO);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
	for (size_t i = 0; i < 8; i++)
	{
		ES_SAFE_DELETE(m_va[i]);
	}

	ES_SAFE_DELETE(m_shader_lighting_texture_normal);

	ES_SAFE_DELETE(m_shader_texture);

	ES_SAFE_DELETE(m_shader_distortion_texture);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ModelRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );

	Shader* shader_lighting_texture_normal = NULL;		    
	Shader* shader_texture = NULL;
	Shader* shader_distortion_texture = NULL;

	std::string vs_ltn_src = g_model_vs_src;
	std::string fs_ltn_src = g_model_fs_src;

	std::string vs_t_src = g_model_vs_src;
	std::string fs_t_src = g_model_fs_src;

	std::string vs_d_t_src = g_model_distortion_vs_src;
	std::string fs_d_t_src = g_model_distortion_fs_src;

	vs_ltn_src = Replace(vs_ltn_src, "TextureEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "TextureEnable", "true");
	vs_ltn_src = Replace(vs_ltn_src, "LightingEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "LightingEnable", "true");
	vs_ltn_src = Replace(vs_ltn_src, "NormalMapEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "NormalMapEnable", "true");

	vs_t_src = Replace(vs_t_src, "TextureEnable", "true");
	fs_t_src = Replace(fs_t_src, "TextureEnable", "true");
	vs_t_src = Replace(vs_t_src, "LightingEnable", "false");
	fs_t_src = Replace(fs_t_src, "LightingEnable", "false");
	vs_t_src = Replace(vs_t_src, "NormalMapEnable", "false");
	fs_t_src = Replace(fs_t_src, "NormalMapEnable", "false");

	vs_d_t_src = Replace(vs_d_t_src, "TextureEnable", "true");
	fs_d_t_src = Replace(fs_d_t_src, "TextureEnable", "true");

#ifdef __EFFEKSEER_BUILD_VERSION16__
	ShaderCodeView ltnVS[2]{
		ShaderCodeView(g_flipbook_interpolation_vs_src),
		ShaderCodeView(vs_ltn_src.c_str()),
	};

	ShaderCodeView ltnPS[2]{ShaderCodeView(g_flipbook_interpolation_ps_src), ShaderCodeView(fs_ltn_src.c_str())};

	shader_lighting_texture_normal = Shader::Create(renderer->GetGraphicsDevice(), ltnVS, 2, ltnPS, 2, "ModelRenderer1", true);
	if (shader_lighting_texture_normal == NULL)
		goto End;

	ShaderCodeView tVS[2]{ShaderCodeView(g_flipbook_interpolation_vs_src), ShaderCodeView(vs_t_src.c_str())};
	ShaderCodeView tPS[2]{ShaderCodeView(g_flipbook_interpolation_ps_src), ShaderCodeView(fs_t_src.c_str())};

	shader_texture = Shader::Create(renderer->GetGraphicsDevice(), tVS, 2, tPS, 2, "ModelRenderer5", true);
	if (shader_texture == NULL)
		goto End;

	ShaderCodeView dVS[]{ShaderCodeView(g_flipbook_interpolation_vs_src), ShaderCodeView(vs_d_t_src.c_str())};
	ShaderCodeView dPS[]{ShaderCodeView(g_flipbook_interpolation_ps_src), ShaderCodeView(fs_d_t_src.c_str())};

	shader_distortion_texture = Shader::Create(renderer->GetGraphicsDevice(), dVS, 2, dPS, 2, "ModelRenderer7", true);
	if (shader_distortion_texture == NULL)
		goto End;
#else
	ShaderCodeView ltnVS(vs_ltn_src.c_str());
	ShaderCodeView ltnPS(fs_ltn_src.c_str());

	shader_lighting_texture_normal = Shader::Create(renderer->GetGraphicsDevice(), &ltnVS, 1, &ltnPS, 1, "ModelRenderer1", true);
	if (shader_lighting_texture_normal == NULL)
		goto End;

	ShaderCodeView tVS(vs_t_src.c_str());
	ShaderCodeView tPS(fs_t_src.c_str());

	shader_texture = Shader::Create(renderer->GetGraphicsDevice(), &tVS, 1, &tPS, 1, "ModelRenderer5", true);
	if (shader_texture == NULL)
		goto End;

	ShaderCodeView dVS(vs_d_t_src.c_str());
	ShaderCodeView dPS(fs_d_t_src.c_str());

	shader_distortion_texture = Shader::Create(renderer->GetGraphicsDevice(), &dVS, 1, &dPS, 1, "ModelRenderer7", true);
	if (shader_distortion_texture == NULL)
		goto End;
#endif
	return new ModelRenderer( 
		renderer, 
		shader_lighting_texture_normal,
		shader_texture,
		shader_distortion_texture);
End:;

	ES_SAFE_DELETE(shader_lighting_texture_normal);
	ES_SAFE_DELETE(shader_texture);
	ES_SAFE_DELETE(shader_distortion_texture);
	return nullptr;
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<
		RendererImplemented>(
		m_renderer,
		parameter,
		instanceParameter,
		userData);
}

void ModelRenderer::EndRendering( const efkModelNodeParam& parameter, void* userData )
{
	if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
	{
		m_renderer->SetVertexArray(m_va[6]);
	}
	else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
	{
		m_renderer->SetVertexArray(m_va[0]);
	}
	else
	{
		m_renderer->SetVertexArray(m_va[4]);
	}

    if(parameter.ModelIndex < 0)
    {
        return;
    }
    
	auto model = (Model*) parameter.EffectPointer->GetModel(parameter.ModelIndex);
	if(model == nullptr)
	{
        return;
    }
    
    for(auto i = 0; i < model->GetFrameCount(); i++)
    {
        model->InternalModels[i].TryDelayLoad();
    }
	
    m_shader_lighting_texture_normal->SetVertexSize(model->GetVertexSize());
    m_shader_texture->SetVertexSize(model->GetVertexSize());
    m_shader_distortion_texture->SetVertexSize(model->GetVertexSize());
	
#if defined(MODEL_SOFTWARE_INSTANCING)
	EndRendering_<
		RendererImplemented,
		Shader,
		GLuint,
		Model,
		true,
		20>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_texture,
		m_shader_distortion_texture,
		parameter );
#else
	EndRendering_<
		RendererImplemented,
		Shader,
		Model,
		false,
		1>(
		m_renderer,
		m_shader_lighting_texture_normal,
		m_shader_texture,
		m_shader_distortion_texture,
		parameter );
#endif

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
