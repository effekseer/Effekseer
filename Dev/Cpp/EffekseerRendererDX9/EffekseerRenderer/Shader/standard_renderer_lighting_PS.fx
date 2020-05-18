#define SamplerState sampler2D
// 1.6
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#endif


struct PS_Input
{
	float4 Position : SV_POSITION;
	float4 VColor : COLOR;
	float2 UV1 : TEXCOORD0;
	float3 WorldP : TEXCOORD1;
	float3 WorldN : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
	float3 WorldB : TEXCOORD4;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float2 AlphaUV : TEXCOORD5;
    float FlipbookRate : TEXCOORD6;
    float2 FlipbookNextIndexUV : TEXCOORD7;
    float AlphaThreshold : TEXCOORD8;
#endif
};

#ifdef __EFFEKSEER_BUILD_VERSION16__
cbuffer PS_ConstanBuffer : register(b0)
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;

    float4 fFlipbookParameter; // x:enable, y:interpolationType
};
#else
float4	fLightDirection		: register( c0 );
float4	fLightColor		: register( c1 );
float4	fLightAmbient		: register( c2 );
#endif

Texture2D	g_colorTexture		: register( t0 );
sampler2D	g_colorSampler		: register( s0 );

Texture2D	g_normalTexture		: register( t1 );
sampler2D	g_normalSampler		: register( s1 );

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture 	: register( t2 );
SamplerState g_alphaSampler : register( s2 );
#endif

float4 PS( const PS_Input Input ) : COLOR
{
	half3 texNormal = (tex2D(g_normalSampler, Input.UV1).xyz - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

	float4 Output = tex2D(g_colorSampler, Input.UV1) * Input.VColor;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.VColor, Input.FlipbookNextIndexUV, Input.FlipbookRate);

    // alpha texture
	Output.a *= tex2D(g_alphaSampler, Input.AlphaUV).a;
    
    // alpha threshold
    if(Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
#endif

	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient);

	if (Output.a == 0.0)
		discard;

	return Output;
}
