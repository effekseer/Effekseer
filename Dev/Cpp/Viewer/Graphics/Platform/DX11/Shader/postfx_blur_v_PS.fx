
Texture2D		g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
};

float4 PS( const PS_Input Input ) : SV_Target
{
	float2 size;
	float level;
	g_texture.GetDimensions(0, size.x, size.y, level);
	float4 color = g_texture.Sample(g_sampler, Input.UV) * 0.223067435;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, -5.152032242 / size.y)) * 0.005291685;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, -3.250912787 / size.y)) * 0.072975516;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, -1.384912144 / size.y)) * 0.310199082;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, +1.384912144 / size.y)) * 0.310199082;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, +3.250912787 / size.y)) * 0.072975516;
	color += g_texture.Sample(g_sampler, Input.UV + float2(0, +5.152032242 / size.y)) * 0.005291685;
	return float4(color.rgb, 1.0);
}
