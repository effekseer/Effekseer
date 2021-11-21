struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
};

struct AdvancedParameter
{
    float2 AlphaUV;
    float2 UVDistortionUV;
    float2 BlendUV;
    float2 BlendAlphaUV;
    float2 BlendUVDistortionUV;
    float2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _289_fLightDirection : register(c0);
    float4 _289_fLightColor : register(c1);
    float4 _289_fLightAmbient : register(c2);
    float4 _289_fFlipbookParameter : register(c3);
    float4 _289_fUVDistortionParameter : register(c4);
    float4 _289_fBlendTextureParameter : register(c5);
    float4 _289_fCameraFrontDirection : register(c6);
    float4 _289_fFalloffParameter : register(c7);
    float4 _289_fFalloffBeginColor : register(c8);
    float4 _289_fFalloffEndColor : register(c9);
    float4 _289_fEmissiveScaling : register(c10);
    float4 _289_fEdgeColor : register(c11);
    float4 _289_fEdgeParameter : register(c12);
    float4 _289_softParticleParam : register(c13);
    float4 _289_reconstructionParam1 : register(c14);
    float4 _289_reconstructionParam2 : register(c15);
    float4 _289_mUVInversedBack : register(c16);
    float4 _289_miscFlags : register(c17);
};

uniform sampler2D Sampler_sampler_uvDistortionTex : register(s2);
uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_alphaTex : register(s1);
uniform sampler2D Sampler_sampler_blendUVDistortionTex : register(s5);
uniform sampler2D Sampler_sampler_blendTex : register(s3);
uniform sampler2D Sampler_sampler_blendAlphaTex : register(s4);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD5;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

AdvancedParameter DisolveAdvancedParameter(PS_Input psinput)
{
    AdvancedParameter ret;
    ret.AlphaUV = psinput.Alpha_Dist_UV.xy;
    ret.UVDistortionUV = psinput.Alpha_Dist_UV.zw;
    ret.BlendUV = psinput.Blend_FBNextIndex_UV.xy;
    ret.BlendAlphaUV = psinput.Blend_Alpha_Dist_UV.xy;
    ret.BlendUVDistortionUV = psinput.Blend_Alpha_Dist_UV.zw;
    ret.FlipbookNextIndexUV = psinput.Blend_FBNextIndex_UV.zw;
    ret.FlipbookRate = psinput.UV_Others.z;
    ret.AlphaThreshold = psinput.UV_Others.w;
    return ret;
}

float2 UVDistortionOffset(float2 uv, float2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    float2 UVOffset = (tex2D(SPIRV_Cross_Combinedts, uv).xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

float3 PositivePow(float3 base, float3 power)
{
    return pow(max(abs(base), 1.1920928955078125e-07f.xxx), power);
}

float3 LinearToSRGB(float3 c)
{
    float3 param = c;
    float3 param_1 = 0.4166666567325592041015625f.xxx;
    return max((PositivePow(param, param_1) * 1.05499994754791259765625f) - 0.054999999701976776123046875f.xxx, 0.0f.xxx);
}

float4 LinearToSRGB(float4 c)
{
    float3 param = c.xyz;
    return float4(LinearToSRGB(param), c.w);
}

float4 ConvertFromSRGBTexture(float4 c)
{
    if (_289_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

void ApplyFlipbook(inout float4 dst, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 NextPixelColor = tex2D(SPIRV_Cross_Combinedts, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0f)
        {
            dst = lerp(dst, NextPixelColor, flipbookRate.xxxx);
        }
    }
}

void ApplyTextureBlending(inout float4 dstColor, float4 blendColor, float blendType)
{
    if (blendType == 0.0f)
    {
        float3 _111 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_111.x, _111.y, _111.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _123 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_123.x, _123.y, _123.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _136 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_136.x, _136.y, _136.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _149 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_149.x, _149.y, _149.z, dstColor.w);
                }
            }
        }
    }
}

float3 SRGBToLinear(float3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625f) + 0.6821711063385009765625f.xxx)) + 0.01252287812530994415283203125f.xxx));
}

float4 SRGBToLinear(float4 c)
{
    float3 param = c.xyz;
    return float4(SRGBToLinear(param), c.w);
}

float4 ConvertToScreen(float4 c)
{
    if (_289_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _289_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= _289_fUVDistortionParameter.x;
    float4 param_3 = tex2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    float4 Output = ConvertFromSRGBTexture(param_3) * Input.Color;
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, _289_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    float4 AlphaTexColor = tex2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_6 = advancedParam.BlendUVDistortionUV;
    float2 param_7 = _289_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _289_fUVDistortionParameter.y;
    float4 param_8 = tex2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_8);
    float4 BlendAlphaTextureColor = tex2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, _289_fBlendTextureParameter.x);
    Output = param_9;
    if (_289_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_289_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_289_fFalloffEndColor, _289_fFalloffBeginColor, pow(CdotN, _289_fFalloffParameter.z).xxxx);
        if (_289_fFalloffParameter.y == 0.0f)
        {
            float3 _486 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_486.x, _486.y, _486.z, Output.w);
        }
        else
        {
            if (_289_fFalloffParameter.y == 1.0f)
            {
                float3 _499 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_499.x, _499.y, _499.z, Output.w);
            }
            else
            {
                if (_289_fFalloffParameter.y == 2.0f)
                {
                    float3 _512 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_512.x, _512.y, _512.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _526 = Output.xyz * _289_fEmissiveScaling.x;
    Output = float4(_526.x, _526.y, _526.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _558 = lerp(_289_fEdgeColor.xyz * _289_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _289_fEdgeParameter.x).xxx);
    Output = float4(_558.x, _558.y, _558.z, Output.w);
    float4 param_10 = Output;
    return ConvertToScreen(param_10);
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV_Others = Input_UV_Others;
    Input.WorldN = Input_WorldN;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    float4 _593 = _main(Input);
    _entryPointOutput = _593;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
