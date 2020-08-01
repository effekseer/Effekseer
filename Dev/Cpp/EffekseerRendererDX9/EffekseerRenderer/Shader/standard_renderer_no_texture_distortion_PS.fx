
Texture2D	g_backTexture		: register( t1 );
SamplerState	g_backSampler		: register( s1 );

float4		g_scale			: register(c0);
float4 mUVInversedBack		: register(c1);

struct PS_Input
{
	float4 Color		: TEXCOORD0;
	float2 UV		: TEXCOORD1;

	float4 Pos		: TEXCOORD2;
	float4 PosU		: TEXCOORD3;
	float4 PosR		: TEXCOORD4;
};


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color;
	Output.xyz = float3(1.0,1.0,1.0);

	if(Output.a <= 0.0f) discard;

	float2 pos = Input.Pos.xy / Input.Pos.w;
	float2 posU = Input.PosU.xy / Input.PosU.w;
	float2 posR = Input.PosR.xy / Input.PosR.w;

	float xscale = (Output.x * 2.0 - 1.0) * Input.Color.x * g_scale.x;
	float yscale = (Output.y * 2.0 - 1.0) * Input.Color.y * g_scale.x;

	float2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	float3 color = tex2D(g_backSampler, uv);
	Output.xyz = color;

	return Output;
}
