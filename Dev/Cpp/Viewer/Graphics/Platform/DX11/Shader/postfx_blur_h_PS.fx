
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
	float4 color = g_texture.Sample(g_sampler, Input.UV       ) * 0.312500;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-5.5 / size.x, 0)) * 0.015625;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-3.5 / size.x, 0)) * 0.093750;
	color += g_texture.Sample(g_sampler, Input.UV + float2(-1.5 / size.x, 0)) * 0.234375;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+1.5 / size.x, 0)) * 0.234375;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+3.5 / size.x, 0)) * 0.093750;
	color += g_texture.Sample(g_sampler, Input.UV + float2(+5.5 / size.x, 0)) * 0.015625;
	return float4(color.rgb, 1.0);
}
