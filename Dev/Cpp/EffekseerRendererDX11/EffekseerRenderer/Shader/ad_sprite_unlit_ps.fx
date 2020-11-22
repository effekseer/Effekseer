
Texture2D _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);

Texture2D _alphaTex : register(t1);
SamplerState sampler_alphaTex : register(s1);

Texture2D _uvDistortionTex : register(t2);
SamplerState sampler_uvDistortionTex : register(s2);

Texture2D _blendTex : register(t3);
SamplerState sampler_blendTex : register(s3);

Texture2D _blendAlphaTex : register(t4);
SamplerState sampler_blendAlphaTex : register(s4);

Texture2D _blendUVDistortionTex : register(t5);
SamplerState sampler_blendUVDistortionTex : register(s5);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t6);
SamplerState sampler_depthTex : register(s6);
#endif

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

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;

	float4 PosP : TEXCOORD1;
	float4 PosU : TEXCOORD2;
	float4 PosR : TEXCOORD3;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

#include "ad_common_ps.fx"
#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

	float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, advancedParam.UVDistortionUV, uvDistortionParameter.zw);
	UVOffset *= uvDistortionParameter.x;

	float4 Output = Input.Color * _colorTex.Sample(sampler_colorTex, Input.UV + UVOffset);

	ApplyFlipbook(Output, _colorTex, sampler_colorTex, flipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);

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

	// apply alpha texture
	float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
	Output.a *= AlphaTexColor.r * AlphaTexColor.a;

	// blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, advancedParam.BlendUVDistortionUV, uvDistortionParameter.zw);
	BlendUVOffset.y = uvDistortionParameter.z + uvDistortionParameter.w * BlendUVOffset.y;
	BlendUVOffset *= uvDistortionParameter.y;

	float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
	BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;

	ApplyTextureBlending(Output, BlendTextureColor, blendTextureParameter.x);

	Output.rgb *= emissiveScaling.x;

	// zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
	{
		discard;
	}

	Output.rgb = lerp(
		edgeColor.rgb * edgeParameter.y,
		Output.rgb,
		ceil((Output.a - advancedParam.AlphaThreshold) - edgeParameter.x));

	return Output;
}
