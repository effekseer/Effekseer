
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

#include <string>

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
	"OUT mediump vec4 v_Normal;\n"
	"OUT mediump vec4 v_Binormal;\n"
	"OUT mediump vec4 v_Tangent;\n"
	"OUT mediump vec4 v_TexCoord;\n"
	"OUT lowp vec4 v_Color;\n"
	
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
	"uniform vec4 LightDirection;\n"
	"uniform vec4 LightColor;\n"
	"uniform vec4 LightAmbient;\n"

	"void main() {\n"
#if defined(MODEL_SOFTWARE_INSTANCING)
	"	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];\n"
	"	vec4 uvOffset = a_UVOffset;\n"
	"	vec4 modelColor = a_ModelColor;\n"
#else
	"	mat4 modelMatrix = ModelMatrix;\n"
	"	vec4 uvOffset = UVOffset;\n"
	"	vec4 modelColor = ModelColor * a_Color;\n"
#endif
	"	vec4 localPosition = modelMatrix * a_Position;\n"
	"	gl_Position = ProjectionMatrix * localPosition;\n"
	
	"	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;\n"

	"	if (LightingEnable) {\n"
	"		mat3 lightMatrix = mat3(modelMatrix[0].xyz, modelMatrix[1].xyz, modelMatrix[2].xyz);"
	"		vec3 localNormal = normalize( lightMatrix * a_Normal.xyz );\n"
	"		float diffuse = 1.0;\n"
	"		if (NormalMapEnable) {\n"
	"			v_Normal = vec4(localNormal, 1.0);\n"
	"			v_Binormal = vec4(normalize( lightMatrix * a_Binormal.xyz ), 1.0);\n"
	"			v_Tangent = vec4(normalize( lightMatrix * a_Tangent.xyz ), 1.0);\n"
	"		} else {\n"
	"			diffuse = max(0.0, dot(localNormal, LightDirection.xyz));\n"
	"		}\n"
	"		v_Color = modelColor * vec4(diffuse * LightColor.rgb, 1.0);\n"
	"	} else {\n"
	"		v_Color = modelColor;\n"
	"	}\n"
	"}\n";

static const char g_model_fs_src[] = 
	"IN mediump vec4 v_Normal;\n"
	"IN mediump vec4 v_Binormal;\n"
	"IN mediump vec4 v_Tangent;\n"
	"IN mediump vec4 v_TexCoord;\n"
	"IN lowp vec4 v_Color;\n"

	"uniform sampler2D ColorTexture;\n"
	"uniform sampler2D NormalTexture;\n"
	"uniform vec4 LightDirection;\n"
	"uniform vec4 LightColor;\n"
	"uniform vec4 LightAmbient;\n"

	"void main() {\n"
	"	vec4 diffuse = vec4(1.0);\n"
	"	if (LightingEnable && NormalMapEnable) {\n"
	"		vec3 texNormal = (TEX2D(NormalTexture, v_TexCoord.xy).xyz - 0.5) * 2.0;\n"
	"		mat3 normalMatrix = mat3(v_Tangent.xyz, v_Binormal.xyz, v_Normal.xyz );\n"
	"		vec3 localNormal = normalize( normalMatrix * texNormal );\n;"
	"		//FRAGCOLOR.xyz = localNormal.xyz; FRAGCOLOR.w = 1.0; return;\n"
	"		diffuse = vec4(max(0.0, dot(localNormal, LightDirection.xyz)));\n"
	"	}\n"
	"	if (TextureEnable) {\n"
	"		FRAGCOLOR = v_Color * TEX2D(ColorTexture, v_TexCoord.xy);\n"
	"		FRAGCOLOR.xyz = FRAGCOLOR.xyz * diffuse.xyz;\n"
	"	} else {\n"
	"		FRAGCOLOR = v_Color;\n"
	"		FRAGCOLOR.xyz = FRAGCOLOR.xyz * diffuse.xyz;\n"
	"	}\n"
	"   \n"
	"	if (LightingEnable) {\n"
	"		FRAGCOLOR.xyz = FRAGCOLOR.xyz + LightAmbient.xyz;\n"
	"	}\n"
	"}\n";


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
}
)";

static const char g_model_distortion_fs_src [] =
"IN mediump vec4 v_Normal;\n"
"IN mediump vec4 v_Binormal;\n"
"IN mediump vec4 v_Tangent;\n"
"IN mediump vec4 v_TexCoord;\n"
"IN mediump vec4 v_Pos;\n"
"IN lowp vec4 v_Color;\n"

R"(
uniform sampler2D uTexture0;
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;


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

	vec2 pos = v_Pos.xy / v_Pos.w;
	vec2 posU = v_Tangent.xy / v_Tangent.w;
	vec2 posR = v_Binormal.xy / v_Binormal.w;

	float xscale = (FRAGCOLOR.x * 2.0 - 1.0) * v_Color.x * g_scale.x;
	float yscale = (FRAGCOLOR.y * 2.0 - 1.0) * v_Color.y * g_scale.x;

	vec2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
//	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	vec3 color = TEX2D(uBackTexture0, uv).xyz;
	FRAGCOLOR.xyz = color;
}
)";

static ShaderAttribInfo g_model_attribs[ModelRenderer::NumAttribs] = {
	{"a_Position",		GL_FLOAT,			3,  0,	false},
	{"a_Normal",		GL_FLOAT,			3, 12,	false},
	{"a_Binormal",		GL_FLOAT,			3, 24,	false},
	{"a_Tangent",		GL_FLOAT,			3, 36,	false},
	{"a_TexCoord",		GL_FLOAT,			2, 48,	false},
	{"a_Color", GL_UNSIGNED_BYTE,			4, 56,	true },
#if defined(MODEL_SOFTWARE_INSTANCING)
	{"a_InstanceID",	GL_FLOAT,			1,  0,	false},
	{"a_UVOffset",		GL_FLOAT,			4,	0,	false},
	{"a_ModelColor",	GL_FLOAT,			4,  0,	false},
#endif
};

static ShaderUniformInfo g_model_uniforms[ModelRenderer::NumUniforms] = {
	{"ProjectionMatrix"	},
	{"ModelMatrix"		},
#if !defined(MODEL_SOFTWARE_INSTANCING)
	{"UVOffset"			},
	{"ModelColor"		},
#endif
	{"ColorTexture"		},
	{"NormalTexture"	},
	{"LightDirection"	},
	{"LightColor"		},
	{"LightAmbient"		},
	{ "g_scale" },
};

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
	: m_renderer(renderer)
	, m_shader_lighting_texture_normal(shader_lighting_texture_normal)
	, m_shader_lighting_normal(shader_lighting_normal)
	, m_shader_lighting_texture(shader_lighting_texture)
	, m_shader_lighting(shader_lighting)
	, m_shader_texture(shader_texture)
	, m_shader(shader)
	, m_shader_distortion_texture(shader_distortion_texture)
	, m_shader_distortion(shader_distortion)
{
	for (size_t i = 0; i < 8; i++)
	{
		m_va[i] = nullptr;
	}

	shader_lighting_texture_normal->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_lighting_texture_normal->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[0]);
	shader_lighting_texture_normal->SetTextureSlot(0, shader_lighting_texture_normal->GetUniformId("ColorTexture"));
	shader_lighting_texture_normal->SetTextureSlot(1, shader_lighting_texture_normal->GetUniformId("NormalTexture"));


	shader_lighting_normal->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_lighting_normal->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[1]);
	shader_lighting_normal->SetTextureSlot(1, shader_lighting_normal->GetUniformId("NormalTexture"));


	shader_lighting_texture->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_lighting_texture->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[2]);
	shader_lighting_texture->SetTextureSlot(0, shader_lighting_texture->GetUniformId("ColorTexture"));

	shader_lighting->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_lighting->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[3]);

	shader_texture->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_texture->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[4]);
	shader_texture->SetTextureSlot(0, shader_texture->GetUniformId("ColorTexture"));
	
	shader->GetAttribIdList(NumAttribs, g_model_attribs);
	shader->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[5]);

	shader_distortion_texture->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_distortion_texture->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[6]);
	shader_distortion_texture->SetTextureSlot(0, shader_distortion_texture->GetUniformId("uTexture0"));
	shader_distortion_texture->SetTextureSlot(1, shader_distortion_texture->GetUniformId("uBackTexture0"));

	shader_distortion->GetAttribIdList(NumAttribs, g_model_attribs);
	shader_distortion->GetUniformIdList(NumUniforms, g_model_uniforms, m_uniformLoc[7]);
	shader_distortion->SetTextureSlot(0, shader_distortion->GetUniformId("uTexture0"));
	shader_distortion->SetTextureSlot(1, shader_distortion->GetUniformId("uBackTexture0"));

	Shader* shaders[6];
	shaders[0] = m_shader_lighting_texture_normal;
	shaders[1] = m_shader_lighting_normal;
	shaders[2] = m_shader_lighting_texture;
	shaders[3] = m_shader_lighting;
	shaders[4] = m_shader_texture;
	shaders[5] = m_shader;
	
	for( int32_t i = 0; i < 6; i++ )
	{
		shaders[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44,
			shaders[i]->GetUniformId("ProjectionMatrix"),
			0
			);

		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44,
			shaders[i]->GetUniformId("ModelMatrix"),
			sizeof(Effekseer::Matrix44)
			);

		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("UVOffset"),
			sizeof(Effekseer::Matrix44) * 2
			);

		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("ModelColor"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 1
			);

		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightDirection"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 0
			);
		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightColor"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 1
			);
		shaders[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders[i]->GetUniformId("LightAmbient"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 2
			);

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
	}

	Shader* shaders_d[2];
	shaders_d[0] = shader_distortion_texture;
	shaders_d[1] = shader_distortion;

	for (int32_t i = 0; i < 2; i++)
	{
		shaders_d[i]->SetVertexSize(sizeof(::Effekseer::Model::Vertex));

		shaders_d[i]->SetVertexConstantBufferSize(sizeof(::EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44,
			shaders_d[i]->GetUniformId("ProjectionMatrix"),
			0
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_MATRIX44,
			shaders_d[i]->GetUniformId("ModelMatrix"),
			sizeof(Effekseer::Matrix44)
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("UVOffset"),
			sizeof(Effekseer::Matrix44) * 2
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("ModelColor"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 1
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("LightDirection"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 0
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("LightColor"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 1
			);

		shaders_d[i]->AddVertexConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("LightAmbient"),
			sizeof(Effekseer::Matrix44) * 2 + sizeof(float[4]) * 2 + sizeof(float[4]) * 2
			);

		shaders_d[i]->SetPixelConstantBufferSize(sizeof(float) * 4);
		shaders_d[i]->AddPixelConstantLayout(
			CONSTANT_TYPE_VECTOR4,
			shaders_d[i]->GetUniformId("g_scale"),
			sizeof(float[4]) * 0
			);
	}

	m_va[0] = VertexArray::Create(renderer, m_shader_lighting_texture_normal, nullptr, nullptr);
	m_va[1] = VertexArray::Create(renderer, m_shader_lighting_normal, nullptr, nullptr);

	m_va[2] = VertexArray::Create(renderer, m_shader_lighting_texture, nullptr, nullptr);
	m_va[3] = VertexArray::Create(renderer, m_shader_lighting, nullptr, nullptr);

	m_va[4] = VertexArray::Create(renderer, m_shader_texture, nullptr, nullptr);
	m_va[5] = VertexArray::Create(renderer, m_shader, nullptr, nullptr);

	m_va[6] = VertexArray::Create(renderer, m_shader_distortion_texture, nullptr, nullptr);
	m_va[7] = VertexArray::Create(renderer, m_shader_distortion, nullptr, nullptr);

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
	ES_SAFE_DELETE(m_shader_lighting_normal);

	ES_SAFE_DELETE(m_shader_lighting_texture);
	ES_SAFE_DELETE(m_shader_lighting);

	ES_SAFE_DELETE(m_shader_texture);
	ES_SAFE_DELETE(m_shader);

	ES_SAFE_DELETE(m_shader_distortion_texture);
	ES_SAFE_DELETE(m_shader_distortion);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer* ModelRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );

	Shader* shader_lighting_texture_normal = NULL;
	Shader* shader_lighting_normal = NULL;
		    
	Shader* shader_lighting_texture = NULL;
	Shader* shader_lighting = NULL;
		    
	Shader* shader_texture = NULL;
	Shader* shader = NULL;

	Shader* shader_distortion_texture = NULL;
	Shader* shader_distortion = NULL;

	std::string vs_ltn_src = g_model_vs_src;
	std::string fs_ltn_src = g_model_fs_src;

	std::string vs_ln_src = g_model_vs_src;
	std::string fs_ln_src = g_model_fs_src;

	std::string vs_lt_src = g_model_vs_src;
	std::string fs_lt_src = g_model_fs_src;

	std::string vs_l_src = g_model_vs_src;
	std::string fs_l_src = g_model_fs_src;

	std::string vs_t_src = g_model_vs_src;
	std::string fs_t_src = g_model_fs_src;

	std::string vs_src = g_model_vs_src;
	std::string fs_src = g_model_fs_src;

	std::string vs_d_t_src = g_model_distortion_vs_src;
	std::string fs_d_t_src = g_model_distortion_fs_src;

	std::string vs_d_src = g_model_distortion_vs_src;
	std::string fs_d_src = g_model_distortion_fs_src;

	vs_ltn_src = Replace(vs_ltn_src, "TextureEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "TextureEnable", "true");
	vs_ltn_src = Replace(vs_ltn_src, "LightingEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "LightingEnable", "true");
	vs_ltn_src = Replace(vs_ltn_src, "NormalMapEnable", "true");
	fs_ltn_src = Replace(fs_ltn_src, "NormalMapEnable", "true");

	vs_ln_src = Replace(vs_ln_src, "TextureEnable", "false");
	fs_ln_src = Replace(fs_ln_src, "TextureEnable", "false");
	vs_ln_src = Replace(vs_ln_src, "LightingEnable", "true");
	fs_ln_src = Replace(fs_ln_src, "LightingEnable", "true");
	vs_ln_src = Replace(vs_ln_src, "NormalMapEnable", "true");
	fs_ln_src = Replace(fs_ln_src, "NormalMapEnable", "true");

	vs_lt_src = Replace(vs_lt_src, "TextureEnable", "true");
	fs_lt_src = Replace(fs_lt_src, "TextureEnable", "true");
	vs_lt_src = Replace(vs_lt_src, "LightingEnable", "true");
	fs_lt_src = Replace(fs_lt_src, "LightingEnable", "true");
	vs_lt_src = Replace(vs_lt_src, "NormalMapEnable", "false");
	fs_lt_src = Replace(fs_lt_src, "NormalMapEnable", "false");

	vs_l_src = Replace(vs_l_src, "TextureEnable", "false");
	fs_l_src = Replace(fs_l_src, "TextureEnable", "false");
	vs_l_src = Replace(vs_l_src, "LightingEnable", "true");
	fs_l_src = Replace(fs_l_src, "LightingEnable", "true");
	vs_l_src = Replace(vs_l_src, "NormalMapEnable", "false");
	fs_l_src = Replace(fs_l_src, "NormalMapEnable", "false");

	vs_t_src = Replace(vs_t_src, "TextureEnable", "true");
	fs_t_src = Replace(fs_t_src, "TextureEnable", "true");
	vs_t_src = Replace(vs_t_src, "LightingEnable", "false");
	fs_t_src = Replace(fs_t_src, "LightingEnable", "false");
	vs_t_src = Replace(vs_t_src, "NormalMapEnable", "false");
	fs_t_src = Replace(fs_t_src, "NormalMapEnable", "false");

	vs_src = Replace(vs_src, "TextureEnable", "false");
	fs_src = Replace(fs_src, "TextureEnable", "false");
	vs_src = Replace(vs_src, "LightingEnable", "false");
	fs_src = Replace(fs_src, "LightingEnable", "false");
	vs_src = Replace(vs_src, "NormalMapEnable", "false");
	fs_src = Replace(fs_src, "NormalMapEnable", "false");

	vs_d_t_src = Replace(vs_d_t_src, "TextureEnable", "true");
	fs_d_t_src = Replace(fs_d_t_src, "TextureEnable", "true");

	vs_d_src = Replace(vs_d_src, "TextureEnable", "false");
	fs_d_src = Replace(fs_d_src, "TextureEnable", "false");

	shader_lighting_texture_normal = Shader::Create(renderer,
		vs_ltn_src.c_str(), vs_ltn_src.length(), fs_ltn_src.c_str(), fs_ltn_src.length(), "ModelRenderer");
	if (shader_lighting_texture_normal == NULL) goto End;

	shader_lighting_normal = Shader::Create(renderer,
		vs_ln_src.c_str(), vs_ln_src.length(), fs_ln_src.c_str(), fs_ln_src.length(), "ModelRenderer");
	if (shader_lighting_normal == NULL) goto End;

	shader_lighting_texture = Shader::Create(renderer,
		vs_lt_src.c_str(), vs_lt_src.length(), fs_lt_src.c_str(), fs_lt_src.length(), "ModelRenderer");
	if (shader_lighting_texture == NULL) goto End;

	shader_lighting = Shader::Create(renderer,
		vs_l_src.c_str(), vs_l_src.length(), fs_l_src.c_str(), fs_l_src.length(), "ModelRenderer");
	if (shader_lighting == NULL) goto End;

	shader_texture = Shader::Create(renderer,
		vs_t_src.c_str(), vs_t_src.length(), fs_t_src.c_str(), fs_t_src.length(), "ModelRenderer");
	if (shader_texture == NULL) goto End;

	shader = Shader::Create( renderer, 
		vs_src.c_str(), vs_src.length(), fs_src.c_str(), fs_src.length(), "ModelRenderer");
	if (shader == NULL) goto End;

	shader_distortion_texture = Shader::Create(renderer,
		vs_d_t_src.c_str(), vs_d_t_src.length(), fs_d_t_src.c_str(), fs_d_t_src.length(), "ModelRenderer");
	if (shader_distortion_texture == NULL) goto End;

	shader_distortion = Shader::Create(renderer,
		vs_d_src.c_str(), vs_d_src.length(), fs_d_src.c_str(), fs_d_src.length(), "ModelRenderer");
	if (shader_distortion == NULL) goto End;

	return new ModelRenderer( 
		renderer, 
		shader_lighting_texture_normal,
		shader_lighting_normal,
		shader_lighting_texture,
		shader_lighting,
		shader_texture,
		shader,
		shader_distortion_texture,
		shader_distortion);
End:;

	ES_SAFE_DELETE(shader_lighting_texture_normal);
	ES_SAFE_DELETE(shader_lighting_normal);

	ES_SAFE_DELETE(shader_lighting_texture);
	ES_SAFE_DELETE(shader_lighting);

	ES_SAFE_DELETE(shader_texture);
	ES_SAFE_DELETE(shader);

	ES_SAFE_DELETE(shader_distortion_texture);
	ES_SAFE_DELETE(shader_distortion);
	return NULL;

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
	if (parameter.Distortion)
	{
		if (parameter.ColorTextureIndex >= 0)
		{
			m_renderer->SetVertexArray(m_va[6]);
		}
		else
		{
			m_renderer->SetVertexArray(m_va[7]);
		}
	}
	else if (parameter.Lighting)
	{
		if (parameter.NormalTextureIndex >= 0)
		{
			if (parameter.ColorTextureIndex >= 0)
			{
				m_renderer->SetVertexArray(m_va[0]);
			}
			else
			{
				m_renderer->SetVertexArray(m_va[1]);
			}
			}
		else
		{
			if (parameter.ColorTextureIndex >= 0)
			{
				m_renderer->SetVertexArray(m_va[2]);
			}
			else
			{
				m_renderer->SetVertexArray(m_va[3]);
			}
		}
	}
	else
	{
		if (parameter.ColorTextureIndex >= 0)
		{
			m_renderer->SetVertexArray(m_va[4]);
		}
		else
		{
			m_renderer->SetVertexArray(m_va[5]);
		}
	}

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
		m_shader_lighting_normal,
		m_shader_lighting_texture,
		m_shader_lighting,
		m_shader_texture,
		m_shader,
		m_shader_distortion_texture,
		m_shader_distortion,
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
		m_shader_lighting_normal,
		m_shader_lighting_texture,
		m_shader_lighting,
		m_shader_texture,
		m_shader,
		m_shader_distortion_texture,
		m_shader_distortion,
		parameter );
#endif

	
	/*
	return;
	
	if( m_matrixes.size() == 0 ) return;
	if( parameter.ModelIndex < 0 ) return;
	
	Model* model = (Model*)parameter.EffectPointer->GetModel( parameter.ModelIndex );
	if( model == NULL ) return;

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
	

	GLuint textures[2];
	textures[0] = 0;
	textures[1] = 0;

	if( parameter.ColorTextureIndex >= 0 )
	{
		// テクスチャ有り
		textures[0] = (GLuint) parameter.EffectPointer->GetImage(parameter.ColorTextureIndex);
	}
	
	if( parameter.NormalTextureIndex >= 0 )
	{
		textures[1] = (GLuint) parameter.EffectPointer->GetImage(parameter.NormalTextureIndex);
	}

	m_renderer->SetTextures(shader_, textures, 2);

	state.TextureFilterTypes[0] = parameter.TextureFilter;
	state.TextureWrapTypes[0] = parameter.TextureWrap;
	state.TextureFilterTypes[1] = parameter.TextureFilter;
	state.TextureWrapTypes[1] = parameter.TextureWrap;

	m_renderer->GetRenderState()->Update( false );
	
	// ここから
	ModelRendererVertexConstantBuffer<1>* vcb = (ModelRendererVertexConstantBuffer<1>*)shader_->GetVertexConstantBuffer();
	ModelRendererPixelConstantBuffer* pcb = (ModelRendererPixelConstantBuffer*)shader_->GetPixelConstantBuffer();
	
	if( parameter.Lighting )
	{
		{
			::Effekseer::Vector3D lightDirection = m_renderer->GetLightDirection();
			::Effekseer::Vector3D::Normal( lightDirection, lightDirection );
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
	else
	{
	}

	vcb->CameraMatrix = m_renderer->GetCameraProjectionMatrix();
	
	m_renderer->SetVertexBuffer(model->VertexBuffer, sizeof(Effekseer::Model::VertexWithIndex));
	m_renderer->SetIndexBuffer(model->IndexBuffer);

	m_renderer->SetLayout(shader_);
	
#if defined(MODEL_SOFTWARE_INSTANCING)
	for( size_t loop = 0; loop < m_matrixes.size(); )
	{
		int32_t modelCount = Effekseer::Min( m_matrixes.size() - loop, MaxInstanced );
		
		glUniformMatrix4fv( m_uniformLoc[UniformModelMatrix],
			modelCount, GL_FALSE, &m_matrixes[loop].Values[0][0] );
		
		for( int32_t num = 0; num < modelCount; num++ )
		{
			glVertexAttrib1f( m_attribLoc[AttribInstanceID], (float)num );
			
			float lc[4];
			ColorToFloat4( m_colors[loop + num], lc );
			glVertexAttrib4fv( m_attribLoc[AttribModelColor], lc );

			glDrawElements( GL_TRIANGLES, model->IndexCount, GL_UNSIGNED_INT, NULL );
		}

		loop += modelCount;
	}
#else
	for( size_t loop = 0; loop < m_matrixes.size(); )
	{
		vcb->ModelMatrix[0] = m_matrixes[loop];
		vcb->ModelUV[0][0] = m_uv[loop].X;
		vcb->ModelUV[0][1] = m_uv[loop].Y;
		vcb->ModelUV[0][2] = m_uv[loop].Width;
		vcb->ModelUV[0][3] = m_uv[loop].Height;
		
		ColorToFloat4( m_colors[loop], vcb->ModelColor[0] );
		shader_->SetConstantBuffer();
		
		//glDrawElements( GL_TRIANGLES, model->IndexCount, GL_UNSIGNED_INT, NULL );
		m_renderer->DrawPolygon( model->VertexCount, model->IndexCount );

		loop += 1;
	}
#endif

	m_renderer->EndShader(shader_);

	m_renderer->GetRenderState()->Pop();

	//*/
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
