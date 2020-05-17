#define SamplerState sampler2D
// 1.6
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__

cbuffer PS_ConstanBuffer : register(b0)
{
    float4	fLightDirection;
    float4	fLightColor;
    float4	fLightAmbient;

    float4  fFlipbookParameter; // x:enable, y:interpolationType
};

#else // else __EFFEKSEER_BUILD_VERSION16__

#ifdef ENABLE_LIGHTING
float4	fLightDirection		: register( c0 );
float4	fLightColor		: register( c1 );
float4	fLightAmbient		: register( c2 );
#endif

#endif // end __EFFEKSEER_BUILD_VERSION16__

#ifdef ENABLE_COLOR_TEXTURE
Texture2D	g_colorTexture		: register( t0 );
sampler2D	g_colorSampler		: register( s0 );
#endif

#ifdef ENABLE_NORMAL_TEXTURE
Texture2D	g_normalTexture		: register( t1 );
sampler2D	g_normalSampler		: register( s1 );
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D	    g_alphaTexture		: register( t2 );
SamplerState	g_alphaSampler		: register( s2 );
#endif

struct PS_Input
{
	float2 UV		: TEXCOORD0;
#if ENABLE_NORMAL_TEXTURE
	half3 Normal	: TEXCOORD1;
	half3 Binormal	: TEXCOORD2;
	half3 Tangent	: TEXCOORD3;

#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV  : TEXCOORD4;
    
    float FlipbookRate  : TEXCOORD5;
    float2 FlipbookNextIndexUV : TEXCOORD6;
    
    float AlphaThreshold : TEXCOORD7;
#endif

#else

#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV  : TEXCOORD1;
    
    float FlipbookRate  : TEXCOORD2;
    float2 FlipbookNextIndexUV : TEXCOORD3;
    
    float AlphaThreshold : TEXCOORD4;
#endif

#endif
	float4 Color	: COLOR;
};

float4 PS(const PS_Input Input) : COLOR
{
	float4 Output = tex2D(g_colorSampler, Input.UV) * Input.Color;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.Color, Input.FlipbookNextIndexUV, Input.FlipbookRate);

    Output.a *= tex2D(g_alphaSampler, Input.AlphaUV).a;
    
    // alpha threshold
    if (Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
#endif

#if ENABLE_LIGHTING
	half3 texNormal = (tex2D(g_normalSampler, Input.UV).xyz  - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
		texNormal,
		half3x3( (half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal )
		) );

	float diffuse = max( dot( fLightDirection.xyz, localNormal.xyz ), 0.0 );
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

	if( Output.a == 0.0 ) discard;

	return Output;
}
