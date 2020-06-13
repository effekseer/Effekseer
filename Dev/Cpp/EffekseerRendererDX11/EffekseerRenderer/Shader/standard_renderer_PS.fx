
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#endif

#include "UVDistortionUtils.fx"

Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture    : register( t1 );
SamplerState g_alphaSampler : register( s1 );

Texture2D g_uvDistortionTexture     : register( t2 );
SamplerState g_uvDistortionSampler  : register( s2 );

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 flipbookParameter; // x:enable, y:interpolationType
    float4 uvDistortionParameter; // x:intensity
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
    
	float2 AlphaUV              : TEXCOORD4;
	float2 UVDistortionUV       : TEXCOORD5;
    float FlipbookRate          : TEXCOORD6;
    float2 FlipbookNextIndexUV  : TEXCOORD7;
    float AlphaThreshold        : TEXCOORD8;
#endif
};

float4 PS( const PS_Input Input ) : SV_Target
{
    float2 UVOffset = float2(0.0, 0.0);
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    UVOffset = g_uvDistortionTexture.Sample(g_uvDistortionSampler, Input.UVDistortionUV).rg * 2.0 - 1.0;
    UVOffset *= uvDistortionParameter.x;
#endif
    
	float4 Output = Input.Color * g_texture.Sample(g_sampler, Input.UV + UVOffset * GetPixelSize(g_texture));

#ifdef __EFFEKSEER_BUILD_VERSION16__    
	ApplyFlipbook(Output, g_texture, g_sampler, flipbookParameter, Input.Color, Input.FlipbookNextIndexUV + UVOffset * GetPixelSize(g_texture), Input.FlipbookRate);
    /*
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
    */
    
    // alpha texture
    Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV + UVOffset * GetPixelSize(g_alphaTexture)).a;
    
    // alpha threshold
    if (Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
    
#endif

	if(Output.a == 0.0f) discard;

	return Output;
}
