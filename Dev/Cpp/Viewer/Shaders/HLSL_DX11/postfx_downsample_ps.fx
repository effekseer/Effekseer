
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 g_size : register(c0);

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

float4 main(const PS_Input Input)
	: SV_Target
{
	float2 size = g_size.xy;
	float4 color1 = g_texture.Sample(g_sampler, Input.UV + float2(-0.5, -0.5) / size);
	float4 color2 = g_texture.Sample(g_sampler, Input.UV + float2(+0.5, -0.5) / size);
	float4 color3 = g_texture.Sample(g_sampler, Input.UV + float2(-0.5, +0.5) / size);
	float4 color4 = g_texture.Sample(g_sampler, Input.UV + float2(+0.5, +0.5) / size);
	return (color1 + color2 + color3 + color4) * 0.25;
}
