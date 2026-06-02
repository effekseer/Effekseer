
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

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
	float4 PosP : TEXCOORD4;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	return float4(1.0, 1.0, 1.0, 1.0);
}
