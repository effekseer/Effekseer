
struct PS_Input
{
	float4 Position : SV_POSITION;
	float4 VColor : COLOR;
	float2 UV : TEXCOORD0;
	float3 WorldN : TEXCOORD1;
	float3 WorldT : TEXCOORD2;
	float3 WorldB : TEXCOORD3;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;

    float4 fFlipbookParameter; // x:enable, y:interpolationType

    float4 fUVDistortionParameter; // x:intensity, y: blendIntensity, zw:uvInversed
    float4 fBlendTextureParameter; // x:blendType
    
    float4 fEmissiveScaling; // x:emissiveScaling
    
    float4 fEdgeColor;
    float4 fEdgeParameter; // x:threshold, y:colorScaling
};

Texture2D	g_colorTexture		: register( t0 );
SamplerState g_colorSampler : register(s0);

Texture2D	g_normalTexture		: register( t1 );
SamplerState g_normalSampler : register(s1);

Texture2D g_alphaTexture 	            : register( t2 );
SamplerState g_alphaSampler             : register( s2 );

Texture2D g_uvDistortionTexture         : register( t3 );
SamplerState g_uvDistortionSampler      : register( s3 );

Texture2D g_blendTexture                : register( t4 );
SamplerState g_blendSampler             : register( s4 );

Texture2D g_blendAlphaTexture           : register( t5 );
SamplerState g_blendAlphaSampler        : register( s5 );

Texture2D g_blendUVDistortionTexture    : register( t6 );
SamplerState g_blendUVDistortionSampler : register( s6 );

#include "renderer_common_PS.fx"

float4 main(const PS_Input Input) : SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, advancedParam.UVDistortionUV, fUVDistortionParameter.zw);
	UVOffset *= fUVDistortionParameter.x;

	float diffuse = 1.0;

	half3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV + UVOffset).xyz;
	half3 texNormal = (loN - 0.5) * 2.0;
	half3 localNormal = (half3)normalize(mul(texNormal, half3x3((half3)Input.WorldT, (half3)Input.WorldB, (half3)Input.WorldN)));

	diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);

	float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    
	ApplyFlipbook(Output, g_colorTexture, g_colorSampler, fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);
    
    // apply alpha texture
	float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
	Output.a *= AlphaTexColor.r * AlphaTexColor.a;
    
    // blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV, fUVDistortionParameter.zw);
    BlendUVOffset *= fUVDistortionParameter.y;
    
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;
    
    ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);
    
    Output.rgb *= fEmissiveScaling.x;
    
    // zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
	{
		discard;
	}

	Output.xyz = Output.xyz * (float3(diffuse, diffuse, diffuse) + fLightAmbient);
    
    Output.rgb = lerp(
                fEdgeColor.rgb * fEdgeParameter.y, 
                Output.rgb, 
                ceil((Output.a - advancedParam.AlphaThreshold) - fEdgeParameter.x));
    
	return Output;
}
