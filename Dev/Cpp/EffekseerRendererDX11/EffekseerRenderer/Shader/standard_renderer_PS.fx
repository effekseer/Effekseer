
Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

Texture2D g_alphaTexture            : register( t1 );
SamplerState g_alphaSampler         : register( s1 );

Texture2D g_uvDistortionTexture     : register( t2 );
SamplerState g_uvDistortionSampler  : register( s2 );

Texture2D g_blendTexture            : register( t3 );
SamplerState g_blendSampler         : register( s3 );

Texture2D g_blendAlphaTexture       : register( t4 );
SamplerState g_blendAlphaSampler    : register( s4 );

Texture2D g_blendUVDistortionTexture    : register( t5 );
SamplerState g_blendUVDistortionSampler : register( s5 );

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 flipbookParameter; // x:enable, y:interpolationType
    float4 uvDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed
    float4 blendTextureParameter; // x:blendType
    float4 emissiveScaling; // x:emissiveScaling
    float4 edgeColor;
    float4 edgeParameter; // x:threshold, y:colorScaling
};

struct PS_Input
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;

	float4 Position : TEXCOORD1;
	float4 PosU : TEXCOORD2;
	float4 PosR : TEXCOORD3;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

#include "renderer_common_PS.fx"

float4 main(const PS_Input Input)
	: SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, advancedParam.UVDistortionUV, uvDistortionParameter.zw);
    UVOffset *= uvDistortionParameter.x;   
    
	float4 Output = Input.Color * g_texture.Sample(g_sampler, Input.UV + UVOffset);

	ApplyFlipbook(Output, g_texture, g_sampler, flipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);
    
    // apply alpha texture
	float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.a *= AlphaTexColor.r * AlphaTexColor.a;
    
    // blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV, uvDistortionParameter.zw);
    BlendUVOffset.y = uvDistortionParameter.z + uvDistortionParameter.w * BlendUVOffset.y;
    BlendUVOffset *= uvDistortionParameter.y;
    
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;
    
    ApplyTextureBlending(Output, BlendTextureColor, blendTextureParameter.x);
    
    Output.rgb *= emissiveScaling.x;
    
    // zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }

    Output.rgb = lerp(
                    edgeColor.rgb * edgeParameter.y, 
                    Output.rgb, 
                    ceil((Output.a - advancedParam.AlphaThreshold) - edgeParameter.x));
    
	return Output;
}
