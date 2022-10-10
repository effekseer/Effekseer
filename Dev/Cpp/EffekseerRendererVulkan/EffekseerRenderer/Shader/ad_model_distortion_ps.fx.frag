#version 430

struct PS_Input
{
    vec4 PosVS;
    vec4 UV_Others;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
};

struct AdvancedParameter
{
    vec2 AlphaUV;
    vec2 UVDistortionUV;
    vec2 BlendUV;
    vec2 BlendAlphaUV;
    vec2 BlendUVDistortionUV;
    vec2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
};

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
} _377;

layout(location = 3, set = 1, binding = 4) uniform sampler2D Sampler_sampler_uvDistortionTex;
layout(location = 0, set = 1, binding = 1) uniform sampler2D Sampler_sampler_colorTex;
layout(location = 2, set = 1, binding = 3) uniform sampler2D Sampler_sampler_alphaTex;
layout(location = 6, set = 1, binding = 7) uniform sampler2D Sampler_sampler_blendUVDistortionTex;
layout(location = 4, set = 1, binding = 5) uniform sampler2D Sampler_sampler_blendTex;
layout(location = 5, set = 1, binding = 6) uniform sampler2D Sampler_sampler_blendAlphaTex;
layout(location = 1, set = 1, binding = 2) uniform sampler2D Sampler_sampler_backTex;
layout(location = 7, set = 1, binding = 8) uniform sampler2D Sampler_sampler_depthTex;

layout(location = 0) centroid in vec4 Input_UV_Others;
layout(location = 1) in vec4 Input_ProjBinormal;
layout(location = 2) in vec4 Input_ProjTangent;
layout(location = 3) in vec4 Input_PosP;
layout(location = 4) centroid in vec4 Input_Color;
layout(location = 5) in vec4 Input_Alpha_Dist_UV;
layout(location = 6) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 7) in vec4 Input_Blend_FBNextIndex_UV;
layout(location = 0) out vec4 _entryPointOutput;

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

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 param = c;
    vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

vec4 LinearToSRGB(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

vec4 ConvertFromSRGBTexture(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
}

vec2 UVDistortionOffset(vec2 uv, vec2 uvInversed, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    vec4 sampledColor = texture(SPIRV_Cross_Combinedts, uv);
    if (convertFromSRGB)
    {
        vec4 param = sampledColor;
        bool param_1 = convertFromSRGB;
        sampledColor = ConvertFromSRGBTexture(param, param_1);
    }
    vec2 UVOffset = (sampledColor.xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout vec4 dst, vec4 flipbookParameter, vec4 vcolor, vec2 nextUV, float flipbookRate, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        vec4 sampledColor = texture(SPIRV_Cross_Combinedts, nextUV);
        if (convertFromSRGB)
        {
            vec4 param = sampledColor;
            bool param_1 = convertFromSRGB;
            sampledColor = ConvertFromSRGBTexture(param, param_1);
        }
        vec4 NextPixelColor = sampledColor * vcolor;
        if (flipbookParameter.y == 1.0)
        {
            dst = mix(dst, NextPixelColor, vec4(flipbookRate));
        }
    }
}

void ApplyTextureBlending(inout vec4 dstColor, vec4 blendColor, float blendType)
{
    if (blendType == 0.0)
    {
        vec3 _172 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_172.x, _172.y, _172.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _184 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_184.x, _184.y, _184.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _197 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_197.x, _197.y, _197.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _210 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_210.x, _210.y, _210.z, dstColor.w);
                }
            }
        }
    }
}

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 param_1 = advancedParam.UVDistortionUV;
    vec2 param_2 = _377.fUVDistortionParameter.zw;
    bool param_3 = false;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, param_3, Sampler_sampler_uvDistortionTex);
    UVOffset *= _377.fUVDistortionParameter.x;
    vec4 Output = texture(Sampler_sampler_colorTex, vec2(Input.UV_Others.xy) + UVOffset);
    Output.w *= Input.Color.w;
    vec4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    bool param_6 = false;
    ApplyFlipbook(param_4, _377.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, param_6, Sampler_sampler_colorTex);
    Output = param_4;
    vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_7 = advancedParam.BlendUVDistortionUV;
    vec2 param_8 = _377.fUVDistortionParameter.zw;
    bool param_9 = false;
    vec2 BlendUVOffset = UVDistortionOffset(param_7, param_8, param_9, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _377.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_10 = Output;
    ApplyTextureBlending(param_10, BlendTextureColor, _377.fBlendTextureParameter.x);
    Output = param_10;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _377.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _377.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _377.mUVInversedBack.x + (_377.mUVInversedBack.y * uv.y);
    vec3 color = vec3(texture(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    if (!(_377.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_11 = backgroundZ;
        float param_12 = screenPos.z;
        vec4 param_13 = _377.softParticleParam;
        vec4 param_14 = _377.reconstructionParam1;
        vec4 param_15 = _377.reconstructionParam2;
        Output.w *= SoftParticle(param_11, param_12, param_13, param_14, param_15);
    }
    return Output;
}

void main()
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
    vec4 _686 = _main(Input);
    _entryPointOutput = _686;
}

