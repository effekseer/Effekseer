
Texture2D		g_texture0		: register( t0 );
Texture2D		g_texture1		: register( t1 );
Texture2D		g_texture2		: register( t2 );
Texture2D		g_texture3		: register( t3 );
SamplerState	g_sampler		: register( s0 );

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
};

float4 PS( const PS_Input Input ) : SV_Target
{
	const float4 gain = float4(0.25, 0.5, 1.0, 2.0);
	float3 c0 = g_texture0.Sample(g_sampler, Input.UV).rgb * gain.x;
	float3 c1 = g_texture1.Sample(g_sampler, Input.UV).rgb * gain.y;
	float3 c2 = g_texture2.Sample(g_sampler, Input.UV).rgb * gain.z;
	float3 c3 = g_texture3.Sample(g_sampler, Input.UV).rgb * gain.w;
	return float4(c0 + c1 + c2 + c3, 1.0);
}
