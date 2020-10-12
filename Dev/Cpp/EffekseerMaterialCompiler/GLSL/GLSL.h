
#pragma once

#include <iostream>
#undef min

namespace Effekseer
{
namespace GLSL
{

static const char* material_common_define_450 = R"(
#version 450
#define LAYOUT(ind) layout(location = ind)
#define lowp
#define mediump
#define highp
#define IN in
#define OUT out
#define CENTROID centroid

)";

static const char* material_common_define_not_450 = R"(
#define LAYOUT(ind) 
)";

static const char* material_common_define =
	R"(
#define MOD mod
#define FRAC fract
#define LERP mix

float atan2(in float y, in float x) {
    return x == 0.0 ? sign(y)* 3.141592 / 2.0 : atan(y, x);
}

)";

static const char* material_common_vs_define = R"()"

											   R"(

#define TEX2D textureLod

)";

static const char* material_common_fs_define = R"()"

											   R"(

#define TEX2D texture

)";

static const char g_material_model_vs_src_pre[] =
	R"(

LAYOUT(0) IN vec4 a_Position;
LAYOUT(1) IN vec3 a_Normal;
LAYOUT(2) IN vec3 a_Binormal;
LAYOUT(3) IN vec3 a_Tangent;
LAYOUT(4) IN vec2 a_TexCoord;
LAYOUT(5) IN vec4 a_Color;
)"
	R"(

LAYOUT(0) CENTROID OUT lowp vec4 v_VColor;
LAYOUT(1) CENTROID OUT mediump vec2 v_UV1;
LAYOUT(2) CENTROID OUT mediump vec2 v_UV2;
LAYOUT(3) OUT mediump vec3 v_WorldP;
LAYOUT(4) OUT mediump vec3 v_WorldN;
LAYOUT(5) OUT mediump vec3 v_WorldT;
LAYOUT(6) OUT mediump vec3 v_WorldB;
LAYOUT(7) OUT mediump vec2 v_ScreenUV;
//$C_OUT1$
//$C_OUT2$
)";

static const char g_material_model_vs_src_pre_uniform[] =

	R"(
uniform mat4 ProjectionMatrix;

#ifdef _INSTANCING_

uniform mat4 ModelMatrix[10];
uniform vec4 UVOffset[10];
uniform vec4 ModelColor[10];

#else

uniform mat4 ModelMatrix;
uniform vec4 UVOffset;
uniform vec4 ModelColor;

#endif

uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;
uniform vec4 cameraPosition;
)";

static const char g_material_model_vs_src_suf1[] =
	R"(

vec2 GetUV(vec2 uv)
{
	uv.y = mUVInversed.x + mUVInversed.y * uv.y;
	return uv;
}

vec2 GetUVBack(vec2 uv)
{
	uv.y = mUVInversed.z + mUVInversed.w * uv.y;
	return uv;
}


void main()
{
#ifdef _INSTANCING_
	mat4 modelMatrix = ModelMatrix[int(gl_InstanceID)];
	vec4 uvOffset = UVOffset[int(gl_InstanceID)];
	vec4 modelColor = ModelColor[int(gl_InstanceID)] * a_Color;
#else
	mat4 modelMatrix = ModelMatrix;
	vec4 uvOffset = UVOffset;
	vec4 modelColor = ModelColor * a_Color;
#endif

	mat3 modelMatRot = mat3(modelMatrix);
	vec3 worldPos = (modelMatrix * a_Position).xyz;
	vec3 worldNormal = normalize(modelMatRot * a_Normal);
	vec3 worldBinormal = normalize(modelMatRot * a_Binormal);
	vec3 worldTangent = normalize(modelMatRot * a_Tangent);
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	// Calculate ObjectScale
	objectScale.x = length(modelMatRot * vec3(1.0, 0.0, 0.0));
	objectScale.y = length(modelMatRot * vec3(0.0, 1.0, 0.0));
	objectScale.z = length(modelMatRot * vec3(0.0, 0.0, 1.0));

	// UV
	vec2 uv1 = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;
	vec2 uv2 = uv1;

	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;

	vec3 pixelNormalDir = worldNormal;
	
	vec4 vcolor = modelColor;
)";

static const char g_material_model_vs_src_suf2[] =
	R"(
	worldPos = worldPos + worldPositionOffset;

	v_WorldP = worldPos;
	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
	v_UV1 = uv1;
	v_UV2 = uv2;
	v_VColor = vcolor;
	gl_Position = ProjectionMatrix * vec4(worldPos, 1.0);
	v_ScreenUV.xy = gl_Position.xy / gl_Position.w;
	v_ScreenUV.xy = vec2(v_ScreenUV.x + 1.0, v_ScreenUV.y + 1.0) * 0.5;

	#ifdef _Y_INVERTED_
	gl_Position.y = - gl_Position.y;
	#endif
}
)";

static const char g_material_sprite_vs_src_pre_simple[] =
	R"(

LAYOUT(0) IN vec4 atPosition;
LAYOUT(1) IN vec4 atColor;
LAYOUT(2) IN vec4 atTexCoord;
)"

	R"(
LAYOUT(0) CENTROID OUT lowp vec4 v_VColor;
LAYOUT(1) CENTROID OUT mediump vec2 v_UV1;
LAYOUT(2) CENTROID OUT mediump vec2 v_UV2;
LAYOUT(3) OUT mediump vec3 v_WorldP;
LAYOUT(4) OUT mediump vec3 v_WorldN;
LAYOUT(5) OUT mediump vec3 v_WorldT;
LAYOUT(6) OUT mediump vec3 v_WorldB;
LAYOUT(7) OUT mediump vec2 v_ScreenUV;
)";

static const char g_material_sprite_vs_src_pre_simple_uniform[] =
	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;
uniform vec4 cameraPosition;

)";

static const char g_material_sprite_vs_src_pre[] =
	R"(

LAYOUT(0) IN vec4 atPosition;
LAYOUT(1) IN vec4 atColor;
LAYOUT(2) IN vec3 atNormal;
LAYOUT(3) IN vec3 atTangent;
LAYOUT(4) IN vec2 atTexCoord;
LAYOUT(5) IN vec2 atTexCoord2;
//$C_IN1$
//$C_IN2$
)"

	R"(
LAYOUT(0) CENTROID OUT lowp vec4 v_VColor;
LAYOUT(1) CENTROID OUT mediump vec2 v_UV1;
LAYOUT(2) CENTROID OUT mediump vec2 v_UV2;
LAYOUT(3) OUT mediump vec3 v_WorldP;
LAYOUT(4) OUT mediump vec3 v_WorldN;
LAYOUT(5) OUT mediump vec3 v_WorldT;
LAYOUT(6) OUT mediump vec3 v_WorldB;
LAYOUT(7) OUT mediump vec2 v_ScreenUV;
//$C_OUT1$
//$C_OUT2$
)";

static const char g_material_sprite_vs_src_pre_uniform[] =

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;
uniform vec4 predefined_uniform;
uniform vec4 cameraPosition;

)";

static const char g_material_sprite_vs_src_suf1_simple[] =

	R"(

vec2 GetUV(vec2 uv)
{
	uv.y = mUVInversed.x + mUVInversed.y * uv.y;
	return uv;
}

vec2 GetUVBack(vec2 uv)
{
	uv.y = mUVInversed.z + mUVInversed.w * uv.y;
	return uv;
}

void main() {
	vec3 worldPos = atPosition.xyz;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	// UV
	vec2 uv1 = atTexCoord.xy;
	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = uv1;

	// NBT
	vec3 worldNormal = vec3(0.0, 0.0, 0.0);
	vec3 worldBinormal = vec3(0.0, 0.0, 0.0);
	vec3 worldTangent = vec3(0.0, 0.0, 0.0);
	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;

	vec3 pixelNormalDir = worldNormal;
	vec4 vcolor = atColor;
)";

static const char g_material_sprite_vs_src_suf1[] =

	R"(

vec2 GetUV(vec2 uv)
{
	uv.y = mUVInversed.x + mUVInversed.y * uv.y;
	return uv;
}

vec2 GetUVBack(vec2 uv)
{
	uv.y = mUVInversed.z + mUVInversed.w * uv.y;
	return uv;
}

void main() {
	vec3 worldPos = atPosition.xyz;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	// UV
	vec2 uv1 = atTexCoord.xy;
	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = atTexCoord2.xy;
	//uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	vec3 worldNormal = (atNormal - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldTangent = (atTangent - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldBinormal = cross(worldNormal, worldTangent);

	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
	vec3 pixelNormalDir = worldNormal;
	vec4 vcolor = atColor;
)";

static const char g_material_sprite_vs_src_suf2[] =

	R"(
	worldPos = worldPos + worldPositionOffset;

	vec4 cameraPos = uMatCamera * vec4(worldPos, 1.0);
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	v_WorldP = worldPos;
	v_VColor = vcolor;

	v_UV1 = uv1;
	v_UV2 = uv2;
	v_ScreenUV.xy = gl_Position.xy / gl_Position.w;
	v_ScreenUV.xy = vec2(v_ScreenUV.x + 1.0, v_ScreenUV.y + 1.0) * 0.5;

	#ifdef _Y_INVERTED_
	gl_Position.y = - gl_Position.y;
	#endif
}

)";

static const char g_material_fs_src_pre[] =
	R"(

LAYOUT(0) CENTROID IN lowp vec4 v_VColor;
LAYOUT(1) CENTROID IN mediump vec2 v_UV1;
LAYOUT(2) CENTROID IN mediump vec2 v_UV2;
LAYOUT(3) IN mediump vec3 v_WorldP;
LAYOUT(4) IN mediump vec3 v_WorldN;
LAYOUT(5) IN mediump vec3 v_WorldT;
LAYOUT(6) IN mediump vec3 v_WorldB;
LAYOUT(7) IN mediump vec2 v_ScreenUV;
//$C_PIN1$
//$C_PIN2$

)";

static const char g_material_fs_src_pre_uniform[] =
	R"(

uniform vec4 mUVInversedBack;
uniform vec4 predefined_uniform;
uniform vec4 cameraPosition;

)";

static const char g_material_fs_src_suf1[] =
	R"(

vec2 GetUV(vec2 uv)
{
	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;
	return uv;
}

vec2 GetUVBack(vec2 uv)
{
	uv.y = mUVInversedBack.z + mUVInversedBack.w * uv.y;
	return uv;
}

#ifdef _MATERIAL_LIT_

const float lightScale = 3.14;

float saturate(float v)
{
	return max(min(v, 1.0), 0.0);
}

float calcD_GGX(float roughness, float dotNH)
{
	float alpha = roughness*roughness;
	float alphaSqr = alpha*alpha;
	float pi = 3.14159;
	float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0;
	return (alpha / denom) * (alpha / denom) / pi;
}

float calcF(float F0, float dotLH)
{
	float dotLH5 = pow(1.0-dotLH,5.0);
	return F0 + (1.0-F0)*(dotLH5);
}

float calcG_Schlick(float roughness, float dotNV, float dotNL)
{
	// UE4
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	// float k = roughness * roughness / 2.0;

	float gV = dotNV*(1.0 - k) + k;
	float gL = dotNL*(1.0 - k) + k;

	return 1.0 / (gV * gL);
}

float calcLightingGGX(vec3 N, vec3 V, vec3 L, float roughness, float F0)
{
	vec3 H = normalize(V+L);

	float dotNL = saturate( dot(N,L) );
	float dotLH = saturate( dot(L,H) );
	float dotNH = saturate( dot(N,H) ) - 0.001;
	float dotNV = saturate( dot(N,V) ) + 0.001;

	float D = calcD_GGX(roughness, dotNH);
	float F = calcF(F0, dotLH);
	float G = calcG_Schlick(roughness, dotNV, dotNL);

	return dotNL * D * F * G / 4.0;
}

vec3 calcDirectionalLightDiffuseColor(vec3 diffuseColor, vec3 normal, vec3 lightDir, float ao)
{
	vec3 color = vec3(0.0,0.0,0.0);

	float NoL = dot(normal,lightDir);
	color.xyz = lightColor.xyz * lightScale * max(NoL,0.0) * ao / 3.14;
	color.xyz = color.xyz * diffuseColor.xyz;
	return color;
}

#endif

void main()
{
	vec2 uv1 = v_UV1;
	vec2 uv2 = v_UV2;
	vec3 worldPos = v_WorldP;
	vec3 worldNormal = v_WorldN;
	vec3 worldTangent = v_WorldT;
	vec3 worldBinormal = v_WorldB;
	vec3 pixelNormalDir = worldNormal;
	vec4 vcolor = v_VColor;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);
)";

static const char g_material_fs_src_suf2_lit[] =
	R"(

	vec3 viewDir = normalize(cameraPosition.xyz - worldPos);
	vec3 diffuse = calcDirectionalLightDiffuseColor(baseColor, pixelNormalDir, lightDirection.xyz, ambientOcclusion);
	vec3 specular = lightColor.xyz * lightScale * calcLightingGGX(pixelNormalDir, viewDir, lightDirection.xyz, roughness, 0.9);

	vec4 Output =  vec4(metallic * specular + (1.0 - metallic) * diffuse + baseColor * lightAmbientColor.xyz * ambientOcclusion, opacity);
	Output.xyz = Output.xyz + emissive.xyz;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	FRAGCOLOR = Output;
}

)";

static const char g_material_fs_src_suf2_unlit[] =
	R"(

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	FRAGCOLOR = vec4(emissive, opacity);
}

)";

static const char g_material_fs_src_suf2_refraction[] =
	R"(
	float airRefraction = 1.0;

	vec3 dir = mat3(cameraMat) * pixelNormalDir;
	vec2 distortUV = dir.xy * (refraction - airRefraction);

	distortUV += v_ScreenUV;
	distortUV = GetUVBack(distortUV);	

	#ifdef _Y_INVERTED_
	distortUV.y = 1.0f - distortUV.y;
	#endif

	vec4 bg = TEX2D(background, distortUV);
	FRAGCOLOR = bg;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;
}

)";

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

class ShaderGenerator
{
	bool useUniformBlock_ = false;
	bool useSet_ = false;
	int32_t textureBindingOffset_ = 0;

	std::string Replace(std::string target, std::string from_, std::string to_)
	{
		std::string::size_type Pos(target.find(from_));

		while (Pos != std::string::npos)
		{
			target.replace(Pos, from_.length(), to_);
			Pos = target.find(from_, Pos + to_.length());
		}

		return target;
	}

	std::string GetType(int32_t i)
	{
		if (i == 1)
			return "float";
		if (i == 2)
			return "vec2";
		if (i == 3)
			return "vec3";
		if (i == 4)
			return "vec4";
		if (i == 16)
			return "mat4";
		assert(0);
		return "";
	}

	std::string GetElement(int32_t i)
	{
		if (i == 1)
			return ".x";
		if (i == 2)
			return ".xy";
		if (i == 3)
			return ".xyz";
		if (i == 4)
			return ".xyzw";
		assert(0);
		return "";
	}

	void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name)
	{
		maincode << "uniform " << GetType(type) << " " << name << ";" << std::endl;
	}

	void ExportTexture(std::ostringstream& maincode, const char* name, int bind, int stage)
	{
		if (useUniformBlock_)
		{
			if (useSet_)
			{
				maincode << "layout(set = " << stage << ", binding = " << (bind + textureBindingOffset_) << ") uniform sampler2D " << name
						 << ";" << std::endl;
			}
			else
			{
				maincode << "layout(binding = " << (bind + textureBindingOffset_) << ") uniform sampler2D " << name << ";" << std::endl;
			}
		}
		else
		{
			maincode << "uniform sampler2D " << name << ";" << std::endl;
		}
	}

	void ExportHeader(std::ostringstream& maincode, Material* material, int stage, bool isSprite, bool isOutputDefined, bool is450)
	{
		if (is450)
		{
			maincode << material_common_define_450;
		}
		else
		{
			maincode << material_common_define_not_450;
		}

		maincode << material_common_define;

		// Adhoc
		if (is450)
		{
			if (stage == 0)
			{
				maincode << material_common_vs_define;
			}
			else if (stage == 1)
			{
				maincode << material_common_fs_define;
			}
		}

		if (stage == 0)
		{
			if (isSprite)
			{
				if (material->GetIsSimpleVertex())
				{
					maincode << g_material_sprite_vs_src_pre_simple;
				}
				else
				{
					maincode << g_material_sprite_vs_src_pre;
				}
			}
			else
			{
				maincode << g_material_model_vs_src_pre;
			}
		}
		else
		{
			maincode << g_material_fs_src_pre;
		}

		if (isOutputDefined && stage == 1)
		{
			maincode << "#define FRAGCOLOR out_flagColor" << std::endl;
			maincode << "layout(location = 0) out vec4 out_flagColor;" << std::endl;
			maincode << std::endl;
		}
	}

	void ExportDefaultUniform(std::ostringstream& maincode, Material* material, int stage, bool isSprite)
	{
		if (stage == 0)
		{
			if (isSprite)
			{
				if (material->GetIsSimpleVertex())
				{
					maincode << g_material_sprite_vs_src_pre_simple_uniform;
				}
				else
				{
					maincode << g_material_sprite_vs_src_pre_uniform;
				}
			}
			else
			{
				maincode << g_material_model_vs_src_pre_uniform;
			}
		}
		else
		{
			maincode << g_material_fs_src_pre_uniform;
		}
	}

	void ExportMain(std::ostringstream& maincode,
					Material* material,
					int stage,
					bool isSprite,
					MaterialShaderType shaderType,
					const std::string& baseCode,
					bool useUniformBlock,
					bool isInstancing)
	{
		if (stage == 0)
		{
			if (isSprite)
			{
				if (material->GetIsSimpleVertex())
				{
					maincode << g_material_sprite_vs_src_suf1_simple;
				}
				else
				{
					maincode << g_material_sprite_vs_src_suf1;
				}
			}
			else
			{
				maincode << g_material_model_vs_src_suf1;
			}

			if (material->GetCustomData1Count() > 0)
			{
				if (isSprite)
				{
					maincode << GetType(material->GetCustomData1Count()) + " customData1 = atCustomData1;\n";
				}
				else if (isInstancing)
				{
					maincode << GetType(4) + " customData1 = customData1s[int(gl_InstanceID)];\n";
				}
				maincode << "v_CustomData1 = customData1" + GetElement(material->GetCustomData1Count()) + ";\n";
			}

			if (material->GetCustomData2Count() > 0)
			{
				if (isSprite)
				{
					maincode << GetType(material->GetCustomData2Count()) + " customData2 = atCustomData2;\n";
				}
				else if (isInstancing)
				{
					maincode << GetType(4) + " customData2 = customData2s[int(gl_InstanceID)];\n";
				}
				maincode << "v_CustomData2 = customData2" + GetElement(material->GetCustomData2Count()) + ";\n";
			}

			maincode << baseCode;

			if (isSprite)
			{
				maincode << g_material_sprite_vs_src_suf2;
			}
			else
			{
				maincode << g_material_model_vs_src_suf2;
			}
		}
		else
		{
			maincode << g_material_fs_src_suf1;

			if (material->GetCustomData1Count() > 0)
			{
				maincode << GetType(material->GetCustomData1Count()) + " customData1 = v_CustomData1;\n";
			}

			if (material->GetCustomData2Count() > 0)
			{
				maincode << GetType(material->GetCustomData2Count()) + " customData2 = v_CustomData2;\n";
			}

			maincode << baseCode;

			if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
			{
				maincode << g_material_fs_src_suf2_refraction;
			}
			else
			{
				if (material->GetShadingModel() == Effekseer::ShadingModelType::Lit)
				{
					maincode << g_material_fs_src_suf2_lit;
				}
				else if (material->GetShadingModel() == Effekseer::ShadingModelType::Unlit)
				{
					maincode << g_material_fs_src_suf2_unlit;
				}
			}
		}
	}

public:
	ShaderData GenerateShader(Material* material,
							  MaterialShaderType shaderType,
							  int32_t maximumTextureCount,
							  bool useUniformBlock,
							  bool isOutputDefined,
							  bool is450,
							  bool useSet,
							  int textureBindingOffset,
							  bool isYInverted,
							  bool isInstancing)
	{
		useUniformBlock_ = useUniformBlock;
		useSet_ = useSet;
		textureBindingOffset_ = textureBindingOffset;

		bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
		bool isRefrection = material->GetHasRefraction() &&
							(shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

		ShaderData shaderData;

		for (int stage = 0; stage < 2; stage++)
		{
			std::ostringstream maincode;

			ExportHeader(maincode, material, stage, isSprite, isOutputDefined, is450);

			if (isYInverted)
			{
				maincode << "#define _Y_INVERTED_ 1" << std::endl;
			}

			if (isInstancing)
			{
				maincode << "#define _INSTANCING_ 1" << std::endl;
				maincode << "#define gl_InstanceID gl_InstanceIndex" << std::endl;
			}

			int32_t actualTextureCount = std::min(maximumTextureCount, material->GetTextureCount());

			for (int32_t i = 0; i < actualTextureCount; i++)
			{
				auto textureIndex = material->GetTextureIndex(i);
				auto textureName = material->GetTextureName(i);

				ExportTexture(maincode, textureName, i, stage);
			}

			for (int32_t i = actualTextureCount; i < actualTextureCount + 1; i++)
			{
				ExportTexture(maincode, "background", i, stage);
			}

			// Uniform block begin
			if (useUniformBlock)
			{
				if (useSet_)
				{
					if (stage == 0)
					{
						maincode << "layout(set = 0, binding = 0) uniform Block {" << std::endl;
					}
					else if (stage == 1)
					{
						maincode << "layout(set = 1, binding = 0) uniform Block {" << std::endl;
					}
				}
				else
				{
					if (stage == 0)
					{
						maincode << "layout(binding = 0) uniform Block {" << std::endl;
					}
					else if (stage == 1)
					{
						maincode << "layout(binding = 0) uniform Block {" << std::endl;
					}
				}
			}

			ExportDefaultUniform(maincode, material, stage, isSprite);

			if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Lit && stage == 1)
			{
				ExportUniform(maincode, 4, "lightDirection");
				ExportUniform(maincode, 4, "lightColor");
				ExportUniform(maincode, 4, "lightAmbientColor");

				maincode << "#define _MATERIAL_LIT_ 1" << std::endl;
			}
			else if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
			{
			}

			if (isRefrection && stage == 1)
			{
				ExportUniform(maincode, 16, "cameraMat");
			}

			if (!isSprite && stage == 0)
			{
				if (material->GetCustomData1Count() > 0)
				{
					if (isInstancing)
					{
						maincode << "uniform vec4 customData1s[10];" << std::endl;
					}
					else
					{
						maincode << "uniform vec4 customData1;" << std::endl;
					}
				}
				if (material->GetCustomData2Count() > 0)
				{
					if (isInstancing)
					{
						maincode << "uniform vec4 customData2s[10];" << std::endl;
					}
					else
					{
						maincode << "uniform vec4 customData2;" << std::endl;
					}
				}
			}

			for (int32_t i = 0; i < material->GetUniformCount(); i++)
			{
				auto uniformIndex = material->GetUniformIndex(i);
				auto uniformName = material->GetUniformName(i);

				ExportUniform(maincode, 4, uniformName);
			}

			// Uniform block end
			if (useUniformBlock)
			{
				maincode << "};" << std::endl;
			}

			auto baseCode = std::string(material->GetGenericCode());
			baseCode = Replace(baseCode, "$F1$", "float");
			baseCode = Replace(baseCode, "$F2$", "vec2");
			baseCode = Replace(baseCode, "$F3$", "vec3");
			baseCode = Replace(baseCode, "$F4$", "vec4");
			baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
			baseCode = Replace(baseCode, "$UV$", "uv");
			baseCode = Replace(baseCode, "$MOD", "mod");

			// replace textures
			for (int32_t i = 0; i < actualTextureCount; i++)
			{
				auto textureIndex = material->GetTextureIndex(i);
				auto textureName = std::string(material->GetTextureName(i));

				std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
				std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

				if (stage == 0)
				{
					baseCode = Replace(baseCode, keyP, "TEX2D(" + textureName + ",GetUV(");
					baseCode = Replace(baseCode, keyS, "), 0.0)");
				}
				else
				{
					baseCode = Replace(baseCode, keyP, "TEX2D(" + textureName + ",GetUV(");
					baseCode = Replace(baseCode, keyS, "))");
				}
			}

			// invalid texture
			for (size_t i = actualTextureCount; i < material->GetTextureCount(); i++)
			{
				auto textureIndex = material->GetTextureIndex(i);
				auto textureName = std::string(material->GetTextureName(i));

				std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
				std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

				baseCode = Replace(baseCode, keyP, "vec4(");
				baseCode = Replace(baseCode, keyS, ",0.0,1.0)");
			}

			ExportMain(maincode, material, stage, isSprite, shaderType, baseCode, useUniformBlock, isInstancing);

			if (stage == 0)
			{
				shaderData.CodeVS = maincode.str();
			}
			else
			{
				shaderData.CodePS = maincode.str();
			}
		}

		// custom data
		int32_t layoutOffset = 6;
		int32_t pvLayoutOffset = 8;

		if (material->GetCustomData1Count() > 0)
		{
			if (isSprite)
			{
				shaderData.CodeVS = Replace(shaderData.CodeVS,
											"//$C_IN1$",
											"LAYOUT(" + std::to_string(layoutOffset) + ") " + "IN " +
												GetType(material->GetCustomData1Count()) + " atCustomData1;");
			}
			shaderData.CodeVS = Replace(shaderData.CodeVS,
										"//$C_OUT1$",
										"LAYOUT(" + std::to_string(pvLayoutOffset) + ") " + "OUT mediump " +
											GetType(material->GetCustomData1Count()) + " v_CustomData1;");
			shaderData.CodePS = Replace(shaderData.CodePS,
										"//$C_PIN1$",
										"LAYOUT(" + std::to_string(pvLayoutOffset) + ") " + "IN mediump " +
											GetType(material->GetCustomData1Count()) + " v_CustomData1;");

			layoutOffset += 1;
			pvLayoutOffset += 1;
		}

		if (material->GetCustomData2Count() > 0)
		{
			if (isSprite)
			{
				shaderData.CodeVS = Replace(shaderData.CodeVS,
											"//$C_IN2$",
											"LAYOUT(" + std::to_string(layoutOffset) + ") " + "IN " +
												GetType(material->GetCustomData2Count()) + " atCustomData2;");
			}
			shaderData.CodeVS = Replace(shaderData.CodeVS,
										"//$C_OUT2$",
										"LAYOUT(" + std::to_string(pvLayoutOffset) + ") " + "OUT mediump " +
											GetType(material->GetCustomData2Count()) + " v_CustomData2;");
			shaderData.CodePS = Replace(shaderData.CodePS,
										"//$C_PIN2$",
										"LAYOUT(" + std::to_string(pvLayoutOffset) + ") " + "IN mediump " +
											GetType(material->GetCustomData2Count()) + " v_CustomData2;");
		}

		return shaderData;
	}
};

} // namespace GLSL

} // namespace Effekseer
