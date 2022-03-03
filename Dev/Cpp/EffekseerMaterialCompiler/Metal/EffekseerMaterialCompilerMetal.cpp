#include "EffekseerMaterialCompilerMetal.h"
#include "../3rdParty/LLGI/src/Metal/LLGI.CompilerMetal.h"

#include <iostream>

namespace Effekseer
{

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

namespace Metal
{

static const char* material_common_define = R"(
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;

#define FRAC fract
#define LERP mix

template <typename T1, typename T2>
inline auto MOD(T1 x, T2 y) -> decltype(x - y * floor(x/y)) {
    return x - y * floor(x/y);
}

)";

static const char* material_common_define_vs = R"(

// Dummy
float CalcDepthFade(float2 screenUV, float meshZ, float softParticleParam) { return 1.0f; }

)";

static const char g_material_model_vs_src_pre[] =
    R"(
struct ShaderInput1 {
  float4 a_Position [[attribute(0)]];
  float3 a_Normal [[attribute(1)]];
  float3 a_Binormal [[attribute(2)]];
  float3 a_Tangent [[attribute(3)]];
  float2 a_TexCoord [[attribute(4)]];
  float4 a_Color [[attribute(5)]];
};

struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_VColor;
  float2 v_UV1;
  float2 v_UV2;
  float3 v_WorldP;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
  float4 v_PosP;
  //$C_OUT1$
  //$C_OUT2$
};
struct ShaderUniform1 {
  float4x4 ProjectionMatrix;
  float4x4 ModelMatrix[40];
  float4 UVOffset[40];
  float4 ModelColor[40];
  float4 mUVInversed;
  float4 predefined_uniform;
  float4 cameraPosition;
//$UNIFORMS$
};
)";

static const char g_material_model_vs_src_suf1[] =
    R"(
vertex ShaderOutput1 main0 (ShaderInput1 i [[stage_in]], constant ShaderUniform1& u [[buffer(0)]], uint instanceIndex [[instance_id]]
//$IN_TEX$
)
{
    ShaderOutput1 o;
    float4x4 modelMatrix = u.ModelMatrix[instanceIndex];
    float4 uvOffset = u.UVOffset[instanceIndex];
    float4 modelColor = u.ModelColor[instanceIndex];
    float3x3 modelMatRot;
    modelMatRot[0] = modelMatrix[0].xyz;
    modelMatRot[1] = modelMatrix[1].xyz;
    modelMatRot[2] = modelMatrix[2].xyz;
    float3 worldPos = (modelMatrix * i.a_Position).xyz;
    float3 worldNormal = normalize(modelMatRot * i.a_Normal);
    float3 worldBinormal = normalize(modelMatRot * i.a_Binormal);
    float3 worldTangent = normalize(modelMatRot * i.a_Tangent);
    float3 objectScale = float3(1.0, 1.0, 1.0);
	// Calculate ObjectScale
	objectScale.x = length(modelMatRot * float3(1.0, 0.0, 0.0));
	objectScale.y = length(modelMatRot * float3(0.0, 1.0, 0.0));
	objectScale.z = length(modelMatRot * float3(0.0, 0.0, 1.0));

    // UV
    float2 uv1 = i.a_TexCoord.xy * uvOffset.zw + uvOffset.xy;
    float2 uv2 = i.a_TexCoord.xy;

    float3 pixelNormalDir = worldNormal;
    
    float4 vcolor = modelColor;

    // Dummy
    float2 screenUV = float2(0.0f, 0.0f);
    float meshZ =  0.0f;
)";

static const char g_material_model_vs_src_suf2[] =
    R"(
    worldPos = worldPos + worldPositionOffset;

    o.v_WorldP = worldPos;
    o.v_WorldN = worldNormal;
    o.v_WorldB = worldBinormal;
    o.v_WorldT = worldTangent;
    o.v_UV1 = uv1;
    o.v_UV2 = uv2;
    o.v_VColor = vcolor;
    o.gl_Position = u.ProjectionMatrix * float4(worldPos, 1.0);
    o.v_PosP = o.gl_Position;
    //o.v_ScreenUV.xy = o.gl_Position.xy / o.gl_Position.w;
    //o.v_ScreenUV.xy = float2(o.v_ScreenUV.x + 1.0, o.v_ScreenUV.y + 1.0) * 0.5;
    return o;
}
)";

static const char g_material_sprite_vs_src_pre_simple[] =
    R"(
struct ShaderInput1 {
  float4 atPosition [[attribute(0)]];
  float4 atColor [[attribute(1)]];
  float4 atTexCoord [[attribute(2)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_VColor;
  float2 v_UV1;
  float2 v_UV2;
  float3 v_WorldP;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
  float4 v_PosP;
};

struct ShaderUniform1 {
  float4x4 uMatCamera;
  float4x4 uMatProjection;
  float4 mUVInversed;
  float4 predefined_uniform;
  float4 cameraPosition;
//$UNIFORMS$
};
)";

static const char g_material_sprite_vs_src_pre[] =
    R"(
struct ShaderInput1 {
  float4 atPosition [[attribute(0)]];
  float4 atColor [[attribute(1)]];
  float3 atNormal [[attribute(2)]];
  float3 atTangent [[attribute(3)]];
  float2 atTexCoord [[attribute(4)]];
  float2 atTexCoord2 [[attribute(5)]];
  //$C_IN1$
  //$C_IN2$
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_VColor;
  float2 v_UV1;
  float2 v_UV2;
  float3 v_WorldP;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
  float4 v_PosP;
  //$C_OUT1$
  //$C_OUT2$
};
struct ShaderUniform1 {
  float4x4 uMatCamera;
  float4x4 uMatProjection;
  float4 mUVInversed;
  float4 predefined_uniform;
  float4 cameraPosition;
//$UNIFORMS$
};
)";

static const char g_material_sprite_vs_src_suf1_simple[] =

    R"(
vertex ShaderOutput1 main0 (ShaderInput1 i [[stage_in]], constant ShaderUniform1& u [[buffer(0)]]
//$IN_TEX$
)
{
    ShaderOutput1 o;
    float3 worldPos = i.atPosition.xyz;
    float3 objectScale = float3(1.0, 1.0, 1.0);

    // UV
    float2 uv1 = i.atTexCoord.xy;
    float2 uv2 = uv1;

    // NBT
    float3 worldNormal = float3(0.0, 0.0, 0.0);
    float3 worldBinormal = float3(0.0, 0.0, 0.0);
    float3 worldTangent = float3(0.0, 0.0, 0.0);
    o.v_WorldN = worldNormal;
    o.v_WorldB = worldBinormal;
    o.v_WorldT = worldTangent;

    float3 pixelNormalDir = worldNormal;
    float4 vcolor = i.atColor;

    // Dummy
    float2 screenUV = float2(0.0f, 0.0f);
    float meshZ =  0.0f;
)";

static const char g_material_sprite_vs_src_suf1[] =

    R"(
vertex ShaderOutput1 main0 (ShaderInput1 i [[stage_in]], constant ShaderUniform1& u [[buffer(0)]]
//$IN_TEX$
)
{
    ShaderOutput1 o;
    float3 worldPos = i.atPosition.xyz;
    float3 objectScale = float3(1.0, 1.0, 1.0);

    // UV
    float2 uv1 = i.atTexCoord.xy;
    float2 uv2 = i.atTexCoord2.xy;

    // NBT
    float3 worldNormal = (i.atNormal - float3(0.5, 0.5, 0.5)) * 2.0;
    float3 worldTangent = (i.atTangent - float3(0.5, 0.5, 0.5)) * 2.0;
    float3 worldBinormal = cross(worldNormal, worldTangent);

    o.v_WorldN = worldNormal;
    o.v_WorldB = worldBinormal;
    o.v_WorldT = worldTangent;
    float3 pixelNormalDir = worldNormal;
    float4 vcolor = i.atColor;

    // Dummy
    float2 screenUV = float2(0.0f, 0.0f);
    float meshZ =  0.0f;
)";

static const char g_material_sprite_vs_src_suf2[] =

    R"(
    worldPos = worldPos + worldPositionOffset;

    float4 cameraPos = u.uMatCamera * float4(worldPos, 1.0);
    cameraPos = cameraPos / cameraPos.w;

    o.gl_Position = u.uMatProjection * cameraPos;

    o.v_WorldP = worldPos;
    o.v_VColor = vcolor;

    o.v_UV1 = uv1;
    o.v_UV2 = uv2;
    o.v_PosP = o.gl_Position;
    //o.v_ScreenUV.xy = o.gl_Position.xy / o.gl_Position.w;
    //o.v_ScreenUV.xy = float2(o.v_ScreenUV.x + 1.0, o.v_ScreenUV.y + 1.0) * 0.5;
    return o;
}

)";

static const char g_material_fs_src_pre[] =
    R"(
struct ShaderInput2 {
  float4 v_VColor [[ centroid_no_perspective ]];
  float2 v_UV1 [[ centroid_no_perspective ]];
  float2 v_UV2 [[ centroid_no_perspective ]];
  float3 v_WorldP;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
  float4 v_PosP;
  //$C_PIN1$
  //$C_PIN2$
};
struct ShaderOutput2 {
  float4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 mUVInversedBack;
  float4 predefined_uniform;
  float4 cameraPosition;
  float4 reconstructionParam1;
  float4 reconstructionParam2;
//$UNIFORMS$
};
)";

static const char g_material_fs_src_suf1[] =
    R"(

float ReplacedDepthFade(texture2d<float> efk_depth, sampler s_efk_depth, float4 reconstructionParam1, float4 reconstructionParam2, float magnification, float2 screenUV, float meshZ, float softParticleParam)
{
	float backgroundZ = efk_depth.sample(s_efk_depth, screenUV).x;

	float distance = softParticleParam * magnification;
	float2 rescale = reconstructionParam1.xy;
	float4 params = reconstructionParam2;

	float2 zs = float2(backgroundZ * rescale.x + rescale.y, meshZ);

	float2 depth = (zs * params.w - params.y) / (params.x - zs * params.z);

	float dir = sign(depth.x);
	depth *= dir;
	return min(max((depth.x - depth.y) / distance, 0.0), 1.0);
}

#ifdef _MATERIAL_LIT_

#define lightScale 3.14

/*
float saturate(float v)
{
    return max(min(v, 1.0), 0.0);
}
*/

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

float calcLightingGGX(float3 N, float3 V, float3 L, float roughness, float F0)
{
    float3 H = normalize(V+L);

    float dotNL = saturate( dot(N,L) );
    float dotLH = saturate( dot(L,H) );
    float dotNH = saturate( dot(N,H) ) - 0.001;
    float dotNV = saturate( dot(N,V) ) + 0.001;

    float D = calcD_GGX(roughness, dotNH);
    float F = calcF(F0, dotLH);
    float G = calcG_Schlick(roughness, dotNV, dotNL);

    return dotNL * D * F * G / 4.0;
}

float3 calcDirectionalLightDiffuseColor(float3 lightColor, float3 diffuseColor, float3 normal, float3 lightDir, float ao)
{
    float3 color = float3(0.0,0.0,0.0);

    float NoL = dot(normal,lightDir);
    color.xyz = lightColor.xyz * lightScale * max(NoL,0.0) * ao / 3.14;
    color.xyz = color.xyz * diffuseColor.xyz;
    return color;
}

#endif

fragment ShaderOutput2 main0 (ShaderInput2 i [[stage_in]], constant ShaderUniform2& u [[buffer(0)]]
//$IN_TEX$
)
{
    ShaderOutput2 o;
    float2 uv1 = i.v_UV1;
    float2 uv2 = i.v_UV2;
    float3 worldPos = i.v_WorldP;
    float3 worldNormal = i.v_WorldN;
    float3 worldTangent = i.v_WorldT;
    float3 worldBinormal = i.v_WorldB;
    float3 pixelNormalDir = worldNormal;
    float4 vcolor = i.v_VColor;
    float3 objectScale = float3(1.0, 1.0, 1.0);
    float2 screenUV = i.v_PosP.xy / i.v_PosP.w;
	float meshZ =  i.v_PosP.z / i.v_PosP.w;
    screenUV.xy = float2(screenUV.x + 1.0, screenUV.y + 1.0) * 0.5;
    float2 screenUV_distort = screenUV;
    screenUV = float2(screenUV.x, u.mUVInversedBack.z + u.mUVInversedBack.w * screenUV.y);
)";

static const char g_material_fs_src_suf2_lit[] =
    R"(

    float3 viewDir = normalize(u.cameraPosition.xyz - worldPos);
    float3 diffuse = calcDirectionalLightDiffuseColor(u.lightColor.xyz, baseColor, pixelNormalDir, u.lightDirection.xyz, ambientOcclusion);
    float3 specular = u.lightColor.xyz * lightScale * calcLightingGGX(pixelNormalDir, viewDir, u.lightDirection.xyz, roughness, 0.9);

    float4 Output =  float4(metallic * specular + (1.0 - metallic) * diffuse + baseColor * u.lightAmbientColor.xyz * ambientOcclusion, opacity);
    Output.xyz = Output.xyz + emissive.xyz;

    if(opacityMask <= 0.0) discard_fragment();
    if(opacity <= 0.0) discard_fragment();

    o.gl_FragColor = Output;
    return o;
}

)";

static const char g_material_fs_src_suf2_unlit[] =
    R"(

    if(opacityMask <= 0.0) discard_fragment();
    if(opacity <= 0.0) discard_fragment();

    o.gl_FragColor = float4(emissive, opacity);
    return o;
}

)";

static const char g_material_fs_src_suf2_refraction[] =
    R"(
    float airRefraction = 1.0;

    float3x3 tmpvar_1;
    tmpvar_1[0] = u.cameraMat[0].xyz;
    tmpvar_1[1] = u.cameraMat[1].xyz;
    tmpvar_1[2] = u.cameraMat[2].xyz;

    float3 dir = float3x3(tmpvar_1) * pixelNormalDir;
    float2 distortUV = dir.xy * (refraction - airRefraction);

    distortUV += screenUV_distort;
    distortUV = float2(distortUV.x, u.mUVInversedBack.z + u.mUVInversedBack.w * distortUV.y);
    distortUV.y = 1.0 - distortUV.y;
    float4 bg = efk_background.sample(s_efk_background, distortUV);
    o.gl_FragColor = bg;

    if(opacityMask <= 0.0) discard_fragment();
    if(opacity <= 0.0) discard_fragment();
    return o;
}
)";

static const char g_getUV_helper_vs[] =
"float2(IN.x, u.mUVInversed.x + u.mUVInversed.y * IN.y)";

static const char g_getUVBack_helper_vs[] =
"float2(IN.x, u.mUVInversed.z + u.mUVInversed.w * IN.y)";

static const char g_getUV_helper_fs[] =
"float2(IN.x, u.mUVInversedBack.x + u.mUVInversedBack.y * IN.y)";

static const char g_getUVBack_helper_fs[] =
"float2(IN.x, u.mUVInversedBack.z + u.mUVInversedBack.w * IN.y)";

/*
static const char g_getUV_helper_vs[] = R"(
    float2 OUT = IN;
    OUT.y = u.mUVInversed.x + u.mUVInversed.y * OUT.y;
)";

static const char g_getUVBack_helper_vs[] = R"(
    float2 OUT = IN;
    OUT.y = u.mUVInversed.z + u.mUVInversed.w * OUT.y;
)";

static const char g_getUV_helper_fs[] = R"(
    float2 OUT = IN;
    OUT.y = u.mUVInversedBack.x + u.mUVInversedBack.y * OUT.y;
)";

static const char g_getUVBack_helper_fs[] = R"(
    float2 OUT = IN;
    OUT.y = u.mUVInversedBack.z + u.mUVInversedBack.w * OUT.y;
)";
*/
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
        return "float2";
    if (i == 3)
        return "float3";
    if (i == 4)
        return "float4";
    if (i == 16)
        return "float4x4";
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

std::string GetUVReplacement(const std::string& varName, int stage)
{
    auto helper = (stage == 0)? g_getUV_helper_vs : g_getUV_helper_fs;
    return Replace(helper, "IN", varName);
}

std::string GetUVBackReplacement(const std::string& varName, int stage)
{
    auto helper = (stage == 0)? g_getUVBack_helper_vs : g_getUVBack_helper_fs;
    return Replace(helper, "IN", varName);
}

void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name)
{
    maincode << "  " << GetType(type) << " " << name << ";" << std::endl;
}

void ExportTexture(std::ostringstream& maincode, const char* name, int& index)
{
    maincode << ", texture2d<float> " << name << " [[texture(" << index << ")]],";
    maincode << "sampler s_" << name << " [[sampler(" << index << ")]]" << std::endl;
    index++;
}

void ExportHeader(std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite)
{
    maincode << material_common_define;

    if (stage == 0)
	{
		maincode << material_common_define_vs;
	}

    if (stage == 0)
    {
        if (isSprite)
        {
            if (materialFile->GetIsSimpleVertex())
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
    std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite, MaterialShaderType shaderType, const std::string& baseCode, const std::string& textures)
{
    std::string suf1;
    if (stage == 0)
    {
        if (isSprite)
        {
            if (materialFile->GetIsSimpleVertex())
            {
                suf1 = g_material_sprite_vs_src_suf1_simple;
            }
            else
            {
                suf1 = g_material_sprite_vs_src_suf1;
            }
        }
        else
        {
            suf1 = g_material_model_vs_src_suf1;
        }
        
        suf1 = Replace(suf1, "//$IN_TEX$", textures);
        maincode << suf1;
        
        if (materialFile->GetCustomData1Count() > 0)
        {
            maincode << GetType(materialFile->GetCustomData1Count()) + " customData1 = ";
            maincode << (isSprite? "i.atCustomData1" : "u.customData1") + GetElement(materialFile->GetCustomData1Count()) + ";\n";
            maincode << "o.v_CustomData1 = customData1" + GetElement(materialFile->GetCustomData1Count()) + ";\n";
        }

        if (materialFile->GetCustomData2Count() > 0)
        {
            maincode << GetType(materialFile->GetCustomData2Count()) + " customData2 = ";
            maincode << (isSprite? "i.atCustomData2" : "u.customData2") + GetElement(materialFile->GetCustomData2Count()) + ";\n";
            maincode << "o.v_CustomData2 = customData2" + GetElement(materialFile->GetCustomData2Count()) + ";\n";
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
        suf1 = g_material_fs_src_suf1;
        suf1 = Replace(suf1, "//$IN_TEX$", textures);
        maincode << suf1;

        if (materialFile->GetCustomData1Count() > 0)
        {
            maincode << GetType(materialFile->GetCustomData1Count()) + " customData1 = i.v_CustomData1;\n";
        }

        if (materialFile->GetCustomData2Count() > 0)
        {
            maincode << GetType(materialFile->GetCustomData2Count()) + " customData2 = i.v_CustomData2;\n";
        }

        maincode << baseCode;

        if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
        {
            maincode << g_material_fs_src_suf2_refraction;
        }
        else
        {
            if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Lit)
            {
                maincode << g_material_fs_src_suf2_lit;
            }
            else if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Unlit)
            {
                maincode << g_material_fs_src_suf2_unlit;
            }
        }
    }
}

ShaderData GenerateShader(MaterialFile* materialFile, MaterialShaderType shaderType, int32_t maximumTextureCount)
{
    bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
    bool isRefrection =
        materialFile->GetHasRefraction() && (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

    ShaderData shaderData;

    for (int stage = 0; stage < 2; stage++)
    {
        std::ostringstream maincode;

        ExportHeader(maincode, materialFile, stage, isSprite);

        std::ostringstream userUniforms;
        std::ostringstream textures;
        int t_index = 0;
        
        if (!isSprite && stage == 0)
        {
            if (materialFile->GetCustomData1Count() > 0)
            {
                ExportUniform(userUniforms, 4, "customData1");
            }
            if (materialFile->GetCustomData2Count() > 0)
            {
                ExportUniform(userUniforms, 4, "customData2");
            }
        }

        int32_t actualTextureCount = std::min(maximumTextureCount, materialFile->GetTextureCount());

        for (size_t i = 0; i < actualTextureCount; i++)
        {
            //auto textureIndex = materialFile->GetTextureIndex(i);
            auto textureName = materialFile->GetTextureName(i);

            ExportTexture(textures, textureName, t_index);
        }

        if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Lit && stage == 1)
        {
            ExportUniform(userUniforms, 4, "lightDirection");
            ExportUniform(userUniforms, 4, "lightColor");
            ExportUniform(userUniforms, 4, "lightAmbientColor");

            maincode << "#define _MATERIAL_LIT_ 1" << std::endl;
        }
        else if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
        {
        }

        if (isRefrection && stage == 1)
        {
            ExportUniform(userUniforms, 16, "cameraMat");
        }

        ExportTexture(textures, "efk_background", t_index);
		ExportTexture(textures, "efk_depth", t_index);

        for (int32_t i = 0; i < materialFile->GetUniformCount(); i++)
        {
            auto uniformName = materialFile->GetUniformName(i);

            ExportUniform(userUniforms, 4, uniformName);
        }

        auto baseCode = std::string(materialFile->GetGenericCode());
        baseCode = Replace(baseCode, "$F1$", "float");
        baseCode = Replace(baseCode, "$F2$", "float2");
        baseCode = Replace(baseCode, "$F3$", "float3");
        baseCode = Replace(baseCode, "$F4$", "float4");
        baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$EFFECTSCALE$", "predefined_uniform.y");
		baseCode = Replace(baseCode, "$UV$", "uv");
        baseCode = Replace(baseCode, "$MOD", "mod");
        
        
        // replace uniforms
        for (size_t i = 0; i < materialFile->GetUniformCount(); i++)
        {
            auto name = materialFile->GetUniformName(i);
            baseCode = Replace(baseCode, name, std::string("u.") + name);
        }
        baseCode = Replace(baseCode, "predefined_uniform", std::string("u.") + "predefined_uniform");
        baseCode = Replace(baseCode, "cameraPosition", std::string("u.") + "cameraPosition");

        // replace textures
        for (size_t i = 0; i < actualTextureCount; i++)
        {
            auto textureIndex = materialFile->GetTextureIndex(i);
            auto textureName = std::string(materialFile->GetTextureName(i));

            std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
            std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";
            
            std::size_t posP = baseCode.find(keyP);
            while (posP != std::string::npos)
            {
                std::size_t posS = baseCode.find(keyS, posP);
                if (posS == std::string::npos) break;
                
                // get var between prefix and suffix
                std::size_t varPos = posP + keyP.length();
                std::string varName = baseCode.substr(varPos, posS - varPos);

                std::ostringstream texSample;
                texSample << textureName << ".sample(s_" << textureName << ", ";
                texSample << GetUVReplacement(varName, stage) << ")";
                
                baseCode = baseCode.replace(posP, posS + keyS.length() - posP, texSample.str());
                posP = baseCode.find(keyP, posP + texSample.str().length());
            }
        }

        // invalid texture
        for (size_t i = actualTextureCount; i < materialFile->GetTextureCount(); i++)
        {
            auto textureIndex = materialFile->GetTextureIndex(i);
            auto textureName = std::string(materialFile->GetTextureName(i));

            std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
            std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

            baseCode = Replace(baseCode, keyP, "float4(");
            baseCode = Replace(baseCode, keyS, ",0.0,1.0)");
        }
        
        // Depth
        if (stage == 1)
        {
            baseCode = Replace(baseCode, "CalcDepthFade(", "ReplacedDepthFade(efk_depth, s_efk_depth, u.reconstructionParam1, u.reconstructionParam2,u.predefined_uniform.y,");
        }
        
        ExportMain(maincode, materialFile, stage, isSprite, shaderType, baseCode, textures.str());
        
        maincode.str(Replace(maincode.str(), "//$UNIFORMS$", userUniforms.str()));
        
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
    if (materialFile->GetCustomData1Count() > 0)
    {
        if (isSprite)
        {
            shaderData.CodeVS =
                Replace(shaderData.CodeVS, "//$C_IN1$", GetType(materialFile->GetCustomData1Count()) + " atCustomData1 [[attribute(6)]];");
        }
        shaderData.CodeVS =
            Replace(shaderData.CodeVS, "//$C_OUT1$", GetType(materialFile->GetCustomData1Count()) + " v_CustomData1;");
        shaderData.CodePS =
            Replace(shaderData.CodePS, "//$C_PIN1$", GetType(materialFile->GetCustomData1Count()) + " v_CustomData1;");
    }

    if (materialFile->GetCustomData2Count() > 0)
    {
        if (isSprite)
        {
            shaderData.CodeVS =
                Replace(shaderData.CodeVS, "//$C_IN2$", GetType(materialFile->GetCustomData2Count()) + " atCustomData2 [[attribute(7)]];");
        }
        shaderData.CodeVS =
            Replace(shaderData.CodeVS, "//$C_OUT2$", GetType(materialFile->GetCustomData2Count()) + " v_CustomData2;");
        shaderData.CodePS =
            Replace(shaderData.CodePS, "//$C_PIN2$", GetType(materialFile->GetCustomData2Count()) + " v_CustomData2;");
    }
    
    return shaderData;
}

} // namespace GL

} // namespace Effekseer

namespace Effekseer
{

class CompiledMaterialBinaryMetal : public CompiledMaterialBinary, public ReferenceObject
{
private:
    std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

    std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
    CompiledMaterialBinaryMetal() {}

    virtual ~CompiledMaterialBinaryMetal() {}

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

CompiledMaterialBinary* MaterialCompilerMetal::Compile(MaterialFile* materialFile, int32_t maximumTextureCount)
{
    auto binary = new CompiledMaterialBinaryMetal();
    //auto compiler = LLGI::CreateSharedPtr(new LLGI::CompilerMetal());

	auto convertToVectorVS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

        std::vector<char> buffer;

        // HACK
        buffer.reserve(7 + str.size() + 1);
        buffer.push_back('m');
        buffer.push_back('t');
        buffer.push_back('l');
        buffer.push_back('c');
        buffer.push_back('o');
        buffer.push_back('d');
        buffer.push_back('e');

        auto len = str.size() + 1;
        for (int i = 0; i < len; i++)
        {
            buffer.push_back(str[i]);
        }
        buffer[buffer.size() - 1] = 0;
        
        LLGI::CompilerResult result;
        result.Binary.resize(1);
        result.Binary[0].resize(buffer.size());
        memcpy(result.Binary[0].data(), buffer.data(), buffer.size());
		//compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Vertex);

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

	auto convertToVectorPS = [](const std::string& str) -> std::vector<uint8_t> {
		std::vector<uint8_t> ret;

        std::vector<char> buffer;

        // HACK
        buffer.reserve(7 + str.size() + 1);
        buffer.push_back('m');
        buffer.push_back('t');
        buffer.push_back('l');
        buffer.push_back('c');
        buffer.push_back('o');
        buffer.push_back('d');
        buffer.push_back('e');

        auto len = str.size() + 1;
        for (int i = 0; i < len; i++)
        {
            buffer.push_back(str[i]);
        }
        buffer[buffer.size() - 1] = 0;
        
		LLGI::CompilerResult result;
        result.Binary.resize(1);
        result.Binary[0].resize(buffer.size());
        memcpy(result.Binary[0].data(), buffer.data(), buffer.size());
        //compiler->Compile(result, str.c_str(), LLGI::ShaderStageType::Pixel);

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

    auto saveBinary = [&materialFile, &binary, &convertToVectorVS, &convertToVectorPS, &maximumTextureCount](MaterialShaderType type) {
        auto shader = Metal::GenerateShader(materialFile, type, maximumTextureCount);
        binary->SetVertexShaderData(type, convertToVectorVS(shader.CodeVS));
        binary->SetPixelShaderData(type, convertToVectorPS(shader.CodePS));
    };

    if (materialFile->GetHasRefraction())
    {
        saveBinary(MaterialShaderType::Refraction);
        saveBinary(MaterialShaderType::RefractionModel);
    }

    saveBinary(MaterialShaderType::Standard);
    saveBinary(MaterialShaderType::Model);

    return binary;
}

CompiledMaterialBinary* MaterialCompilerMetal::Compile(MaterialFile* materialFile) { return Compile(materialFile, Effekseer::UserTextureSlotMax); }

} // namespace Effekseer

#ifdef __SHARED_OBJECT__

extern "C"
{
#ifdef _WIN32
#define EFK_EXPORT __declspec(dllexport)
#else
#define EFK_EXPORT
#endif

    EFK_EXPORT Effekseer::MaterialCompiler* EFK_STDCALL CreateCompiler() { return new Effekseer::MaterialCompilerMetal(); }
}
#endif
