#define SamplerState sampler2D
// 1.6
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#endif


Texture2D	g_texture		: register( t0 );
sampler2D	g_sampler		: register( s0 );

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture : register( t1 );
SamplerState g_alphaSampler : register( s1 );

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 flipbookParameter; // x:enable, y:interpolationType
};
#endif

struct PS_Input
{
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


float4 PS( const PS_Input Input ) : COLOR
{
	float4 Output = Input.Color * tex2D(g_sampler, Input.UV);

#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_texture, g_sampler, flipbookParameter, Input.Color, Input.FlipbookNextIndexUV, Input.FlipbookRate);

    // alpha texture
    Output.a *= tex2D(g_alphaSampler, Input.AlphaUV).a;
    
    // alpha threshold
    if (Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
    
#endif

	if(Output.a == 0.0f) discard;
	
	return Output;
}
