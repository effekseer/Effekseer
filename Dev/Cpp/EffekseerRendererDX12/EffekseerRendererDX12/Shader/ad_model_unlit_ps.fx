struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float4 PosP;
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

cbuffer PS_ConstantBuffer : register(b1)
{
    float4 _433_fLightDirection : packoffset(c0);
    float4 _433_fLightColor : packoffset(c1);
    float4 _433_fLightAmbient : packoffset(c2);
    float4 _433_fFlipbookParameter : packoffset(c3);
    float4 _433_fUVDistortionParameter : packoffset(c4);
    float4 _433_fBlendTextureParameter : packoffset(c5);
    float4 _433_fCameraFrontDirection : packoffset(c6);
    float4 _433_fFalloffParameter : packoffset(c7);
    float4 _433_fFalloffBeginColor : packoffset(c8);
    float4 _433_fFalloffEndColor : packoffset(c9);
    float4 _433_fEmissiveScaling : packoffset(c10);
    float4 _433_fEdgeColor : packoffset(c11);
    float4 _433_fEdgeParameter : packoffset(c12);
    float4 _433_softParticleParam : packoffset(c13);
    float4 _433_reconstructionParam1 : packoffset(c14);
    float4 _433_reconstructionParam2 : packoffset(c15);
    float4 _433_mUVInversedBack : packoffset(c16);
    float4 _433_miscFlags : packoffset(c17);
};

Texture2D<float4> _uvDistortionTex : register(t2);
SamplerState sampler_uvDistortionTex : register(s2);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _alphaTex : register(t1);
SamplerState sampler_alphaTex : register(s1);
Texture2D<float4> _blendUVDistortionTex : register(t5);
SamplerState sampler_blendUVDistortionTex : register(s5);
Texture2D<float4> _blendTex : register(t3);
SamplerState sampler_blendTex : register(s3);
Texture2D<float4> _blendAlphaTex : register(t4);
SamplerState sampler_blendAlphaTex : register(s4);
Texture2D<float4> _depthTex : register(t6);
SamplerState sampler_depthTex : register(s6);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD5;
    float4 Input_PosP : TEXCOORD6;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
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

float2 UVDistortionOffset(Texture2D<float4> t, SamplerState s, float2 uv, float2 uvInversed, bool convertFromSRGB)
{
    float4 sampledColor = t.Sample(s, uv);
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

void ApplyFlipbook(inout float4 dst, Texture2D<float4> t, SamplerState s, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, bool convertFromSRGB)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 sampledColor = t.Sample(s, nextUV);
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
        float4 _219 = dstColor;
        float3 _222 = (blendColor.xyz * blendColor.w) + (_219.xyz * (1.0f - blendColor.w));
        dstColor.x = _222.x;
        dstColor.y = _222.y;
        dstColor.z = _222.z;
    }
    else
    {
        if (blendType == 1.0f)
        {
            float4 _237 = dstColor;
            float3 _239 = _237.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _239.x;
            dstColor.y = _239.y;
            dstColor.z = _239.z;
        }
        else
        {
            if (blendType == 2.0f)
            {
                float4 _254 = dstColor;
                float3 _256 = _254.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _256.x;
                dstColor.y = _256.y;
                dstColor.z = _256.z;
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float4 _271 = dstColor;
                    float3 _273 = _271.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _273.x;
                    dstColor.y = _273.y;
                    dstColor.z = _273.z;
                }
            }
        }
    }
}

float SoftParticle(float backgroundZ, float meshZ, float4 softparticleParam, float4 reconstruct1, float4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    float2 rescale = reconstruct1.xy;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
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
    bool convertColorSpace = _433_miscFlags.x != 0.0f;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _433_fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2, param_3);
    UVOffset *= _433_fUVDistortionParameter.x;
    float4 param_4 = _colorTex.Sample(sampler_colorTex, Input.UV_Others.xy + UVOffset);
    bool param_5 = convertColorSpace;
    float4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    float4 param_6 = Output;
    float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, _colorTex, sampler_colorTex, _433_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8);
    Output = param_6;
    float4 param_9 = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    bool param_10 = convertColorSpace;
    float4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_11 = advancedParam.BlendUVDistortionUV;
    float2 param_12 = _433_fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_11, param_12, param_13);
    BlendUVOffset *= _433_fUVDistortionParameter.y;
    float4 param_14 = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    bool param_15 = convertColorSpace;
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    float4 param_16 = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    bool param_17 = convertColorSpace;
    float4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, _433_fBlendTextureParameter.x);
    Output = param_18;
    if (_433_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_433_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_433_fFalloffEndColor, _433_fFalloffBeginColor, pow(CdotN, _433_fFalloffParameter.z).xxxx);
        if (_433_fFalloffParameter.y == 0.0f)
        {
            float4 _625 = Output;
            float3 _627 = _625.xyz + FalloffBlendColor.xyz;
            Output.x = _627.x;
            Output.y = _627.y;
            Output.z = _627.z;
        }
        else
        {
            if (_433_fFalloffParameter.y == 1.0f)
            {
                float4 _642 = Output;
                float3 _644 = _642.xyz - FalloffBlendColor.xyz;
                Output.x = _644.x;
                Output.y = _644.y;
                Output.z = _644.z;
            }
            else
            {
                if (_433_fFalloffParameter.y == 2.0f)
                {
                    float4 _659 = Output;
                    float3 _661 = _659.xyz * FalloffBlendColor.xyz;
                    Output.x = _661.x;
                    Output.y = _661.y;
                    Output.z = _661.z;
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float4 _677 = Output;
    float3 _679 = _677.xyz * _433_fEmissiveScaling.x;
    Output.x = _679.x;
    Output.y = _679.y;
    Output.z = _679.z;
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    screenUV.y = _433_mUVInversedBack.x + (_433_mUVInversedBack.y * screenUV.y);
    if (_433_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_19 = backgroundZ;
        float param_20 = screenPos.z;
        float4 param_21 = _433_softParticleParam;
        float4 param_22 = _433_reconstructionParam1;
        float4 param_23 = _433_reconstructionParam2;
        Output.w *= SoftParticle(param_19, param_20, param_21, param_22, param_23);
    }
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float4 _765 = Output;
    float _768 = Output.w;
    float3 _777 = lerp(_433_fEdgeColor.xyz * _433_fEdgeParameter.y, _765.xyz, ceil((_768 - advancedParam.AlphaThreshold) - _433_fEdgeParameter.x).xxx);
    Output.x = _777.x;
    Output.y = _777.y;
    Output.z = _777.z;
    float4 param_24 = Output;
    bool param_25 = convertColorSpace;
    return ConvertToScreen(param_24, param_25);
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
    Input.PosP = Input_PosP;
    float4 _821 = _main(Input);
    _entryPointOutput = _821;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    gl_FragCoord.w = 1.0 / gl_FragCoord.w;
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
