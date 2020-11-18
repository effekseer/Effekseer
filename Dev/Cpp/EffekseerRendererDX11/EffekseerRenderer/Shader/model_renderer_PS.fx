
struct FalloffParameter
{
	float4 Param; // x:enable, y:colorblendtype, z:pow
	float4 BeginColor;
	float4 EndColor;
};

cbuffer PS_ConstanBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	float4 fFlipbookParameter; // x:enable, y:interpolationType

	float4 fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

	float4 fBlendTextureParameter; // x:blendType

	float4 fCameraFrontDirection;

	FalloffParameter fFalloffParam;

	float4 fEmissiveScaling; // x:emissiveScaling

	float4 fEdgeColor;
	float4 fEdgeParameter; // x:threshold, y:colorScaling

	// which is used for only softparticle
	float4 softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	float4 reconstructionParam2;
};

#ifdef ENABLE_LIGHTING

Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

Texture2D g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);

Texture2D g_alphaTexture : register(t2);
SamplerState g_alphaSampler : register(s2);

Texture2D g_uvDistortionTexture : register(t3);
SamplerState g_uvDistortionSampler : register(s3);

Texture2D g_blendTexture : register(t4);
SamplerState g_blendSampler : register(s4);

Texture2D g_blendAlphaTexture : register(t5);
SamplerState g_blendAlphaSampler : register(s5);

Texture2D g_blendUVDistortionTexture : register(t6);
SamplerState g_blendUVDistortionSampler : register(s6);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D g_depthTexture : register(t7);
SamplerState g_depthSampler : register(s7);
#endif

#else

Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

Texture2D g_alphaTexture : register(t1);
SamplerState g_alphaSampler : register(s1);

Texture2D g_uvDistortionTexture : register(t2);
SamplerState g_uvDistortionSampler : register(s2);

Texture2D g_blendTexture : register(t3);
SamplerState g_blendSampler : register(s3);

Texture2D g_blendAlphaTexture : register(t4);
SamplerState g_blendAlphaSampler : register(s4);

Texture2D g_blendUVDistortionTexture : register(t5);
SamplerState g_blendUVDistortionSampler : register(s5);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D g_depthTexture : register(t6);
SamplerState g_depthSampler : register(s6);
#endif


#endif

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;

	float3 Normal : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	linear centroid float4 Color : COLOR;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;

#ifndef DISABLED_SOFT_PARTICLE
	float4 PosP : TEXCOORD8;
#endif
};

#include "renderer_common_PS.fx"
#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

	float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, advancedParam.UVDistortionUV, fUVDistortionParameter.zw);
	UVOffset *= fUVDistortionParameter.x;

	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV + UVOffset) * Input.Color;

#if ENABLE_LIGHTING
	half3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV + UVOffset).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
			texNormal,
			half3x3((half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal)));
#endif

	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);

	// apply alpha texture
	float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
	Output.a *= AlphaTexColor.r * AlphaTexColor.a;

	// blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV, fUVDistortionParameter.zw);
	BlendUVOffset *= fUVDistortionParameter.y;

	float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
	BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;

	ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);

#if ENABLE_LIGHTING
	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	// apply falloff
	if (fFalloffParam.Param.x == 1)
	{
		float3 cameraVec = normalize(-fCameraFrontDirection.xyz);
#if ENABLE_LIGHTING
		float CdotN = saturate(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)));
#else
		float CdotN = saturate(dot(cameraVec, normalize(Input.Normal)));
#endif
		float4 FalloffBlendColor = lerp(fFalloffParam.EndColor, fFalloffParam.BeginColor, pow(CdotN, fFalloffParam.Param.z));

		if (fFalloffParam.Param.y == 0) // add
		{
			Output.rgb += FalloffBlendColor.rgb;
		}
		else if (fFalloffParam.Param.y == 1) // sub
		{
			Output.rgb -= FalloffBlendColor.rgb;
		}
		else if (fFalloffParam.Param.y == 2) // mul
		{
			Output.rgb *= FalloffBlendColor.rgb;
		}

		Output.a *= FalloffBlendColor.a;
	}

	Output.rgb *= fEmissiveScaling.x;

#ifndef DISABLED_SOFT_PARTICLE
	// softparticle
	float4 screenPos = Input.PosP / Input.PosP.w;
	float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
	screenUV.y = 1.0f - screenUV.y;

#ifdef __OPENGL__
	screenUV.y = 1.0 - screenUV.y;
#endif

	float backgroundZ = g_depthTexture.Sample(g_depthSampler, screenUV).x;
	if (softParticleAndReconstructionParam1.x != 0.0f)
	{
		Output.a *= SoftParticle(
			backgroundZ,
			screenPos.z,
			softParticleAndReconstructionParam1.x,
			softParticleAndReconstructionParam1.yz,
			reconstructionParam2);
	}
#endif

	// zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
	{
		discard;
	}

	Output.rgb = lerp(
		fEdgeColor.rgb * fEdgeParameter.y,
		Output.rgb,
		ceil((Output.a - advancedParam.AlphaThreshold) - fEdgeParameter.x));

	return Output;
}
