
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 g_toneparams : register(c0);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float3 texel = g_texture.Sample(g_sampler, Input.UV).rgb;

	// To luminance
	float lum = g_toneparams.x * dot(texel, float3(0.299f, 0.587f, 0.114f));

	// Reinhard
	lum = lum / (1.0f + lum);

	return float4(lum * texel, 1.0f);
}
