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
    float4 _338_fLightDirection : register(c0);
    float4 _338_fLightColor : register(c1);
    float4 _338_fLightAmbient : register(c2);
    float4 _338_fFlipbookParameter : register(c3);
    float4 _338_fUVDistortionParameter : register(c4);
    float4 _338_fBlendTextureParameter : register(c5);
    float4 _338_fCameraFrontDirection : register(c6);
    float4 _338_fFalloffParameter : register(c7);
    float4 _338_fFalloffBeginColor : register(c8);
    float4 _338_fFalloffEndColor : register(c9);
    float4 _338_fEmissiveScaling : register(c10);
    float4 _338_fEdgeColor : register(c11);
    float4 _338_fEdgeParameter : register(c12);
    float4 _338_softParticleParam : register(c13);
    float4 _338_reconstructionParam1 : register(c14);
    float4 _338_reconstructionParam2 : register(c15);
    float4 _338_mUVInversedBack : register(c16);
    float4 _338_miscFlags : register(c17);
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

float4 ConvertFromSRGBTexture(float4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

float2 UVDistortionOffset(float2 uv, float2 uvInversed, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    float4 sampledColor = tex2D(SPIRV_Cross_Combinedts, uv);
    if (convertFromSRGB)
    {
        float4 param = sampledColor;
        bool param_1 = convertFromSRGB;
        sampledColor = ConvertFromSRGBTexture(param, param_1);
    }
    float2 UVOffset = (sampledColor.xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout float4 dst, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 sampledColor = tex2D(SPIRV_Cross_Combinedts, nextUV);
        if (convertFromSRGB)
        {
            float4 param = sampledColor;
            bool param_1 = convertFromSRGB;
            sampledColor = ConvertFromSRGBTexture(param, param_1);
        }
        float4 NextPixelColor = sampledColor * vcolor;
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
        float3 _214 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_214.x, _214.y, _214.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _226 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_226.x, _226.y, _226.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _239 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_239.x, _239.y, _239.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _252 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_252.x, _252.y, _252.z, dstColor.w);
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

float4 ConvertToScreen(float4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

float4 _main(PS_Input Input)
{
    bool convertColorSpace = _338_miscFlags.x != 0.0f;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _338_fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, param_3, Sampler_sampler_uvDistortionTex);
    UVOffset *= _338_fUVDistortionParameter.x;
    float4 param_4 = tex2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    bool param_5 = convertColorSpace;
    float4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    float4 param_6 = Output;
    float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, _338_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8, Sampler_sampler_colorTex);
    Output = param_6;
    float4 param_9 = tex2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    bool param_10 = convertColorSpace;
    float4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_11 = advancedParam.BlendUVDistortionUV;
    float2 param_12 = _338_fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    float2 BlendUVOffset = UVDistortionOffset(param_11, param_12, param_13, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _338_fUVDistortionParameter.y;
    float4 param_14 = tex2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    bool param_15 = convertColorSpace;
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    float4 param_16 = tex2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    bool param_17 = convertColorSpace;
    float4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, _338_fBlendTextureParameter.x);
    Output = param_18;
    if (_338_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_338_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_338_fFalloffEndColor, _338_fFalloffBeginColor, pow(CdotN, _338_fFalloffParameter.z).xxxx);
        if (_338_fFalloffParameter.y == 0.0f)
        {
            float3 _532 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_532.x, _532.y, _532.z, Output.w);
        }
        else
        {
            if (_338_fFalloffParameter.y == 1.0f)
            {
                float3 _545 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_545.x, _545.y, _545.z, Output.w);
            }
            else
            {
                if (_338_fFalloffParameter.y == 2.0f)
                {
                    float3 _558 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_558.x, _558.y, _558.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _572 = Output.xyz * _338_fEmissiveScaling.x;
    Output = float4(_572.x, _572.y, _572.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _604 = lerp(_338_fEdgeColor.xyz * _338_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _338_fEdgeParameter.x).xxx);
    Output = float4(_604.x, _604.y, _604.z, Output.w);
    float4 param_19 = Output;
    bool param_20 = convertColorSpace;
    return ConvertToScreen(param_19, param_20);
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
    float4 _641 = _main(Input);
    _entryPointOutput = _641;
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
