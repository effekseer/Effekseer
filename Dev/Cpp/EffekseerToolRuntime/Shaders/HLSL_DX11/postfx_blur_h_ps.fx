
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
	float4 color = g_texture.Sample(g_sampler, Input.UV) * 0.223067435;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-5.152032242 / size.x, 0)) * 0.005291685;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-3.250912787 / size.x, 0)) * 0.072975516;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-1.384912144 / size.x, 0)) * 0.310199082;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+1.384912144 / size.x, 0)) * 0.310199082;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+3.250912787 / size.x, 0)) * 0.072975516;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+5.152032242 / size.x, 0)) * 0.005291685;
	return float4(color.rgb, 1.0);
}
