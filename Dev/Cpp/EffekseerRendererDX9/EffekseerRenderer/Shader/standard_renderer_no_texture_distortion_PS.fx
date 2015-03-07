
Texture2D	g_backTexture		: register( t1 );
SamplerState	g_backSampler		: register( s1 );

struct PS_Input
{
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;
};


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;

	if(Output.a == 0.0f) discard;

	float2 pos = Input.Pos.xy / Input.Pos.w;
	float2 posU = Input.PosU.xy / Input.PosU.w;
	float2 posR = Input.PosR.xy / Input.PosR.w;

	float2 uv = pos + (posR - pos) * (Output.x * 2.0 - 1.0) + (posU - pos) * (Output.y * 2.0 - 1.0);
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	float3 color = tex2D(g_backSampler, uv);
	Output.xyz = color;

	return Output;
}
