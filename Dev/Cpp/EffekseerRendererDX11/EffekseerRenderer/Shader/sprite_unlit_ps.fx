
Texture2D _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t1);
SamplerState sampler_depthTex : register(s1);
#endif

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
	float4 PosP : TEXCOORD1;
};

// Unused
cbuffer PS_ConstanBuffer : register(b0)
{
	float4 flipbookParameter;	 // x:enable, y:interpolationType
	float4 uvDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed
	float4 blendTextureParameter; // x:blendType
	float4 emissiveScaling;		  // x:emissiveScaling
	float4 edgeColor;
	float4 edgeParameter; // x:threshold, y:colorScaling

	// which is used for only softparticle
	float4 softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	float4 reconstructionParam2;
};

#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = Input.Color * _colorTex.Sample(sampler_colorTex, Input.UV);

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
