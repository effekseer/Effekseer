
cbuffer PS_ConstanBuffer : register(b0)
{
	float4 fLightDirection;
	float4 fLightColor;
	float4 fLightAmbient;

	float4 fFlipbookParameter; // x:enable, y:interpolationType

	float4 fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

	float4 fBlendTextureParameter; // x:blendType

	float4 fCameraFrontDirection;

	float4 fFalloffParameter; // x:enable, y:colorblendtype, z:pow
	float4 fFalloffBeginColor;
	float4 fFalloffEndColor;

	float4 fEmissiveScaling; // x:emissiveScaling

	float4 fEdgeColor;
	float4 fEdgeParameter; // x:threshold, y:colorScaling

	// which is used for only softparticle
	float4 softParticleParam;
	float4 reconstructionParam1;
	float4 reconstructionParam2;
	float4 mUVInversedBack;
};

Texture2D _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);

#if ENABLE_LIGHTING

Texture2D _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);
#endif

#else

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t1);
SamplerState sampler_depthTex : register(s1);
#endif

#endif


struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
#if ENABLE_LIGHTING
	half3 WorldN : TEXCOORD1;
	half3 WorldB : TEXCOORD2;
	half3 WorldT : TEXCOORD3;
#endif
	float4 PosP : TEXCOORD4;
};

#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV) * Input.Color;

#if ENABLE_LIGHTING
	half3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
			texNormal,
			half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	Output.rgb *= fEmissiveScaling.x;

#ifndef DISABLED_SOFT_PARTICLE
	// softparticle
	float4 screenPos = Input.PosP / Input.PosP.w;
	float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
	screenUV.y = 1.0f - screenUV.y;

#ifdef __OPENGL__
	screenUV.y = 1.0 - screenUV.y;
#endif

	screenUV.y = mUVInversedBack.x + mUVInversedBack.y * screenUV.y;

	if (softParticleParam.w != 0.0f)
	{
		float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
		Output.a *= SoftParticle(
			backgroundZ,
			screenPos.z,
			softParticleParam,
			reconstructionParam1,
			reconstructionParam2);
	}
#endif


	if (Output.a == 0.0)
		discard;

	return Output;
}
