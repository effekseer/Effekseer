struct PS_Input
{
    float4 PosVS;
    float4 UV_Others;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
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

cbuffer PS_ConstantBuffer : register(b1)
{
    float4 _393_g_scale : packoffset(c0);
    float4 _393_mUVInversedBack : packoffset(c1);
    float4 _393_fFlipbookParameter : packoffset(c2);
    float4 _393_fUVDistortionParameter : packoffset(c3);
    float4 _393_fBlendTextureParameter : packoffset(c4);
    float4 _393_softParticleParam : packoffset(c5);
    float4 _393_reconstructionParam1 : packoffset(c6);
    float4 _393_reconstructionParam2 : packoffset(c7);
};

Texture2D<float4> _uvDistortionTex : register(t3);
SamplerState sampler_uvDistortionTex : register(s3);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _alphaTex : register(t2);
SamplerState sampler_alphaTex : register(s2);
Texture2D<float4> _blendUVDistortionTex : register(t6);
SamplerState sampler_blendUVDistortionTex : register(s6);
Texture2D<float4> _blendTex : register(t4);
SamplerState sampler_blendTex : register(s4);
Texture2D<float4> _blendAlphaTex : register(t5);
SamplerState sampler_blendAlphaTex : register(s5);
Texture2D<float4> _backTex : register(t1);
SamplerState sampler_backTex : register(s1);
Texture2D<float4> _depthTex : register(t7);
SamplerState sampler_depthTex : register(s7);

static float4 gl_FragCoord;
static float4 Input_UV_Others;
static float4 Input_ProjBinormal;
static float4 Input_ProjTangent;
static float4 Input_PosP;
static float4 Input_Color;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_UV_Others : TEXCOORD0;
    float4 Input_ProjBinormal : TEXCOORD1;
    float4 Input_ProjTangent : TEXCOORD2;
    float4 Input_PosP : TEXCOORD3;
    centroid float4 Input_Color : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD7;
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
        float4 _169 = dstColor;
        float3 _172 = (blendColor.xyz * blendColor.w) + (_169.xyz * (1.0f - blendColor.w));
        dstColor.x = _172.x;
        dstColor.y = _172.y;
        dstColor.z = _172.z;
    }
    else
    {
        if (blendType == 1.0f)
        {
            float4 _187 = dstColor;
            float3 _189 = _187.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _189.x;
            dstColor.y = _189.y;
            dstColor.z = _189.z;
        }
        else
        {
            if (blendType == 2.0f)
            {
                float4 _204 = dstColor;
                float3 _206 = _204.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _206.x;
                dstColor.y = _206.y;
                dstColor.z = _206.z;
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float4 _221 = dstColor;
                    float3 _223 = _221.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _223.x;
                    dstColor.y = _223.y;
                    dstColor.z = _223.z;
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

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _393_fUVDistortionParameter.zw;
    bool param_3 = false;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2, param_3);
    UVOffset *= _393_fUVDistortionParameter.x;
    float4 Output = _colorTex.Sample(sampler_colorTex, float2(Input.UV_Others.xy) + UVOffset);
    Output.w *= Input.Color.w;
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    bool param_6 = false;
    ApplyFlipbook(param_4, _colorTex, sampler_colorTex, _393_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, param_6);
    Output = param_4;
    float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_7 = advancedParam.BlendUVDistortionUV;
    float2 param_8 = _393_fUVDistortionParameter.zw;
    bool param_9 = false;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_7, param_8, param_9);
    BlendUVOffset *= _393_fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_10 = Output;
    ApplyTextureBlending(param_10, BlendTextureColor, _393_fBlendTextureParameter.x);
    Output = param_10;
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float2 pos = Input.PosP.xy / Input.PosP.w.xx;
    float2 posR = Input.ProjTangent.xy / Input.ProjTangent.w.xx;
    float2 posU = Input.ProjBinormal.xy / Input.ProjBinormal.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _393_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _393_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _393_mUVInversedBack.x + (_393_mUVInversedBack.y * uv.y);
    float3 color = float3(_backTex.Sample(sampler_backTex, uv).xyz);
    Output.x = color.x;
    Output.y = color.y;
    Output.z = color.z;
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    if (_393_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_11 = backgroundZ;
        float param_12 = screenPos.z;
        float4 param_13 = _393_softParticleParam;
        float4 param_14 = _393_reconstructionParam1;
        float4 param_15 = _393_reconstructionParam2;
        Output.w *= SoftParticle(param_11, param_12, param_13, param_14, param_15);
    }
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV_Others = Input_UV_Others;
    Input.ProjBinormal = Input_ProjBinormal;
    Input.ProjTangent = Input_ProjTangent;
    Input.PosP = Input_PosP;
    Input.Color = Input_Color;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    float4 _706 = _main(Input);
    _entryPointOutput = _706;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    gl_FragCoord.w = 1.0 / gl_FragCoord.w;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_ProjBinormal = stage_input.Input_ProjBinormal;
    Input_ProjTangent = stage_input.Input_ProjTangent;
    Input_PosP = stage_input.Input_PosP;
    Input_Color = stage_input.Input_Color;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
