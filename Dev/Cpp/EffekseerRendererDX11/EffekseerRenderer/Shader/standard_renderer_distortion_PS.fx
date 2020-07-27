
#include "FlipbookInterpolationUtils_PS.fx"
#include "TextureBlendingUtils_PS.fx"

Texture2D	g_texture		: register( t0 );
SamplerState	g_sampler		: register( s0 );

Texture2D	g_backTexture		: register( t1 );
SamplerState	g_backSampler		: register( s1 );

Texture2D g_alphaTexture        : register(t2);
SamplerState g_alphaSampler     : register(s2);

Texture2D g_uvDistortionTexture     : register(t3);
SamplerState g_uvDistortionSampler  : register(s3);

Texture2D g_blendTexture            : register( t4 );
SamplerState g_blendSampler         : register( s4 );

Texture2D g_blendAlphaTexture       : register( t5 );
SamplerState g_blendAlphaSampler    : register( s5 );

Texture2D g_blendUVDistortionTexture    : register( t6 );
SamplerState g_blendUVDistortionSampler : register( s6 );

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 g_scale;
    float4 mUVInversedBack;
    float4 flipbookParameter;       // x:enable, y:interpolationType
    float4 uvDistortionParameter;   // x:intensity, y:blendIntensity
    float4 blendTextureParameter; // x:blendType
};

struct PS_Input
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV		: TEXCOORD0;

	float4 Pos		: TEXCOORD1;
	float4 PosU		: TEXCOORD2;
	float4 PosR		: TEXCOORD3;
    
    float2 AlphaUV              : TEXCOORD4;
    float2 UVDistortionUV       : TEXCOORD5;
    float2 BlendUV              : TEXCOORD6;
    float2 BlendAlphaUV         : TEXCOORD7;
    float2 BlendUVDistortionUV  : TEXCOORD8;
    float FlipbookRate          : TEXCOORD9;
    float2 FlipbookNextIndexUV  : TEXCOORD10;
    float AlphaThreshold        : TEXCOORD11;
};


float4 main( const PS_Input Input ) : SV_Target
{
    float2 UVOffset = float2(0.0, 0.0);
    
    UVOffset = g_uvDistortionTexture.Sample(g_uvDistortionSampler, Input.UVDistortionUV).rg * 2.0 - 1.0;
    UVOffset *= uvDistortionParameter.x;

	float4 Output = g_texture.Sample(g_sampler, Input.UV + UVOffset);
	Output.a = Output.a * Input.Color.a;
    
	ApplyFlipbook(Output, g_texture, g_sampler, flipbookParameter, Input.Color, Input.FlipbookNextIndexUV + UVOffset, Input.FlipbookRate);
    
    // apply alpha texture
    float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, Input.AlphaUV + UVOffset);
    Output.a *= AlphaTexColor.r * AlphaTexColor.a;
    
    // blend texture uv offset
    float2 BlendUVOffset = g_blendUVDistortionTexture.Sample(g_blendUVDistortionSampler, Input.BlendUVDistortionUV).rg * 2.0 - 1.0;
    BlendUVOffset *= uvDistortionParameter.y;
    
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, Input.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, Input.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;
    
    ApplyTextureBlending(Output, BlendTextureColor, blendTextureParameter.x);
    
    // alpha threshold
    if(Output.a <= Input.AlphaThreshold)
    {
        discard;
    }

	if(Output.a == 0.0f) discard;

	float2 pos = Input.Pos.xy / Input.Pos.w;
	float2 posU = Input.PosU.xy / Input.PosU.w;
	float2 posR = Input.PosR.xy / Input.PosR.w;

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
