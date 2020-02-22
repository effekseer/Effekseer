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
    float2 AlphaUV : TEXCOORD7;
    float FlipbookRate : TEXCOORD8;
    float2 FlipbookNextIndexUV : TEXCOORD9;
    float AlphaThreshold : TEXCOORD10;
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
SamplerState g_colorSampler : register(s0);

Texture2D	g_normalTexture		: register( t1 );
SamplerState g_normalSampler : register(s1);

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D g_alphaTexture 	: register( t2 );
SamplerState g_alphaSampler : register( s2 );
#endif

float4 PS(const PS_Input Input) : SV_Target
{
	float diffuse = 1.0;

	half3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV1).xyz;
	half3 texNormal = (loN - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV1) * Input.VColor;
#ifdef __EFFEKSEER_BUILD_VERSION16__
    
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
    
    // alpha texture
	Output.a *= g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV).a;
    
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
