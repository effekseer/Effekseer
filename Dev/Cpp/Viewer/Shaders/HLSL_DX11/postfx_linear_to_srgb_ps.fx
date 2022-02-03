
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float3 texel = g_texture.Sample(g_sampler, Input.UV).rgb;
	return float4(pow(texel, 1.0 / 2.2), 1.0f);
}
