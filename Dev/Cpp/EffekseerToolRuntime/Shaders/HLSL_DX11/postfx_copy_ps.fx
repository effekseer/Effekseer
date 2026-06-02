
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
	float4 Output = g_texture.Sample(g_sampler, Input.UV);
	return Output;
}
