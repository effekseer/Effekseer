
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "FlipbookInterpolationUtils_PS.fx"
#include "TextureBlendingUtils_PS.fx"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
struct FalloffParameter
{
    float4 Param; // x:enable, y:colorblendtype, z:pow
    float4 BeginColor;
    float4 EndColor;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4	fLightDirection;
    float4	fLightColor;
    float4	fLightAmbient;

    float4  fFlipbookParameter; // x:enable, y:interpolationType

    float4  fUVDistortionParameter; // x:intensity, y:blendIntensity

    float4  fBlendTextureParameter; // x:blendType
    
    float4  fCameraFrontDirection;

    FalloffParameter fFalloffParam;

    float4 fEmissiveScaling; // x:emissiveScaling

    float4 fEdgeColor;
    float4 fEdgeParameter; // x:threshold, y:colorScaling
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
SamplerState	g_colorSampler		: register( s0 );
#endif

#ifdef ENABLE_NORMAL_TEXTURE
Texture2D	g_normalTexture		: register( t1 );
SamplerState	g_normalSampler		: register( s1 );
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
Texture2D	    g_alphaTexture		: register( t2 );
SamplerState	g_alphaSampler		: register( s2 );

Texture2D       g_uvDistortionTexture : register( t3 );
SamplerState    g_uvDistortionSampler : register( s3 );

Texture2D       g_blendTexture : register( t4 );
SamplerState    g_blendSampler : register( s4 );

Texture2D       g_blendAlphaTexture : register( t5 );
SamplerState    g_blendAlphaSampler : register( s5 );

Texture2D       g_blendUVDistortionTexture : register( t6 );
SamplerState    g_blendUVDistortionSampler : register( s6 );
#endif


struct PS_Input
{
	float4 Pos		: SV_POSITION;
	float2 UV		: TEXCOORD0;
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    float3 Normal		: TEXCOORD1;
	float3 Binormal		: TEXCOORD2;
	float3 Tangent		: TEXCOORD3;  
    
    float2 AlphaUV              : TEXCOORD4;
    float2 UVDistortionUV       : TEXCOORD5;
    float2 BlendUV              : TEXCOORD6;
    float2 BlendAlphaUV         : TEXCOORD7;
    float2 BlendUVDistortionUV  : TEXCOORD8;
    
    float FlipbookRate          : TEXCOORD9;
    float2 FlipbookNextIndexUV  : TEXCOORD10;
    
    float AlphaThreshold        : TEXCOORD11;
#else

#if ENABLE_NORMAL_TEXTURE
	half3 Normal		: TEXCOORD1;
	half3 Binormal		: TEXCOORD2;
	half3 Tangent		: TEXCOORD3;   
#endif
    
#endif
	float4 Color	: COLOR;
};

float4 PS( const PS_Input Input ) : SV_Target
{
    float2 UVOffset = float2(0.0, 0.0);
    
#ifdef __EFFEKSEER_BUILD_VERSION16__
    UVOffset = g_uvDistortionTexture.Sample(g_uvDistortionSampler, Input.UVDistortionUV).rg * 2.0 - 1.0;
    UVOffset *= fUVDistortionParameter.x;
#endif
    
	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV + UVOffset) * Input.Color;

#if ENABLE_LIGHTING
	half3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV + UVOffset).xyz  - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(
		mul(
		texNormal ,
		half3x3( (half3)Input.Tangent, (half3)Input.Binormal, (half3)Input.Normal ) ) );
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.Color, Input.FlipbookNextIndexUV + UVOffset, Input.FlipbookRate);
    
    // apply alpha texture
    float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV + UVOffset);
    Output.a *= AlphaTexColor.r * AlphaTexColor.a;
    
    // blend texture uv offset
    float2 BlendUVOffset = g_blendUVDistortionTexture.Sample(g_blendUVDistortionSampler, Input.BlendUVDistortionUV).rg * 2.0 - 1.0;
    BlendUVOffset *= fUVDistortionParameter.y;
    
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, Input.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, Input.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;
    
    ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);
#endif
    
#if ENABLE_LIGHTING
	float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
	Output.xyz = Output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif
    
#if __EFFEKSEER_BUILD_VERSION16__
    // apply falloff
    if(fFalloffParam.Param.x == 1)
    {
        float3 cameraVec = normalize(-fCameraFrontDirection.xyz);
#if ENABLE_LIGHTING
        float CdotN = saturate(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)));
#else
        float CdotN = saturate(dot(cameraVec, normalize(Input.Normal)));
#endif
        float4 FalloffBlendColor = lerp(fFalloffParam.EndColor, fFalloffParam.BeginColor, pow(CdotN, fFalloffParam.Param.z));

        if(fFalloffParam.Param.y == 0) // add
        {
            Output.rgb += FalloffBlendColor.rgb;
        }
        else if(fFalloffParam.Param.y == 1) // sub
        {
            Output.rgb -= FalloffBlendColor.rgb;
        }
        else if(fFalloffParam.Param.y == 2) // mul
        {
            Output.rgb *= FalloffBlendColor.rgb;
        }
    
        Output.a *= FalloffBlendColor.a;
    }
    
    Output.rgb *= fEmissiveScaling.x;
    
    // alpha threshold
    if (Output.a <= Input.AlphaThreshold)
    {
        discard;
    }
    
    Output.rgb = lerp(
                fEdgeColor.rgb * fEdgeParameter.y, 
                Output.rgb, 
                ceil((Output.a - Input.AlphaThreshold) - fEdgeParameter.x));
#endif

	if( Output.a == 0.0 ) discard;

	return Output;
}
