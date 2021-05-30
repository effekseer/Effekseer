
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
	float value = g_texture.Sample(g_sampler, Input.UV).r / 5.0;

	return float4(value, 1.0f - value, 0.0f, 1.0f);
}
