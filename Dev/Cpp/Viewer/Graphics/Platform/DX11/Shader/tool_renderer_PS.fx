
Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );


struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
};


float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color * g_texture	.Sample(g_sampler, Input.UV);

	if(Output.a == 0.0f) discard;

	return Output;
}
