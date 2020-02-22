
Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture : register( t1 );
SamplerState g_alphaSampler : register( s1 );

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 g_flipbookParameter; // x:enable, y:interpolationType
};
#endif

struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float4 Posision	: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;
    
	float2 AlphaUV : TEXCOORD4;
    float FlipbookRate : TEXCOORD5;
    float2 FlipbookNextIndexUV : TEXCOORD6;
    float AlphaThreshold : TEXCOORD7;
#endif
};

float4 PS( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color * g_texture	.Sample(g_sampler, Input.UV);

#ifdef __EFFEKSEER_BUILD_VERSION16__
    // flipbook interpolation
    if(g_flipbookParameter.x > 0)
    {
        float4 NextPixelColor = g_texture.Sample(g_sampler, Input.FlipbookNextIndexUV) * Input.Color;
    
        // lerp
        if(g_flipbookParameter.y == 1)
        {
            Output = lerp(Output, NextPixelColor, Input.FlipbookRate);
        }
    }
    
    // alpha texture
    Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV).a;
    
    // alpha threshold
    if (Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
    
#endif

	if(Output.a == 0.0f) discard;

	return Output;
}
