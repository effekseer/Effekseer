
Texture2D	g_texture		: register(t0);
SamplerState	g_sampler		: register(s0);

Texture2D		g_backTexture		: register(t1);
SamplerState	g_backSampler		: register(s1);

Texture2D g_alphaTexture            : register(t2);
SamplerState g_alphaSampler         : register(s2);

Texture2D g_uvDistortionTexture     : register(t3);
SamplerState g_uvDistortionSampler  : register(s3);

Texture2D g_blendTexture            : register(t4);
SamplerState g_blendSampler         : register(s4);

Texture2D g_blendAlphaTexture       : register(t5);
SamplerState g_blendAlphaSampler    : register(s5);

Texture2D g_blendUVDistortionTexture : register(t6);
SamplerState g_blendUVDistortionSampler : register(s6);


cbuffer PS_ConstanBuffer : register(b0)
{
    float4	g_scale;
    float4	mUVInversedBack;

    float4  fFlipbookParameter; // x:enable, y:interpolationType

    float4  fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

    float4  fBlendTextureParameter; // x:blendType
};


struct PS_Input
{
	float4 Position		: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float4 Binormal		: TEXCOORD1;
	float4 Tangent		: TEXCOORD2;
	float4 Pos		: TEXCOORD3;
	float4 Color		: COLOR0;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;

	// x - FlipbookRate, y - AlphaThreshold
	float2 Others : TEXCOORD7;
};

#include "renderer_common_PS.fx"

float4 main( const PS_Input Input ) : SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, advancedParam.UVDistortionUV, fUVDistortionParameter.zw);
    UVOffset *= fUVDistortionParameter.x;
    
	float4 Output = g_texture.Sample(g_sampler, Input.UV + UVOffset);

	Output.a = Output.a * Input.Color.a;
    
	ApplyFlipbook(Output, g_texture, g_sampler, fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate);

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
    
    // zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
	{
		discard;
	}

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
