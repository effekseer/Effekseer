struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
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
    float4 _339_fLightDirection : register(c0);
    float4 _339_fLightColor : register(c1);
    float4 _339_fLightAmbient : register(c2);
    float4 _339_fFlipbookParameter : register(c3);
    float4 _339_fUVDistortionParameter : register(c4);
    float4 _339_fBlendTextureParameter : register(c5);
    float4 _339_fCameraFrontDirection : register(c6);
    float4 _339_fFalloffParameter : register(c7);
    float4 _339_fFalloffBeginColor : register(c8);
    float4 _339_fFalloffEndColor : register(c9);
    float4 _339_fEmissiveScaling : register(c10);
    float4 _339_fEdgeColor : register(c11);
    float4 _339_fEdgeParameter : register(c12);
    float4 _339_softParticleParam : register(c13);
    float4 _339_reconstructionParam1 : register(c14);
    float4 _339_reconstructionParam2 : register(c15);
    float4 _339_mUVInversedBack : register(c16);
    float4 _339_miscFlags : register(c17);
};

uniform sampler2D Sampler_sampler_uvDistortionTex : register(s3);
uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_normalTex : register(s1);
uniform sampler2D Sampler_sampler_alphaTex : register(s2);
uniform sampler2D Sampler_sampler_blendUVDistortionTex : register(s6);
uniform sampler2D Sampler_sampler_blendTex : register(s4);
uniform sampler2D Sampler_sampler_blendAlphaTex : register(s5);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float3 Input_WorldB;
static float3 Input_WorldT;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float3 Input_WorldB : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD7;
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
    bool convertColorSpace = _339_miscFlags.x != 0.0f;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _339_fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, param_3, Sampler_sampler_uvDistortionTex);
    UVOffset *= _339_fUVDistortionParameter.x;
    float4 param_4 = tex2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    bool param_5 = convertColorSpace;
    float4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    float3 texNormal = (tex2D(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float4 param_6 = Output;
    float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, _339_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8, Sampler_sampler_colorTex);
    Output = param_6;
    float4 param_9 = tex2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    bool param_10 = convertColorSpace;
    float4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_11 = advancedParam.BlendUVDistortionUV;
    float2 param_12 = _339_fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    float2 BlendUVOffset = UVDistortionOffset(param_11, param_12, param_13, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _339_fUVDistortionParameter.y;
    float4 param_14 = tex2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    bool param_15 = convertColorSpace;
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    float4 param_16 = tex2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    bool param_17 = convertColorSpace;
    float4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, _339_fBlendTextureParameter.x);
    Output = param_18;
    float diffuse = max(dot(_339_fLightDirection.xyz, localNormal), 0.0f);
    float3 _549 = Output.xyz * ((_339_fLightColor.xyz * diffuse) + _339_fLightAmbient.xyz);
    Output = float4(_549.x, _549.y, _549.z, Output.w);
    if (_339_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_339_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_339_fFalloffEndColor, _339_fFalloffBeginColor, pow(CdotN, _339_fFalloffParameter.z).xxxx);
        if (_339_fFalloffParameter.y == 0.0f)
        {
            float3 _595 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_595.x, _595.y, _595.z, Output.w);
        }
        else
        {
            if (_339_fFalloffParameter.y == 1.0f)
            {
                float3 _608 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_608.x, _608.y, _608.z, Output.w);
            }
            else
            {
                if (_339_fFalloffParameter.y == 2.0f)
                {
                    float3 _621 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_621.x, _621.y, _621.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _635 = Output.xyz * _339_fEmissiveScaling.x;
    Output = float4(_635.x, _635.y, _635.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _667 = lerp(_339_fEdgeColor.xyz * _339_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _339_fEdgeParameter.x).xxx);
    Output = float4(_667.x, _667.y, _667.z, Output.w);
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
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    float4 _710 = _main(Input);
    _entryPointOutput = _710;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldB = stage_input.Input_WorldB;
    Input_WorldT = stage_input.Input_WorldT;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
