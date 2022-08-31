#include "ShaderGenerator.h"
#include "../Common/ShaderGeneratorCommon.h"

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"

#ifdef _WIN32
#undef max
#undef min
#endif

namespace Effekseer
{
namespace DirectX
{
namespace HLSL
{

static const char* material_light_vs = R"(
float3 GetLightDirection() {
	return float3(0,0,0);
}
float3 GetLightColor() {
	return float3(0,0,0);
}
float3 GetLightAmbientColor() {
	return float3(0,0,0);
}
)";

static const char* material_light_ps = R"(
float3 GetLightDirection() {
	return lightDirection.xyz;
}
float3 GetLightColor() {
	return lightColor.xyz;
}
float3 GetLightAmbientColor() {
	return lightAmbientColor.xyz;
}
)";

inline std::string GetMaterialCommonDefine(ShaderGeneratorTarget type)
{
	std::stringstream ss;

	ss << R"(
#define MOD fmod
#define FRAC frac
#define LERP lerp
)";
	if (type == ShaderGeneratorTarget::DirectX11 || type == ShaderGeneratorTarget::DirectX12)
	{
		ss << R"(
#define C_LINEAR linear
#define C_CENTROID centroid
)";
	}
	else
	{
		ss << R"(
#define C_LINEAR
#define C_CENTROID
)";
	}

	if (type == ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
#define POSITION0 POSITION
#define SV_POSITION POSITION
#define SV_Target COLOR
)";
	}

	if (type == ShaderGeneratorTarget::PSSL)
	{
		ss << R"(
#define SV_POSITION S_POSITION
#define cbuffer ConstantBuffer
#define SV_Target S_TARGET_OUTPUT
#define SampleLevel SampleLOD
#define SV_InstanceID S_INSTANCE_ID
)";
	}

	return ss.str();
}

static char* material_common_vs_functions = R"(

float2 GetUV(float2 uv)
{
	uv.y = mUVInversed.x + mUVInversed.y * uv.y;
	return uv;
}

float2 GetUVBack(float2 uv)
{
	uv.y = mUVInversed.z + mUVInversed.w * uv.y;
	return uv;
}

// Dummy
float CalcDepthFade(float2 screenUV, float meshZ, float softParticleParam) { return 1.0f; }

)";

static char* material_sprite_vs_pre_simple = R"(

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float2 UV		: TEXCOORD0;
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	C_LINEAR C_CENTROID float4 VColor		: COLOR;
	C_LINEAR C_CENTROID float2 UV1		: TEXCOORD0;
	C_LINEAR C_CENTROID float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float4 PosP : TEXCOORD6;
	//float2 ScreenUV : TEXCOORD6;
};

cbuffer VSConstantBuffer : register(b0) {

float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);
float4 predefined_uniform : register(c9);
float4 cameraPosition : register(c10);

)";

static char* material_sprite_vs_pre = R"(

struct VS_Input
{
	float3 Pos		: POSITION0;
	float4 Color		: NORMAL0;
	float4 Normal		: NORMAL1;
	float4 Tangent		: NORMAL2;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	//$C_IN1$
	//$C_IN2$
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	C_LINEAR C_CENTROID float4 VColor		: COLOR;
	C_LINEAR C_CENTROID float2 UV1		: TEXCOORD0;
	C_LINEAR C_CENTROID float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float4 PosP : TEXCOORD6;
	//float2 ScreenUV : TEXCOORD6;
	//$C_OUT1$
	//$C_OUT2$
};

cbuffer VSConstantBuffer : register(b0) {

float4x4 mCamera		: register(c0);
float4x4 mProj			: register(c4);
float4 mUVInversed		: register(c8);
float4 predefined_uniform : register(c9);
float4 cameraPosition : register(c10);

)";

static char* material_sprite_vs_suf1_simple = R"(


VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float4 worldPos = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 };
	float3 worldNormal = float3(0.0, 0.0, 0.0);
	float3 worldBinormal = float3(0.0, 0.0, 0.0);
	float3 worldTangent = float3(0.0, 0.0, 0.0);
	float3 objectScale = float3(1.0, 1.0, 1.0);

	// UV
	float uv1 = Input.UV;
	float uv2 = Input.UV;
	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	//uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = worldNormal;
	float4 vcolor = Input.Color;

	// Dummy
	float2 screenUV = float2(0.0, 0.0);
	float meshZ =  0.0f;
)";

static char* material_sprite_vs_suf1 = R"(

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	float3 worldPos = Input.Pos;
	float3 worldNormal = (Input.Normal - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldTangent = (Input.Tangent - float3(0.5, 0.5, 0.5)) * 2.0;
	float3 worldBinormal = cross(worldNormal, worldTangent);
	float3 objectScale = float3(1.0, 1.0, 1.0);

	// UV
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV2;
	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	//uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	float3 pixelNormalDir = worldNormal;
	float4 vcolor = Input.Color;

	// Dummy
	float2 screenUV = float2(0.0, 0.0);
	float meshZ =  0.0f;
)";

static char* material_sprite_vs_suf2 = R"(

	worldPos = worldPos + worldPositionOffset;

	float4 cameraPos = mul(mCamera, float4(worldPos, 1.0));
	cameraPos = cameraPos / cameraPos.w;
	Output.Position = mul(mProj, cameraPos);

	Output.WorldP = worldPos;
	Output.VColor = Input.Color;
	Output.UV1 = uv1;
	Output.UV2 = uv2;

	Output.PosP = Output.Position;
	//Output.ScreenUV = Output.Position.xy / Output.Position.w;
	//Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

	return Output;
}

)";

inline std::string GetModelVS_Pre(ShaderGeneratorTarget type)
{
	std::stringstream ss;

	ss << R"(
struct VS_Input
{
	float3 Pos		: POSITION0;
	float3 Normal		: NORMAL0;
	float3 Binormal		: NORMAL1;
	float3 Tangent		: NORMAL2;
	float2 UV		: TEXCOORD0;
	float4 Color		: NORMAL3;
)";

	if (type == ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
	float Index : BLENDINDICES0;
)";
	}
	else
	{
		ss << R"(
	uint Index : SV_InstanceID;
)";
	}

	ss << R"(
};

struct VS_Output
{
	float4 Position		: SV_POSITION;
	C_LINEAR C_CENTROID float4 VColor		: COLOR;
	C_LINEAR C_CENTROID float2 UV1		: TEXCOORD0;
	C_LINEAR C_CENTROID float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float4 PosP : TEXCOORD6;
	//float2 ScreenUV : TEXCOORD6;
	//$C_OUT1$
	//$C_OUT2$
};

cbuffer VSConstantBuffer : register(b0) {
)";

	if (type == ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[10]		: register( c4 );
float4	fUV[10]			: register( c44 );
float4	fModelColor[10]		: register( c54 );

float4 mUVInversed		: register(c64);
float4 predefined_uniform : register(c65);
float4 cameraPosition : register(c66);
)";
	}
	else
	{
		ss << R"(
float4x4 mCameraProj		: register( c0 );
float4x4 mModel[40]		: register( c4 );
float4	fUV[40]			: register( c164 );
float4	fModelColor[40]		: register( c204 );

float4 mUVInversed		: register(c244);
float4 predefined_uniform : register(c245);
float4 cameraPosition : register(c246);
)";
	}

	ss << R"(
// custom1
// custom2
)";
	return ss.str();
}

static char* model_vs_suf1 = R"(

VS_Output main( const VS_Input Input )
{
	float4x4 matModel = mModel[Input.Index];
	float4 uv = fUV[Input.Index];
	float4 modelColor = fModelColor[Input.Index] * Input.Color;

	VS_Output Output = (VS_Output)0;
	float4 localPosition = { Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0 }; 

	float3x3 matRotModel = (float3x3)matModel;

	float3 worldPos = mul( matModel, localPosition ).xyz;
	float3 worldNormal = normalize( mul( matRotModel, Input.Normal ) );
	float3 worldBinormal = normalize( mul( matRotModel, Input.Binormal ) );
	float3 worldTangent = normalize( mul( matRotModel, Input.Tangent ) );
	float3 objectScale = float3(1.0, 1.0, 1.0);

	// Calculate ObjectScale
	objectScale.x = length(mul(matRotModel, float3(1.0, 0.0, 0.0)));
	objectScale.y = length(mul(matRotModel, float3(0.0, 1.0, 0.0)));
	objectScale.z = length(mul(matRotModel, float3(0.0, 0.0, 1.0)));

	float2 uv1;
	uv1.x = Input.UV.x * uv.z + uv.x;
	uv1.y = Input.UV.y * uv.w + uv.y;
	float2 uv2 = Input.UV;

	//uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	//uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	float3 pixelNormalDir = worldNormal;
	float4 vcolor = modelColor;

	// Dummy
	float2 screenUV = float2(0.0, 0.0);
	float meshZ =  0.0f;
)";

static char* model_vs_suf2 = R"(

	worldPos = worldPos + worldPositionOffset;

	Output.Position = mul( mCameraProj,  float4(worldPos, 1.0) );

	Output.WorldP = worldPos;
	Output.WorldN = worldNormal;
	Output.WorldB = worldBinormal;
	Output.WorldT = worldTangent;

	Output.VColor = modelColor;
	Output.UV1 = uv1;
	Output.UV2 = uv2;

	Output.PosP = Output.Position;
	//Output.ScreenUV = Output.Position.xy / Output.Position.w;
	//Output.ScreenUV.xy = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;

	return Output;
}

)";

inline std::string GetMaterialPS_Pre(ShaderGeneratorTarget type)
{
	std::stringstream ss;

	ss << R"(
struct PS_Input
{
)";

	if (type != ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
	float4 Position		: SV_POSITION;
)";
	}

	ss << R"(
	C_LINEAR C_CENTROID float4 VColor		: COLOR;
	C_LINEAR C_CENTROID float2 UV1		: TEXCOORD0;
	C_LINEAR C_CENTROID float2 UV2		: TEXCOORD1;
	float3 WorldP	: TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float4 PosP : TEXCOORD6;
	//float2 ScreenUV : TEXCOORD6;
	//$C_PIN1$
	//$C_PIN2$
};
)";

	if (type == ShaderGeneratorTarget::DirectX9 || type == ShaderGeneratorTarget::DirectX11 || type == ShaderGeneratorTarget::PSSL)
	{
		ss << R"(
cbuffer PSConstantBuffer : register(b0) {
)";
	}
	else
	{
		ss << R"(
cbuffer PSConstantBuffer : register(b1) {
)";
	}

	return ss.str();
}

inline std::string GetMaterialPS_Suf1(ShaderGeneratorTarget type)
{
	std::stringstream ss;

	ss << R"(

float2 GetUV(float2 uv)
{
	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;
	return uv;
}

float2 GetUVBack(float2 uv)
{
	uv.y = mUVInversedBack.z + mUVInversedBack.w * uv.y;
	return uv;
}

float CalcDepthFade(float2 screenUV, float meshZ, float softParticleParam)
{
)";

	if (type == ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
	float backgroundZ = tex2D(efk_depth_sampler, GetUVBack(screenUV)).x;
)";
	}
	else
	{
		ss << R"(
	float backgroundZ = efk_depth_texture.Sample(efk_depth_sampler, GetUVBack(screenUV)).x;
)";
	}

	ss << R"(
	float distance = softParticleParam * predefined_uniform.y;
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

float3 calcDirectionalLightDiffuseColor(float3 diffuseColor, float3 normal, float3 lightDir, float ao)
{
	float3 color = float3(0.0,0.0,0.0);

	float NoL = dot(normal,lightDir);
	color.xyz = lightColor.xyz * lightScale * max(NoL,0.0) * ao / 3.14;
	color.xyz = color.xyz * diffuseColor.xyz;
	return color;
}

#endif

float4 main( const PS_Input Input ) : SV_Target
{
	float2 uv1 = Input.UV1;
	float2 uv2 = Input.UV2;
	float3 worldPos = Input.WorldP;
	float3 worldNormal = Input.WorldN;
	float3 worldBinormal = Input.WorldB;
	float3 worldTangent = Input.WorldT;
	float3 objectScale = float3(1.0, 1.0, 1.0);

	float3 pixelNormalDir = worldNormal;
	float4 vcolor = Input.VColor;

	float2 screenUV = Input.PosP.xy / Input.PosP.w;
	float meshZ =  Input.PosP.z / Input.PosP.w;
	screenUV.xy = float2(screenUV.x + 1.0, 1.0 - screenUV.y) * 0.5;
)";

	return ss.str();
}

static char* g_material_ps_suf2_unlit = R"(

	float4 Output = float4(emissive, opacity);

	if(opacityMask <= 0.0f) discard;
	if(opacity <= 0.0) discard;

	return ConvertToScreen(Output);
}

)";

static char* g_material_ps_suf2_lit = R"(
	float3 viewDir = normalize(cameraPosition.xyz - worldPos);
	float3 diffuse = calcDirectionalLightDiffuseColor(baseColor, pixelNormalDir, lightDirection.xyz, ambientOcclusion);
	float3 specular = lightColor.xyz * lightScale * calcLightingGGX(pixelNormalDir, viewDir, lightDirection.xyz, roughness, 0.9);

	float4 Output =  float4(metallic * specular + (1.0 - metallic) * diffuse + baseColor * lightAmbientColor.xyz * ambientOcclusion, opacity);
	Output.xyz = Output.xyz + emissive.xyz;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	return ConvertToScreen(Output);
}

)";

inline std::string GetMaterialPS_Suf2_Refraction(ShaderGeneratorTarget type)
{
	std::stringstream ss;

	ss << R"(

	float airRefraction = 1.0;
	float3 dir = mul((float3x3)cameraMat, pixelNormalDir);
	dir.y = -dir.y;

	float2 distortUV = 	dir.xy * (refraction - airRefraction);

	distortUV += screenUV;
	distortUV = GetUVBack(distortUV);	

)";

	if (type == ShaderGeneratorTarget::DirectX9)
	{
		ss << R"(
	float4 bg = tex2D(efk_background_sampler, distortUV);
)";
	}
	else
	{
		ss << R"(
	float4 bg = efk_background_texture.Sample(efk_background_sampler, distortUV);
)";
	}

	ss << R"(
	float4 Output = bg;

	if(opacityMask <= 0.0) discard;
	if(opacity <= 0.0) discard;

	return Output;
}

)";

	return ss.str();
}

} // namespace HLSL

std::string ShaderGenerator::Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

std::string ShaderGenerator::GetType(int32_t i)
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

std::string ShaderGenerator::GetElement(int32_t i)
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

void ShaderGenerator::ExportUniform(std::ostringstream& maincode, int32_t type, const char* name, int32_t registerId)
{
	maincode << GetType(type) << " " << name << " : register(c" << registerId << ");" << std::endl;
}

void ShaderGenerator::ExportTexture(std::ostringstream& maincode, const char* name, int32_t registerId)
{
	maincode << "Texture2D " << name << "_texture : register(t" << registerId << ");" << std::endl;

	if (target_ == ShaderGeneratorTarget::DirectX9)
	{
		maincode << "sampler2D " << name << "_sampler : register(s" << registerId << ");" << std::endl;
	}
	else
	{
		maincode << "SamplerState " << name << "_sampler : register(s" << registerId << ");" << std::endl;
	}
}

int32_t ShaderGenerator::ExportHeader(std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite, int instanceCount)
{
	auto cind = 0;

	maincode << common_define_;

	// gradient
	bool hasGradient = false;
	bool hasNoise = false;

	for (const auto& type : materialFile->RequiredMethods)
	{
		if (type == MaterialFile::RequiredPredefinedMethodType::Gradient)
		{
			hasGradient = true;
		}
		else if (type == MaterialFile::RequiredPredefinedMethodType::Noise)
		{
			hasNoise = true;
		}
	}

	if (hasGradient)
	{
		maincode << Effekseer::Shader::GetGradientFunctions();
	}

	if (hasNoise)
	{
		maincode << Effekseer::Shader::GetNoiseFunctions();
	}

	for (const auto& gradient : materialFile->FixedGradients)
	{
		maincode << Effekseer::Shader::GetFixedGradient(gradient.Name.c_str(), gradient.Data);
	}

	if (stage == 0)
	{
		if (isSprite)
		{
			if (materialFile->GetIsSimpleVertex())
			{
				maincode << sprite_vs_pre_simple_;
			}
			else
			{
				maincode << sprite_vs_pre_;
			}
			cind = 11;
		}
		else
		{
			maincode << model_vs_pre_;
			cind = 7 + instanceCount * 6;
		}
	}
	else
	{
		maincode << ps_pre_;
		cind = 2;
	}

	return cind;
}

void ShaderGenerator::ExportMain(std::ostringstream& maincode,
								 MaterialFile* materialFile,
								 int stage,
								 bool isSprite,
								 MaterialShaderType shaderType,
								 const std::string& baseCode)
{
	if (stage == 0)
	{
		if (isSprite)
		{
			if (materialFile->GetIsSimpleVertex())
			{
				maincode << sprite_vs_suf1_simple_;
			}
			else
			{
				maincode << sprite_vs_suf1_;
			}
		}
		else
		{
			maincode << model_vs_suf1_;
		}

		if (materialFile->GetCustomData1Count() > 0)
		{
			if (isSprite)
			{
				maincode << GetType(materialFile->GetCustomData1Count()) + " customData1 = Input.CustomData1;\n";
			}
			else
			{
				maincode << GetType(materialFile->GetCustomData1Count()) + " customData1 = customData1_[Input.Index];\n";
			}
			maincode << "Output.CustomData1 = customData1" + GetElement(materialFile->GetCustomData1Count()) + ";\n";
		}

		if (materialFile->GetCustomData2Count() > 0)
		{
			if (isSprite)
			{
				maincode << GetType(materialFile->GetCustomData2Count()) + " customData2 = Input.CustomData2;\n";
			}
			else
			{
				maincode << GetType(materialFile->GetCustomData2Count()) + " customData2 = customData2_[Input.Index];\n";
			}
			maincode << "Output.CustomData2 = customData2" + GetElement(materialFile->GetCustomData2Count()) + ";\n";
		}
	}
	else
	{
		maincode << ps_suf1_;

		if (materialFile->GetCustomData1Count() > 0)
		{
			maincode << GetType(materialFile->GetCustomData1Count()) + " customData1 = Input.CustomData1;\n";
		}

		if (materialFile->GetCustomData2Count() > 0)
		{
			maincode << GetType(materialFile->GetCustomData2Count()) + " customData2 = Input.CustomData2;\n";
		}
	}

	maincode << baseCode;

	if (stage == 0)
	{
		if (isSprite)
		{
			maincode << sprite_vs_suf2_;
		}
		else
		{
			maincode << model_vs_suf2_;
		}
	}
	else
	{
		if (shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel)
		{
			maincode << ps_suf2_refraction_;
		}
		else
		{
			if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Lit)
			{
				maincode << ps_suf2_lit_;
			}
			else if (materialFile->GetShadingModel() == Effekseer::ShadingModelType::Unlit)
			{
				maincode << ps_suf2_unlit_;
			}
			else
			{
				assert(0);
			}
		}
	}
}

ShaderGenerator::ShaderGenerator(
	ShaderGeneratorTarget target)
	: common_define_(HLSL::GetMaterialCommonDefine(target).c_str())
	, common_vs_define_(HLSL::material_common_vs_functions)
	, sprite_vs_pre_(HLSL::material_sprite_vs_pre)
	, sprite_vs_pre_simple_(HLSL::material_sprite_vs_pre_simple)
	, model_vs_pre_(HLSL::GetModelVS_Pre(target).c_str())
	, sprite_vs_suf1_(HLSL::material_sprite_vs_suf1)
	, sprite_vs_suf1_simple_(HLSL::material_sprite_vs_suf1_simple)
	, model_vs_suf1_(HLSL::model_vs_suf1)
	, sprite_vs_suf2_(HLSL::material_sprite_vs_suf2)
	, model_vs_suf2_(HLSL::model_vs_suf2)
	, ps_pre_(HLSL::GetMaterialPS_Pre(target).c_str())
	, ps_suf1_(HLSL::GetMaterialPS_Suf1(target).c_str())
	, ps_suf2_lit_(HLSL::g_material_ps_suf2_lit)
	, ps_suf2_unlit_(HLSL::g_material_ps_suf2_unlit)
	, ps_suf2_refraction_(HLSL::GetMaterialPS_Suf2_Refraction(target).c_str())
	, target_(target)
{
}

ShaderData ShaderGenerator::GenerateShader(MaterialFile* materialFile,
										   MaterialShaderType shaderType,
										   int32_t maximumUniformCount,
										   int32_t maximumTextureCount,
										   int32_t pixelShaderTextureSlotOffset,
										   int32_t instanceCount)
{
	ShaderData shaderData;

	bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
	bool isRefrection = materialFile->GetHasRefraction() &&
						(shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

	for (int stage = 0; stage < 2; stage++)
	{
		std::ostringstream maincode;

		auto cind = ExportHeader(maincode, materialFile, stage, isSprite, instanceCount);

		if (stage == 1)
		{
			ExportUniform(maincode, 4, "mUVInversedBack", 0);
			ExportUniform(maincode, 4, "predefined_uniform", 1);
			ExportUniform(maincode, 4, "cameraPosition", cind + 0);
			ExportUniform(maincode, 4, "reconstructionParam1", cind + 1);
			ExportUniform(maincode, 4, "reconstructionParam2", cind + 2);
			cind += 3;
		}

		if (stage == 1)
		{
			ExportUniform(maincode, 4, "lightDirection", cind);
			cind++;
			ExportUniform(maincode, 4, "lightColor", cind);
			cind++;
			ExportUniform(maincode, 4, "lightAmbientColor", cind);
			cind++;
		}

		if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Lit && stage == 1)
		{
			maincode << "#define _MATERIAL_LIT_ 1" << std::endl;
		}
		else if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		if (isRefrection && stage == 1)
		{
			ExportUniform(maincode, 16, "cameraMat", cind);
			cind += 4;
		}

		if (!isSprite && stage == 0)
		{
			if (materialFile->GetCustomData1Count() > 0)
			{
				maincode << "float4 customData1_[" << instanceCount << "]"
						 << " : register(c" << cind << ");" << std::endl;
				cind += instanceCount;
			}
			if (materialFile->GetCustomData2Count() > 0)
			{
				maincode << "float4 customData2_[" << instanceCount << "]"
						 << " : register(c" << cind << ");" << std::endl;
				cind += instanceCount;
			}
		}

		int32_t actualUniformCount = std::min(maximumUniformCount, materialFile->GetUniformCount());

		for (int32_t i = 0; i < actualUniformCount; i++)
		{
			ExportUniform(maincode, 4, materialFile->GetUniformName(i), cind);
			cind++;
		}

		for (size_t i = 0; i < materialFile->Gradients.size(); i++)
		{
			// TODO : remove a magic number
			for (size_t j = 0; j < 13; j++)
			{
				ExportUniform(maincode, 4, (materialFile->Gradients[i].Name + "_" + std::to_string(j)).c_str(), cind);
				cind++;
			}
		}

		// finish constant buffer
		maincode << "};" << std::endl;

		for (int32_t i = actualUniformCount; i < materialFile->GetUniformCount(); i++)
		{
			maincode << "const " << GetType(4) << " " << materialFile->GetUniformName(i) << " = float4(0,0,0,0);" << std::endl;
		}

		int32_t textureSlotOffset = 0;

		if (stage == 1)
		{
			textureSlotOffset = pixelShaderTextureSlotOffset;
		}

		int32_t actualTextureCount = std::min(maximumTextureCount, materialFile->GetTextureCount());

		for (int32_t i = 0; i < actualTextureCount; i++)
		{
			ExportTexture(maincode, materialFile->GetTextureName(i), i + textureSlotOffset);
		}

		textureSlotOffset += actualTextureCount;

		// background
		ExportTexture(maincode, "efk_background", 0 + textureSlotOffset);

		// depth
		ExportTexture(maincode, "efk_depth", 1 + textureSlotOffset);

		if (std::find(materialFile->RequiredMethods.begin(), materialFile->RequiredMethods.end(), MaterialFile::RequiredPredefinedMethodType::Light) != materialFile->RequiredMethods.end())
		{
			if (stage == 0)
			{
				maincode << HLSL::material_light_vs;
			}
			else
			{
				maincode << HLSL::material_light_ps;
			}
		}

		auto baseCode = std::string(materialFile->GetGenericCode());
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "float2");
		baseCode = Replace(baseCode, "$F3$", "float3");
		baseCode = Replace(baseCode, "$F4$", "float4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$EFFECTSCALE$", "predefined_uniform.y");
		baseCode = Replace(baseCode, "$LOCALTIME$", "predefined_uniform.w");
		baseCode = Replace(baseCode, "$UV$", "uv");
		baseCode = Replace(baseCode, "$MOD", "fmod");

		// replace textures
		for (int32_t i = 0; i < actualTextureCount; i++)
		{

			std::string prefix;
			std::string suffix;

			if (materialFile->GetTextureColorType(i) == Effekseer::TextureColorType::Color)
			{
				prefix = "ConvertFromSRGBTexture(";
				suffix = ")";
			}

			std::string keyP = "$TEX_P" + std::to_string(materialFile->GetTextureIndex(i)) + "$";
			std::string keyS = "$TEX_S" + std::to_string(materialFile->GetTextureIndex(i)) + "$";

			if (target_ == ShaderGeneratorTarget::DirectX9)
			{
				if (stage == 0)
				{
					baseCode = Replace(baseCode, keyP, prefix + std::string("tex2Dlod(") + materialFile->GetTextureName(i) + "_sampler,float4(GetUV(");
					baseCode = Replace(baseCode, keyS, "),0,1))" + suffix);
				}
				else
				{
					baseCode = Replace(baseCode, keyP, prefix + std::string("tex2D(") + materialFile->GetTextureName(i) + "_sampler,GetUV(");
					baseCode = Replace(baseCode, keyS, "))" + suffix);
				}
			}
			else
			{
				if (stage == 0)
				{
					baseCode = Replace(baseCode,
									   keyP,
									   prefix + std::string(materialFile->GetTextureName(i)) + "_texture.SampleLevel(" +
										   materialFile->GetTextureName(i) + "_sampler,GetUV(");
					baseCode = Replace(baseCode, keyS, "),0)" + suffix);
				}
				else
				{
					baseCode = Replace(baseCode,
									   keyP,
									   prefix + std::string(materialFile->GetTextureName(i)) + "_texture.Sample(" + materialFile->GetTextureName(i) +
										   "_sampler,GetUV(");
					baseCode = Replace(baseCode, keyS, "))" + suffix);
				}
			}
		}

		// invalid texture
		for (int32_t i = actualTextureCount; i < materialFile->GetTextureCount(); i++)
		{
			auto textureIndex = materialFile->GetTextureIndex(i);
			auto textureName = std::string(materialFile->GetTextureName(i));

			std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
			std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

			baseCode = Replace(baseCode, keyP, "float4(");
			baseCode = Replace(baseCode, keyS, ",0.0,1.0)");
		}

		maincode << Effekseer::Shader::GetLinearGammaFunctions();

		if (stage == 0)
		{
			maincode << common_vs_define_;
		}

		ExportMain(maincode, materialFile, stage, isSprite, shaderType, baseCode);

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
	int32_t inputSlot = 2;
	int32_t outputSlot = 7;
	if (materialFile->GetCustomData1Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS,
						"//$C_IN1$",
						GetType(materialFile->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(inputSlot) + ";");
		}

		shaderData.CodeVS =
			Replace(shaderData.CodeVS,
					"//$C_OUT1$",
					GetType(materialFile->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(outputSlot) + ";");
		shaderData.CodePS =
			Replace(shaderData.CodePS,
					"//$C_PIN1$",
					GetType(materialFile->GetCustomData1Count()) + " CustomData1 : TEXCOORD" + std::to_string(outputSlot) + ";");

		inputSlot++;
		outputSlot++;
	}

	if (materialFile->GetCustomData2Count() > 0)
	{
		if (isSprite)
		{
			shaderData.CodeVS =
				Replace(shaderData.CodeVS,
						"//$C_IN2$",
						GetType(materialFile->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(inputSlot) + ";");
		}
		shaderData.CodeVS =
			Replace(shaderData.CodeVS,
					"//$C_OUT2$",
					GetType(materialFile->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(outputSlot) + ";");
		shaderData.CodePS =
			Replace(shaderData.CodePS,
					"//$C_PIN2$",
					GetType(materialFile->GetCustomData2Count()) + " CustomData2 : TEXCOORD" + std::to_string(outputSlot) + ";");
	}

	return shaderData;
}

} // namespace DirectX

} // namespace Effekseer