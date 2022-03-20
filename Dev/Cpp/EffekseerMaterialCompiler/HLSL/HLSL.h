
/**
@note
Refeence https://github.com/unitycoder/UnityBuiltinShaders/blob/master/CGIncludes/HLSLSupport.cginc
*/

#pragma once

#undef min
#undef max

namespace Effekseer
{
namespace HLSL
{

enum class ShaderType
{
	DirectX9,
	DirectX11,
	DirectX12,
	PSSL,
	XBOXONE,
};

static const char* material_gradient = R"(

struct Gradient
{
	int colorCount;
	int alphaCount;
	int reserved1;
	int reserved2;
	float4 colors[8];
	float2 alphas[8];
};

float4 SampleGradient(Gradient gradient, float t)
{
	float3 color = gradient.colors[0].xyz;
	for(int i = 1; i < 8; i++)
	{
		float a = clamp((t - gradient.colors[i-1].w) / (gradient.colors[i].w - gradient.colors[i-1].w), 0.0, 1.0) * step(float(i), float(gradient.colorCount-1));
		color = lerp(color, gradient.colors[i].xyz, a);
	}

	float alpha = gradient.alphas[0].x;
	for(int i = 1; i < 8; i++)
	{
		float a = clamp((t - gradient.alphas[i-1].y) / (gradient.alphas[i].y - gradient.alphas[i-1].y), 0.0, 1.0) * step(float(i), float(gradient.alphaCount-1));
		alpha = lerp(alpha, gradient.alphas[i].x, a);
	}

	return float4(color, alpha);
}

Gradient GradientParameter(float4 param_v, float4 param_c1, float4 param_c2, float4 param_c3, float4 param_c4, float4 param_c5, float4 param_c6, float4 param_c7, float4 param_c8, float4 param_a1, float4 param_a2, float4 param_a3, float4 param_a4)
{
	Gradient g;
	g.colorCount = int(param_v.x);
	g.alphaCount = int(param_v.y);
	g.reserved1 = int(param_v.z);
	g.reserved2 = int(param_v.w);
	g.colors[0] = param_c1;
	g.colors[1] = param_c2;
	g.colors[2] = param_c3;
	g.colors[3] = param_c4;
	g.colors[4] = param_c5;
	g.colors[5] = param_c6;
	g.colors[6] = param_c7;
	g.colors[7] = param_c8;
	g.alphas[0].xy = param_a1.xy;
	g.alphas[1].xy = param_a1.zw;
	g.alphas[2].xy = param_a2.xy;
	g.alphas[3].xy = param_a2.zw;
	g.alphas[4].xy = param_a3.xy;
	g.alphas[5].xy = param_a3.zw;
	g.alphas[6].xy = param_a4.xy;
	g.alphas[7].xy = param_a4.zw;
	return g;
}

)";

inline std::string GetFixedGradient(const char* name, const Gradient& gradient)
{
	std::stringstream ss;

	ss << "Gradient " << name << "() {" << std::endl;
	ss << "Gradient g;" << std::endl;
	ss << "g.colorCount = " << gradient.ColorCount << ";" << std::endl;
	ss << "g.alphaCount = " << gradient.AlphaCount << ";" << std::endl;
	ss << "g.reserved1 = 0;" << std::endl;
	ss << "g.reserved2 = 0;" << std::endl;

	for (int32_t i = 0; i < gradient.Colors.size(); i++)
	{
		ss << "g.colors[" << i << "].x = " << gradient.Colors[i].Color[0] * gradient.Colors[i].Intensity << ";" << std::endl;
		ss << "g.colors[" << i << "].y = " << gradient.Colors[i].Color[1] * gradient.Colors[i].Intensity << ";" << std::endl;
		ss << "g.colors[" << i << "].z = " << gradient.Colors[i].Color[2] * gradient.Colors[i].Intensity << ";" << std::endl;
		ss << "g.colors[" << i << "].w = " << gradient.Colors[i].Position << ";" << std::endl;
	}

	for (int32_t i = 0; i < gradient.Alphas.size(); i++)
	{
		ss << "g.alphas[" << i << "].x = " << gradient.Alphas[i].Alpha << ";" << std::endl;
		ss << "g.alphas[" << i << "].y = " << gradient.Alphas[i].Position << ";" << std::endl;
	}

	ss << "return g; }" << std::endl;

	return ss.str();
}

inline std::string GetMaterialCommonDefine(ShaderType type)
{
	std::stringstream ss;

	ss << R"(
#define MOD fmod
#define FRAC frac
#define LERP lerp
)";
	if (type == ShaderType::DirectX11 || type == ShaderType::DirectX12)
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

	if (type == ShaderType::DirectX9)
	{
		ss << R"(
#define POSITION0 POSITION
#define SV_POSITION POSITION
#define SV_Target COLOR
)";
	}

	if (type == ShaderType::PSSL)
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

static char* material_common_functions = R"(

#define FLT_EPSILON 1.192092896e-07f

float3 PositivePow(float3 base, float3 power)
{
	return pow(max(abs(base), float3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)), power);
}

// based on http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
half3 SRGBToLinear(half3 c)
{
	return min(c, c * (c * (c * 0.305306011 + 0.682171111) + 0.012522878));
}

half4 SRGBToLinear(half4 c)
{
	return half4(SRGBToLinear(c.rgb), c.a);
}

half3 LinearToSRGB(half3 c)
{
	return max(1.055 * PositivePow(c, 0.416666667) - 0.055, 0.0);
}

half4 LinearToSRGB(half4 c)
{
	return half4(LinearToSRGB(c.rgb), c.a);
}

half4 ConvertFromSRGBTexture(half4 c)
{
	if (predefined_uniform.z == 0.0f)
	{
		return c;
	}

	return LinearToSRGB(c);
}

half4 ConvertToScreen(half4 c)
{
	if (predefined_uniform.z == 0.0f)
	{
		return c;
	}

	return SRGBToLinear(c);
}

)";

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

inline std::string GetModelVS_Pre(ShaderType type)
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

	if (type == ShaderType::DirectX9)
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

	if (type == ShaderType::DirectX9)
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

inline std::string GetMaterialPS_Pre(ShaderType type)
{
	std::stringstream ss;

	ss << R"(
struct PS_Input
{
)";

	if (type != ShaderType::DirectX9)
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

	if (type == ShaderType::DirectX9 || type == ShaderType::DirectX11 || type == ShaderType::PSSL)
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

inline std::string GetMaterialPS_Suf1(ShaderType type)
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

	if (type == ShaderType::DirectX9)
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

inline std::string GetMaterialPS_Suf2_Refraction(ShaderType type)
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

	if (type == ShaderType::DirectX9)
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
} // namespace Effekseer