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

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _369_fLightDirection : packoffset(c0);
    float4 _369_fLightColor : packoffset(c1);
    float4 _369_fLightAmbient : packoffset(c2);
    float4 _369_fFlipbookParameter : packoffset(c3);
    float4 _369_fUVDistortionParameter : packoffset(c4);
    float4 _369_fBlendTextureParameter : packoffset(c5);
    float4 _369_fCameraFrontDirection : packoffset(c6);
    float4 _369_fFalloffParameter : packoffset(c7);
    float4 _369_fFalloffBeginColor : packoffset(c8);
    float4 _369_fFalloffEndColor : packoffset(c9);
    float4 _369_fEmissiveScaling : packoffset(c10);
    float4 _369_fEdgeColor : packoffset(c11);
    float4 _369_fEdgeParameter : packoffset(c12);
    float4 _369_softParticleParam : packoffset(c13);
    float4 _369_reconstructionParam1 : packoffset(c14);
    float4 _369_reconstructionParam2 : packoffset(c15);
    float4 _369_mUVInversedBack : packoffset(c16);
    float4 _369_miscFlags : packoffset(c17);
};

Texture2D<float4> _uvDistortionTex : register(t3);
SamplerState sampler_uvDistortionTex : register(s3);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);
Texture2D<float4> _alphaTex : register(t2);
SamplerState sampler_alphaTex : register(s2);
Texture2D<float4> _blendUVDistortionTex : register(t6);
SamplerState sampler_blendUVDistortionTex : register(s6);
Texture2D<float4> _blendTex : register(t4);
SamplerState sampler_blendTex : register(s4);
Texture2D<float4> _blendAlphaTex : register(t5);
SamplerState sampler_blendAlphaTex : register(s5);
Texture2D<float4> _depthTex : register(t7);
SamplerState sampler_depthTex : register(s7);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float3 Input_WorldB;
static float3 Input_WorldT;
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
    float3 Input_WorldB : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD7;
    float4 Input_PosP : TEXCOORD8;
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

float2 UVDistortionOffset(Texture2D<float4> t, SamplerState s, float2 uv, float2 uvInversed)
{
    float2 UVOffset = (t.Sample(s, uv).xy * 2.0f) - 1.0f.xx;
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
    if (_369_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

void ApplyFlipbook(inout float4 dst, Texture2D<float4> t, SamplerState s, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 NextPixelColor = t.Sample(s, nextUV) * vcolor;
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
        float3 _119 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_119.x, _119.y, _119.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _131 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_131.x, _131.y, _131.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _144 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_144.x, _144.y, _144.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _157 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_157.x, _157.y, _157.z, dstColor.w);
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

float4 ConvertToScreen(float4 c)
{
    if (_369_miscFlags.x == 0.0f)
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
    float2 param_2 = _369_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= _369_fUVDistortionParameter.x;
    float4 param_3 = _colorTex.Sample(sampler_colorTex, Input.UV_Others.xy + UVOffset);
    float4 Output = ConvertFromSRGBTexture(param_3) * Input.Color;
    float3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, _colorTex, sampler_colorTex, _369_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5);
    Output = param_4;
    float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_6 = advancedParam.BlendUVDistortionUV;
    float2 param_7 = _369_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_6, param_7);
    BlendUVOffset *= _369_fUVDistortionParameter.y;
    float4 param_8 = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_8);
    float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, _369_fBlendTextureParameter.x);
    Output = param_9;
    float diffuse = max(dot(_369_fLightDirection.xyz, localNormal), 0.0f);
    float3 _582 = Output.xyz * ((_369_fLightColor.xyz * diffuse) + _369_fLightAmbient.xyz);
    Output = float4(_582.x, _582.y, _582.z, Output.w);
    if (_369_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_369_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_369_fFalloffEndColor, _369_fFalloffBeginColor, pow(CdotN, _369_fFalloffParameter.z).xxxx);
        if (_369_fFalloffParameter.y == 0.0f)
        {
            float3 _628 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_628.x, _628.y, _628.z, Output.w);
        }
        else
        {
            if (_369_fFalloffParameter.y == 1.0f)
            {
                float3 _641 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_641.x, _641.y, _641.z, Output.w);
            }
            else
            {
                if (_369_fFalloffParameter.y == 2.0f)
                {
                    float3 _654 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_654.x, _654.y, _654.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _668 = Output.xyz * _369_fEmissiveScaling.x;
    Output = float4(_668.x, _668.y, _668.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    screenUV.y = _369_mUVInversedBack.x + (_369_mUVInversedBack.y * screenUV.y);
    if (_369_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_10 = backgroundZ;
        float param_11 = screenPos.z;
        float4 param_12 = _369_softParticleParam;
        float4 param_13 = _369_reconstructionParam1;
        float4 param_14 = _369_reconstructionParam2;
        Output.w *= SoftParticle(param_10, param_11, param_12, param_13, param_14);
    }
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _762 = lerp(_369_fEdgeColor.xyz * _369_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _369_fEdgeParameter.x).xxx);
    Output = float4(_762.x, _762.y, _762.z, Output.w);
    float4 param_15 = Output;
    return ConvertToScreen(param_15);
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
    Input.PosP = Input_PosP;
    float4 _806 = _main(Input);
    _entryPointOutput = _806;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldB = stage_input.Input_WorldB;
    Input_WorldT = stage_input.Input_WorldT;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
