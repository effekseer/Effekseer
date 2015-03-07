
Texture2D	g_texture		: register( t0 );
sampler2D	g_sampler		: register( s0 );

Texture2D	g_backTexture		: register( t1 );
SamplerState	g_backSampler		: register( s1 );

struct PS_Input
{
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
};


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color * tex2D(g_sampler, Input.UV);

	if(Output.a == 0.0f) discard;

	float2 uv = Output.xy + Input.Pos.xy;
	float3 color = tex2D(g_backSampler, uv);
	Output.xyz = color;

	return Output;
}
