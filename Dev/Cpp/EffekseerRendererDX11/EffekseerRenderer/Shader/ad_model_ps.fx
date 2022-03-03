
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

#ifdef ENABLE_LIGHTING

Texture2D _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);

Texture2D _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);

Texture2D _alphaTex : register(t2);
SamplerState sampler_alphaTex : register(s2);

Texture2D _uvDistortionTex : register(t3);
SamplerState sampler_uvDistortionTex : register(s3);

Texture2D _blendTex : register(t4);
SamplerState sampler_blendTex : register(s4);

Texture2D _blendAlphaTex : register(t5);
SamplerState sampler_blendAlphaTex : register(s5);

Texture2D _blendUVDistortionTex : register(t6);
SamplerState sampler_blendUVDistortionTex : register(s6);

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex : register(t7);
SamplerState sampler_depthTex : register(s7);
#endif

#else

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


#endif

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	// xy uv z - FlipbookRate, w - AlphaThreshold
	linear centroid float4 UV_Others : TEXCOORD0;
	float3 WorldN : TEXCOORD1;
#if ENABLE_LIGHTING
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
#endif

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

#ifndef DISABLED_SOFT_PARTICLE
	float4 PosP : TEXCOORD7;
#endif
};

#include "ad_common_ps.fx"
#include "SoftParticle_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

	float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, advancedParam.UVDistortionUV, fUVDistortionParameter.zw);
	UVOffset *= fUVDistortionParameter.x;

	float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV_Others.xy + UVOffset) * Input.Color;

#if ENABLE_LIGHTING
	half3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
			texNormal,
			half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));
#endif

	ApplyFlipbook(Output, _colorTex, sampler_colorTex, fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);

	// apply alpha texture
	float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
	Output.a *= AlphaTexColor.r * AlphaTexColor.a;

	// blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, advancedParam.BlendUVDistortionUV, fUVDistortionParameter.zw);
	BlendUVOffset *= fUVDistortionParameter.y;

	float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
	BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;

	ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);

#if ENABLE_LIGHTING
	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	// apply falloff
	if (fFalloffParameter.x == 1)
	{
		float3 cameraVec = normalize(-fCameraFrontDirection.xyz);
#if ENABLE_LIGHTING
		float CdotN = saturate(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)));
#else
		float CdotN = saturate(dot(cameraVec, normalize(Input.WorldN)));
#endif
		float4 FalloffBlendColor = lerp(fFalloffEndColor, fFalloffBeginColor, pow(CdotN, fFalloffParameter.z));

		if (fFalloffParameter.y == 0) // add
		{
			Output.rgb += FalloffBlendColor.rgb;
		}
		else if (fFalloffParameter.y == 1) // sub
		{
			Output.rgb -= FalloffBlendColor.rgb;
		}
		else if (fFalloffParameter.y == 2) // mul
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
