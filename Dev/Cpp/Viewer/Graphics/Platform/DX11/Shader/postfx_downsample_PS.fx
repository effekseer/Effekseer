
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
	float4 color1 = g_texture.Sample(g_sampler, Input.UV + float2(-0.5, -0.5) / size);
	float4 color2 = g_texture.Sample(g_sampler, Input.UV + float2(+0.5, -0.5) / size);
	float4 color3 = g_texture.Sample(g_sampler, Input.UV + float2(-0.5, +0.5) / size);
	float4 color4 = g_texture.Sample(g_sampler, Input.UV + float2(+0.5, +0.5) / size);
	return (color1 + color2 + color3 + color4) * 0.25;
}
