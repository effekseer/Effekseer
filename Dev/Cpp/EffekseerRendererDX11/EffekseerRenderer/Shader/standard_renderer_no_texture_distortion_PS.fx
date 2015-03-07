
Texture2D	g_backTexture		: register( t1 );
SamplerState	g_backSampler		: register( s1 );

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
};


float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color;

	if(Output.a == 0.0f) discard;

	float2 uv = Output.xy;
	float3 color = g_backTexture.Sample(g_backSampler, uv);
	Output.xyz = color;

	return Output;
}
