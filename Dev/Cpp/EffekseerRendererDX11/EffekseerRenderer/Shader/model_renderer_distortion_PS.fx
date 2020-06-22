
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#include "TextureBlendingUtils_PS.fx"
#endif

Texture2D	g_texture		: register(t0);
SamplerState	g_sampler		: register(s0);

Texture2D		g_backTexture		: register(t1);
SamplerState	g_backSampler		: register(s1);

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture    : register(t2);
SamplerState g_alphaSampler : register(s2);

Texture2D g_uvDistortionTexture     : register(t3);
SamplerState g_uvDistortionSampler  : register(s3);

Texture2D g_blendTexture    : register(t4);
SamplerState g_blendSampler : register(s4);

Texture2D g_blendAlphaTexture : register(t5);
SamplerState g_blendAlphaSampler : register(s5);
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer PS_ConstanBuffer : register(b0)
{
    float4	g_scale;
    float4	mUVInversedBack;

    float4  fFlipbookParameter; // x:enable, y:interpolationType

    float4  fUVDistortionParameter; // x:intensity

    float4  fBlendTextureParameter; // x:blendType
};
#else
float4		g_scale			: register(c0);
float4 mUVInversedBack		: register(c1);
#endif

struct PS_Input
{
	float4 Position		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Normal		: TEXCOORD1;
	float4 Binormal		: TEXCOORD2;
	float4 Tangent		: TEXCOORD3;
	float4 Pos		: TEXCOORD4;
	float4 Color		: COLOR0;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV              : TEXCOORD5;
    float2 UVDistortionUV       : TEXCOORD6;
    float2 BlendUV              : TEXCOORD7;
    float2 BlendAlphaUV         : TEXCOORD8;
    float FlipbookRate          : TEXCOORD9;
    float2 FlipbookNextIndexUV  : TEXCOORD10;
    float AlphaThreshold        : TEXCOORD11;
#endif
};

float4 PS( const PS_Input Input ) : SV_Target
{
    float2 UVOffset = float2(0.0, 0.0);
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    UVOffset = g_uvDistortionTexture.Sample(g_uvDistortionSampler, Input.UVDistortionUV).rg * 2.0 - 1.0;
    UVOffset *= fUVDistortionParameter.x;
    
    return g_blendAlphaTexture.Sample(g_blendAlphaSampler, Input.BlendAlphaUV);
#endif
    
	float4 Output = g_texture.Sample(g_sampler, Input.UV + UVOffset);

	Output.a = Output.a * Input.Color.a;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_texture, g_sampler, fFlipbookParameter, Input.Color, Input.FlipbookNextIndexUV + UVOffset, Input.FlipbookRate);
	/*
    if(fFlipbookParameter.x > 0)
    {
        float4 NextPixelColor = g_backTexture.Sample(g_backSampler, Input.FlipbookNextIndexUV) * Input.Color;
        
        if(fFlipbookParameter.y == 1)
        {
            Output = lerp(Output, NextPixelColor, Input.FlipbookRate);
        }
    }
    */

    Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV + UVOffset).a;
    
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, Input.BlendUV);
    ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);
    
    // alpha threshold
    if(Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
#endif

	if (Output.a == 0.0f) discard;

	float2 pos = Input.Pos.xy / Input.Pos.w;
		float2 posU = Input.Tangent.xy / Input.Tangent.w;
		float2 posR = Input.Binormal.xy / Input.Binormal.w;

		float xscale = (Output.x * 2.0 - 1.0) * Input.Color.x * g_scale.x;
	float yscale = (Output.y * 2.0 - 1.0) * Input.Color.y * g_scale.x;

	float2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

		uv.x = (uv.x + 1.0) * 0.5;
	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	float3 color = g_backTexture.Sample(g_backSampler, uv);
		Output.xyz = color;

	return Output;
}
