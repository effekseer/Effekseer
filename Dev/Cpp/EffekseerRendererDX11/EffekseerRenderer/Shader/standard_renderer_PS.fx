
Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture : register( t1 );
SamplerState g_alphaSampler : register( s1 );
#endif

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
#ifdef __EFFEKSEER_BUILD_VERSION16__
	float2 AlphaUV : TEXCOORD4;
#endif
};


float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color * g_texture	.Sample(g_sampler, Input.UV);

#ifdef __EFFEKSEER_BUILD_VERSION16__
	Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV).a;
#endif

	if(Output.a == 0.0f) discard;

	return Output;
}
