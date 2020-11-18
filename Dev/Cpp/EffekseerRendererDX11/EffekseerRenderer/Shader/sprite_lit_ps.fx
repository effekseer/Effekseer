struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 VColor : COLOR;
	linear centroid float2 UV1 : TEXCOORD0;
	linear centroid float2 UV2 : TEXCOORD1;
	float3 WorldP : TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float2 ScreenUV : TEXCOORD6;
	float4 PosP : TEXCOORD7;
};

cbuffer VS_ConstantBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	// Unused
	float4 fFlipbookParameter; // x:enable, y:interpolationType

	float4 fUVDistortionParameter; // x:intensity, y: blendIntensity, zw:uvInversed
	float4 fBlendTextureParameter; // x:blendType

	float4 fEmissiveScaling; // x:emissiveScaling

	float4 fEdgeColor;
	float4 fEdgeParameter; // x:threshold, y:colorScaling

	// which is used for only softparticle
	float4 softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	float4 reconstructionParam2;
}

Texture2D g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

Texture2D g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D g_depthTexture : register(t2);
SamplerState g_depthSampler : register(s2);
#endif

#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	half3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV1).xyz;
	half3 texNormal = (loN - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV1) * Input.VColor;
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient);

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
