
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	linear centroid float4 Color : COLOR;
	linear centroid float2 UV : TEXCOORD0;
};

// Unused
cbuffer PS_ConstanBuffer : register(b0)
{
	float4 flipbookParameter;	  // x:enable, y:interpolationType
	float4 uvDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed
	float4 blendTextureParameter; // x:blendType
	float4 emissiveScaling;		  // x:emissiveScaling
	float4 edgeColor;
	float4 edgeParameter; // x:threshold, y:colorScaling
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float4 Output = Input.Color * g_texture.Sample(g_sampler, Input.UV);

	if (Output.a == 0.0f)
		discard;

	return Output;
}
