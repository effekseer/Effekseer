#version 420

struct PS_Input
{
    vec4 Position;
    vec4 Color;
    vec2 UV;
    vec4 Pos;
    vec4 PosU;
    vec4 PosR;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
    vec2 Others;
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
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
} _186;

layout(set = 1, binding = 4) uniform sampler2D Sampler_g_uvDistortionSampler;
layout(set = 1, binding = 1) uniform sampler2D Sampler_g_sampler;
layout(set = 1, binding = 3) uniform sampler2D Sampler_g_alphaSampler;
layout(set = 1, binding = 7) uniform sampler2D Sampler_g_blendUVDistortionSampler;
layout(set = 1, binding = 5) uniform sampler2D Sampler_g_blendSampler;
layout(set = 1, binding = 6) uniform sampler2D Sampler_g_blendAlphaSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_backSampler;

layout(location = 0) in vec4 Input_Color;
layout(location = 1) in vec2 Input_UV;
layout(location = 2) in vec4 Input_Pos;
layout(location = 3) in vec4 Input_PosU;
layout(location = 4) in vec4 Input_PosR;
layout(location = 5) in vec4 Input_Alpha_Dist_UV;
layout(location = 6) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 7) in vec4 Input_Blend_FBNextIndex_UV;
layout(location = 8) in vec2 Input_Others;
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
    ret.FlipbookRate = psinput.Others.x;
    ret.AlphaThreshold = psinput.Others.y;
    return ret;
}

void ApplyFlipbook(inout vec4 dst, vec4 flipbookParameter, vec4 vcolor, vec2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        vec4 NextPixelColor = texture(SPIRV_Cross_Combinedts, nextUV) * vcolor;
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
        vec3 _77 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_77.x, _77.y, _77.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _89 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_89.x, _89.y, _89.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _102 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_102.x, _102.y, _102.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _115 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_115.x, _115.y, _115.z, dstColor.w);
                }
            }
        }
    }
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 UVOffset = vec2(0.0);
    UVOffset = (texture(Sampler_g_uvDistortionSampler, advancedParam.UVDistortionUV).xy * 2.0) - vec2(1.0);
    UVOffset *= _186.uvDistortionParameter.x;
    vec4 Output = texture(Sampler_g_sampler, Input.UV + UVOffset);
    Output.w *= Input.Color.w;
    vec4 param_1 = Output;
    float param_2 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_1, _186.flipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_2, Sampler_g_sampler);
    Output = param_1;
    vec4 AlphaTexColor = texture(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 BlendUVOffset = (texture(Sampler_g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV).xy * 2.0) - vec2(1.0);
    BlendUVOffset *= _186.uvDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_3 = Output;
    ApplyTextureBlending(param_3, BlendTextureColor, _186.blendTextureParameter.x);
    Output = param_3;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _186.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _186.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _186.mUVInversedBack.x + (_186.mUVInversedBack.y * uv.y);
    vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Pos = Input_Pos;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    vec4 _436 = _main(Input);
    _entryPointOutput = _436;
}

