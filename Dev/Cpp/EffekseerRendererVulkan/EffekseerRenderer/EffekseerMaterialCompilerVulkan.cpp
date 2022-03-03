#include "EffekseerMaterialCompilerVulkan.h"
#include <iostream>

#include "../3rdParty/LLGI/src/Vulkan/LLGI.CompilerVulkan.h"

#include "../EffekseerMaterialCompiler/GLSL/GLSL.h"

#undef min
namespace Effekseer
{

namespace Vulkan
{	
	/*

	static char* material_common_define = R"(
#version 450
#define MOD mod
#define FRAC fract
#define LERP mix

float atan2(in float y, in float x) {
    return x == 0.0 ? sign(y)* 3.141592 / 2.0 : atan(y, x);
}

)";

static const char g_material_model_vs_src_pre[] =
	R"(
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Binormal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec2 a_TexCoord;
layout(location = 5) in vec4 a_Color;
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
in float a_InstanceID;
in vec4 a_UVOffset;
in vec4 a_ModelColor;
)"
#endif
	R"(

layout(location = 0) out lowp vec4 v_VColor;
layout(location = 1) out mediump vec2 v_UV1;
layout(location = 2) out mediump vec2 v_UV2;
layout(location = 3) out mediump vec3 v_WorldP;
layout(location = 4) out mediump vec3 v_WorldN;
layout(location = 5) out mediump vec3 v_WorldT;
layout(location = 6) out mediump vec3 v_WorldB;
layout(location = 7) out mediump vec2 v_ScreenUV;
//$C_OUT1$
//$C_OUT2$

layout(set = 0, binding = 0) uniform Block
{
    uniform mat4 ProjectionMatrix;
)"
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
    uniform vec4 mUVInversed;
    uniform vec4 predefined_uniform;
};

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

)";

static const char g_material_model_vs_src_suf1[] =
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
	mat3 modelMatRot = mat3(modelMatrix);
	vec3 worldPos = (modelMatrix * a_Position).xyz;
	vec3 worldNormal = normalize(modelMatRot * a_Normal);
	vec3 worldBinormal = normalize(modelMatRot * a_Binormal);
	vec3 worldTangent = normalize(modelMatRot * a_Tangent);

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
}
)";

static const char g_material_sprite_vs_src_pre_simple[] =
	R"(
layout(location = 0) in vec4 atPosition;
layout(location = 1) in vec4 atColor;
layout(location = 2) in vec4 atTexCoord;
)"

	R"(
layout(location = 0) out lowp vec4 v_VColor;
layout(location = 1) out mediump vec2 v_UV1;
layout(location = 2) out mediump vec2 v_UV2;
layout(location = 3) out mediump vec3 v_WorldP;
layout(location = 4) out mediump vec3 v_WorldN;
layout(location = 5) out mediump vec3 v_WorldT;
layout(location = 6) out mediump vec3 v_WorldB;
layout(location = 7) out mediump vec2 v_ScreenUV;
)"

	R"(
layout(set = 0, binding = 0) uniform Block {
    uniform mat4 uMatCamera;
    uniform mat4 uMatProjection;
    uniform vec4 mUVInversed;
    uniform vec4 predefined_uniform;
};

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

)";

static const char g_material_sprite_vs_src_pre[] =
	R"(
layout(location = 0) in vec4 atPosition;
layout(location = 1) in vec4 atColor;
layout(location = 2) in vec3 atNormal;
layout(location = 3) in vec3 atTangent;
layout(location = 4) in vec2 atTexCoord;
layout(location = 5) in vec2 atTexCoord2;
//$C_IN1$
//$C_IN2$
)"

	R"(
layout(location = 0) out lowp vec4 v_VColor;
layout(location = 1) out mediump vec2 v_UV1;
layout(location = 2) out mediump vec2 v_UV2;
layout(location = 3) out mediump vec3 v_WorldP;
layout(location = 4) out mediump vec3 v_WorldN;
layout(location = 5) out mediump vec3 v_WorldT;
layout(location = 6) out mediump vec3 v_WorldB;
layout(location = 7) out mediump vec2 v_ScreenUV;
//$C_OUT1$
//$C_OUT2$
)"

	R"(
layout(set = 0, binding = 0) uniform Block {
    uniform mat4 uMatCamera;
    uniform mat4 uMatProjection;
    uniform vec4 mUVInversed;
    uniform vec4 predefined_uniform;
};

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

)";

static const char g_material_sprite_vs_src_suf1_simple[] =

	R"(

void main() {
	vec3 worldPos = atPosition.xyz;

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

void main() {
	vec3 worldPos = atPosition.xyz;

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
}

)";

static const char g_material_fs_src_pre[] =
	R"(

layout(location = 0) in vec4 v_VColor;
layout(location = 1) in vec2 v_UV1;
layout(location = 2) in vec2 v_UV2;
layout(location = 3) in vec3 v_WorldP;
layout(location = 4) in vec3 v_WorldN;
layout(location = 5) in vec3 v_WorldT;
layout(location = 6) in vec3 v_WorldB;
layout(location = 7) in vec2 v_ScreenUV;
//$C_PIN1$
//$C_PIN2$

layout(location = 0) out vec4 out_flagColor;

layout(set = 1, binding = 0) uniform Block
{
    vec4 mUVInversedBack;
    vec4 predefined_uniform;
};

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


)";

static const char g_material_fs_src_suf1[] =
	R"(

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

)";

static const char g_material_fs_src_suf2_lit[] =
	R"(

	vec3 viewDir = normalize(cameraPosition.xyz - worldPos);
	vec3 diffuse = calcDirectionalLightDiffuseColor(baseColor, pixelNormalDir, lightDirection.xyz, ambientOcclusion);
	vec3 specular = lightColor.xyz * lightScale * calcLightingGGX(worldNormal, viewDir, lightDirection.xyz, roughness, 0.9);

	vec4 Output =  vec4(metallic * specular + (1.0 - metallic) * diffuse + lightAmbientColor.xyz, opacity);
	Output.xyz = Output.xyz + emissive.xyz;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	out_flagColor = Output;
}

)";

static const char g_material_fs_src_suf2_unlit[] =
	R"(

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	out_flagColor = vec4(emissive, opacity);
}

)";

static const char g_material_fs_src_suf2_refraction[] =
	R"(
	float airRefraction = 1.0;

	vec3 dir = mat3(cameraMat) * pixelNormalDir;
	vec2 distortUV = dir.xy * (refraction - airRefraction);

	distortUV += v_ScreenUV;
	distortUV = GetUVBack(distortUV);	

	vec4 bg = TEX2D(background, distortUV);
	out_flagColor = bg;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;
}

)";

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

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

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

void ExportTexture(std::ostringstream& maincode, int bindingIndex, const char* name) { maincode << "layout(binding = " << bindingIndex << ") uniform sampler2D " << name << ";" << std::endl; }

void ExportHeader(std::ostringstream& maincode, Material* material, int stage, bool isSprite)
{
	maincode << material_common_define;

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
}

void ExportMain(
	std::ostringstream& maincode, Material* material, int stage, bool isSprite, MaterialShaderType shaderType, const std::string& baseCode)
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
			maincode << "v_CustomData1 = customData1" + GetElement(material->GetCustomData1Count()) + ";\n";
		}

		if (material->GetCustomData2Count() > 0)
		{
			if (isSprite)
			{
				maincode << GetType(material->GetCustomData2Count()) + " customData2 = atCustomData2;\n";
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

ShaderData GenerateShader(Material* material, MaterialShaderType shaderType, int32_t maximumTextureCount)
{
	bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
	bool isRefrection =
		material->GetHasRefraction() && (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

	ShaderData shaderData;

	for (int stage = 0; stage < 2; stage++)
	{
		std::ostringstream maincode;

		ExportHeader(maincode, material, stage, isSprite);

		int32_t actualTextureCount = std::min(maximumTextureCount, material->GetTextureCount());

		for (size_t i = 0; i < actualTextureCount; i++)
		{
			auto textureIndex = material->GetTextureIndex(i);
			auto textureName = material->GetTextureName(i);

			ExportTexture(maincode, i + 1, textureName);
		}

		for (size_t i = actualTextureCount; i < actualTextureCount + 1; i++)
		{
			ExportTexture(maincode, i + 1, "background");
		}

		if (material->GetShadingModel() == ::Effekseer::ShadingModelType::Lit && stage == 1)
		{
			ExportUniform(maincode, 4, "cameraPosition");
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
				maincode << "uniform vec4 customData1;" << std::endl;
			}
			if (material->GetCustomData2Count() > 0)
			{
				maincode << "uniform vec4 customData2;" << std::endl;
			}
		}

		for (int32_t i = 0; i < material->GetUniformCount(); i++)
		{
			auto uniformIndex = material->GetUniformIndex(i);
			auto uniformName = material->GetUniformName(i);

			ExportUniform(maincode, 4, uniformName);
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
		for (size_t i = 0; i < actualTextureCount; i++)
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

		ExportMain(maincode, material, stage, isSprite, shaderType, baseCode);

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
	if (material->GetCustomData1Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS, "//$C_IN1$", "in " + GetType(material->GetCustomData1Count()) + " atCustomData1;");
		}
		shaderData.CodeVS =
			Replace(shaderData.CodeVS, "//$C_OUT1$", "out mediump " + GetType(material->GetCustomData1Count()) + " v_CustomData1;");
		shaderData.CodePS =
			Replace(shaderData.CodePS, "//$C_PIN1$", "in mediump " + GetType(material->GetCustomData1Count()) + " v_CustomData1;");
	}

	if (material->GetCustomData2Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS, "//$C_IN2$", "in " + GetType(material->GetCustomData2Count()) + " atCustomData2;");
		}
		shaderData.CodeVS =
			Replace(shaderData.CodeVS, "//$C_OUT2$", "out mediump " + GetType(material->GetCustomData2Count()) + " v_CustomData2;");
		shaderData.CodePS =
			Replace(shaderData.CodePS, "//$C_PIN2$", "in mediump " + GetType(material->GetCustomData2Count()) + " v_CustomData2;");
	}

	return shaderData;
}
*/
} // namespace Vulkan












static void Serialize(std::vector<uint8_t>& dst, const LLGI::CompilerResult& result)
{

	uint32_t binarySize = 0;

	binarySize += sizeof(uint32_t);

	for (size_t i = 0; i < result.Binary.size(); i++)
	{
		binarySize += sizeof(uint32_t);
		binarySize += result.Binary[i].size();
	}

	dst.resize(binarySize);

	uint32_t offset = 0;
	uint32_t count = result.Binary.size();

	memcpy(dst.data() + offset, &count, sizeof(int32_t));
	offset += sizeof(int32_t);

	for (size_t i = 0; i < result.Binary.size(); i++)
	{
		uint32_t size = result.Binary[i].size();

		memcpy(dst.data() + offset, &size, sizeof(int32_t));
		offset += sizeof(int32_t);

		memcpy(dst.data() + offset, result.Binary[i].data(), result.Binary[i].size());
		offset += result.Binary[i].size();
	}
}


} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryVulkan : public CompiledMaterialBinary, ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryVulkan() {}

	virtual ~CompiledMaterialBinaryVulkan() {}

	void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		vertexShaders_.at(static_cast<int>(type)) = data;
	}

	void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data) { pixelShaders_.at(static_cast<int>(type)) = data; }

	const uint8_t* GetVertexShaderData(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetVertexShaderSize(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).size(); }

	const uint8_t* GetPixelShaderData(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).data(); }

	int32_t GetPixelShaderSize(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).size(); }

	int AddRef() override { return ReferenceObject::AddRef(); }

	int Release() override { return ReferenceObject::Release(); }

	int GetRef() override { return ReferenceObject::GetRef(); }
};

CompiledMaterialBinary* MaterialCompilerVulkan::Compile(MaterialFile* material, int32_t maximumTextureCount)
{
	// to use options
	auto compiler = LLGI::CreateSharedPtr(new LLGI::CompilerVulkan());

	auto binary = new CompiledMaterialBinaryVulkan();

	auto convertToVectorVS = [compiler](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		LLGI::CompilerResult result;
		compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Vertex);

		if (result.Binary.size() > 0)
		{
			Serialize(ret, result);
		}
		else
		{
			std::cout << "VertexShader Compile Error" << std::endl;
			std::cout << result.Message << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto convertToVectorPS = [compiler](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

		LLGI::CompilerResult result;
		compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Pixel);

		if (result.Binary.size() > 0)
		{
			Serialize(ret, result);
		}
		else
		{
			std::cout << "PixelShader Compile Error" << std::endl;
			std::cout << result.Message << std::endl;
			std::cout << str << std::endl;
		}

		return ret;
	};

	auto saveBinary = [&material, &binary, &convertToVectorVS, &convertToVectorPS, &maximumTextureCount](MaterialShaderType type) {
		
		GLSL::ShaderGenerator generator;
		auto shader = generator.GenerateShader(material, type, maximumTextureCount, true, true, true, true, 1, true, true, 40);

		//auto shader = Vulkan::GenerateShader(material, type, maximumTextureCount);
		binary->SetVertexShaderData(type, convertToVectorVS(shader.CodeVS));
		binary->SetPixelShaderData(type, convertToVectorPS(shader.CodePS));
	};

	if (material->GetHasRefraction())
	{
		saveBinary(MaterialShaderType::Refraction);
		saveBinary(MaterialShaderType::RefractionModel);
	}

	saveBinary(MaterialShaderType::Standard);
	saveBinary(MaterialShaderType::Model);

	return binary;
}

CompiledMaterialBinary* MaterialCompilerVulkan::Compile(MaterialFile* material)
{
	return Compile(material, Effekseer::UserTextureSlotMax);
}

} // namespace Effekseer
