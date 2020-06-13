
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#endif

#include "UVDistortionUtils.fx"

struct PS_Input
{
	float4 Position : SV_POSITION;
	float4 VColor : COLOR;
	float2 UV1 : TEXCOORD0;
	float2 UV2 : TEXCOORD1;
	float3 WorldP : TEXCOORD2;
	float3 WorldN : TEXCOORD3;
	float3 WorldT : TEXCOORD4;
	float3 WorldB : TEXCOORD5;
	float2 ScreenUV : TEXCOORD6;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV              : TEXCOORD7;
    float2 UVDistortionUV       : TEXCOORD8;
    float FlipbookRate          : TEXCOORD9;
    float2 FlipbookNextIndexUV  : TEXCOORD10;
    float AlphaThreshold        : TEXCOORD11;
#endif
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer PS_ConstanBuffer : register(b0)
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;

    float4 fFlipbookParameter; // x:enable, y:interpolationType

    float4 fUVDistortionParameter; // x:intensity
};
#else
float4	fLightDirection		: register( c0 );
float4	fLightColor		: register( c1 );
float4	fLightAmbient		: register( c2 );
#endif

Texture2D	g_colorTexture		: register( t0 );
SamplerState g_colorSampler : register(s0);

Texture2D	g_normalTexture		: register( t1 );
SamplerState g_normalSampler : register(s1);

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture 	: register( t2 );
SamplerState g_alphaSampler : register( s2 );

Texture2D g_uvDistortionTexture     : register( t3 );
SamplerState g_uvDistortionSampler  : register( s3 );
#endif

float4 PS(const PS_Input Input) : SV_Target
{
	float diffuse = 1.0;

	half3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV1).xyz;
	half3 texNormal = (loN - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

    float2 UVOffset = float2(0.0, 0.0);
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    UVOffset = g_uvDistortionTexture.Sample(g_uvDistortionSampler, Input.UVDistortionUV).rg * 2.0 - 1.0;
    UVOffset *= fUVDistortionParameter.x;
#endif  
    
	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV1 + UVOffset * GetPixelSize(g_colorTexture)) * Input.VColor;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.VColor, Input.FlipbookNextIndexUV + UVOffset * GetPixelSize(g_colorTexture), Input.FlipbookRate);
    /*
    // flipbook interpolation
    if(fFlipbookParameter.x > 0)
    {
        float4 NextPixelColor = g_colorTexture.Sample(g_colorSampler, Input.FlipbookNextIndexUV) * Input.VColor;
        
    // lerp
        if(fFlipbookParameter.y == 1)
        {
            Output = lerp(Output, NextPixelColor, Input.FlipbookRate);
        }
    }
    */
    
    // alpha texture
	Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV + UVOffset * GetPixelSize(g_alphaTexture)).a;
    
    // alpha threshold
    if(Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
#endif
	Output.xyz = Output.xyz * (float3(diffuse, diffuse, diffuse) + fLightAmbient);

	if (Output.a == 0.0)
		discard;

	return Output;
}
