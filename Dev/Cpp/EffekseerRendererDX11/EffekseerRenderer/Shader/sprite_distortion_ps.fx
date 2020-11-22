
Texture2D _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);

Texture2D _backTex : register(t1);
SamplerState sampler_backTex : register(s1);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);
#endif


cbuffer VS_ConstantBuffer : register(b0)
{
	float4 g_scale;
	float4 mUVInversedBack;

	// Unused
	float4 flipbookParameter;	 // x:enable, y:interpolationType
	float4 uvDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed
	float4 blendTextureParameter; // x:blendType

	// which is used for only softparticle
	float4 softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	float4 reconstructionParam2;
}

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;

	float4 PosP : TEXCOORD1;
	float4 PosU : TEXCOORD2;
	float4 PosR : TEXCOORD3;
};

#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV);
	Output.a = Output.a * Input.Color.a;

	float2 pos = Input.PosP.xy / Input.PosP.w;
	float2 posU = Input.PosU.xy / Input.PosU.w;
	float2 posR = Input.PosR.xy / Input.PosR.w;

	float xscale = (Output.x * 2.0 - 1.0) * Input.Color.x * g_scale.x;
	float yscale = (Output.y * 2.0 - 1.0) * Input.Color.y * g_scale.x;

	float2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

#ifdef __OPENGL__
	uv.y = 1.0 - uv.y;
#endif

	float3 color = _backTex.Sample(sampler_backTex, uv);
	Output.xyz = color;

#ifndef DISABLED_SOFT_PARTICLE
	// softparticle
	float4 screenPos = Input.PosP / Input.PosP.w;
	float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
	screenUV.y = 1.0f - screenUV.y;

#ifdef __OPENGL__
	screenUV.y = 1.0 - screenUV.y;
#endif

	float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
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


	if (Output.a == 0.0f)
		discard;

	return Output;
}
