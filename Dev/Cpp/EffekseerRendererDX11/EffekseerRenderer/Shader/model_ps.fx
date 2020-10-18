
cbuffer VS_ConstantBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	// which is used for only softparticle
	float4 softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	float4 reconstructionParam2;
}

#ifdef ENABLE_COLOR_TEXTURE
Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);
#endif

#ifdef ENABLE_NORMAL_TEXTURE
Texture2D g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);
#endif

#if ENABLE_LIGHTING

#ifndef DISABLED_SOFT_PARTICLE
Texture2D g_depthTexture : register(t2);
SamplerState g_depthSampler : register(s2);
#endif

#else

#ifndef DISABLED_SOFT_PARTICLE
Texture2D g_depthTexture : register(t1);
SamplerState g_depthSampler : register(s1);
#endif

#endif


struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float2 UV : TEXCOORD0;
#if ENABLE_NORMAL_TEXTURE
	half3 Normal : TEXCOORD1;
	half3 Binormal : TEXCOORD2;
	half3 Tangent : TEXCOORD3;
#endif
	linear centroid float4 Color : COLOR;
	float4 PosP : TEXCOORD4;
};

#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV) * Input.Color;

#if ENABLE_LIGHTING && ENABLE_NORMAL_TEXTURE
	half3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
			texNormal,
			half3x3((half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

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


	if (Output.a == 0.0)
		discard;

	return Output;
}
